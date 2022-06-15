
#include <arpa/inet.h>
#include <netdev.h>

/* support both enable and disable "SAL_USING_POSIX" */
#if defined(RT_USING_SAL)
#include <netdb.h>
#include <sys/socket.h>
#else
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#endif /* RT_USING_SAL */

#include "MBT_osNetwork.h"



static int get_net_state(void)
{
	int rc;
	
	rc = netdev_is_internet_up(netdev_get_by_family(AF_INET)) ? MDB_NETWORK_ONLINE : MDB_NETWORK_OFFLINE;
	
	return rc;
	
}

static int wait_for_ready(int timeout)
{
#define NETDEV_PING_DATA_SIZE       32	
	
	int ret;
	int recv_timeout = timeout;
	struct netdev_ping_resp ping_resp;
	struct netdev *netdev = netdev_get_by_family(AF_INET);
	const char *target_name = "www.baidu.com";
	
	if(timeout == 0)
		recv_timeout = 9999999;

	rt_memset(&ping_resp, 0x00, sizeof(struct netdev_ping_resp));
	
_retry:
	ret = netdev->ops->ping(netdev, target_name, NETDEV_PING_DATA_SIZE, recv_timeout, &ping_resp);
	if (ret == -RT_ETIMEOUT)
	{
			if(timeout == 0)
				goto _retry;
	}
	else if (ret == -RT_ERROR)
	{
			if(timeout == 0)
				goto _retry;
	}
	
	
	return ret;
}



mdb_network_t* mdb_network_service_init(void)
{
	
	mdb_network_t* mdbn = m_malloc(sizeof(mdb_network_t));
	if(!mdbn){
		return NULL;
	}
	
	
	mdbn->get_net_state = get_net_state;
	
	mdbn->wait_for_ready = wait_for_ready;
	
	return mdbn;
}





