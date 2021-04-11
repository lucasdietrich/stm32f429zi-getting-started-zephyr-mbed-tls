#ifndef _HTTP_REQUEST_H
#define _HTTP_REQUEST_H

#define HTTP_SERVER_REQUEST_URL_SIZE        120

/*___________________________________________________________________________*/

class c_http_request
{
public:

    enum method_t { GET, POST, PUT, DEL };

    /*___________________________________________________________________________*/

    c_http_request();

    method_t method;
    char url[HTTP_SERVER_REQUEST_URL_SIZE];

};

#endif