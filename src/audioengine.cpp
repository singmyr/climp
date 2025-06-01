#include "audioengine.hpp"

#include "miniaudio/miniaudio.c"

static ma_engine* engine = nullptr;
static ma_device* device = nullptr;

static ma_sound* active_track = nullptr;

AudioEngine::AudioEngine() {
    if (engine == nullptr) {
        engine = new ma_engine;
        ma_result result;
        result = ma_engine_init(NULL, engine);
        if (result != MA_SUCCESS) {
            // todo how to handle this?
        }
    }

    if (device == nullptr) {
        device = ma_engine_get_device(engine);
    }
}

AudioEngine::~AudioEngine() {
    if (engine != nullptr) {
        ma_engine_uninit(engine);

        free(engine);
    }

    if (active_track != nullptr) {
        free(active_track);
    }
}

void AudioEngine::playOrPause() {
    if (ma_device_is_started(device)) {
        ma_device_stop(device);
    } else {
        ma_device_start(device);
    }
}


void AudioEngine::playTrack(const char* path, bool repeat) {
    ma_result result;

    if (active_track == nullptr) {
        active_track = new ma_sound;
    } else {
        ma_sound_uninit(active_track);
        free(active_track);
        active_track = new ma_sound;
    }

    // MA_SOUND_FLAG_STREAM
    result = ma_sound_init_from_file(engine, path, MA_SOUND_FLAG_DECODE, NULL, NULL, active_track);
    if (result != MA_SUCCESS) {
        // todo how to handle this?
        return;
    }

    if (repeat) {
        ma_sound_set_looping(active_track, MA_TRUE);
    } else {
        ma_sound_set_looping(active_track, MA_FALSE);
    }

    result = ma_sound_start(active_track);
    if (result != MA_SUCCESS) {
        // todo how to handle this?
        return;
    }
}

float AudioEngine::getLengthInSeconds() {
    float song_length = 0.0f;

    if (active_track != nullptr) {
        ma_sound_get_length_in_seconds(active_track, &song_length);
    }

    return song_length;
}

float AudioEngine::getCursorInSeconds() {
    float song_cursor = 0.0f;

    if (active_track != nullptr) {
        ma_sound_get_cursor_in_seconds(active_track, &song_cursor);
    }

    return song_cursor;
}

bool AudioEngine::isTrackFinished() {
    if (active_track != nullptr) {
        return ma_sound_at_end(active_track) == MA_TRUE;
    }

    return false;
}