install:
	gcc -O3 src/server/server.c src/server/log.c src/server/handlers.c \
		src/encryption/RSA.c src/encryption/shifre.c \
		src/encryption/rsa_modules.c src/wrappers.c \
		-lm -pthread  -o chat-server

	gcc -O3 src/client/client.c src/client/graphics/handlers.c src/server/handlers.c \
		src/encryption/RSA.c src/encryption/shifre.c src/encryption/rsa_modules.c \
		src/wrappers.c \
		-lm -pthread -lncurses -o chat-client

	strip --strip-unneeded chat-server
	strip --strip-unneeded chat-client