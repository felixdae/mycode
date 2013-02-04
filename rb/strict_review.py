#!/usr/bin/python
# -*- coding:utf8 -*-
import os
import sys
import subprocess
import urllib2
import cookielib
import base64
import re
import shelve
import datetime
import ConfigParser
try:
    import json
except ImportError:
    import simplejson as json

from urlparse import urljoin

from .utils import get_cmd_output, split

def get_os_conf_dir():
    platform = sys.platform
    if platform.startswith('win'):
        try:
            return os.environ['ALLUSERSPROFILE']
        except KeyError:
            print >>sys.stderr, 'Unspported operation system:%s'%platform
            sys.exit(1)
    return '/usr/local'

def get_os_temp_dir():
    import tempfile
    return tempfile.gettempdir()

def get_os_log_dir():
    platform = sys.platform
    if platform.startswith('win'):
        return get_os_conf_dir()
    return '/var/log'

def LoadReposConf(rc):
    OS_CONF_DIR = get_os_conf_dir()

    conf = ConfigParser.ConfigParser()

    conf_file = os.path.join(OS_CONF_DIR, 'reviewboard-svn-hooks', rc + '_conf.ini')
    if not conf.read(conf_file):
        raise StandardError('invalid configuration file:%s'%conf_file)

    REPOS_PATH = conf.get('rule', 'repos')
    MIN_SHIP_IT_COUNT = conf.getint('rule', 'min_ship_it_count')
    MIN_EXPERT_SHIP_IT_COUNT = conf.getint('rule', 'min_expert_ship_it_count')
    experts = conf.get('rule', 'experts')
    EXPERTS = split(experts)
    review_path = conf.get('rule', 'review_path')
    REVIEW_PATH = split(review_path)
    white_list = conf.get('rule', 'white_list')
    WHITE_LIST = split(white_list)
    return {"REPOS_PATH":REPOS_PATH,"MIN_SHIP_IT_COUNT":MIN_SHIP_IT_COUNT,\
            "MIN_EXPERT_SHIP_IT_COUNT":MIN_EXPERT_SHIP_IT_COUNT,\
            "EXPERTS":EXPERTS,"REVIEW_PATH":REVIEW_PATH,"WHITE_LIST":WHITE_LIST} 

def LoadConf():
    OS_CONF_DIR = get_os_conf_dir()

    conf = ConfigParser.ConfigParser()

    conf_file = os.path.join(OS_CONF_DIR, 'reviewboard-svn-hooks', 'conf.ini')
    if not conf.read(conf_file):
        raise StandardError('invalid configuration file:%s'%conf_file)

    repos_list = conf.get('common', 'repos_list')
    REPOS_LIST = split(repos_list)
    DEBUG = conf.getint('common', 'debug')
    RB_SERVER = conf.get('reviewboard', 'url')
    USERNAME = conf.get('reviewboard', 'username')
    PASSWORD = conf.get('reviewboard', 'password')
    ret = {"DEBUG":DEBUG,"RB_SERVER":RB_SERVER,"USERNAME":USERNAME,"PASSWORD":PASSWORD,"REPOS_LIST":REPOS_LIST}
    for rc in REPOS_LIST:
        rcf = LoadReposConf(rc)
        ret[rcf["REPOS_PATH"]]=rcf
    return ret
    
global_conf = LoadConf()

def GetConf(option):
    return global_conf[option]

def GetReposConf(repos, option):
    return global_conf[repos][option]

def debug(s):
    if not GetConf('DEBUG'):
        return
    f = open(os.path.join(get_os_log_dir(), 'reviewboard-svn-hooks', 'debug.log'), 'at')
    print >>f, str(datetime.datetime.now()), s
    f.close()

class SvnError(StandardError):
    pass

COOKIE_FILE = os.path.join(get_os_temp_dir(), 'reviewboard-svn-hooks-cookies.txt')

