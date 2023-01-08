#!/usr/bin/bash
cd src/server
gcc server.c ../wrappers.c -o server

cd ../client
gcc client.c ../wrappers.c -o client
