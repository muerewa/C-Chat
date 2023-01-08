#!/usr/bin/bash

mkdir "elfdir"
cd src/server
gcc server.c ../wrappers.c -o ../../elfdir/server

cd ../client
gcc client.c ../wrappers.c -o ../../elfdir/client