class Opener(object):
    def __init__(self, server, username, password, cookie_file = None):
        self._server = server
        if cookie_file is None:
            cookie_file = COOKIE_FILE
        self._auth = base64.b64encode(username + ':' + password)
        cookie_jar = cookielib.MozillaCookieJar(cookie_file)
        cookie_handler = urllib2.HTTPCookieProcessor(cookie_jar)
        self._opener = urllib2.build_opener(cookie_handler)

    def open(self, path, ext_headers, *a, **k):
        url = urljoin(self._server, path)
        return self.abs_open(url, ext_headers, *a, **k)

    def abs_open(self, url, ext_headers, *a, **k):
        debug('url open:%s' % url)
        r = urllib2.Request(url)
        for k, v in ext_headers:
            r.add_header(k, v)
        r.add_header('Authorization', 'Basic ' + self._auth)
        try:
            rsp = self._opener.open(r)
            return rsp.read()
        except urllib2.URLError, e:
            raise SvnError(str(e))

def make_svnlook_cmd(directive, repos, txn):
    cmd =['/usr/local/svn/bin/svnlook', directive, '-t',  txn, repos]
    debug(cmd)
    return cmd

def get_review_id(repos, txn):
    svnlook = make_svnlook_cmd('log', repos, txn)
    log = get_cmd_output(svnlook)
    debug(log)
    rid = re.search(r'review:[ ]*([0-9]+)', log, re.M | re.I)
    if rid:
        return rid.group(1)
    raise SvnError('No review id.')

def add_to_rid_db(rid):
    USED_RID_DB = shelve.open(os.path.join(get_os_conf_dir(),
        'reviewboard-svn-hooks',
        'rb-svn-hooks-used-rid.db'))
    if USED_RID_DB.has_key(rid):
        raise SvnError, "review-id(%s) is already used."%rid
    USED_RID_DB[rid] = rid
    USED_RID_DB.sync()
    USED_RID_DB.close()

def check_rb(repos, txn):
    rid = get_review_id(repos, txn)
    path = 'api/review-requests/' + str(rid) + '/reviews/'
    opener = Opener(GetConf('RB_SERVER'), GetConf('USERNAME'), GetConf('PASSWORD'))
    rsp = opener.open(path, {})
    reviews = json.loads(rsp)
    if reviews['stat'] != 'ok':
        raise SvnError, "get reviews error."
    ship_it_users = set()
    for item in reviews['reviews']:
        ship_it = int(item['ship_it'])
        if ship_it:
            ship_it_users.add(item['links']['user']['title'])
    
    if len(ship_it_users) < GetReposConf(repos, 'MIN_SHIP_IT_COUNT'):
        raise SvnError, "not enough of ship_it."
    expert_count = 0
    for user in ship_it_users:
        if user in GetReposConf(repos, 'EXPERTS'):
            expert_count += 1
    if expert_count < GetReposConf(repos, 'MIN_EXPERT_SHIP_IT_COUNT'):
        raise SvnError, 'not enough of key user ship_it.'
    add_to_rid_db(rid)

def _main():
    debug('command:' + str(sys.argv))
    repos = sys.argv[1]
    txn = sys.argv[2]

    svnauthor = make_svnlook_cmd('author', repos, txn)
    author = get_cmd_output(svnauthor).strip()
    debug(author)
#    for ktx in WHITE_LIST:
#        debug("xxx:"+ktx+":yyy")
    if author in GetReposConf(repos, 'WHITE_LIST'):
        debug(author+" in whitelist")
        return
#    debug("xxx"+str(len(REVIEW_PATH))+"yyy")
    if not GetReposConf(repos, 'REVIEW_PATH'):
        check_rb(repos, txn)
        return 

    svnlook = make_svnlook_cmd('changed', repos, txn)
    changed = get_cmd_output(svnlook)
    debug(changed)
    for line in changed.split('\n'):
        f = line[4:]
        for review_path in GetReposConf(repos, 'REVIEW_PATH'):
            if review_path in f:
                check_rb(repos, txn)
                return

def main():
    try:
        _main()
    except SvnError, e:
        print >> sys.stderr, str(e)
        exit(1)
    except Exception, e:
        print >> sys.stderr, str(e)
        import traceback
        traceback.print_exc(file=sys.stderr)
        exit(1)
    else:
        exit(0)

