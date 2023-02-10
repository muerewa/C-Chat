install:
	gcc -O3 src/server/server.c src/server/log.c src/server/serverHandlers.c src/encryption/RSA.c src/encryption/shifre.c src/encryption/rsa_modules.c src/wrappers.c -lm -pthread  -o chat-server
	gcc -O3 src/client/client.c src/client/clientHandlers.c src/encryption/RSA.c src/encryption/shifre.c src/encryption/rsa_modules.c src/wrappers.c -lm -pthread  -o chat-client
	strip --strip-unneeded chat-server
	strip --strip-unneeded chat-client