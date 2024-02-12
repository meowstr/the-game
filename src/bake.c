#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int write_file( const char * filename, int index )
{
    int i = 0;
    FILE * file = fopen( filename, "rb" );

    if ( !file ) {
        fprintf( stderr, "failed to open: %s\n", filename );
        exit( 1 );
    }

    int n = 0;
    while ( !feof( file ) ) {
        unsigned char c;
        if ( fread( &c, 1, 1, file ) == 0 ) break;

        if ( n % 10 == 0 ) printf( "\n    " );
        n++;

        printf( "0x%.2X", (int) c );
        printf( ", " );
        i++;
    }

    fclose( file );

    return i;
}

const char * path_stem( const char * path )
{
    static char buffer[ 256 ];
    int i;

    for ( i = strlen( path ) - 1; i >= 0; i-- ) {
        if ( path[ i ] == '/' ) {
            break;
        }
    }
    i++;

    strncpy( buffer, path + i, 256 );
    return buffer;
}

int main( int argc, char ** argv )
{
    int file_count = argc - 1;

    int offset_list[ 128 ];
    int len_list[ 128 ];

    assert( file_count > 0 );

    printf( "unsigned char res_data[] = {" );

    int pos = 0;
    for ( int i = 0; i < file_count; i++ ) {
        int len = write_file( argv[ i + 1 ], i );
        len_list[ i ] = len;
        offset_list[ i ] = pos;
        pos += len;
    }

    printf( "\n};\n" );

    printf( "const char * res_data_name_list[] = {\n" );

    for ( int i = 0; i < file_count; i++ ) {
        printf( "    \"%s\",\n", path_stem( argv[ i + 1 ] ) );
    }

    printf( "};\n" );

    printf( "const int res_data_offset_list[] = {\n" );

    for ( int i = 0; i < file_count; i++ ) {
        printf( "    %d,\n", offset_list[ i ] );
    }

    printf( "};\n" );

    printf( "const int res_data_size_list[] = {\n" );

    for ( int i = 0; i < file_count; i++ ) {
        printf( "    %d,\n", len_list[ i ] );
    }

    printf( "};\n" );

    printf( "const int res_data_count = %d;\n", file_count );

    assert( argc >= 2 );
}
