#include "if.h"
#include "hw.h"


#include <logging/log.h>
LOG_MODULE_REGISTER(net_dhcpv4_client, LOG_LEVEL_DBG);

static struct net_mgmt_event_callback mgmt_cb[3];

static void net_event_handler(struct net_mgmt_event_callback *cb,
		    uint32_t mgmt_event,
		    struct net_if *iface)
{
    switch (mgmt_event)
    {

    case NET_EVENT_IPV4_ADDR_DEL:
        hw_set_led(led_t::BLUE, led_state_t::OFF);

        LOG_INF("~ NET_EVENT_IPV4_ADDR_DEL");
        break;

    case NET_EVENT_IF_UP:
        hw_set_led(led_t::GREEN, led_state_t::ON);
        hw_set_led(led_t::RED, led_state_t::OFF);

        LOG_INF("~ NET_EVENT_IF_UP");
        break;

    case NET_EVENT_IF_DOWN:
        hw_set_led(led_t::GREEN, led_state_t::OFF);
        hw_set_led(led_t::BLUE, led_state_t::OFF);
        hw_set_led(led_t::RED, led_state_t::ON);

        LOG_INF("~ NET_EVENT_IF_DOWN");
        break;

    case NET_EVENT_IPV4_ADDR_ADD:
        hw_set_led(led_t::BLUE, led_state_t::ON);

        LOG_INF("~ We now have an IP addr");

        for (uint_fast16_t i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) // short to 1 here (1 interface)
        {
            char buf[NET_IPV4_ADDR_LEN];

            if (iface->config.ip.ipv4->unicast[i].addr_type != NET_ADDR_DHCP)
            {
                LOG_INF("\n NET_ADDR_DHCP ~ continue");
                continue;
            }

            LOG_INF("Your address: %s",
                    log_strdup(net_addr_ntop(AF_INET,
                                             &iface->config.ip.ipv4->unicast[i].address.in_addr,
                                             buf, sizeof(buf))));
            LOG_INF("Lease time: %u seconds",
                    iface->config.dhcpv4.lease_time);
            LOG_INF("Subnet: %s",
                    log_strdup(net_addr_ntop(AF_INET,
                                             &iface->config.ip.ipv4->netmask,
                                             buf, sizeof(buf))));
            LOG_INF("Router: %s",
                    log_strdup(net_addr_ntop(AF_INET,
                                             &iface->config.ip.ipv4->gw,
                                             buf, sizeof(buf))));
        }
        break;

    default:
        break;
    }
}

void init_if(void) {
    struct net_if *iface;

	LOG_INF("Run dhcpv4 client");

    /*
    net_mgmt_init_event_callback(&mgmt_cb[0], handler, NET_EVENT_ETHERNET_CARRIER_ON | NET_EVENT_ETHERNET_CARRIER_OFF);
	net_mgmt_add_event_callback(&mgmt_cb[0]);
    */

    net_mgmt_init_event_callback(&mgmt_cb[1], net_event_handler, NET_EVENT_IF_UP | NET_EVENT_IF_DOWN);
	net_mgmt_add_event_callback(&mgmt_cb[1]);

	net_mgmt_init_event_callback(&mgmt_cb[2], net_event_handler, NET_EVENT_IPV4_ADDR_ADD | NET_EVENT_IPV4_ADDR_DEL);
	net_mgmt_add_event_callback(&mgmt_cb[2]);

    
	iface = net_if_get_default();

	net_dhcpv4_start(iface);
}

void down_if(void) {
    struct net_if *iface;

    LOG_INF("Down ethernet interface");

    iface = net_if_get_default();

    net_if_down(iface);
}