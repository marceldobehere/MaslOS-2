#include "main.h"

Window* window;
GuiInstance* guiInstance;
GuiComponentStuff::ButtonComponent* openBtn;
GuiComponentStuff::ButtonComponent* playBtn;
GuiComponentStuff::TextComponent* pathText;
GuiComponentStuff::TextComponent* timeText;


Audio::AudioBuffer* musicBuffer;
Audio::BasicAudioSource* musicSource;
char* musicFileData;
int musicFileLen;
const int waveDataOffset = 44;
int musicFilePos;
bool paused;
const char* musicPath;

using namespace STDIO;

int main(int argc, char** argv)
{
    initWindowManagerStuff();
    initAudioStuff(48000, 48000 / 2, 16, 2);
    initStdio(false);
    
    window = requestWindow();
    if (window == NULL)
        return 0;

    const char* path = "";
    if (argc > 0)
        path = argv[0];
    musicPath = StrCopy(path);

    _Free(window->Title);
    window->Title = StrCopy("Music Player");
    window->Dimensions.width = 240;
    window->Dimensions.height = 60;
    window->Resizeable = false;
    setWindow(window);

    guiInstance = new GuiInstance(window);
    guiInstance->Init();
    guiInstance->screen->backgroundColor = Colors.white;


    // Open Button
    guiInstance->CreateComponentWithId(1011, GuiComponentStuff::ComponentType::BUTTON);
    openBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1011);
    openBtn->position.x = 0;
    openBtn->position.y = 0;
    _Free(openBtn->textComp->text);
    openBtn->textComp->text = StrCopy("Open");
    openBtn->size.FixedX = 50;
    openBtn->size.FixedY = 20;
    openBtn->OnMouseClickHelp = (void*)NULL;
    openBtn->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnOpenClick;


    // Play Button
    guiInstance->CreateComponentWithId(1012, GuiComponentStuff::ComponentType::BUTTON);
    playBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1012);
    playBtn->position.x = 50;
    playBtn->position.y = 0;
    _Free(playBtn->textComp->text);
    playBtn->textComp->text = StrCopy("Play");
    playBtn->size.FixedX = 50;
    playBtn->size.FixedY = 20;
    playBtn->OnMouseClickHelp = (void*)NULL;
    playBtn->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnPlayClick;

    // Time text
    guiInstance->CreateComponentWithId(1013, GuiComponentStuff::ComponentType::TEXT);
    timeText = (GuiComponentStuff::TextComponent*)guiInstance->GetComponentFromId(1013);
    timeText->position.x = 0;
    timeText->position.y = 20;
    _Free(timeText->text);
    timeText->text = StrCopy("00:00/00:00");
    timeText->size.FixedX = 100;
    timeText->size.FixedY = 20;

    // Path Text
    guiInstance->CreateComponentWithId(1014, GuiComponentStuff::ComponentType::TEXT);
    pathText = (GuiComponentStuff::TextComponent*)guiInstance->GetComponentFromId(1014);
    pathText->position.x = 0;
    pathText->position.y = 40;
    _Free(pathText->text);
    pathText->text = StrCopy(path);
    pathText->size.FixedX = 100;
    pathText->size.FixedY = 20;



    // Init other stuff
    musicSource = NULL;
    musicBuffer = NULL;
    musicFileData = NULL;
    musicFileLen = 0;
    musicFilePos = 0;
    paused = true;
    println("> Init complete");

    LoadFile(path);

    println("> Entering Main Loop");
    MainLoop();
    return 0;
}

void MainLoop()
{
    while (!CheckForWindowClosed(window))
    {
        DoAudioCheck();

        guiInstance->Update();
        CheckMusic();
        UpdateText();

        DoAudioCheck();
        guiInstance->Render(false);

        DoAudioCheck();
        programWaitMsg();
    }
}

