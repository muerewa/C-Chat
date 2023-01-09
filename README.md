### Description
____
Simple terminal chat written in C using sockets
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
Use this command for start client:
```
chat-client
```
