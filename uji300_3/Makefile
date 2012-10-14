all:	paper hotelserver hotelclient hotelgw hotegwclient

paper: paper/paper.x 
#paperserver_proc.c paper_svc.c paper_xdr.c paperclient.c paper_clnt.c ll.c ../common.c
	cd ./paper/
	rpcgen paper.x
	gcc -Wall -o paperserver paperserver_proc.c paper_svc.c paper_xdr.c ll.c ../common.c
	gcc -Wall -o paperclient paperclient.c paper_clnt.c paper_xdr.c ll.c ../common.c

hotelserver: IReserver.java
	cd ../hotel/HotelReservation/
	rm -rf ./bin 2> /dev/null
	mkdir bin
	javac -d bin/ *.java

hotelclient: HotelReservationClient.java
	cd ../HotelReservationClient/
	cp ../HotelReservation/bin/IReserver.class  .
	rm -rf ./bin 2> /dev/null
	mkdir bin
	javac -d bin/ *.java

hotelgw: hotelgw.java
	cd ../../hotelgw
	cp ../hotel/HotelReservation/bin/IReserver.class  .
	rm -rf ./bin 2> /dev/null
	mkdir bin
	javac -d bin/ *.java

hotelgwclient:
	cd ../hotelgwclient
	gcc -Wall -o hotelgwclient hotelgwclient.c ../common.c
