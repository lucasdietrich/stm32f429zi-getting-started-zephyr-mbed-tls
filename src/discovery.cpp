#include <stdint.h>

#include <errno.h>

#include <net/socket.h>
#include <net/net_core.h>
#include <net/net_ip.h>

#include "discovery.h"


#include <logging/log.h>
LOG_MODULE_REGISTER(discovery, LOG_LEVEL_DBG);

// todo replace with : K_THREAD_DEFINE
K_THREAD_STACK_DEFINE(stack_discovery, DISCOVERY_THREAD_STACK_SIZE);

// for linker
c_discovery * c_discovery::instance;
struct k_thread c_discovery::discovery_thread;

void c_discovery::set_port(uint16_t port)
{
    port = port;
}

c_discovery * c_discovery::get_instance(void)
{
    if (instance != nullptr)
    {
        return instance;
    }

    return nullptr;

    // todo
    // return c_discovery(nullptr);
}

void c_discovery::thread_start(void)
{
    // read about threads
    // https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/reference/kernel/threads/index.html
    k_thread_create(&discovery_thread, stack_discovery,
                                            K_THREAD_STACK_SIZEOF(stack_discovery),
                                            c_discovery::thread,
                                            NULL, NULL, NULL,
                                            DISCOVERY_THREAD_STACK_SIZE, 0, K_NO_WAIT);

    //k_thread_start(discovery_rid);
}

void c_discovery::thread(void *, void *, void *)
{
    int ret;
    int sock;

    // TODO USE VARIABLE
    int port = DISCOVERY_PORT;

    // TODO OPTIMIZE INIT

    // addr IPv4
    struct sockaddr_in addr4;

	(void)memset(&addr4, 0, sizeof(addr4));
	addr4.sin_family = AF_INET;
	addr4.sin_port = htons(port);

    // bind addr (general)
    struct sockaddr *bind_addr = (struct sockaddr *)&addr4;
    socklen_t bind_addrlen = sizeof(addr4);


    sock = socket(bind_addr->sa_family, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
		LOG_ERR("Failed to create UDP socket (%d): %d", sock, errno);
		// return -errno;
        return;
	}

    ret = bind(sock, bind_addr, bind_addrlen);
	if (ret < 0) {
		LOG_ERR("Failed to bind UDP socket (%d): %d", sock, errno);
		// return -errno;
        return;
	}

    // process
    int received;

    int counter = 0;

    struct sockaddr client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    char recv_buffer[60];

	while (true) {
        LOG_INF("Waiting for UDP packets on port %d (%d)...", port, sock);

        received = recvfrom(sock, recv_buffer, sizeof(recv_buffer), 0, &client_addr, &client_addr_len);

        if (received < 0)
        {
            /* Socket error */
            LOG_ERR("UDP (%d): Connection error %d", sock, errno);
            ret = -errno;
            break;
        }
        else if (received > 0)
        {
            // https://www.qnx.com/developers/docs/6.5.0SP1.update/com.qnx.doc.neutrino_lib_ref/a/atomic_add.html
            // atomic_add(&data->udp.bytes_received, received);

            LOG_INF("received %d bytes", received);
        }
        else // received = 0 : TIMEOUT
        {
            continue;
        }

        // loopback
        ret = sendto(sock, recv_buffer, received, 0, &client_addr, client_addr_len);

        if (ret < 0)
        {
            LOG_ERR("UDP (%d): Failed to send %d", sock, errno);
            ret = -errno;
            break;
        }

        LOG_DBG("UDP (%d): Received and replied with %d bytes", sock, received);

        LOG_INF("%d UDP: Sent %u packets", sock, ++counter);
    }

    // relinquish the CPU (but no delay)
    k_yield();

    // handle errors
}