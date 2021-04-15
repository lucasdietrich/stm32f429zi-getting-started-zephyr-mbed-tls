#include "if.h"
#include "hw.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(if, LOG_LEVEL_INF);

/*___________________________________________________________________________*/

static struct net_mgmt_event_callback mgmt_cb[2];

extern struct k_poll_signal signal_net_addr_up;

/*___________________________________________________________________________*/

static void net_event_handler(struct net_mgmt_event_callback *cb,
		    uint32_t mgmt_event,
		    struct net_if *iface)
{
    switch (mgmt_event)
    {

    // if lost its IP address
    case NET_EVENT_IPV4_ADDR_DEL:
        hw_set_led(GREEN, led_state_t::OFF);

        LOG_WRN("~ NET_EVENT_IPV4_ADDR_DEL");
        break;


    // fun : pluging the RJ45 Ethernet cable trigger this interrupt
    case NET_EVENT_IF_UP:
        hw_set_led(RED, led_state_t::OFF);

        // todo DHCP should set this led up
        hw_set_led(GREEN, led_state_t::ON);

        LOG_DBG("~ NET_EVENT_IF_UP");

        // todo call DHCP here if CONFIG_NET_CONFIG_AUTO_INIT=n
        
        break;

    // fun : unpluging the RJ45 Ethernet cable trigger this interrupt
    case NET_EVENT_IF_DOWN:
        hw_set_led(GREEN, led_state_t::OFF);
        hw_set_led(RED, led_state_t::ON);

        // todo when disconnectin the Ethernet Wire this happens, see how to handle ...

        LOG_WRN("~ NET_EVENT_IF_DOWN");
        break;

    // client DHCP succeeded, the if get an IP address
    case NET_EVENT_IPV4_ADDR_ADD:
        hw_set_led(GREEN, led_state_t::ON);

        LOG_DBG("~ NET_EVENT_IPV4_ADDR_ADD");

        // display address
        for (uint_fast16_t i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) // short to 1 here (1 interface)
        {
            char buf[NET_IPV4_ADDR_LEN];

            if (iface->config.ip.ipv4->unicast[i].addr_type != NET_ADDR_DHCP)
            {
                //TODO
                LOG_INF("\n NET_ADDR_DHCP ~ continue");
                continue;
            }

            LOG_INF("IP address received from DHCP server");

            LOG_INF("Your address: %s",
                    log_strdup(net_addr_ntop(AF_INET,
                                             &iface->config.ip.ipv4->unicast[i].address.in_addr,
                                             buf, sizeof(buf))));
            LOG_DBG("Lease time: %u seconds",
                    iface->config.dhcpv4.lease_time);
            LOG_DBG("Subnet: %s",
                    log_strdup(net_addr_ntop(AF_INET,
                                             &iface->config.ip.ipv4->netmask,
                                             buf, sizeof(buf))));
            LOG_DBG("Router: %s",
                    log_strdup(net_addr_ntop(AF_INET,
                                             &iface->config.ip.ipv4->gw,
                                             buf, sizeof(buf))));
        }

        // signal that net if has IP addr
        k_poll_signal_raise(&signal_net_addr_up, 1);

        break;

    default:
        break;
    }
}

void init_if(void) {
    // settings callbacks

    // interface is already up when CONFIG_NET_CONFIG_AUTO_INIT=y and then following events are never called
    // todo CONFIG_NET_CONFIG_AUTO_INIT=n and call net_config_init (see ISSUE 001)
    net_mgmt_init_event_callback(&mgmt_cb[0], net_event_handler, NET_EVENT_IF_UP | NET_EVENT_IF_DOWN);
	net_mgmt_add_event_callback(&mgmt_cb[0]);

    // events for DHCP
	net_mgmt_init_event_callback(&mgmt_cb[1], net_event_handler, NET_EVENT_IPV4_ADDR_ADD | NET_EVENT_IPV4_ADDR_DEL);
	net_mgmt_add_event_callback(&mgmt_cb[1]);

    struct net_if *iface;

    iface = net_if_get_default();

    // start DHCP
    net_dhcpv4_start(iface);
}

uint64_t get_sntp_time(void) {
    int err;
    struct sntp_time get_time;

    
    // 0.fr.pool.ntp.org : 62.210.206.214
    // 1. / 2. / 3.

    // need dns to be configured
    err = sntp_simple("0.fr.pool.ntp.org", 10000, &get_time);

    if (err < 0) {
        if (err == -ETIMEDOUT) {
            LOG_ERR("sntp_client : get_sntp_time ETIMEDOUT");
        } else {
            LOG_ERR("sntp_client : get_sntp_time err %d", err);
        }

        return 0;
    }

    LOG_DBG("sntp_client : time since Epoch: high word: %u, low word: %u",
            (uint32_t)(get_time.seconds >> 32), (uint32_t)get_time.seconds);

    // return time in seconds
    return get_time.seconds;

    /*

    // settime
    struct timeval tv;
    tv.tv_sec = get_time.seconds;
    tv.tv_usec = get_time.fraction;

    struct timezone tz;
    tz.tz_dsttime = DST_WET;
    tz.tz_minuteswest = 60;
    
	err = settimeofday(&tv, &tz);
	printk("settimeofday: %d\n", err);

    // print
    // gettimeofday()

    */
}

void down_if(void) {
    struct net_if *iface;

    LOG_INF("Down ethernet interface");

    iface = net_if_get_default();

    net_if_down(iface);
}

uint32_t get_ip(void)
{
    struct net_if *iface;

    iface = net_if_get_default();

    // struct in_addr
    uint32_t ip = (uint32_t) (iface->config.ip.ipv4->unicast[0].address.in_addr.s_addr);

    return ip;
}

int get_ip(char * buffer, unsigned int buffer_len)
{
    if (buffer_len >= NET_IPV4_ADDR_LEN)
    {
        struct net_if *iface;

        iface = net_if_get_default();

        net_addr_ntop(AF_INET, &iface->config.ip.ipv4->unicast[0].address.in_addr, buffer, buffer_len);

        return 0;
    }
    return -1;
}