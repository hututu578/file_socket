all:
	gcc file_server.c -o server
	gcc file_client.c -o client
	
clean:
	rm -f client server