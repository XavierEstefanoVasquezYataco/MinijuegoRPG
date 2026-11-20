#pragma once
#include "raylib.h"

enum class MusicTrack {
    NONE,
    MENU_THEME,
    MAP_THEME,
    COMBAT_THEME
};

class AudioManager {
public:
    AudioManager() = default;
    ~AudioManager();

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    void Init();
    void Shutdown();
    void Play(MusicTrack track);
    void Update();

private:
    static constexpr int TRACK_COUNT = 3;
    static constexpr float MASTER_VOLUME = 0.55f;
    static constexpr float FADE_SECONDS = 0.8f;

    Music tracks[TRACK_COUNT]{};
    bool loaded[TRACK_COUNT]{};
    MusicTrack current = MusicTrack::NONE;
    bool ready = false;
    float fade = 1.0f;

    static int IndexOf(MusicTrack track);
};
