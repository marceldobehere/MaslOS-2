#include <libm/syscallManager.h>
#include <libm/audio/audioInterface.h>

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


    // Wait till audio is done playing
    while (audioSource->readyToSend)
    {
        DoAudioCheck();

        programWaitMsg();
    }

    programWait(1000);
    return 0;
}