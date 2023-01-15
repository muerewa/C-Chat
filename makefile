install:
	gcc src/server/server.c src/server/log.c src/server/handlers.c src/encryption/RSA.c src/encryption/shifre.c src/encryption/rsa_modules.c src/wrappers.c -lm -pthread  -o chat-server
	gcc src/client/client.c src/server/handlers.c src/encryption/RSA.c src/encryption/shifre.c src/encryption/rsa_modules.c src/wrappers.c -lm -pthread  -o chat-client