#!/bin/bash
#

if [ $# -lt 1 ];then
    echo "usage: "$0" pid"
    exit -1
fi
MYPID=$1

echo "=======";
echo PID:$MYPID
echo "--------"
Rss=`echo 0 $(cat /proc/$MYPID/smaps  | grep Rss | awk '{print $2}' | sed 's#^#+#') | bc;`
Shared=`echo 0 $(cat /proc/$MYPID/smaps  | grep Shared | awk '{print $2}' | sed 's#^#+#') | bc;`
Private=`echo 0 $(cat /proc/$MYPID/smaps  | grep Private | awk '{print $2}' | sed 's#^#+#') | bc;`
Swap=`echo 0 $(cat /proc/$MYPID/smaps  | grep Swap | awk '{print $2}' | sed 's#^#+#') | bc;`
Pss=`echo 0 $(cat /proc/$MYPID/smaps  | grep Pss | awk '{print $2}' | sed 's#^#+#') | bc;`

#Mem=`echo "$Rss + $Shared + $Private + $Swap + $Pss"|bc -l`

echo "Rss     " $Rss
echo "Shared  " $Shared
echo "Private " $Private
echo "Swap    " $Swap
echo "Pss     " $Pss
echo "=================";
#echo "Mem     " $Mem
#echo "=================";
