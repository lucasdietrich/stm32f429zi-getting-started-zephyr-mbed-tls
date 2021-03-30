#ifndef _APP_H
#define _APP_H


struct app_config_t {
    const char *name;
};

// make this class a *singleton*
class c_application
{
public:
    static c_application * p_instance;
    
public:
    struct app_config_t config;

    c_application();

    /*
    static c_application* get_instance(void);
    */

    void init(void);
};


#endif