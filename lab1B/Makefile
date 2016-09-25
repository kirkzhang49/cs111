all: client server

client:client.c
	gcc -g client.c -o client -lpthread -lmcrypt -ldl

server:server.c
	gcc -g server.c -o server -lpthread -lmcrypt -ldl
clean:
	rm client server *.txt
dist:
	tar -cvzf lab1b-604582162.tar.gz server.c Makefile README client.c my.key 