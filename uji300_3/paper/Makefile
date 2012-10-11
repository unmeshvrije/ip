all:
	#rpcgen paper.x
	gcc -o paperserver paperserver_proc.c paper_svc.c paper_xdr.c ll.c ../common.c
	gcc -o paperclient paperclient.c paper_clnt.c paper_xdr.c ll.c ../common.c
