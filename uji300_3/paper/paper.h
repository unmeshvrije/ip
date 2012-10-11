/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _PAPER_H_RPCGEN
#define _PAPER_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif

#define MAX_BUF 1024
#define MAX_AUTHORS 5
#define MAX_FILE_NAME 256
#define MAX_PAPER_NAME 200
#define MAX_AUTHOR_NAME 50

struct PAPER {
	int iPaperNo;
	char szAuthors[MAX_AUTHOR_NAME];
	char szPaperTitle[MAX_PAPER_NAME];
	char szFileName[MAX_FILE_NAME];
	long lFileSize;
	char ByteFileChunk[MAX_BUF];
	int iChunkIndex;
	long lChunkSize;
};
typedef struct PAPER PAPER;


#define PAPER_STORAGE_SERVER 0x20000001
#define VERSION1 1

#if defined(__STDC__) || defined(__cplusplus)
#define SendDetails 1
extern  int * senddetails_1(PAPER *, CLIENT *);
extern  int * senddetails_1_svc(PAPER *, struct svc_req *);
#define FetchInfo 2
extern  PAPER * fetchinfo_1(int *, CLIENT *);
extern  PAPER * fetchinfo_1_svc(int *, struct svc_req *);
#define FetchDetails 3
extern  PAPER * fetchdetails_1(int *, CLIENT *);
extern  PAPER * fetchdetails_1_svc(int *, struct svc_req *);
#define FetchList 4
extern  PAPER * fetchlist_1(void *, CLIENT *);
extern  PAPER * fetchlist_1_svc(void *, struct svc_req *);
#define RemoveDetails 5
extern  int * removedetails_1(int *, CLIENT *);
extern  int * removedetails_1_svc(int *, struct svc_req *);
extern int paper_storage_server_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define SendDetails 1
extern  int * senddetails_1();
extern  int * senddetails_1_svc();
#define FetchInfo 2
extern  PAPER * fetchinfo_1();
extern  PAPER * fetchinfo_1_svc();
#define FetchDetails 3
extern  PAPER * fetchdetails_1();
extern  PAPER * fetchdetails_1_svc();
#define FetchList 4
extern  PAPER * fetchlist_1();
extern  PAPER * fetchlist_1_svc();
#define RemoveDetails 5
extern  int * removedetails_1();
extern  int * removedetails_1_svc();
extern int paper_storage_server_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_PAPER (XDR *, PAPER*);
extern  bool_t xdr_PAPER (XDR *, PAPER*);

#else /* K&R C */
extern bool_t xdr_PAPER ();
extern bool_t xdr_PAPER ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_PAPER_H_RPCGEN */