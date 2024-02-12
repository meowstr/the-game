#include "logging.hpp"

#include <AL/al.h>

#include <stdint.h>
#include <string.h>

struct wav_header_t {
    uint8_t chunk_id[ 4 ];
    uint8_t chunk_size[ 4 ];
    uint8_t format[ 4 ];
    uint8_t subchunk1_id[ 4 ];
    uint8_t subchunk1_size[ 4 ];
    uint8_t audio_format[ 2 ];
    uint8_t num_channels[ 2 ];
    uint8_t sample_rate[ 4 ];
    uint8_t byte_rate[ 4 ];
    uint8_t block_align[ 2 ];
    uint8_t bits_per_sample[ 2 ];
    uint8_t subchunk2_id[ 4 ];
    uint8_t subchunk2_size[ 4 ];
};

static int to_int( uint8_t ( &d )[ 4 ] )
{
    int x;
    memcpy( &x, d, 4 );
    return x;
}

static short to_short( uint8_t ( &d )[ 2 ] )
{
    short x;
    memcpy( &x, d, 2 );
    return x;
}

static const char * to_string( uint8_t ( &d )[ 4 ] )
{
    static char buffer[ 5 ];
    buffer[ 4 ] = '\0';
    memcpy( buffer, d, 4 );
    return buffer;
}

static void print_header( wav_header_t header )
{
    INFO_LOG( "audio description" );
    INFO_LOG( "  chunk id:        %s", to_string( header.chunk_id ) );
    INFO_LOG( "  chunk size:      %d", to_int( header.chunk_size ) );
    INFO_LOG( "  format:          %s", to_string( header.format ) );
    INFO_LOG( "  subchunk 1 id:   %s", to_string( header.subchunk1_id ) );
    INFO_LOG( "  subchunk 1 size: %d", to_int( header.subchunk1_size ) );
    INFO_LOG( "  audio format:    %d", to_short( header.audio_format ) );
    INFO_LOG( "  num channels:    %d", to_short( header.num_channels ) );
    INFO_LOG( "  sample rate:     %d", to_int( header.sample_rate ) );
    INFO_LOG( "  byte rate:       %d", to_int( header.byte_rate ) );
    INFO_LOG( "  block align:     %d", to_short( header.block_align ) );
    INFO_LOG( "  bits per sample: %d", to_short( header.bits_per_sample ) );
    INFO_LOG( "  subchunk 2 id:   %s", to_string( header.subchunk2_id ) );
    INFO_LOG( "  subchunk 2 size: %d", to_int( header.subchunk2_size ) );
}

int load_wav( unsigned char * data, int data_len )
{
    wav_header_t header;
    memcpy( &header, data, sizeof( wav_header_t ) );

    print_header( header );

    int waveform_size = to_int( header.subchunk2_size );
    uint8_t * waveform_data = new uint8_t[ waveform_size ];

    memcpy( waveform_data, data + sizeof( wav_header_t ), waveform_size );

    bool stereo = ( to_short( header.num_channels ) > 1 );
    short bits_per_sample = to_short( header.bits_per_sample );

    ALenum format;

    if ( stereo && bits_per_sample == 8 ) {
        format = AL_FORMAT_STEREO8;
    }
    if ( stereo && bits_per_sample == 16 ) {
        format = AL_FORMAT_STEREO16;
    }
    if ( !stereo && bits_per_sample == 8 ) {
        format = AL_FORMAT_MONO8;
    }
    if ( !stereo && bits_per_sample == 16 ) {
        format = AL_FORMAT_MONO16;
    }

    unsigned int buffer;

    alGenBuffers( 1, &buffer );

    alBufferData(
        buffer,
        format,
        waveform_data,
        waveform_size,
        to_int( header.sample_rate )
    );

    return (int) buffer;
}
