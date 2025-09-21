#!/bin/sh

set -e

if [ ! -d "$1" ]; then
	echo Invalid directory in first argument
	exit 1
fi

WHOAMI=`whoami`
DATE=`date +'%y-%m-%d_%T'`
if [ -f $HOME/.lab1_tmstmp ]; then
	FIND_NEWER="-newer $HOME/.lab1_tmstmp"
fi

FILE=/tmp/avt219-v13/${WHOAMI}_$DATE.txt

find "$1" -mindepth 1 $FIND_NEWER > "$FILE" 2>/dev/null || true #ignore 'Permission denied'

if [ -s "$FILE" ]; then
	mail -q "$FILE" -s "Files update $DATE" "$WHOAMI" < /dev/null
fi

touch $HOME/.lab1_tmstmp
rm "$FILE"

