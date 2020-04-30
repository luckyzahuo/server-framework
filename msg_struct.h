#ifndef MSG_STRUCT_H
#define	MSG_STRUCT_H

#include "util/inc/platform_define.h"

typedef struct SendMsg_t {
	char rpc_status;
	int htonl_cmdid;
	int htonl_rpcid;
	Iobuf_t iov[4];
} SendMsg_t;

#ifdef __cplusplus
extern "C" {
#endif

__declspec_dll SendMsg_t* makeSendMsg(SendMsg_t* msg, int cmdid, const void* data, unsigned int len);
__declspec_dll SendMsg_t* makeSendMsgRpcReq(SendMsg_t* msg, int cmdid, int rpcid, const void* data, unsigned int len);
__declspec_dll SendMsg_t* makeSendMsgRpcResp(SendMsg_t* msg, int rpcid, const void* data, unsigned int len);
__declspec_dll SendMsg_t* makeSendMsgEmpty(SendMsg_t* msg);

#ifdef __cplusplus
}
#endif

#endif // !MSG_STRUCT_H