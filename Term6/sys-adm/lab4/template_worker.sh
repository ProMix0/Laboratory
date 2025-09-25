#!/bin/sh

LOG_FILE=/tmp/workers/report_`basename $0`.log

log(){
	printf '[%d] %s %s\n' "$$" "$(date +'%y/%m/%d %T')" "$1"  >> $LOG_FILE
}

log 'К работе приступил'

FIFO=/tmp/workers/run/$$.pid
mkfifo $FIFO

TIMEOUT=$((60 + $RANDOM % 541))
TIME_LEFT=$TIMEOUT
TIME_SPENT=0

handle_commands(){
	while true; do
	cmd="$(head -n 1 $FIFO)"
	echo read cmd "$cmd"
		case "$cmd" in
			'how much?')
			echo "$TIMEOUT $TIME_LEFT" > $FIFO
			;;
			'work twice!')
			TIME_LEFT=$((TIME_LEFT * 2))
			log 'Да когда ж меня отпустят, я уже заполнил контрольные недели!'
			;;
		esac
	done
}
#handle_commands &

while test "$TIME_LEFT" -gt 0; do
	TIME_LEFT=$((TIME_LEFT - 1))
	TIME_SPENT=$((TIME_SPENT + 1))
	cmd=$(timeout 1 cat $FIFO)
	if test "$?" -eq 0; then
		echo read cmd "$cmd"
		case "$cmd" in
			'how much?')
			echo "$TIMEOUT $TIME_LEFT" > $FIFO
			;;
			'work twice!')
			TIME_LEFT=$((TIME_LEFT * 2))
			log 'Да когда ж меня отпустят, я уже заполнил контрольные недели!'
			;;
		esac
	fi
done

rm $FIFO
log "На сегодня работа завершена, работал $((TIME_SPENT / 60)) минут и $((TIME_SPENT % 60)) секунд"