void UpdateText()
{
    if (musicSource != NULL && musicFileData != NULL)
    {
        musicFilePos = musicSource->samplesSent * (musicSource->buffer->bitsPerSample / 8) * musicSource->buffer->channelCount;
        int64_t byteDivisor = (musicSource->buffer->bitsPerSample / 8) * musicSource->buffer->channelCount * musicSource->buffer->sampleRate;
        int64_t musicFilePosSec = musicFilePos / byteDivisor;
        int64_t musicFileLenSec = musicFileLen / byteDivisor;

        int startSec = musicFilePosSec % 60;
        int startMin = (musicFilePosSec / 60); // % 60;

        int endSec = musicFileLenSec % 60;
        int endMin = (musicFileLenSec / 60); // % 60;

        const char* toFree = NULL;
        _Free(timeText->text);
        timeText->text = StrCopy("");

        toFree = StrPadLeft(to_string(startMin), '0', 2, false);
        timeText->text = StrAppend(timeText->text, toFree, true);
        _Free(toFree);
        timeText->text = StrAppend(timeText->text, ":", true);
        toFree = StrPadLeft(to_string(startSec), '0', 2, false);
        timeText->text = StrAppend(timeText->text, toFree, true);
        _Free(toFree);
        
        timeText->text = StrAppend(timeText->text, " / ", true);
        
        toFree = StrPadLeft(to_string(endMin), '0', 2, false);
        timeText->text = StrAppend(timeText->text, toFree, true);
        _Free(toFree);
        timeText->text = StrAppend(timeText->text, ":", true);
        toFree = StrPadLeft(to_string(endSec), '0', 2, false);
        timeText->text = StrAppend(timeText->text, toFree, true);
        _Free(toFree);
    }

    int tSize = StrLen(pathText->text) * 8;
    tSize = max(240, tSize + 8);
    if (window->Dimensions.width != tSize)
    {
        window->Dimensions.width = tSize;
        setWindow(window);
    }
}

void CheckMusic()
{
    if (musicSource == NULL)
        return;

    if (musicBuffer->totalSampleCount == 0)
    {
        if (!paused)
        {
            paused = true;
            _Free(playBtn->textComp->text);
            playBtn->textComp->text = StrCopy("Play");
        }
        return;
    }
    
    if (musicBuffer->sampleCount == 0)
    {
        musicSource->samplesSent = 0;
        musicBuffer->sampleCount = musicBuffer->totalSampleCount;

        paused = true;
        _Free(playBtn->textComp->text);
        playBtn->textComp->text = StrCopy("Play");
        UpdateText();
    }
}

void LoadFile(const char* path)
{
    if (path == NULL)
        return;

    printlnf("> Attempting to Load \"%s\"", path);
    if (StrEquals(path, ""))
        return;
    
    if (!fsFileExists(path))
        return;

    _Free(musicPath);
    musicPath = StrCopy(path);
    _Free(pathText->text);
    pathText->text = StrCopy(path);
    paused = true;
    musicFilePos = 0;
    musicFileData = NULL;
    musicFileLen = 0;
    _Free(timeText->text);
    timeText->text = StrCopy("00:00 / 00:00");

    char* fData = NULL;
    uint64_t fDataLen = 0;
    

    if (fsReadFile(path, (void**)(&fData), &fDataLen))
    {
        printlnf("> Loaded file data successfully (%d bytes)", fDataLen);
        LoadWavData(fData, fDataLen);
    }
    else
    {
        println("> Failed to load file data");
    }
}

