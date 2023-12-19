.PHONY: build

build:
	clang client.c
	clang daemon.c -o /home/misha/daemon
	cp -f systemd.unit ~/.config/systemd/user/rgz.service
	systemctl --user daemon-reload

start:
	systemctl --user start rgz

stop:
	systemctl --user stop rgz

status:
	-systemctl --user status rgz
