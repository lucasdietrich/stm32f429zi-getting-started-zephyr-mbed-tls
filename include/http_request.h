#ifndef _HTTP_REQUEST_H
#define _HTTP_REQUEST_H

#include <net/http_parser.h>

/*___________________________________________________________________________*/

#define HTTP_REQUEST_BUFFER_SIZE        2000

/*___________________________________________________________________________*/

#define GET_HTTP_REQUEST_OBJECT(parser) (((c_http_request*) parser->data))

int on_message_complete(struct http_parser *parser);

int on_url(struct http_parser *parser, const char *at, size_t length);

/*___________________________________________________________________________*/

class c_http_request
{
public:

    struct http_parser parser;

    struct http_parser_settings settings;

    char buffer[HTTP_REQUEST_BUFFER_SIZE];

    char * p;

    uint16_t remaining = HTTP_REQUEST_BUFFER_SIZE;

    /*___________________________________________________________________________*/

    c_http_request();

    void clear(void);

    void parse_appended(int appended_size);

    /*___________________________________________________________________________*/
};


#endif