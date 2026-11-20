#include "AudioManager.h"
#include <algorithm>

namespace {
    const char* const TRACK_FILES[3] = {
        "assets/audio/menu.wav",
        "assets/audio/mapa.wav",
        "assets/audio/combate.wav"
    };
}

AudioManager::~AudioManager() {
    Shutdown();
}

int AudioManager::IndexOf(MusicTrack track) {
    switch (track) {
    case MusicTrack::MENU_THEME:   return 0;
    case MusicTrack::MAP_THEME:    return 1;
    case MusicTrack::COMBAT_THEME: return 2;
    default:                       return -1;
    }
}

void AudioManager::Init() {
    if (ready) {
        return;
    }

    InitAudioDevice();
    ready = IsAudioDeviceReady();
    if (!ready) {
        return;
    }

    for (int i = 0; i < TRACK_COUNT; ++i) {
        loaded[i] = false;
        if (FileExists(TRACK_FILES[i])) {
            tracks[i] = LoadMusicStream(TRACK_FILES[i]);
            loaded[i] = tracks[i].frameCount > 0;
            if (loaded[i]) {
                tracks[i].looping = true;
            }
        }
    }
}

void AudioManager::Shutdown() {
    if (!ready) {
        return;
    }

    for (int i = 0; i < TRACK_COUNT; ++i) {
        if (loaded[i]) {
            StopMusicStream(tracks[i]);
            UnloadMusicStream(tracks[i]);
            loaded[i] = false;
        }
    }

    CloseAudioDevice();
    ready = false;
    current = MusicTrack::NONE;
}

void AudioManager::Play(MusicTrack track) {
    if (!ready || track == current) {
        return;
    }

    const int previous = IndexOf(current);
    if (previous >= 0 && loaded[previous]) {
        StopMusicStream(tracks[previous]);
    }

    current = track;
    fade = 0.0f;

    const int next = IndexOf(track);
    if (next >= 0 && loaded[next]) {
        SetMusicVolume(tracks[next], 0.0f);
        PlayMusicStream(tracks[next]);
    }
}

void AudioManager::Update() {
    if (!ready) {
        return;
    }

    const int index = IndexOf(current);
    if (index < 0 || !loaded[index]) {
        return;
    }

    fade = std::min(1.0f, fade + GetFrameTime() / FADE_SECONDS);
    SetMusicVolume(tracks[index], MASTER_VOLUME * fade);
    UpdateMusicStream(tracks[index]);
}
