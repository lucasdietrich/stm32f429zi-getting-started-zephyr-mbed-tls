#ifndef _HTTP_RESPONSE_H
#define _HTTP_RESPONSE_H

#include <net/http_parser.h>
#include <net/http_parser_url.h>

/*___________________________________________________________________________*/

#define HTTP_RESPONSE_BUFFER_SIZE        2000

/*___________________________________________________________________________*/

class c_http_response
{
public:
    char buffer[HTTP_RESPONSE_BUFFER_SIZE];

    char * p;

    /*___________________________________________________________________________*/

    c_http_response();

    /*___________________________________________________________________________*/
};



#endif