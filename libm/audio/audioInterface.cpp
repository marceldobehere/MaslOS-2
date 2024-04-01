#include "audioInterface.h"
#include <libm/syscallManager.h>
#include <libm/stubs.h>

Audio::BasicAudioDestination* globalAudioDest = NULL;

void initAudioStuff()
{
    initAudioStuff(48000, 48000 / 5, 16, 2);
}

void initAudioStuff(Audio::AudioBuffer* buffer)
{
    initAudioStuff(buffer->sampleRate, buffer->sampleCount, buffer->bitsPerSample, buffer->channelCount);
}

void initAudioStuff(int sampleRate, int sampleCount, int bitsPerSample, int channelCount)
{

    globalAudioDest = new Audio::BasicAudioDestination(
        new Audio::AudioBuffer(bitsPerSample, sampleRate, channelCount, sampleCount)
    );

    // TODO: Send the Audio Destination (Data) to the kernel
    audioSetupBuffer(sampleRate, sampleCount, bitsPerSample, channelCount);
}


void DoAudioCheck()
{
    if (globalAudioDest == NULL)
        return;

    if (!audioDataNeeded())
        return;

    int samplesRec = globalAudioDest->RequestBuffers();
    if (samplesRec < 1)
        return;

    // Get the data
    void* bufferData = globalAudioDest->buffer->data;
    int sampleCount = globalAudioDest->buffer->sampleCount;
    int sampleRate = globalAudioDest->buffer->sampleRate;
    int bitsPerSample = globalAudioDest->buffer->bitsPerSample;

    // TODO: Send the audio data to the kernel
    audioSendData(bufferData, globalAudioDest->buffer->byteCount, samplesRec);

    // Clear the buffer
    globalAudioDest->buffer->ClearBuffer();
    globalAudioDest->buffer->sampleCount = globalAudioDest->buffer->totalSampleCount;

    // Free the messages
    while (true)
    {
        GenericMessagePacket* msg = msgGetConv(Audio::REQUEST_AUDIO_CONVO_ID);
        if (msg == NULL)
            break;

        msg->Free();
        _Free(msg);
    }
}