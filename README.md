[![CI сборка](https://github.com/muerewa/C-Chat/actions/workflows/c-cpp.yml/badge.svg?branch=master)](https://github.com/muerewa/C-Chat/actions/workflows/c-cpp.yml)
### Description
____
Simple terminal chat written in C using sockets

The goal of this project is a lightweight, secure and functional chat with the ability to quickly set up and use on the linux operating system.

### Install
____
Build server and chat:
```
./build.sh
```
Start chat server:
```
sudo systemctl start chat-server.service 
```
Add chat server to auto start (optional):
```
sudo systemctl enable chat-server.service
```
### Control
____
Stop chat server:
```
sudo systemctl stop chat-server.service
```
Use this commands for start client or server in your terminal:
```
chat-client -i ip_address -p port
chat-server -i ip_adress -p port -s password
```
### Uninstall
____
For uninstalling this programm your must do:
1. Stop server service and remove it from auto start (of course if you added earlier)
    ```
    sudo systemctl disable server-chat.service
    sudo systemctl stop server-chat.service
    ```
2. Delete daemon unit file
    ```
    sudo rm /etc/systemd/system/chat-server.service
    ```
    Also you will probably need to run ```sudo systemctl daemon-reload``` to force systemd to reread the daemons files
3. Delete bin files from ```/usr/bin```
    ```
    sudo rm /usr/bin/server-chat
    sudo rm /usr/bin/client-chat
    ```
Uninstalled :heavy_check_mark:
