#include "http_request.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(http_request, LOG_LEVEL_DBG);

#include <stdio.h>

/*___________________________________________________________________________*/

const http_parser_settings c_http_request::settings = {
    .on_message_begin = c_http_request::on_message_begin,
    .on_url = c_http_request::on_url,
    .on_status = c_http_request::on_status,
    .on_header_field = c_http_request::on_header_field,
    .on_header_value = c_http_request::on_header_value,
    .on_body = c_http_request::on_body,
    .on_message_complete = c_http_request::on_message_complete,
};

int c_http_request::on_message_begin(struct http_parser *parser)
{
    LOG_DBG("");

    return 0;
}

int c_http_request::on_url(struct http_parser *parser, const char *at, size_t length)
{
    http_parser_parse_url(at, length, 0, &((c_http_request*) parser->data)->url_parser);

    LOG_DBG("method : %s", log_strdup(http_method_str((enum http_method) parser->method)));
    LOG_HEXDUMP_DBG(at, length, "url");

    return 0;
}

// does not occurs on request
int c_http_request::on_status(struct http_parser *parser, const char *at, size_t length)
{
    char status[50];
    memcpy(status, at, length);
    status[length] = 0;

    LOG_DBG("%s", log_strdup(status));

    return 0;
}

int c_http_request::on_header_field(struct http_parser *parser, const char *at, size_t length)
{
    char field[50];
    memcpy(field, at, length);
    field[length] = 0;

    LOG_DBG("%s", log_strdup(field));

    return 0;
}

int c_http_request::on_header_value(struct http_parser *parser, const char *at, size_t length)
{
    char value[50];
    memcpy(value, at, length);
    value[length] = 0;

    LOG_DBG("%s", log_strdup(value));

    return 0;
}

int c_http_request::on_headers_complete(struct http_parser *parser)
{
    LOG_DBG("");

    return 0;
}

int c_http_request::on_body(struct http_parser *parser, const char *at, size_t length)
{
    LOG_DBG("");

    return 0;
}

int c_http_request::on_message_complete(struct http_parser *parser)
{
    // GET_HTTP_REQUEST_OBJECT(parser)->
    LOG_DBG("message complete");

    return 0;

    // if message complete, send a k_work_item to process the request
}

/*___________________________________________________________________________*/

void c_http_request::clear(void)
{
    // init parser calbacks
    // http_parser_settings_init(&http_settings);

    // init parser
    http_parser_init(&parser, HTTP_REQUEST);

    http_parser_url_init(&url_parser);

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


bool c_http_request::is_complete(void) const
{
    return parser.state == s_start_req;
}

/*___________________________________________________________________________*/
