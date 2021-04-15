#include <stdint.h>
#include <errno.h>

#include <net/socket.h>
#include <net/net_core.h>
#include <net/net_ip.h>
#include <net/net_if.h>

#include "discovery.h"
#include "if.h"
#include "app.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(discovery, LOG_LEVEL_INF);


/*___________________________________________________________________________*/

// todo replace with : K_THREAD_DEFINE
K_THREAD_STACK_DEFINE(stack_discovery, DISCOVERY_THREAD_STACK_SIZE);

// delay : https://github.com/zephyrproject-rtos/zephyr/issues/21305#issue-536508184
/*
K_THREAD_DEFINE(
        discovery, DISCOVERY_THREAD_STACK_SIZE,
        c_discovery::thread, NULL, NULL, NULL,
        DISCOVERY_THREAD_PRIORITY, 0, 0); // see how to set K_FOREVER
*/

// for linker

/*___________________________________________________________________________*/

c_discovery * c_discovery::p_instance;

struct k_thread c_discovery::discovery_thread;

// class variables
uint32_t c_discovery::counter;
char c_discovery::recv_buffer[60];
char c_discovery::send_buffer[60];

/*___________________________________________________________________________*/

void c_discovery::set_port(uint16_t port)
{
    LOG_DBG("Settings port to %d", port);

    this->port = port;
}

void c_discovery::show_port(void)
{
    LOG_DBG("Port is %d", port);
}

c_discovery *c_discovery::get_instance(void)
{
    return p_instance;

    // todo
    // return c_discovery(nullptr);
}

/*___________________________________________________________________________*/

void c_discovery::thread_start(void)
{
    // read about threads
    // https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/reference/kernel/threads/index.html

    k_tid_t thread_id = k_thread_create(&discovery_thread, stack_discovery,
                                            K_THREAD_STACK_SIZEOF(stack_discovery),
                                            c_discovery::thread,
                                            NULL, NULL, NULL,
                                            DISCOVERY_THREAD_PRIORITY, 0, K_FOREVER);

#if CONFIG_THREAD_NAME

    k_thread_name_set(thread_id, "discovery");

#endif
    
    k_thread_start(thread_id);
}

void c_discovery::thread(void *, void *, void *)
{
    int ret;
    int sock;

    // TODO OPTIMIZE INIT

    // addr IPv4
    struct sockaddr_in addr4;

	(void)memset(&addr4, 0, sizeof(addr4));
	addr4.sin_family = AF_INET;
	addr4.sin_port = htons(p_instance->port);

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
    char *p_send_buffer;
    int send_buffer_length;

    counter = 0;

    struct sockaddr client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

	while (true) {
        LOG_INF("UDP server up (%d), waiting for UDP packets on port %d ...", sock, p_instance->port);

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

            LOG_INF("UDP Discovery request received");

            LOG_DBG("received %d bytes", received);
        }
        else // received = 0 : TIMEOUT
        {
            continue;
        }


#if DISCOVERY_LOOPBACK
        // loopback
        p_send_buffer = recv_buffer;
        send_buffer_length = received;
#else
        // udp response
        p_send_buffer = send_buffer;
        send_buffer_length = build_response();
#endif

        ret = sendto(sock, p_send_buffer, send_buffer_length, 0, &client_addr, client_addr_len);

        if (ret < 0)
        {
            LOG_ERR("UDP (%d): Failed to send %d", sock, errno);
            ret = -errno;
            break;
        }

        LOG_DBG("UDP (%d): Received and replied with %d bytes", sock, received);

        LOG_DBG("%d UDP: Sent %u packets", sock, ++counter);
    }

    // relinquish the CPU (but no delay)
    k_yield();

    // handle errors
}

/*___________________________________________________________________________*/

inline size_t c_discovery::build_response(void)
{
    // build response structure
    struct udp_response response;

    app_time_t *p_app_time = &c_application::get_instance().time;
    app_stats_t *p_app_stats = &c_application::get_instance().stats;

    response.ip = get_ip();
    
    memcpy(&response.time, (void *)p_app_time, sizeof(struct app_time_t));
    memcpy(&response.stats, (void *)p_app_stats, sizeof(struct app_stats_t));
    get_ip(response.str_ip, NET_IPV4_ADDR_LEN);

    // set up send_buffer
    memcpy(send_buffer, &response, sizeof(udp_response));

    LOG_DBG("sizeof(struct udp_response) = %d + %d + %d = %d", 4, NET_IPV4_ADDR_LEN, sizeof(struct app_time_t), sizeof(struct udp_response));

    return sizeof(udp_response);
}