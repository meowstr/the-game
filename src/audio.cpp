#include "audio.hpp"

#include "logging.hpp"

#include <AL/alc.h>

struct {
    ALCdevice * device;
    ALCcontext * context;
} intern;

int audio_init()
{
    intern.device = alcOpenDevice( nullptr );
    if ( !intern.device ) {
        ERROR_LOG( "failed to open audio device" );
        return 1;
    }

    intern.context = alcCreateContext( intern.device, nullptr );
    if ( !intern.context ) {
        ERROR_LOG( "failed to create audio context" );
        return 1;
    }

    ALCboolean is_current = alcMakeContextCurrent( intern.context );
    if ( is_current != ALC_TRUE ) {
        ERROR_LOG( "failed to make audio context current" );
        return 1;
    }

    return 0;
}

void audio_destroy()
{
    alcMakeContextCurrent( nullptr );
    alcDestroyContext( intern.context );
    alcCloseDevice( intern.device );
}
