#ifndef _HTTP_REQUEST_H
#define _HTTP_REQUEST_H

#include <net/http_parser.h>
#include <net/http_parser_url.h>

/*___________________________________________________________________________*/

#define HTTP_REQUEST_BUFFER_SIZE        2000

/*___________________________________________________________________________*/

#define GET_HTTP_REQUEST_OBJECT(parser) (((c_http_request*) parser->data))

/*___________________________________________________________________________*/

class c_http_request
{
public:
    struct http_parser parser;

    struct http_parser_url url_parser;

    static const struct http_parser_settings settings;

    char buffer[HTTP_REQUEST_BUFFER_SIZE];

    char * p;

    uint16_t remaining = HTTP_REQUEST_BUFFER_SIZE;

    /*___________________________________________________________________________*/

    static int on_message_begin(struct http_parser *parser);

    static int on_url(struct http_parser *parser, const char *at, size_t length);

    static int on_status(struct http_parser *parser, const char *at, size_t length);

    static int on_header_field(struct http_parser *parser, const char *at, size_t length);

    static int on_header_value(struct http_parser *parser, const char *at, size_t length);

    static int on_headers_complete(struct http_parser *parser);

    static int on_body(struct http_parser *parser, const char *at, size_t length);

    static int on_message_complete(struct http_parser *parser);    

    /*___________________________________________________________________________*/

    c_http_request();

    void clear(void);

    void parse_appended(int appended_size);

    bool is_complete(void) const;

    /*___________________________________________________________________________*/

    c_http_request * allocate(const size_t init_size);

    /*___________________________________________________________________________*/
};

/*___________________________________________________________________________*/




#endif