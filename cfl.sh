#!/bin/bash
if [ $# -lt 2 ];then
    echo "usage: $0 author start_ver end_ver"
fi
author=$1
bv=$2
ev=1000000
if [ $# -gt 2 ];then
    ev=$3
fi

#echo $author, $bv, $ev
for v in `svn log|grep $author|awk '{print $1}'|sed 's/r//g'|awk -v b="$bv" -v e="$ev" '{if ($1>=b && $1<=e)print $1}'`;do svn log -r$v -v|grep -F ".php";done|awk '{print $2}'|sort|uniq
