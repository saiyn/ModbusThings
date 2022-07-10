#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "MBT_config.h"
#include "MBT_osNetwork.h"


static int get_net_state(void)
{
    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

    int rc = getaddrinfo(DEFAULT_SERVER_FQDN, "80", &hints, &result);
    if(rc == EAI_AGAIN){
        return MDB_NETWORK_OFFLINE;
    }else if(rc < 0){
        return MDB_NETWORK_UNKNOWN;
    }

    return MDB_NETWORK_ONLINE;

}

static int wait_for_ready(int timeout)
{



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