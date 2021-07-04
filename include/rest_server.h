#ifndef _REST_SERVER_H
#define _REST_SERVER_H

#include <stdint.h>

class c_rest_server
{
    enum method_t : uint8_t {
        GET = 1u,
        POST = 2u,
        PUT = 3u,
        DEL = 4u,
    };

    typedef struct {
        method_t method;
        const char *url;
    } route_t;

    static c_rest_server* p_instance;

    c_rest_server() {
        p_instance = this;
    }

    static constexpr const route_t routes[] = {
        {method_t::GET, "/home"},
        {method_t::GET, "/config/ethernet"},
        {method_t::GET, "/config/parameters"},
        {method_t::GET, "/config/parameters/:"},
        {method_t::GET, "/config/time"},
    };
};

#endif