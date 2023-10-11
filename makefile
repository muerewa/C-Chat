install:
	gcc -g src/server/server.c src/server/serverHandlers.c src/handlers/writeMsgHandler.c src/handlers/readMsgHandler.c \
 	src/server/log.c src/encryption/RSA.c  \
 	src/wrappers.c -lm -pthread -lssl -lcrypto -O3 -o chat-server

	gcc -g src/client/client.c src/handlers/writeMsgHandler.c src/handlers/readMsgHandler.c \
 	src/client/clientHandlers.c src/encryption/RSA.c \
  	src/wrappers.c -lm -pthread -lssl -lcrypto -O3 -o chat-client
	strip --strip-unneeded chat-server
	strip --strip-unneeded chat-client
