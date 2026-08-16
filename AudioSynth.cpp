#include "AudioSynth.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

AudioSynth& AudioSynth::getInstance() {
    static AudioSynth instance;
    return instance;
}

AudioSynth::AudioSynth() {
}

AudioSynth::~AudioSynth() {
    shutdown();
}

DWORD WINAPI AudioSynth::staticAudioThread(LPVOID param) {
    AudioSynth* synth = (AudioSynth*)param;
    if (synth) {
        synth->audioThreadFunc();
    }
    return 0;
}

bool AudioSynth::init() {
    if (m_running) return true;

    WAVEFORMATEX wfx;
    ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1; // Mono 44.1kHz is lightweight and crisp
    wfx.nSamplesPerSec = SAMPLE_RATE;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    MMRESULT res = waveOutOpen(&m_hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    if (res != MMSYSERR_NOERROR) {
        m_hWaveOut = nullptr;
        return false;
    }

    for (int i = 0; i < NUM_BUFFERS; ++i) {
        ZeroMemory(&m_waveHeaders[i], sizeof(WAVEHDR));
        m_waveHeaders[i].lpData = (LPSTR)m_buffers[i];
        m_waveHeaders[i].dwBufferLength = SAMPLES_PER_BUFFER * sizeof(short);
        m_waveHeaders[i].dwFlags = 0;
        waveOutPrepareHeader(m_hWaveOut, &m_waveHeaders[i], sizeof(WAVEHDR));
    }

    m_running = true;
    m_hThread = CreateThread(NULL, 0, staticAudioThread, this, 0, NULL);
    return true;
}

void AudioSynth::shutdown() {
    if (!m_running) return;
    m_running = false;

    if (m_hThread) {
        WaitForSingleObject(m_hThread, 500);
        CloseHandle(m_hThread);
        m_hThread = nullptr;
    }

    if (m_hWaveOut) {
        waveOutReset(m_hWaveOut);
        for (int i = 0; i < NUM_BUFFERS; ++i) {
            waveOutUnprepareHeader(m_hWaveOut, &m_waveHeaders[i], sizeof(WAVEHDR));
        }
        waveOutClose(m_hWaveOut);
        m_hWaveOut = nullptr;
    }
}

void AudioSynth::setEngineRPM(float rpmPercent, bool isAccelerating) {
    if (rpmPercent < 0.0f) rpmPercent = 0.0f;
    if (rpmPercent > 1.2f) rpmPercent = 1.2f;
    m_engineRpm = rpmPercent;
    m_engineAccel = isAccelerating;
}

void AudioSynth::setNitroActive(bool active) {
    m_nitroActive = active;
}

void AudioSynth::setDriftScreech(float intensity) {
    if (intensity < 0.0f) intensity = 0.0f;
    if (intensity > 1.0f) intensity = 1.0f;
    m_driftIntensity = intensity;
}

void AudioSynth::triggerCrash() {
    m_crashTimer = 1.0f; // 1 second crash noise burst
}

void AudioSynth::triggerCoin() {
    m_coinTimer = 0.25f;
    m_coinPhase = 0.0f;
}

void AudioSynth::triggerCheckpoint() {
    m_checkpointTimer = 0.6f;
    m_checkpointPhase = 0.0f;
}

void AudioSynth::triggerNitroPickup() {
    m_coinTimer = 0.35f;
    m_coinPhase = 0.0f;
}

void AudioSynth::triggerPoliceSiren(bool active) {
    m_policeSiren = active;
}

void AudioSynth::toggleMute() {
    m_muted = !m_muted;
}

void AudioSynth::audioThreadFunc() {
    int curBuf = 0;

    // Prime the buffers
    for (int i = 0; i < NUM_BUFFERS; ++i) {
        generateAudioBuffer(m_buffers[i], SAMPLES_PER_BUFFER);
        m_waveHeaders[i].dwFlags &= ~WHDR_DONE;
        waveOutWrite(m_hWaveOut, &m_waveHeaders[i], sizeof(WAVEHDR));
    }

    while (m_running) {
        // Wait until current buffer is done playing
        while (m_running && !(m_waveHeaders[curBuf].dwFlags & WHDR_DONE)) {
            Sleep(5);
        }

        if (!m_running) break;

        generateAudioBuffer(m_buffers[curBuf], SAMPLES_PER_BUFFER);
        m_waveHeaders[curBuf].dwFlags &= ~WHDR_DONE;
        waveOutWrite(m_hWaveOut, &m_waveHeaders[curBuf], sizeof(WAVEHDR));

        curBuf = (curBuf + 1) % NUM_BUFFERS;
    }
}

void AudioSynth::generateAudioBuffer(short* buffer, int numSamples) {
    if (m_muted) {
        ZeroMemory(buffer, numSamples * sizeof(short));
        return;
    }

    const float dt = 1.0f / (float)SAMPLE_RATE;
    float currentRpm = m_engineRpm;
    bool accel = m_engineAccel;
    bool nitro = m_nitroActive;
    float drift = m_driftIntensity;
    bool siren = m_policeSiren;
    bool music = m_musicEnabled;

    // Synthwave notes (Frequencies in Hz)
    const float chordBass[4] = { 110.0f, 87.3f, 65.4f, 98.0f };
    const float arpNotes[4][4] = {
        { 220.0f, 261.6f, 329.6f, 440.0f }, // A minor
        { 174.6f, 220.0f, 261.6f, 349.2f }, // F major
        { 130.8f, 164.8f, 196.0f, 261.6f }, // C major
        { 196.0f, 246.9f, 293.7f, 392.0f }  // G major
    };

    const float tempo = 132.0f; // 132 BPM
    const float beatDuration = 60.0f / tempo; // ~0.4545s per beat
    const float sixteenthDuration = beatDuration / 4.0f; // ~0.1136s

    for (int i = 0; i < numSamples; ++i) {
        float sample = 0.0f;

        // 1. ENGINE SOUND GENERATOR
        float baseEngineFreq = 45.0f + currentRpm * 190.0f;
        if (nitro) baseEngineFreq *= 1.35f;

        m_enginePhase += baseEngineFreq * dt;
        if (m_enginePhase >= 1.0f) m_enginePhase -= 1.0f;

        m_enginePhase2 += (baseEngineFreq * 2.01f) * dt;
        if (m_enginePhase2 >= 1.0f) m_enginePhase2 -= 1.0f;

        float saw = (m_enginePhase * 2.0f - 1.0f);
        float pulse = (m_enginePhase2 > 0.4f ? 0.7f : -0.7f);
        float engineWave = (saw * 0.6f + pulse * 0.4f);

        float subPhase = std::sin(m_enginePhase * (float)M_PI);
        engineWave += subPhase * 0.3f;

        float engineVol = 0.18f + (accel ? 0.12f : 0.04f) + (currentRpm * 0.12f);
        sample += engineWave * engineVol;

        // 2. NITRO WHOOSH & EXHAUST ROAR
        if (nitro) {
            float noise = fastRand();
            float nitroSine = std::sin(m_enginePhase * 12.0f * (float)M_PI);
            sample += (noise * 0.22f + nitroSine * 0.12f);
        }

        // 3. TIRE DRIFT SCREECH
        if (drift > 0.05f) {
            m_screechPhase += 1150.0f * dt;
            if (m_screechPhase >= 1.0f) m_screechPhase -= 1.0f;
            float screechNoise = fastRand();
            float screechTone = std::sin(m_screechPhase * 6.28318f);
            sample += (screechTone * 0.18f + screechNoise * 0.25f) * drift;
        }

        // 4. POLICE SIREN
        if (siren) {
            m_sirenLfo += 1.8f * dt; // wail cycle
            if (m_sirenLfo >= 1.0f) m_sirenLfo -= 1.0f;
            float sirenFreq = 700.0f + std::sin(m_sirenLfo * 6.28318f) * 350.0f;
            m_sirenPhase += sirenFreq * dt;
            if (m_sirenPhase >= 1.0f) m_sirenPhase -= 1.0f;
            float sirenTone = std::sin(m_sirenPhase * 6.28318f);
            sample += sirenTone * 0.16f;
        }

        // 5. CRASH SOUND EFFECT
        float cTimer = m_crashTimer;
        if (cTimer > 0.0f) {
            float noise = fastRand();
            float bassImpact = std::sin(cTimer * 80.0f * 6.28318f);
            sample += (noise * 0.55f + bassImpact * 0.4f) * cTimer;
            cTimer -= dt;
            m_crashTimer = (std::max)(0.0f, cTimer);
        }

        // 6. COIN / PICKUP CHIME
        float cnTimer = m_coinTimer;
        if (cnTimer > 0.0f) {
            float coinFreq = (cnTimer > 0.15f) ? 880.0f : 1760.0f;
            m_coinPhase += coinFreq * dt;
            if (m_coinPhase >= 1.0f) m_coinPhase -= 1.0f;
            float coinTone = std::sin(m_coinPhase * 6.28318f);
            sample += coinTone * 0.32f * (cnTimer / 0.35f);
            cnTimer -= dt;
            m_coinTimer = (std::max)(0.0f, cnTimer);
        }

        // 7. CHECKPOINT FANFARE
        float cpTimer = m_checkpointTimer;
        if (cpTimer > 0.0f) {
            float cpFreq = 523.25f; // C5
            if (cpTimer < 0.4f) cpFreq = 659.25f; // E5
            if (cpTimer < 0.2f) cpFreq = 783.99f; // G5
            m_checkpointPhase += cpFreq * dt;
            if (m_checkpointPhase >= 1.0f) m_checkpointPhase -= 1.0f;
            float cpTone = std::sin(m_checkpointPhase * 6.28318f);
            sample += cpTone * 0.35f * (cpTimer / 0.6f);
            cpTimer -= dt;
            m_checkpointTimer = (std::max)(0.0f, cpTimer);
        }

        // 8. SYNTHWAVE PROCEDURAL BACKGROUND MUSIC
        if (music) {
            m_musicTime += dt;
            int totalSixteenths = (int)(m_musicTime / sixteenthDuration);
            int bar = (totalSixteenths / 16) % 4;
            int stepInBar = totalSixteenths % 16;
            int beatInBar = (totalSixteenths / 4) % 4;
            float stepFrac = std::fmod(m_musicTime, sixteenthDuration) / sixteenthDuration;

            float curBassFreq = chordBass[bar];
            if (stepInBar % 2 == 1) curBassFreq *= 2.0f;
            m_bassPhase += curBassFreq * dt;
            if (m_bassPhase >= 1.0f) m_bassPhase -= 1.0f;
            float bassSaw = (m_bassPhase * 2.0f - 1.0f);
            float bassEnv = (1.0f - stepFrac * 0.7f);
            float bassSample = bassSaw * bassEnv * 0.12f;

            int noteIdx = stepInBar % 4;
            float leadFreq = arpNotes[bar][noteIdx];
            m_leadPhase += leadFreq * dt;
            if (m_leadPhase >= 1.0f) m_leadPhase -= 1.0f;
            float leadPulse = (m_leadPhase > 0.5f ? 0.6f : -0.6f);
            float leadEnv = (1.0f - stepFrac * 0.5f);
            float leadSample = leadPulse * leadEnv * 0.08f;

            float beatFrac = std::fmod(m_musicTime, beatDuration) / beatDuration;
            float kickSample = 0.0f;
            if (beatFrac < 0.15f) {
                float kickPhase = beatFrac * 400.0f;
                float kickFreq = 120.0f * (1.0f - beatFrac / 0.15f) + 40.0f;
                kickSample = std::sin(kickPhase * kickFreq * dt * 100.0f) * (1.0f - beatFrac / 0.15f) * 0.28f;
            }

            float snareSample = 0.0f;
            if (beatInBar == 1 || beatInBar == 3) {
                if (beatFrac < 0.2f) {
                    float sNoise = fastRand();
                    snareSample = sNoise * (1.0f - beatFrac / 0.2f) * 0.16f;
                }
            }

            float hihatSample = 0.0f;
            if (stepInBar % 2 == 1 && stepFrac < 0.08f) {
                hihatSample = fastRand() * (1.0f - stepFrac / 0.08f) * 0.06f;
            }

            sample += (bassSample + leadSample + kickSample + snareSample + hihatSample);
        }

        if (sample > 0.95f) sample = 0.95f + std::tanh(sample - 0.95f) * 0.05f;
        else if (sample < -0.95f) sample = -0.95f + std::tanh(sample + 0.95f) * 0.05f;

        buffer[i] = (short)(sample * 32767.0f);
    }
}
