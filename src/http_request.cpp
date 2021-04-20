#include "http_request.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(http_request, LOG_LEVEL_DBG);

/*___________________________________________________________________________*/

int on_message_complete(struct http_parser *parser)
{
    // GET_HTTP_REQUEST_OBJECT(parser)->
    LOG_DBG("message complete");

    return 0;
}

int on_url(struct http_parser *parser, const char *at, size_t length)
{
    LOG_HEXDUMP_DBG(at, length, "on_url");

    return 0;
}

/*___________________________________________________________________________*/

void c_http_request::clear(void)
{
    // init parser calbacks
    http_parser_settings_init(&settings);

    settings.on_message_complete = on_message_complete;

    // init parser
    http_parser_init(&parser, HTTP_REQUEST);

    // set contexte
    parser.data = (void *) this;

    // p - buffer : amout of parsed bytes
    p = buffer;

    remaining = HTTP_REQUEST_BUFFER_SIZE;
}

void c_http_request::parse_appended(int appended_size)
{
    int ret = http_parser_execute(&parser, &settings, p, appended_size);

    p += appended_size;

    remaining -= appended_size;

    LOG_DBG("parser called on latest %d appended bytes - ret=%d - parsed %d/%d [MAX]",
            appended_size, ret, HTTP_REQUEST_BUFFER_SIZE - remaining, HTTP_REQUEST_BUFFER_SIZE);
}

/*___________________________________________________________________________*/
