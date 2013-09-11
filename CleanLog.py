import sys
sys.path.append('/home/rpclogger/cleaner/lib/python2.6/site-packages/')

import MySQLdb
import time

def GetDB(dbname):
    try:
        db=MySQLdb.connect(host="", port=3306, user="", passwd="", db=dbname)
        c=db.cursor()
        return (db,c)
    except Exception,e:
        print e
        return (None,None)

def GetDbList():
    return ['']
    pass

def CleanTable(conn, cursor, table, deadline):
    try:
        sql = "DELETE FROM " + table + " WHERE send_log_time< " + str(deadline)
        cursor.execute(sql)
        conn.commit()
        return cursor.rowcount
    except Exception,e:
        print e
        return -1

def CleanDatabase(dbname, deadline):
    print "database: " + dbname
    db,c=GetDB(dbname)
    try:
        sql = "SHOW TABLES"
        c.execute(sql)
        t = c.fetchone()
        while t != None:
            #print t,"--------------------------"
            print "table: " + t[0];
            rdel = CleanTable(db, c, t[0], deadline)
            print str(rdel) + " rows deleted"
            t = c.fetchone()
        print "database: " + dbname + " done"
        c.close()
        db.close()
        return 0
    except Exception,e:
        print e
        return -1

def main():
    if len(sys.argv) < 2:
        print "usage: %s day-num" %(sys.argv[0])
        return -1
    deadline = int(time.time()) - int(sys.argv[1]) * 24 * 3600
    db_list = GetDbList()
    for dn in db_list:
        CleanDatabase(dn, deadline)
    print

if __name__=="__main__":
    main()
