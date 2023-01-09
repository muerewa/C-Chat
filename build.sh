#!/usr/bin/bash

if [[ $EUID -eq 0 ]]; then
	rm /usr/bin/chat-server
	rm /usr/bin/chat-client
	make
	mv chat-server /usr/bin
	mv chat-client /usr/bin

	cp src/chat-server.service /etc/systemd/system/chat-server.service

	systemctl daemon-reload

else
	echo "script must be run as root"
fi