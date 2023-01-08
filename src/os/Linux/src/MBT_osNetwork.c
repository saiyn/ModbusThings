
#include "MBT_os.h"

#include "MBT_config.h"
#include "MBT_osNetwork.h"
#include "MBT_osMemory.h"

#include "MBT_osLog.h"

static int get_net_state(void)
{
    struct addrinfo hints = {0};
    struct addrinfo *result = NULL;

    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

    int rc = getaddrinfo(DEFAULT_SERVER_FQDN, NULL, &hints, &result);

    if(rc == 0){

        if(result->ai_addrlen == sizeof(struct sockaddr_in)){

            char buf[INET_ADDRSTRLEN] = {0};

            inet_ntop(AF_INET, &((struct sockaddr_in *)(result->ai_addr))->sin_addr, buf, sizeof(buf));

            MBT_OS_LOG_INFO("get server ip:%s", buf);

            freeaddrinfo(result);
        }

    }else{
        MBT_OS_LOG_INFO("get addr info of server fail:%s", gai_strerror(rc));
    }

    if(rc == EAI_AGAIN){
        return MDB_NETWORK_OFFLINE;
    }else if(rc < 0){
        return MDB_NETWORK_UNKNOWN;
    }

    return MDB_NETWORK_ONLINE;

}

static int wait_for_ready(int timeout)
{


    return 0;
}



mdb_network_t* mdb_network_service_init(void)
{

    mdb_network_t *mdbn = m_malloc(sizeof(mdb_network_t));
    if(!mdbn){
        return NULL;
    }

    mdbn->get_net_state = get_net_state;
    mdbn->wait_for_ready = wait_for_ready;


    return mdbn;
}