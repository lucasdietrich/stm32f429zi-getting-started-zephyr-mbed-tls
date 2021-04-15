/**
 * @file http_server.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-04-15
 * 
 * @copyright Copyright (c) 2021
 * 
 * 
 * Do following command to send a HTTP request to the server
 *  curl -v http://192.168.10.233:8080/url
 */

// CONSOLE
// [00:00:46.489,000] <inf> http_server: Connection #0 from 192.168.10.52
// [00:00:46.489,000] <inf> http_server: Recv complete, termination pattern found, size : 83
// [00:00:46.490,000] <inf> http_server: parse_request
//                                       47 45 54 20 2f 20 48 54  54 50 2f 31 2e 31 0d 0a |GET / HT TP/1.1..
//                                       48 6f 73 74 3a 20 31 39  32 2e 31 36 38 2e 31 30 |Host: 19 2.168.10
//                                       2e 32 33 33 3a 38 30 38  30 0d 0a 55 73 65 72 2d |.233:808 0..User-
//                                       41 67 65 6e 74 3a 20 63  75 72 6c 2f 37 2e 37 34 |Agent: c url/7.74
//                                       2e 30 0d 0a 41 63 63 65  70 74 3a 20 2a 2f 2a 0d |.0..Acce pt: */*.
//                                       0a 0d 0a                                         |...
// [00:00:46.490,000] <dbg> http_server: on_url
//                                       2f                                               |/
// [00:00:46.490,000] <inf> http_server: HTTP request GET : parsed 83/83 errno 0
// [00:00:46.490,000] <inf> http_server: Send complete
// [00:00:46.490,000] <inf> http_server: Connection from 192.168.10.52 closed
//

// OUTPUT
// $ curl -v http://192.168.10.233:8080
// *   Trying 192.168.10.233:8080...
//   % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
//                                  Dload  Upload   Total   Spent    Left  Speed
//   0     0    0     0    0     0      0      0 --:--:-- --:--:-- --:--:--     0* Connected to 192.168.10.233 (192.168.10.233) port 8080 (#0)
// > GET / HTTP/1.1
// > Host: 192.168.10.233:8080
// > User-Agent: curl/7.74.0
// > Accept: */*
// >
// * Mark bundle as not supporting multiuse
// < HTTP/1.1 200 OK
// < Content-Type: text/html
// < Connection: close
// <
// { [59 bytes data]
// 100    59    0    59    0     0   1156      0 --:--:-- --:--:-- --:--:--  1156<html><body><h3>Hello from stm32f429zi !</h3></body></html>
// * Closing connection 0
//


#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H

#include <sys/types.h>

#include "http_request.h"

#define HTTP_SERVER_BIND_PORT               8080

#define HTTP_SERVER_THREAD_STACK_SIZE       2048
#define HTTP_SERVER_THREAD_PRIORITY         K_PRIO_PREEMPT(8)

#define HTTP_SERVER_RECV_BUFFER_SIZE        500


/*___________________________________________________________________________*/

class c_http_server
{
private:
    
    static c_http_server *p_instance;

    static struct k_thread http_server_thread;

/*___________________________________________________________________________*/

public:
    static uint32_t counter;
    
    c_http_server() {
        p_instance = this;
    }

    static c_http_server* get_instance();

    static void thread_start();

    static void thread(void *, void *, void *);

/*___________________________________________________________________________*/

    static char recv_buffer[HTTP_SERVER_RECV_BUFFER_SIZE];
    static char *send_buffer;

    static ssize_t p_recv;
    static ssize_t p_send;

    static c_http_request request;

    static inline int read_request(int client);

    static inline size_t parse_request(const char *buffer, size_t len, c_http_request *request);
};

#endif