#ifndef AUDIOENGINE_HPP
#define AUDIOENGINE_HPP

class AudioEngine {
    public:
        AudioEngine();
        ~AudioEngine();

        void playTrack(const char* path, bool repeat);
        void playOrPause();

        bool isTrackFinished();

        float getLengthInSeconds();
        float getCursorInSeconds();
};

#endif