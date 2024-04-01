#include "audioDevStuff.h"
#include "../interrupts/interrupts.h"
#include "../osData/MStack/MStackM.h"
#include "../rnd/rnd.h"
#include "../osData/osData.h"
#include "../devices/pit/pit.h"
#include "../memory/heap.h"
#include <libm/memStuff.h>
#include "../devices/serial/serial.h"
#include "../scheduler/scheduler.h"

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
            pcSpkData = (bool*)_Malloc(pcSpk->destination->buffer->totalSampleCount);   
            _memset(pcSpkData, 0, pcSpk->destination->buffer->totalSampleCount);
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

    void reqMoreData(Audio::BasicAudioDestination* dev)
    {
        if (dev->sources->GetCount() < 1)
            return;
        //Serial::TWritelnf("AUDIO> REQ MORE DATA");

        AddToStack();
        if (Scheduler::osTasks.IsLocked())
        {
            Serial::TWritelnf("AUDIO> REQ MORE DATA, BUT OSTASK LIST IS LOCKED!");
            RemoveFromStack();
            return;
        }
        AddToStack();
        Scheduler::osTasks.Lock();
        RemoveFromStack();

        List<osTask*>* tasks = Scheduler::osTasks.obj;

        List<Audio::BasicAudioSource*>* sources = dev->sources;
        for (int i1 = 0; i1 < sources->GetCount(); i1++)
        {
            Audio::BasicAudioSource* src = sources->ElementAt(i1);

            for (int i2 = 0; i2 < tasks->GetCount(); i2++)
            {
                osTask* task = tasks->ElementAt(i2);
                if (task == NULL || task->audioOutput != src)
                    continue;
                //Serial::Writelnf("  > FOUND SRC %d = %d", i1, i2);

                if (task->messages->GetCount() > 0)
                {
                    // if (task->messages->GetCount() > 5)
                    //     ;//Serial::TWritelnf("AUDIO> REQ MORE DATA, BUT TASK PID %X ALREADY HAS %d MESSAGES!", task->pid, task->messages->GetCount());
                    continue;
                }

                if (task->audioOutput->readyToSend)
                {
                    ;//Serial::TWritelnf("AUDIO> REQ MORE DATA, BUT TASK PID %X IS READY TO SEND (%d/%d)", task->pid, task->audioOutput->samplesSent, task->audioOutput->buffer->totalSampleCount);
                    continue;
                }

                //Serial::TWritelnf("AUDIO> REQ MORE DATA, SENDING MSG TO TASK PID %X", task->pid);
                
                AddToStack();
                GenericMessagePacket* msg = new GenericMessagePacket(0, MessagePacketType::AUDIO_REQUESTED);
                msg->ConvoID = Audio::REQUEST_AUDIO_CONVO_ID;

                Scheduler::osTasks.Unlock();
                SendMessageToTask(msg, task->pid, 1);
                Scheduler::osTasks.Lock();
                //Serial::TWritelnf("AUDIO  > REQ MORE DATA, SENT MSG TO TASK PID %X", task->pid);

                msg->Free();
                _Free(msg);
                RemoveFromStack();
            }
        }
        Scheduler::osTasks.Unlock();
        RemoveFromStack();
    }


    void play(int timeYes)
    {
        checkAC97Stuff();

        if (needMoreData && pcSpk != NULL)
        {
            if (pcSpk != NULL && pcSpkData == NULL)
            {
                pcSpkData = (bool*)_Malloc(pcSpk->destination->buffer->totalSampleCount);   
                _memset(pcSpkData, 0, pcSpk->destination->buffer->totalSampleCount);
            }

            currentRawAudioIndex = 0;
            pcSpk->destination->buffer->sampleCount = pcSpk->destination->buffer->totalSampleCount;
            int c = pcSpk->destination->RequestBuffers();
            pcSpk->destination->buffer->sampleCount = c;
            if (c > 0)
            {
                //Audio::BasicAudioSource* testSrc = pcSpk->destination->sources->ElementAt(0);
                //Serial::TWritelnf("AUDIO> REQUEST BUFFER SUCCESS! (%d / %d) (%d / %d)", c, pcSpk->destination->buffer->totalSampleCount, testSrc->samplesSent, testSrc->buffer->totalSampleCount);
                needMoreData = false;

                // TODO: TEST THE PC SPEAKER
                // Create a bool buffer with the data already converted and
                // if there are non-zero values shorten them by the volume
                uint8_t* data = (uint8_t*)pcSpk->destination->buffer->data;
                c = pcSpk->destination->buffer->totalSampleCount;
                int vol = pcSpk->destination->buffer->volume;
                int cutOff = pcSpk->destination->buffer->sampleRate / 7000;

                _memset(pcSpkData, 0, pcSpk->destination->buffer->totalSampleCount);
                // for (int i = 0; i < c; i++)
                //     pcSpkData[i] = (((int)data[i])*vol) >= 12700;
                
                for (int i1 = 0; i1 < c; i1++)
                {
                    if ((((int)data[i1])*vol) < 12700)
                        continue;

                    // Find the length of the beep and the average volume
                    int tC = 0;
                    long tS = 0;
                    for (int i2 = i1; i2 < c && (((int)data[i2])*vol) >= 12700; i2++)
                        tC++, tS += (int)data[i2];//(((int)data[i2])*vol) / 100;
                    tS = (tS * vol) / 100;
                    // Shorten the beep depending on the volume
                    int tV = tS / tC;
                    int aC = tC * tV / 255;
                    if (aC < cutOff)
                        aC = 0;
                    for (int i2 = i1; i2 < i1 + aC; i2++)
                        pcSpkData[i2] = true;
                    i1 += tC;
                }
            }
            else
            {
                //reqMoreData(pcSpk->destination);
            }
            if (pcSpk->destination->sources->GetCount() > 0)
                reqMoreData(pcSpk->destination);

            // else
            //     if (pcSpk->destination->sources->GetCount() > 0)
            //         Serial::TWritelnf("AUDIO> FAILED TO REQUEST BUFFER!");
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