bool LoadWavData(char* data, int64_t dataLen)
{
    println("> Loading WAV Data");
    paused = true;
    _Free(playBtn->textComp->text);
    playBtn->textComp->text = StrCopy("Play");

    if (musicSource != NULL)
    {
        println("> Destroying old music source");
        musicSource->Destroy();
        musicSource = NULL;
    }

    if (musicFileData != NULL)
    {
        println("> Freeing old music file data");
        _Free(musicFileData);
        musicFileData = NULL;
    }

    println("> Setting WAV Data");
    musicFileData = data;
    if (musicFileData == NULL)
    {
        println("> Failed to set WAV Data (IS NULL)");
        musicFileLen = 0;
        return false;
    }

    println("> Checking WAV SIZE");
    musicFileLen = dataLen;
    if (musicFileLen < 50)
    {
        println("> WAV DATA TOO SMALL");
        _Free(musicFileData);
        musicFileData = NULL;
        return false;
    }

    println("> Checking WAV VALIDITY");
    bool wavValid = true;
    if (!StrEquals(musicFileData + 0, "RIFF", 4))
    {
        wavValid = false;
        goto checkWavValid;
    }

    if (!StrEquals(musicFileData + 8, "WAVE", 4))
    {
        wavValid = false;
        goto checkWavValid;
    }

    if (!StrEquals(musicFileData + 12, "fmt ", 4))
    {
        wavValid = false;
        goto checkWavValid;
    }

    if (!StrEquals(musicFileData + 36, "data", 4))
    {
        wavValid = false;
        goto checkWavValid;
    }

    checkWavValid:
    if (!wavValid)
    {
        println("> WAV DATA INVALID");
        _Free(musicFileData);
        musicFileData = NULL;
        return false;
    }
    else
        println("> WAV DATA VALID");
    

    println("> Parsing WAV Data");
    int channels = *((uint16_t*)(musicFileData + 22));
    int sampleRate = *((uint32_t*)(musicFileData + 24));
    int bitsPerSample = *((uint16_t*)(musicFileData + 34));
    int musicDataSize = *((uint32_t*)(musicFileData + 40));
    int sampleCount = musicDataSize / ((bitsPerSample * channels) / 8);
    printlnf("> Channels: %d, Sample Rate: %d, Bits Per Sample: %d, Data Size: %d, Sample Count: %d", channels, sampleRate, bitsPerSample, musicDataSize, sampleCount);

    
    // if (musicBuffer != NULL)
    // {
    //     println("> Destroying old music buffer");
    //     musicBuffer->Destroy();
    //     musicBuffer = NULL;
    // }
    println("> Creating Audio Buffer");
    musicBuffer = new Audio::AudioBuffer(bitsPerSample, sampleRate, channels, sampleCount);
    _memcpy(musicFileData + waveDataOffset, musicBuffer->data, musicDataSize);

    if (musicSource != NULL)
    {
        println("> Destroying old music source");
        musicSource->Destroy();
        musicSource = NULL;
    }
    println("> Creating Audio Source");
    musicSource = new Audio::BasicAudioSource(musicBuffer);
    musicSource->ConnectTo(globalAudioDest);
    musicSource->buffer->sampleCount = sampleCount;
    musicSource->readyToSend = false;

    println("> Loaded WAV Data Successfully");
    CheckMusic();

    return true;
}


void OnOpenClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
{
    const char* res = Dialog::OpenFileDialog();
    if (res == NULL)
        return;

    LoadFile(res);

    _Free(res);
}

void OnPlayClick(GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
{
    if (paused)
    {
        paused = false;
        if (musicSource != NULL)
            musicSource->readyToSend = true;
        _Free(playBtn->textComp->text);
        playBtn->textComp->text = StrCopy("Stop");
    }
    else
    {
        paused = true;
        if (musicSource != NULL)
            musicSource->readyToSend = false;
        _Free(playBtn->textComp->text);
        playBtn->textComp->text = StrCopy("Play");
    }
}




// int main(int argc, char** argv)
// {
//     initAudioStuff();

//     if (globalAudioDest == NULL)
//         return 0;

//     // Create our Audiosource
//     int sampleRate = 44100;
//     int sampleCount = sampleRate * 1;
//     Audio::BasicAudioSource* audioSource = new Audio::BasicAudioSource(
//         Audio::AudioBuffer::Create16BitMonoBuffer(sampleRate, sampleCount)
//     );

//     // Connect it to the Global Audio Destination
//     audioSource->ConnectTo(globalAudioDest);

//     // Fill the buffer with some audio
//     uint16_t* arr = (uint16_t*)audioSource->buffer->data;
//     int dif = 20;
//     for (int i = 0; i < dif; i++)
//         Audio::FillArray(arr, (i * sampleCount)/dif, sampleCount/dif, ((1000*(i+1)) / dif), sampleRate);
    
//     // Set the sample count and reset the samples sent
//     audioSource->buffer->sampleCount = sampleCount;
//     audioSource->samplesSent = 0;

//     // Set the ready to send flag
//     audioSource->readyToSend = true;


//     // Wait till audio is done playing
//     while (audioSource->readyToSend)
//     {
//         DoAudioCheck();

//         programWaitMsg();
//     }

//     programWait(1000);
//     return 0;
// }