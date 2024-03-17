#include <libm/syscallManager.h>
#include <libm/syscallList.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>

#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/wmStuff/wmStuff.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/rnd/rnd.h>
#include <libm/stubs.h>

#include <libm/gui/guiInstance.h>
#include <libm/gui/guiStuff/components/box/boxComponent.h>
#include <libm/gui/guiStuff/components/rectangle/rectangleComponent.h>
#include <libm/gui/guiStuff/components/text/textComponent.h>
#include <libm/gui/guiStuff/components/button/buttonComponent.h>
#include <libm/gui/guiStuff/components/textField/textFieldComponent.h>

#include <libm/audio/internal/audio.h>

int globalAudioDestSampleRate = 44100;
int globalAudioDestSampleCount = globalAudioDestSampleRate / 4;
Audio::BasicAudioDestination* globalAudioDest = NULL;

void initAudioStuff()
{
    globalAudioDest = new Audio::BasicAudioDestination(
        Audio::AudioBuffer::Create16BitStereoBuffer(globalAudioDestSampleRate, globalAudioDestSampleCount)
    );

    // TODO: Send the Audio Destination (Data) to the kernel
    // ...
}

bool globalAudioDestNeedsAudio = true;
void DoAudioCheck()
{
    if (globalAudioDest == NULL)
        return;

    // TODO: Make the kernel send a message to the program when it needs audio and then catch it here
    // ...

    if (!globalAudioDestNeedsAudio)
        return;

    if (globalAudioDest->RequestBuffers() < 1)
        return;

    // Get the data
    void* bufferData = globalAudioDest->buffer->data;
    uint64_t sampleCount = globalAudioDest->buffer->sampleCount;
    uint64_t sampleRate = globalAudioDest->buffer->sampleRate;
    int bitsPerSample = globalAudioDest->buffer->bitsPerSample;

    // TODO: Send the audio data to the kernel
    // ...

    // Clear the buffer
    globalAudioDest->buffer->ClearBuffer();
    globalAudioDest->buffer->sampleCount = globalAudioDest->buffer->totalSampleCount;
    globalAudioDestNeedsAudio = false;
}

int main(int argc, char** argv)
{
    initAudioStuff();

    if (globalAudioDest == NULL)
        return 0;


    // Create our Audiosource
    int sampleRate = 44100;
    int sampleCount = sampleRate * 1;
    Audio::BasicAudioSource* audioSource = new Audio::BasicAudioSource(
        Audio::AudioBuffer::Create16BitMonoBuffer(sampleRate, sampleCount)
    );

    // Connect it to the Global Audio Destination
    audioSource->ConnectTo(globalAudioDest);

    // Fill the buffer with some audio
    uint16_t* arr = (uint16_t*)audioSource->buffer->data;
    int dif = 20;
    for (int i = 0; i < dif; i++)
        Audio::FillArray(arr, (i * sampleCount)/dif, sampleCount/dif, ((1000*(i+1)) / dif), sampleRate);
    
    // Set the sample count and reset the samples sent
    audioSource->buffer->sampleCount = sampleCount;
    audioSource->samplesSent = 0;

    // Set the ready to send flag
    audioSource->readyToSend = true;




    // Wait forever
    bool exit = false;
    while (!exit)
    {
        DoAudioCheck();
        programWaitMsg();
    }

    return 0;
}