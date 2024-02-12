#include "res.hpp"
#include "logging.hpp"
#include "res_data.h"

#include <string.h>

res_t find_res( const char * name )
{
    for ( int i = 0; i < res_data_count; i++ ) {
        if ( strcmp( name, res_data_name_list[ i ] ) == 0 ) {
            res_t res;
            res.data = res_data + res_data_offset_list[ i ];
            res.size = res_data_size_list[ i ];
            return res;
        }
    }

    ERROR_LOG( "failed to find resource: %s", name );

    return { nullptr, 0 };
}
