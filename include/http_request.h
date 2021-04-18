#ifndef _HTTP_REQUEST_H
#define _HTTP_REQUEST_H

#include <net/http_parser.h>

/*___________________________________________________________________________*/

#define HTTP_SERVER_REQUEST_URL_SIZE        120

/*___________________________________________________________________________*/

class c_http_request
{
public:

    http_parser parser;

    char buffer[2000];

    char * p;

    

    /*___________________________________________________________________________*/

    c_http_request();

};

#endif