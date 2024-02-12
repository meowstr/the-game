#pragma once

struct res_t {
    unsigned char * data;
    int size;
};

res_t find_res( const char * name );
