install:
	gcc -g src/server/server.c src/handlers/writeMsgHandler.c src/handlers/readMsgHandler.c \
 	src/server/log.c src/server/serverHandlers.c src/encryption/RSA.c src/encryption/shifre.c \
 	src/encryption/rsa_modules.c src/wrappers.c -lm -pthread  -o chat-server

	gcc -g src/client/client.c src/handlers/writeMsgHandler.c src/handlers/readMsgHandler.c \
 	src/client/clientHandlers.c src/encryption/RSA.c src/encryption/shifre.c src/encryption/rsa_modules.c \
  	src/wrappers.c -lm -pthread  -o chat-client
