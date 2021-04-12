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
[00:00:01.667,000] <dbg> main_log.main: &app == &c_application::get_instance() : 1
[00:00:01.667,000] <dbg> main_log.main: app name : AdecyApp
[00:00:01.667,000] <dbg> main_log.main: app magic number : adec1
[00:00:03.267,000] <inf> if: Init interface
[00:00:07.274,000] <inf> if: ~ NET_EVENT_IPV4_ADDR_ADD
[00:00:07.274,000] <inf> if: Your address: 192.168.10.233
[00:00:07.274,000] <inf> if: Lease time: 38004 seconds
[00:00:07.274,000] <inf> if: Subnet: 255.255.255.0
[00:00:07.274,000] <inf> if: Router: 192.168.10.1
[00:00:07.524,000] <inf> app: SNTP try 1 / 3
Local (Europe/Paris) Date and time : 2021/04/13 00:05:34
[00:00:09.415,000] <inf> if: sntp_client : time since Epoch: high word: 0, low word: 1618265134
[00:00:09.415,000] <dbg> discovery.set_port: Settings port to 5001
[00:00:09.415,000] <dbg> discovery.show_port: Port is 5001
[00:00:09.415,000] <inf> main_log: System fully initialized
[00:00:09.420,000] <inf> discovery: Waiting for UDP packets on port 5001 (0)...
[00:00:09.420,000] <inf> http_server: HTTP server up (1), waiting for connections on port 8080 ...
[00:00:12.265,000] <inf> http_server: Connection #0 from 192.168.10.52
[00:00:12.265,000] <inf> http_server: Recv complete, termination pattern found, size : 95
[00:00:12.265,000] <inf> http_server: parse_request
                                      47 45 54 20 2f 70 61 72  61 6d 73 2f 32 33 2f 7a |GET /par ams/23/z
                                      64 20 48 54 54 50 2f 31  2e 31 0d 0a 48 6f 73 74 |d HTTP/1 .1..Host
                                      3a 20 31 39 32 2e 31 36  38 2e 31 30 2e 32 33 33 |: 192.16 8.10.233
                                      3a 38 30 38 30 0d 0a 55  73 65 72 2d 41 67 65 6e |:8080..U ser-Agen
                                      74 3a 20 63 75 72 6c 2f  37 2e 37 34 2e 30 0d 0a |t: curl/ 7.74.0..
                                      41 63 63 65 70 74 3a 20  2a 2f 2a 0d 0a 0d 0a    |Accept:  */*....
[00:00:12.265,000] <dbg> http_server: on_url
                                      2f 70 61 72 61 6d 73 2f  32 33 2f 7a 64          |/params/ 23/zd
[00:00:12.265,000] <inf> http_server: HTTP request GET : parsed 95/95 errno 0
[00:00:12.265,000] <inf> http_server: failed to parse request
[00:00:12.265,000] <inf> http_server: Connection from 192.168.10.52 closed


```

## python *requests* with URI

```
import requests

s = requests.get("http://192.168.10.233:8080/params/adecy/list?au=23")

print(s, len(s.text), s.text)
```

The URI `/params/adecy/list?au=23` appears just after the `GET` in the HTTP request.

## Long request

*see branch big_http_response for project configuration*
