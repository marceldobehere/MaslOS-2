#include "audioDevStuff.h"
#include "../interrupts/interrupts.h"
#include "../osData/MStack/MStackM.h"
#include "../rnd/rnd.h"
#include "../osData/osData.h"
#include "../devices/pit/pit.h"
#include "../memory/heap.h"
#include <libm/memStuff.h>

namespace AudioDeviceStuff
{
    Audio::AudioOutputDevice* pcSpk = NULL;
    bool* pcSpkData = NULL;
    int currentRawAudioIndex = 0;
    int rawAudioDiv = 40;//60;
    bool needMoreData = true;
    bool currentState = false;

    
    void init()
    {
        currentRawAudioIndex = 0;
        needMoreData = true;
        currentState = false;
        if (pcSpk != NULL && pcSpkData == NULL)
        {
            pcSpkData = (bool*)_Malloc(pcSpk->destination->buffer->sampleCount);   
            _memset(pcSpkData, 0, pcSpk->destination->buffer->sampleCount);
        }
    }


    void resetTest()
    {
        osData.ac97Driver->needManualRestart = osData.ac97Driver->CheckMusic();
    }

    void checkAC97Stuff()
    {
        AddToStack();

        if (osData.ac97Driver != NULL)
        {
            int64_t currTime = (int64_t)PIT::TimeSinceBootMS();
            int64_t ac97Time = ((int64_t)(osData.ac97Driver->lastCheckTime + 500));
            if (currTime > ac97Time)
            {
                osData.ac97Driver->DoQuickCheck();
                osData.ac97Driver->HandleIRQ(NULL);
                //resetTest();
                
            }
            else if (osData.ac97Driver->doCheck)
            {
                osData.ac97Driver->doCheck = false;
                resetTest();
            }
            else if (osData.ac97Driver->needManualRestart)
            {
                resetTest();
            }
        }
        RemoveFromStack();
    }


    void play(int timeYes)
    {
        checkAC97Stuff();

        if (needMoreData && pcSpk != NULL)
        {
            if (pcSpk != NULL && pcSpkData == NULL)
            {
                pcSpkData = (bool*)_Malloc(pcSpk->destination->buffer->sampleCount);   
                _memset(pcSpkData, 0, pcSpk->destination->buffer->sampleCount);
            }

            currentRawAudioIndex = 0;
            int c = pcSpk->destination->RequestBuffers();
            if (c > 0)
            {
                needMoreData = false;

                // TODO: TEST THE PC SPEAKER
                // Create a bool buffer with the data already converted and
                // if there are non-zero values shorten them by the volume
                uint8_t* data = (uint8_t*)pcSpk->destination->buffer->data;
                c = pcSpk->destination->buffer->sampleCount;
                int vol = pcSpk->destination->buffer->volume;

                _memset(pcSpkData, 0, pcSpk->destination->buffer->sampleCount);
                for (int i1 = 0; i1 < c; i1++)
                {
                    if ((((int)data[i1])*vol) < 12700)
                        continue;

                    // Find the length of the beep and the average volume
                    int tC = 0;
                    long tS = 0;
                    for (int i2 = i1; i2 < c && (((int)data[i2])*vol) >= 12700; i2++)
                        tC++, tS += (((int)data[i2])*vol) / 100;

                    // Shorten the beep depending on the volume
                    int tV = tS / tC;
                    int aC = tC * tV / 255;
                    for (int i2 = i1; i2 < i1 + tC; i2++)
                        pcSpkData[i2] = true;
                }
            }
        }

        if (needMoreData)
        {
            currentRawAudioIndex = 0;
            if (PIT::Divisor != PIT::NonMusicDiv)
                PIT::SetDivisor(PIT::NonMusicDiv);

            if (currentState)
			{
				currentState = false;
				SetSpeakerPosition(false);
			}
            return;
        }
        else
        {
            if (PIT::Divisor != rawAudioDiv)
                PIT::SetDivisor(rawAudioDiv);
        }

        AddToStack();
        {
            int indx = currentRawAudioIndex;

            //uint8_t val = ((uint8_t*)pcSpk->destination->buffer->data)[indx];
            //bool beep = ((((int)val)*pcSpk->destination->buffer->volume)/100) >= 127;
            bool beep = pcSpkData[indx];
			if (beep != currentState)
			{
				currentState = beep;
				SetSpeakerPosition(currentState);
			}

            currentRawAudioIndex++;
            if (currentRawAudioIndex >= pcSpk->destination->buffer->sampleCount)
            {
                needMoreData = true;
                currentRawAudioIndex = 0;
                pcSpk->destination->buffer->ClearBuffer();
                pcSpk->destination->buffer->sampleCount = pcSpk->destination->buffer->totalSampleCount;
            }
        }

        RemoveFromStack();
    }
}