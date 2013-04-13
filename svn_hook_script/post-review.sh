#!/bin/sh

REPOS="$1"
TXN="$2"

if [ $1 = '/usr/local/svndata/web_app' ]
then
	rep_url='svn://192.168.0.25:9999/web_app'
	rev_group='php-dev'
elif [ $1 = '/usr/local/svndata/server' ]
then
	rep_url='svn://192.168.0.25:9999/server'
	rev_group='backend-dev'
else
	exit 0
fi

SVNLOOK="/usr/local/svn/bin/svnlook"
SVNLOOKAUTHOR=$SVNLOOK" author -t "$TXN" "$REPOS
SVNLOOKDIFF=$SVNLOOK" diff -t "$TXN" "$REPOS
SVNLOOKLOG=$SVNLOOK" log -t "$TXN" "$REPOS
echo $SVNLOOKAUTHOR" "$SVNLOOKLOG" "$SVNLOOKDIFF >>/tmp/testpostreview.log

author=`$SVNLOOKAUTHOR`
echo $author >>/tmp/testpostreview.log

diff_file=`mktemp`
$SVNLOOKDIFF > $diff_file

message=`env LANG=zh_CN $SVNLOOKLOG`
echo $message >>/tmp/testpostreview.log
message="${message##*( )}"
message="${message%%*( )}"

cmd1=`echo $message|grep '^[  \f\n\r\t\v]*po-rev'`
cmd2=`echo $message|grep '^[  \f\n\r\t\v]*up-rev:'`

nwmes=`echo $message|sed 's/^[  \f\n\r\t\v]*po-rev[  \f\n\r\t\v]*//'|sed 's/^[  \f\n\r\t\v]*up-rev:[  \f\n\r\t\v]*[0-9]*//'|iconv -cs -fgbk -tutf8 -`
#nwmes=`echo $message|sed 's/[  \f\n\r\t\v]\+/-/g'|iconv -cs -fgbk -tutf8 -`
#echo $nwmes 1>&2
#exit 1
if [ -n "$cmd1" ]
then
#	echo "(xxxx(((("$cmd1")"$cmd2")))" >>/tmp/testpostreview.log
	prcmd="post-review -p --server=http://rpc-rb-mysql.****.com --summary='"$nwmes"' --description='"$nwmes"' --target-groups="$rev_group" --submit-as="$author" --username=felix --password=felix --diff-filename="$diff_file" --repository-url="$rep_url
elif [ -n "$cmd2" ]
then
#	echo "(yyyy(((("$cmd1")"$cmd2")))" >>/tmp/testpostreview.log
	rid=`echo $message|sed 's/^[  \f\n\r\t\v]*up-rev:[  \f\n\r\t\v]*//'|sed 's/[^0-9].*//'`
	prcmd="post-review -p -r "$rid" --diff-only --server=http://rpc-rb-mysql.****.com --change-description='"$nwmes"' --submit-as="$author" --username=felix --password=felix --diff-filename="$diff_file" --repository-url="$rep_url
else
	exit 0
fi

echo $prcmd >>/tmp/testpostreview.log

sh -c "$prcmd" 1>&2
#output=`$prcmd`
#echo $output >>/tmp/testpostreview.log
#echo $output 1>&2

rm -fr $diff_file

exit 1
