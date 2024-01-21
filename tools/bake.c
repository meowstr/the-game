#include <assert.h>
#include <stdio.h>
#include <string.h>

int main( int argc, char ** argv )
{
    assert( argc == 2 );

    FILE * file = fopen( argv[ 1 ], "rb" );

    char var_name[ 256 ];
    strncpy( var_name, argv[ 1 ], 256 );

    // replace . with _
    // replace / with _
    for ( int i = 0; var_name[ i ] != '\0'; i++ ) {
        if ( var_name[ i ] == '.' ) var_name[ i ] = '_';
        if ( var_name[ i ] == '/' ) var_name[ i ] = '_';
        if ( var_name[ i ] == '\\' ) var_name[ i ] = '_';
    }

    printf( "unsigned char %s[] = {", var_name );

    int comma = 0;
    int n = 0;
    while ( !feof( file ) ) {
        unsigned char c;
        if ( fread( &c, 1, 1, file ) == 0 ) break;

        if ( comma ) printf( ", " );
        comma = 1;

        if ( n % 10 == 0) printf( "\n    " );
        n++;

        printf( "0x%.2X", (int) c );
    }

    fclose( file );
    printf( " };\n" );
}
