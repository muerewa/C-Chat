install:
	gcc -O2 src/server/server.c src/server/log.c src/server/handlers.c src/encryption/RSA.c src/encryption/shifre.c src/encryption/rsa_modules.c src/wrappers.c -lm -pthread  -o chat-server
	gcc -O2 src/client/client.c src/server/handlers.c src/encryption/RSA.c src/encryption/shifre.c src/encryption/rsa_modules.c src/wrappers.c -lm -pthread  -o chat-client