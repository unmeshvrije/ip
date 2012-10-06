all:
	rm -Rf ./out 2> /dev/null
	mkdir ./out
	
	gcc -Wall serv1.c common.c -o ./out/serv1
	gcc -Wall serv2.c common.c -o ./out/serv2
	gcc -Wall serv3.c common.c -o ./out/serv3
	gcc -Wall client.c common.c -o  ./out/client
	
	gcc -Wall talk.c chat.c common.c -pthread -lncurses -o ./out/talk
