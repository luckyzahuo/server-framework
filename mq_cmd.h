#ifndef MQ_CMD_H
#define	MQ_CMD_H

enum {
	CMD_REQ_RECONNECT = 1,
	CMD_RET_RECONNECT,

	CMD_REQ_UPLOAD_CLUSTER,
	CMD_RET_UPLOAD_CLUSTER,
	CMD_NOTIFY_NEW_CLUSTER,
	CMD_REQ_REMOVE_CLUSTER,
	CMD_RET_REMOVE_CLUSTER,

	CMD_REQ_TEST,
	CMD_NOTIFY_TEST,

	CMD_RPC_RET_START	= 10000,
	CMD_RET_TEST		= CMD_REQ_TEST + CMD_RPC_RET_START
};

#endif // !MQ_CMD_H
