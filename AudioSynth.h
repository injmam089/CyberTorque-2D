#pragma once
#ifndef AUDIO_SYNTH_H
#define AUDIO_SYNTH_H

#include <windows.h>
#include <mmsystem.h>
#include <cmath>
#include <vector>

class AudioSynth {
public:
    static AudioSynth& getInstance();

    bool init();
    void shutdown();

    // Sound control methods
    void setEngineRPM(float rpmPercent, bool isAccelerating);
    void setNitroActive(bool active);
    void setDriftScreech(float intensity); // 0.0 to 1.0
    void triggerCrash();
    void triggerCoin();
    void triggerCheckpoint();
    void triggerNitroPickup();
    void triggerPoliceSiren(bool active);
    void toggleMute();
    bool isMuted() const { return m_muted; }
    void setMusicEnabled(bool enabled) { m_musicEnabled = enabled; }
    bool isMusicEnabled() const { return m_musicEnabled; }

private:
    AudioSynth();
    ~AudioSynth();

    static DWORD WINAPI staticAudioThread(LPVOID param);
    void audioThreadFunc();
    void generateAudioBuffer(short* buffer, int numSamples);

    HWAVEOUT m_hWaveOut = nullptr;
    static const int NUM_BUFFERS = 3;
    static const int SAMPLES_PER_BUFFER = 2048;
    static const int SAMPLE_RATE = 44100;
    
    WAVEHDR m_waveHeaders[NUM_BUFFERS];
    short m_buffers[NUM_BUFFERS][SAMPLES_PER_BUFFER];

    volatile bool m_running = false;
    HANDLE m_hThread = nullptr;

    // Sound state parameters
    volatile bool m_muted = false;
    volatile bool m_musicEnabled = true;
    volatile float m_engineRpm = 0.0f;
    volatile bool m_engineAccel = false;
    volatile bool m_nitroActive = false;
    volatile float m_driftIntensity = 0.0f;
    volatile bool m_policeSiren = false;

    // Triggered one-shot timers/phases
    volatile float m_crashTimer = 0.0f;
    volatile float m_coinTimer = 0.0f;
    volatile float m_checkpointTimer = 0.0f;

    // Synthesis phase variables (only modified in audio thread)
    float m_enginePhase = 0.0f;
    float m_enginePhase2 = 0.0f;
    float m_screechPhase = 0.0f;
    float m_sirenPhase = 0.0f;
    float m_sirenLfo = 0.0f;
    float m_coinPhase = 0.0f;
    float m_checkpointPhase = 0.0f;
    float m_crashDecay = 0.0f;

    // Music sequencer state
    float m_musicTime = 0.0f;
    int m_musicStep = 0;
    float m_leadPhase = 0.0f;
    float m_bassPhase = 0.0f;

    float fastRand() {
        static unsigned int seed = 123456789;
        seed = (1103515245 * seed + 12345) & 0x7fffffff;
        return ((float)seed / (float)0x7fffffff) * 2.0f - 1.0f;
    }
};

#endif // AUDIO_SYNTH_H
