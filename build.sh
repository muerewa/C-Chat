#!/usr/bin/bash

if [[ $EUID -eq 0 ]]; then
	startDaemonStatus=$(systemctl is-active chat-server.service)
	if [[ $startDaemonStatus == "active" ]]; then
		echo "Stop chat-server.service"
		systemctl stop chat-server.service
	fi

	rm /usr/bin/chat-server /usr/bin/chat-client
	
	make
	
	mv chat-server /usr/bin
	mv chat-client /usr/bin

	cp src/chat-server.service /etc/systemd/system/chat-server.service

	systemctl daemon-reload

	if [[ $startDaemonStatus == "active" ]]; then
		echo "Start chat-server.service"
		systemctl start chat-server.service
	fi

else
	echo "script must be run as root"
fi