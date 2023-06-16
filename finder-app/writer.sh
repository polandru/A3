#!/bin/bash

if [ -z "$1" -o -z $2 ]
then
    exit 1
fi

path=${1%/*}
mkdir -p  $path

if [ -e $1 ]
then 
    rm $1
else
    touch $1
fi

if [ ! -w $1 ]
then
    exit 1
fi

echo $2 > $1

