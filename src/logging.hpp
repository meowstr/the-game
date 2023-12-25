#pragma once

void logger_log(
    int level,
    const char * f_name,
    int line,
    const char * str,
    ...
);

#define DEBUG_LOG( args... ) logger_log( 0, __func__, __LINE__, args );
#define INFO_LOG( args... )  logger_log( 1, __func__, __LINE__, args );
#define ERROR_LOG( args... ) logger_log( 2, __func__, __LINE__, args );
