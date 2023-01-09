install:
	gcc src/server/server.c src/wrappers.c -o chat-server
	gcc src/client/client.c src/wrappers.c -o chat-client
	
