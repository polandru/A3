#!/bin/bash

if [ -z "$1" -o -z "$2" ]
then
    exit 1
fi

path=$1
str=$2


COUNT=0
FCOUNT=0

for file in `find $path`
do 
  if [ ! -d "$file" ]
  then
    C=`grep -c $str $file`
    let "COUNT +=C "
    let "FCOUNT +=1 "
  fi
done

echo The number of files are $FCOUNT and the number of matching lines are $COUNT
