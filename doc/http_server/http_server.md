# HTTP Server

Todo:
- issue : problem : blocking chrome requests (2 requests in parallel)

```
[00:00:13.722,000] <err> net_tcp: net_conn_register(): -2
[00:00:13.722,000] <err> net_tcp: Cannot allocate a new TCP connection
```

- state machine 
- poll

## Chrome Request

Wireshark logs : [chrome_http_request.pcapng](./chrome_http_request.pcapng)

![chrme_http.png](./chrme_http.png)

## Curl Request

Wireshark logs : [curl_http_request.pcapng](./curl_http_request.pcapng)

```
curl http://192.168.10.233:8080 -v
```

Log Console

```
$ curl http://192.168.10.233:8080 -v
*   Trying 192.168.10.233:8080...
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
  0     0    0     0    0     0      0      0 --:--:-- --:--:-- --:--:--     0* Connected to 192.168.10.233 (192.168.10.233) port 8080 (#0)
> GET / HTTP/1.1
> Host: 192.168.10.233:8080
> User-Agent: curl/7.74.0
> Accept: */*
>
* Mark bundle as not supporting multiuse
< HTTP/1.1 200 OK
< Content-Type: text/html
< Connection: close
<
{ [61 bytes data]
100    61    0    61    0     0   1070      0 --:--:-- --:--:-- --:--:--  1070<html><body><h3>Hello from stm32f429zi !</h3></body></html>

* Closing connection 0
```

Log

```
*** Booting Zephyr OS build zephyr-v20500  ***
[00:00:01.521,000] <dbg> net_dhcpv4.net_dhcpv4_init: (main):
[00:00:01.526,000] <dbg> main_log.main: &app == &c_application::get_instance() : 1
[00:00:01.526,000] <dbg> main_log.main: app name : AdecyApp
[00:00:01.526,000] <dbg> main_log.main: app magic number : adec1
[00:00:02.527,000] <inf> if: Init interface
[00:00:02.527,000] <dbg> net_dhcpv4.net_dhcpv4_start: (main): iface 0x20000c10 state=init
[00:00:02.527,000] <dbg> net_dhcpv4.net_dhcpv4_start: (main): wait timeout=6s
[00:00:02.527,000] <dbg> net_dhcpv4.dhcpv4_set_timeout: (main): sched timeout dhcvp4=0x20000c20 timeout=6s
[00:00:02.527,000] <dbg> net_dhcpv4.dhcpv4_manage_timers: (sysworkq): iface 0x20000c10 dhcpv4=0x20000c20 state=init timeleft=6
[00:00:02.527,000] <dbg> net_dhcpv4.dhcpv4_timeout: (sysworkq): Waiting for 6s
[00:00:08.527,000] <dbg> net_dhcpv4.dhcpv4_manage_timers: (sysworkq): iface 0x20000c10 dhcpv4=0x20000c20 state=init timeleft=0
--- 21 messages dropped ---
[00:00:09.533,000] <dbg> net_dhcpv4.net_dhcpv4_input: (rx_q[0]):   ciaddr=0.0.0.0
[00:00:09.533,000] <dbg> net_dhcpv4.net_dhcpv4_input: (rx_q[0]):   yiaddr=192.168.10.233
[00:00:09.533,000] <dbg> net_dhcpv4.net_dhcpv4_input: (rx_q[0]):   siaddr=192.168.10.6
[00:00:09.533,000] <dbg> net_dhcpv4.net_dhcpv4_input: (rx_q[0]):   giaddr=0.0.0.0]
[00:00:09.533,000] <dbg> net_dhcpv4.dhcpv4_parse_options: (rx_q[0]): options_server_id: 192.168.10.6
[00:00:09.533,000] <dbg> net_dhcpv4.dhcpv4_parse_options: (rx_q[0]): options_lease_time: 40258
[00:00:09.533,000] <dbg> net_dhcpv4.dhcpv4_parse_options: (rx_q[0]): options_subnet_mask 255.255.255.0
[00:00:09.533,000] <dbg> net_dhcpv4.dhcpv4_parse_options: (rx_q[0]): options_router: 192.168.10.1
[00:00:09.533,000] <dbg> net_dhcpv4.dhcpv4_parse_options: (rx_q[0]): option unknown: 42
[00:00:09.533,000] <dbg> net_dhcpv4.dhcpv4_parse_options: (<log_strdup alloc failed>): option unknown: 15
[00:00:09.533,000] <dbg> net_dhcpv4.dhcpv4_parse_options: (rx_q[0]): options_end
[00:00:09.533,000] <dbg> net_dhcpv4.dhcpv4_handle_reply: (rx_q[0]): state=requesting msg=ack
[00:00:09.533,000] <inf> net_dhcpv4: Received: 192.168.10.233
[00:00:09.533,000] <dbg> net_dhcpv4.dhcpv4_enter_bound: (rx_q[0]): enter state=bound renewal=20129s rebinding=35225s
[00:00:09.533,000] <dbg> net_dhcpv4.dhcpv4_set_timeout: (rx_q[0]): sched timeout dhcvp4=0x20000c20 timeout=20129s
[00:00:09.533,000] <inf> if: ~ NET_EVENT_IPV4_ADDR_ADD
[00:00:09.533,000] <inf> if: Your address: 192.168.10.233
[00:00:09.533,000] <inf> if: Lease time: 40258 seconds
[00:00:09.533,000] <inf> if: Subnet: 255.255.255.0
[00:00:09.534,000] <inf> if: Router: 192.168.10.1
[00:00:09.534,000] <dbg> net_dhcpv4.dhcpv4_manage_timers: (sysworkq): iface 0x20000c10 dhcpv4=0x20000c20 state=bound timeleft=20129
[00:00:09.534,000] <dbg> net_dhcpv4.dhcpv4_timeout: (sysworkq): Waiting for 20129s
Local (Europe/Paris) Date and time : 2021/04/07 23:18:18
[00:00:09.784,000] <inf> app: SNTP try 1 / 3
[00:00:10.171,000] <inf> if: sntp_client : time since Epoch: high word: 0, low word: 1617830298
[00:00:10.171,000] <dbg> discovery.set_port: Settings port to 5001
[00:00:10.171,000] <dbg> discovery.show_port: Port is 5001
[00:00:10.172,000] <inf> main_log: System fully initialized
[00:00:10.177,000] <inf> discovery: Waiting for UDP packets on port 5001 (0)...
[00:00:10.177,000] <inf> http_server: HTTP server up (1), waiting for connections on port 8080 ...
Local (Europe/Paris) Date and time : 2021/04/07 23:18:48
[00:01:03.078,000] <inf> http_server: Connection #0 from 192.168.10.52
[00:01:03.105,000] <inf> http_server: Recv complete
[00:01:03.105,000] <inf> http_server: Send complete
[00:01:03.105,000] <inf> http_server: Connection from 192.168.10.52 closed
Local (Europe/Paris) Date and time : 2021/04/07 23:19:18
[00:01:30.091,000] <inf> http_server: Connection #1 from 192.168.10.52
[00:01:30.104,000] <inf> http_server: Recv complete
[00:01:30.105,000] <inf> http_server: Send complete
[00:01:30.105,000] <inf> http_server: Connection from 192.168.10.52 closed
Local (Europe/Paris) Date and time : 2021/04/07 23:19:48
```