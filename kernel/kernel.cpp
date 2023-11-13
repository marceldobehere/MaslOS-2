#include "kernel.h"
#include "kernelStuff/memory/memory.h"
#include "kernelStuff/stuff/kernelInit.h"
#include "osData/MStack/MStackM.h"
#include "interrupts/panic.h"
#include "elf/elf.h"
#include "devices/serial/serial.h"
#include "devices/pit/pit.h"

#include <libm/list/list.h>
#include "memory/heap.h"
#include "scheduler/scheduler.h"

#include "saf/saf.h"

#include "interrupts/interrupts.h"
#include <libm/cstrTools.h>
#include "paging/PageTableManager.h"
#include "fsStuff/fsStuff.h"

void boot(void* _bootInfo)
{
    BootInfo* bootInfo = (BootInfo*)_bootInfo;

    osData.NO_INTERRUPTS = false;
    osData.booting = false;
    osData.inBootProcess = true;
    osData.maxNonFatalCrashCount = 5;
    MStackData::stackPointer = 0;
    for (int i = 0; i < 1000; i++)
        MStackData::stackArr[i] = MStack();

    osData.enableStackTrace = RECORD_STACK_TRACE;
    MStackData::BenchmarkEnabled = true;
    AddToStack();
    osData.crashCount = 0;
    //
    //while(true);
    osData.booting = true;
    osData.verboseBoot = false;

    osData.mouseSensitivity = 100;

    PrintAll = true;
    
    InitKernel(bootInfo);

    osData.booting = false;

    osData.inBootProcess = false;

    PIT::Sleep(100);

    GlobalRenderer->Clear(Colors.black);

    // GlobalPageTableManager.PrintPageTable();

    // while (true);
    
    
    Scheduler::SchedulerEnabled = false;

    {
        Serial::Writelnf("> Setting up Nothing Doer Task");
        Elf::LoadedElfFile elf;
        elf.entryPoint = (void*)nothing_task_entry;
        osTask* task = Scheduler::CreateTaskFromElf(elf, 0, NULL, false);
        Scheduler::NothingDoerTask = task;
    }

    {
        uint8_t* data = (uint8_t*)bootInfo->programs->fileData;
        //Serial::Writelnf("data: %X", data);

        SAF::initrdMount* mount = SAF::initrd_mount(data);
        SAF::saf_node_folder_t* topNode = (SAF::saf_node_folder_t*) mount->driver_specific_data;
        Serial::Writelnf("> NODES: %d", topNode->num_children);
        
        SAF::saf_node_folder_t* moduleNode = (SAF::saf_node_folder_t*)SAF::initrd_find("modules/", topNode, (SAF::saf_node_hdr_t*)topNode);
        Serial::Writelnf("> module nodes: %d", moduleNode->num_children);
        for (int i = 0; i < moduleNode->num_children; i++)
        {
            SAF::file_t* file = LoadFileFromNode(mount, (SAF::saf_node_file_t*)((uint64_t)topNode + (uint64_t)moduleNode->children[i]));
            Serial::Writelnf("MODULE> file: \"%s\" %d", file->name, file->size);

            const char* combined = StrCombine("bruh:modules/", file->name);
            if (!FS_STUFF::WriteFileToFullPath(combined, (char*)file->driver_specific_data, file->size, true))
                Panic("ADDING FILE FAILED!", true);
            _Free(combined);

            Elf::LoadedElfFile elf = Elf::LoadElf((uint8_t*)file->driver_specific_data);
            if (!elf.works)
                Panic("FILE NO WORK :(", true);

            Serial::Writelnf("> Adding ELF");

            if (StrEquals(file->name, "test.elf"))
            {
                Scheduler::TestElfFile = file->driver_specific_data;
                Serial::Writelnf("> SET TEST ELF");
            }
            else if (StrEquals(file->name, "desktop.elf"))
            {
                Scheduler::DesktopElfFile = file->driver_specific_data;
                Serial::Writelnf("> SET DESKTOP ELF");
            }
            else
            {
                osTask* task = Scheduler::CreateTaskFromElf(elf, 0, NULL, false);
                
                Scheduler::AddTask(task);
                Serial::Writelnf("> ADDED MODULE");
            }
        }

        SAF::saf_node_folder_t* programNode = (SAF::saf_node_folder_t*)SAF::initrd_find("programs/", topNode, (SAF::saf_node_hdr_t*)topNode);
        Serial::Writelnf("> program nodes: %d", programNode->num_children);
        for (int i = 0; i < programNode->num_children; i++)
        {
            SAF::file_t* file = LoadFileFromNode(mount, (SAF::saf_node_file_t*)((uint64_t)topNode + (uint64_t)programNode->children[i]));
            Serial::Writelnf("PROGRAM> file: %d", file->size);

            const char* combined = StrCombine("bruh:programs/", file->name);
            if (!FS_STUFF::WriteFileToFullPath(combined, (char*)file->driver_specific_data, file->size, true))
                Panic("ADDING FILE FAILED!", true);
            _Free(combined);

            // Elf::LoadedElfFile elf = Elf::LoadElf((uint8_t*)file->driver_specific_data);
            // if (!elf.works)
            //     Panic("FILE NO WORK :(", true);

            // Serial::Writelnf("> Adding ELF");

            // Scheduler::AddTask(Scheduler::CreateTaskFromElf(elf, 0, NULL, true));
            // Serial::Writelnf("> ADDED PROGRAM");
        }
    }

    
    //bootInfo->smpData->cpus[1]->goto_address;

    // GlobalRenderer->Clear(Colors.bred);

    // PIT::Sleep(1000);
    
    // GlobalRenderer->Clear(Colors.black);


    Scheduler::SchedulerEnabled = true;

    while (true);
}


 
volatile void bootTest(Framebuffer fb, ACPI::RSDP2* rsdp, PSF1_FONT* psf1_font, MaslOsAssetStruct* assets, void* freeMemStart, void* extraMemStart, uint64_t freeMemSize, void* kernelStart, uint64_t kernelSize, void* kernelStartV, limineSmpResponse* smpData)
{
    //MStackData::BenchmarkEnabled = false;
    BootInfo tempBootInfo;
    tempBootInfo.framebuffer = &fb;
    tempBootInfo.rsdp = rsdp;

    tempBootInfo.psf1_font = psf1_font;

    tempBootInfo.testImage = assets->testImage;
    tempBootInfo.bootImage = assets->bootImage;
    tempBootInfo.MButton = assets->MButton;
    tempBootInfo.MButtonS = assets->MButtonS;
    tempBootInfo.bgImage = assets->bgImage;
    tempBootInfo.maabZIP = assets->maabZIP;
    tempBootInfo.otherZIP = assets->otherZIP;

    tempBootInfo.mouseZIP = assets->mouseZIP;
    tempBootInfo.windowButtonZIP = assets->windowButtonZIP;
    tempBootInfo.windowIconsZIP = assets->windowIconsZIP;

    tempBootInfo.programs = assets->programs;
    //tempBootInfo.nothingDoer = assets->nothingDoer;

    tempBootInfo.mMapStart = freeMemStart;
    tempBootInfo.m2MapStart = extraMemStart;
    tempBootInfo.mMapSize = freeMemSize;
    
    tempBootInfo.kernelStart = kernelStart;
    tempBootInfo.kernelSize = kernelSize;
    tempBootInfo.kernelStartV = kernelStartV;

    tempBootInfo.smpData = smpData;

    for (int y = 0; y < 100; y++)
        for (int x = 500; x < 600; x++)
            *(uint32_t*)((uint64_t)fb.BaseAddress + 4 * (x + y * fb.PixelsPerScanLine)) = (Colors.yellow * ((x + y + 1) % 2)) + (Colors.green * ((x + y) % 2));

    //while (true);

    
    boot(&tempBootInfo);
    return;
}