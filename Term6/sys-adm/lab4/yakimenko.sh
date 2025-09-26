#!/bin/sh

mkdir -p /tmp/workers/run

LOG_FILE=/tmp/workers/yakimenko.log
OBSERVER_LOG=observer.log

log(){
	printf '%s %s\n' "$(date +'%y/%m/%d %T')" "$1"  >> $2
}

busy_int(){
	log "SIGINT - Попрошу не беспокоить, работа еще не сделана!" "$OBSERVER_LOG"
}
busy_quit(){
	log "SIGQUIT - Попрошу не беспокоить, работа еще не сделана!" "$OBSERVER_LOG"
}

trap busy_int SIGINT
trap busy_quit SIGQUIT

cat /tmp/workers/kafedra.conf | while read -r worker_path; do
	test -n "$worker_path" || continue
	found=false
	worker_name=$(basename "$worker_path")
	for proc in /proc/[[:digit:]]*/cmdline; do
		pid=$(echo $proc | sed -n 's|/proc/\([0-9]\+\)/cmdline|\1|p')
		if grep -q "$worker_path" "$proc"; then
			found=true
			echo 'how much?' > "/tmp/workers/run/$pid.pid"
			if read -t 1 timeout left < "/tmp/workers/run/$pid.pid"; then
				if [ $(($left*100/$timeout)) -lt 70 -a $(($RANDOM % 10)) -lt 3 ]; then
					echo 'work twice!' > "/tmp/workers/run/$pid.pid"
					log "$worker_name поработает ещё" "$LOG_FILE"
				fi
			else
				log "Не удалось связаться с $worker_name!" "$LOG_FILE"
			fi
			break
		fi
	done
	if [[ "$found" = "false" ]]; then
		nohup "$worker_path" > /dev/null &
		log "Работник $worker_name явился на работу" "$LOG_FILE"
	fi
done

