#include "service_comm_cmd.h"
#include "service_comm_proc.h"

static int ret_cluster_list(UserMsg_t* ctrl) {
	ListNode_t* lcur;
	int cluster_self_find;

	logInfo(ptr_g_Log(), "%s recv: %s", __FUNCTION__, (char*)ctrl->data);

	if (!loadClusterNodeFromJsonData(ptr_g_ClusterTable(), (char*)ctrl->data)) {
		logErr(ptr_g_Log(), "%s.loadClusterNodeFromJsonData error", __FUNCTION__);
		return 0;
	}

	cluster_self_find = 0;
	for (lcur = getClusterList(ptr_g_ClusterTable())->head; lcur; lcur = lcur->next) {
		Cluster_t* cluster = pod_container_of(lcur, Cluster_t, m_listnode);
		if (!strcmp(cluster->name, getClusterSelf()->name) &&
			!strcmp(cluster->ip, getClusterSelf()->ip) &&
			cluster->port == getClusterSelf()->port &&
			cluster->socktype == getClusterSelf()->socktype)
		{
			setClusterSelf(cluster);
			cluster_self_find = 1;
			break;
		}
	}
	if (!cluster_self_find) {
		logErr(ptr_g_Log(), "%s cluster self not find", __FUNCTION__);
		return 0;
	}

	if (getClusterSelf()->port) {
		ReactorObject_t* o = openListenerInner(getClusterSelf()->socktype, getClusterSelf()->ip, getClusterSelf()->port);
		if (!o)
			return 0;
		reactorCommitCmd(ptr_g_ReactorAccept(), &o->regcmd);
	}

	return 1;
}

static void rpc_ret_cluster_list(RpcAsyncCore_t* rpc, RpcItem_t* rpc_item) {
	if (rpc_item->ret_msg) {
		UserMsg_t* ctrl = (UserMsg_t*)rpc_item->ret_msg;
		if (0 == ctrl->retcode && ret_cluster_list(ctrl)) {
			return;
		}
	}
	g_Invalid();
}

static void retClusterList(TaskThread_t* thrd, UserMsg_t* ctrl) {
	ret_cluster_list(ctrl);
}

#ifdef __cplusplus
extern "C" {
#endif

int rpcReqClusterList(TaskThread_t* thrd, Cluster_t* sc_cluster) {
	Channel_t* c;
	SendMsg_t msg;
	char* req_data;
	int req_datalen;

	if (!regNumberDispatch(CMD_RET_CLUSTER_LIST, retClusterList)) {
		logErr(ptr_g_Log(), "regNumberDispatch(CMD_RET_CLUSTER_LIST, retClusterList) failure");
		return 0;
	}
	req_data = strFormat(&req_datalen, "{\"ip\":\"%s\",\"port\":%u, \"socktype\":\"%s\"}",
		getClusterSelf()->ip, getClusterSelf()->port, if_socktype2string(getClusterSelf()->socktype));
	if (!req_data) {
		return 0;
	}

	c = clusterConnect(sc_cluster);
	if (!c) {
		logErr(ptr_g_Log(), "channel connecting ServiceCenter, ip:%s, port:%u err ......",
			sc_cluster->ip, sc_cluster->port);
		return 0;
	}

	logInfo(ptr_g_Log(), "channel connecting ServiceCenter, ip:%s, port:%u, and ReqClusterList ......",
		sc_cluster->ip, sc_cluster->port);

	if (!thrd->f_rpc && !thrd->a_rpc) {
		makeSendMsg(&msg, CMD_REQ_CLUSTER_LIST, req_data, req_datalen);
		channelSendv(c, msg.iov, sizeof(msg.iov) / sizeof(msg.iov[0]), NETPACKET_FRAGMENT);
		free(req_data);
	}
	else {
		RpcItem_t* rpc_item;
		if (thrd->f_rpc) {
			rpc_item = newRpcItemFiberReady(thrd, c, 5000);
			if (!rpc_item) {
				free(req_data);
				return 0;
			}
		}
		else {
			rpc_item = newRpcItemAsyncReady(thrd, c, 5000, NULL, rpc_ret_cluster_list);
			if (!rpc_item) {
				free(req_data);
				return 0;
			}
		}
		makeSendMsgRpcReq(&msg, rpc_item->id, CMD_REQ_CLUSTER_LIST, req_data, req_datalen);
		channelSendv(c, msg.iov, sizeof(msg.iov) / sizeof(msg.iov[0]), NETPACKET_FRAGMENT);
		free(req_data);
		if (!thrd->f_rpc)
			return 1;
		rpc_item = rpcFiberCoreYield(thrd->f_rpc);
		if (rpc_item->ret_msg) {
			UserMsg_t* ctrl = (UserMsg_t*)rpc_item->ret_msg;
			if (ctrl->retcode)
				return 0;
			if (!ret_cluster_list(ctrl))
				return 0;
		}
		else {
			return 0;
		}
	}
	return 1;
}

#ifdef __cplusplus
}
#endif