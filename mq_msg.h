#ifndef MQ_MSG_H
#define	MQ_MSG_H

#include "channel_imp.h"

struct HttpFrame_t;
typedef struct UserMsg_t {
	ReactorCmd_t internal;
	Channel_t* channel;
	Sockaddr_t peer_addr;
	struct HttpFrame_t* httpframe;
	char rpc_status;
	int cmdid;
	int rpcid;
	size_t datalen;
	unsigned char data[1];
} UserMsg_t;

typedef struct SendMsg_t {
	char rpc_status;
	int htonl_cmdid;
	int htonl_rpcid;
	Iobuf_t iov[4];
} SendMsg_t;

SendMsg_t* makeSendMsg(SendMsg_t* msg, int cmdid, const void* data, unsigned int len);
SendMsg_t* makeSendMsgRpcReq(SendMsg_t* msg, int cmdid, int rpcid, const void* data, unsigned int len);
SendMsg_t* makeSendMsgRpcResp(SendMsg_t* msg, int rpcid, const void* data, unsigned int len);
SendMsg_t* makeSendMsgEmpty(SendMsg_t* msg);

#endif // !MQ_MSG_H
