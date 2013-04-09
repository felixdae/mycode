#!/bin/sh

REPOS="$1"
TXN="$2"

targetstr="var_dump|print_r"

SVNLOOK="/usr/local/svn/bin/svnlook"
PHP="/usr/local/php/bin/php"
#LOG=$($SVNLOOK log -t "$TXN" "$REPOS")
#CHANGED=$($SVNLOOK changed -t "$TXN" "$REPOS")
#
#if [ "$LOG" = "" ]; then
#      echo "Please input log" 1>&2
#        exit 1
#fi

#FILES=$($SVNLOOK changed -t "$TXN" "$REPOS" | awk '/^[AU].*[^/]$/ {print $NF}')

#echo $FILES>/tmp/svncc.log

flag=0
for FILE in $($SVNLOOK changed -t "$TXN" "$REPOS" | awk '/^[AU].*[pP][hH][pP]$/ {print $NF}'); do
#	for e in $($SVNLOOK cat -t "$TXN" "$REPOS" "$FILE" |\
#sed  "/^[\t]*\/\//d" |\
#sed  "s/\".*\"//" |\
#sed  "s/\/\/[^\"]*//" |\
#sed  "s/\/\*.*\*\///" |\
#sed  "s/\/\*.*\*\///" |\
#sed  "/^[ \t]*\/\*/,/.*\*\//d" |\
#sed  '
#        s-/\*.*\*/--
#/\/\*/{
#:comment
#s-/\*.*--
#:next
#n
#s-^.-//&-
#/\*\//!b next
#s-^.*\*/--
#}
#        /\/\*/b comment
#' |\
#sed 's-//.*--' |\
#grep -E "$targetstr"|awk '{print $1}'); do
#		echo "Please remove Debug Information from: $FILE:$e" 1>&2
#		flag=1
#	done
#	$SVNLOOK cat -t "$TXN" "$REPOS" "$FILE" >>/tmp/xx1
#	$SVNLOOK cat -t "$TXN" "$REPOS" "$FILE"|$PHP -w >>/tmp/xx2
#	$SVNLOOK cat -t "$TXN" "$REPOS" "$FILE"|$PHP -w|grep -c -E $targetstr >>/tmp/xx3
	count=$($SVNLOOK cat -t "$TXN" "$REPOS" "$FILE"|$PHP -w|grep -c -E $targetstr)
	if [ $count -gt 0 ] ;then
		echo "Please remove Debug Information from: $FILE" 1>&2
		flag=1	
	fi
done

if [ $flag = 1 ];then
	exit 1
fi
exit 0

