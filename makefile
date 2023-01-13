install:
	gcc src/server/server.c src/encryption/RSA.c src/encryption/shifre.c src/encryption/rsa_modules.c src/wrappers.c -lm -o chat-server
	gcc src/client/client.c src/wrappers.c -o chat-client