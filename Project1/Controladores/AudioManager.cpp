#include "AudioManager.h"
#include <algorithm>

namespace {
    // Rutas de los archivos de música en el mismo orden que los índices devueltos por IndexOf.
    const char* const TRACK_FILES[3] = {
        "assets/audio/menu.wav",
        "assets/audio/mapa.wav",
        "assets/audio/combate.wav"
    };
}

AudioManager::~AudioManager() {
    // Libera los recursos de audio cuando se destruye el objeto AudioManager.
    Shutdown();
}

int AudioManager::IndexOf(MusicTrack track) {
    // Traduce cada tema a su índice. Devuelve -1 si no tiene música asociada.
    switch (track) {
    case MusicTrack::MENU_THEME:   return 0;
    case MusicTrack::MAP_THEME:    return 1;
    case MusicTrack::COMBAT_THEME: return 2;
    default:                       return -1;
    }
}

void AudioManager::Init() {
    // Evita inicializar el dispositivo de audio dos veces.
    if (ready) {
        return;
    }

    InitAudioDevice();
    ready = IsAudioDeviceReady();
    if (!ready) {
        return;
    }

    // Carga la música disponible y activa la reproducción en bucle para los archivos válidos.
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
    // Omite la limpieza si el dispositivo de audio está inactivo.
    if (!ready) {
        return;
    }

    // Detiene la música y libera los archivos de audio cargados antes de cerrar el dispositivo.
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
    // Evita reiniciar la música que ya está seleccionada.
    if (!ready || track == current) {
        return;
    }

    // Detiene la música anterior antes de cambiar de tema.
    const int previous = IndexOf(current);
    if (previous >= 0 && loaded[previous]) {
        StopMusicStream(tracks[previous]);
    }

    current = track;
    fade = 0.0f;

    // Inicia la nueva música en silencio para que Update aumente su volumen.
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

    // Solo actualiza la reproducción si la música seleccionada se cargó correctamente.
    const int index = IndexOf(current);
    if (index < 0 || !loaded[index]) {
        return;
    }

    // Aumenta el volumen hasta MASTER_VOLUME según el tiempo transcurrido por fotograma.
    fade = std::min(1.0f, fade + GetFrameTime() / FADE_SECONDS);
    SetMusicVolume(tracks[index], MASTER_VOLUME * fade);
    // Alimenta el flujo de audio de Raylib en cada fotograma.
    UpdateMusicStream(tracks[index]);
}
