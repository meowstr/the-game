#include "logging.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void logger_log(
    int level,
    const char * f_name,
    int line,
    const char * format,
    ...
)
{
    const char * pre_string = NULL;
    va_list args;
    va_start( args, format );

    if ( level < 0 || level > 2 ) {
        printf( "[!!! LOGGER !!!] Unknown error level\n" );
        return;
    }

    switch ( level ) {
    case 0:
        pre_string = "[DEBUG]";
        break;
    case 1:
        pre_string = "[INFO ]";
        break;
    case 2:
        pre_string = "[ERROR]";
        break;
    }

    // if ( level >= 2 ) printf( "\33[1;31m\n" );

    char custom_buffer[ 128 ];
    vsnprintf( custom_buffer, 128, format, args );

    printf( "%s [%20s:%-5d] %s\n", pre_string, f_name, line, custom_buffer );

    fflush( stdout );

    // if ( level >= 2 ) printf( "\33[0m\n" );

    va_end( args );
}
