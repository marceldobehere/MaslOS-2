#pragma once
#include <libm/audio/internal/audio.h>

extern Audio::BasicAudioDestination* globalAudioDest;

void initAudioStuff();
void initAudioStuff(int sampleRate, int sampleCount, int bitsPerSample, int channelCount);
void initAudioStuff(Audio::AudioBuffer* buffer);

void DoAudioCheck();