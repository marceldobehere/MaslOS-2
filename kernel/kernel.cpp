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

void CopyRecursive(SAF::initrdMount* mount, const char* tempCombined, SAF::saf_node_folder_t* folderNode, const char* folderName)
{
    SAF::saf_node_folder_t* topNode = (SAF::saf_node_folder_t*) mount->driver_specific_data;
    Serial::Writelnf("SUB> folder: \"%s\" %d (\"%s\")", folderNode->hdr.name, folderNode->num_children, tempCombined);
    for (int i = 0; i < folderNode->num_children; i++)
    {
        SAF::file_t* file = LoadFileFromNode(mount, (SAF::saf_node_file_t*)((uint64_t)topNode + (uint64_t)folderNode->children[i]));
        Serial::Writelnf("SUB> file: \"%s\" %d (Folder: %B)", file->name, file->size, file->isFolder);

        if (file->isFolder)
        {
            const char* combined = StrCombine(tempCombined, file->name);
            FS_STUFF::CreateFolderIfNotExist(combined);
            const char* combined2 = StrCombine(combined, "/");
            CopyRecursive(mount, combined2, (SAF::saf_node_folder_t*)(SAF::saf_node_file_t*)((uint64_t)topNode + (uint64_t)folderNode->children[i]), file->name);
            _Free(combined2);
            _Free(combined);
        }
        else
        {
            const char* combined = StrCombine(tempCombined, file->name);
            if (!FS_STUFF::WriteFileToFullPath(combined, (char*)file->driver_specific_data, file->size, true))
                Panic("ADDING FILE FAILED!", true);
            _Free(combined);
        }
    }
}


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
        osTask* task = Scheduler::CreateTaskFromElf(elf, 0, NULL, false, "", "");
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
            SAF::saf_node_folder_t* moduleNode2 = (SAF::saf_node_folder_t*)((uint64_t)topNode + (uint64_t)moduleNode->children[i]);
            Serial::Writelnf("MODULE> folder: \"%s\" %d", moduleNode2->hdr.name, moduleNode2->num_children);

            const char* tempCombine = NULL;
            {
                const char* combined = StrCombine("bruh:modules/", moduleNode2->hdr.name);
                FS_STUFF::CreateFolderIfNotExist(combined);
                tempCombine = StrCombine(combined, "/");
                _Free(combined);
            }

            for (int j = 0; j < moduleNode2->num_children; j++)
            {
                SAF::file_t* file = LoadFileFromNode(mount, (SAF::saf_node_file_t*)((uint64_t)topNode + (uint64_t)moduleNode2->children[j]));
                Serial::Writelnf("MODULE> file: \"%s\" %d (Folder: %B)", file->name, file->size, file->isFolder);

                if (file->isFolder)
                {
                    const char* combined = StrCombine(tempCombine, file->name);
                    FS_STUFF::CreateFolderIfNotExist(combined);
                    const char* combined2 = StrCombine(combined, "/");
                    CopyRecursive(mount, combined2, (SAF::saf_node_folder_t*)(SAF::saf_node_file_t*)((uint64_t)topNode + (uint64_t)moduleNode2->children[j]), file->name);
                    _Free(combined2);
                    _Free(combined);
                }
                else
                {
                    const char* combined = StrCombine(tempCombine, file->name);
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
                    else if (StrEquals(file->name, "startMenu.elf"))
                    {
                        Scheduler::StartMenuElfFile = file->driver_specific_data;
                        Serial::Writelnf("> SET START MENU ELF");
                    }
                    // else
                    // {
                    //     osTask* task = Scheduler::CreateTaskFromElf(elf, 0, NULL, false);
                        
                    //     Scheduler::AddTask(task);
                    //     Serial::Writelnf("> ADDED MODULE");
                    // }
                }
            }

            _Free(tempCombine);
        }

        SAF::saf_node_folder_t* programNode = (SAF::saf_node_folder_t*)SAF::initrd_find("programs/", topNode, (SAF::saf_node_hdr_t*)topNode);
        Serial::Writelnf("> program nodes: %d", programNode->num_children);
        for (int i = 0; i < programNode->num_children; i++)
        {
            SAF::saf_node_folder_t* programNode2 = (SAF::saf_node_folder_t*)((uint64_t)topNode + (uint64_t)programNode->children[i]);
            Serial::Writelnf("PROGRAM> folder: \"%s\" %d", programNode2->hdr.name, programNode2->num_children);

            const char* tempCombine = NULL;
            {
                const char* combined = StrCombine("bruh:programs/", programNode2->hdr.name);
                FS_STUFF::CreateFolderIfNotExist(combined);
                tempCombine = StrCombine(combined, "/");
                _Free(combined);
            }

            for (int j = 0; j < programNode2->num_children; j++)
            {
                SAF::file_t* file = LoadFileFromNode(mount, (SAF::saf_node_file_t*)((uint64_t)topNode + (uint64_t)programNode2->children[j]));
                Serial::Writelnf("PROGRAM> file: \"%s\" %d (Folder: %B)", file->name, file->size, file->isFolder);

                if (file->isFolder)
                {
                    const char* combined = StrCombine(tempCombine, file->name);
                    Serial::Writelnf("PROGRAM> folder: \"%s\"", combined);
                    FS_STUFF::CreateFolderIfNotExist(combined);
                    const char* combined2 = StrCombine(combined, "/");
                    CopyRecursive(mount, combined2, (SAF::saf_node_folder_t*)(SAF::saf_node_file_t*)((uint64_t)topNode + (uint64_t)programNode2->children[j]), file->name);
                    _Free(combined2);
                    _Free(combined);
                }
                else
                {
                    const char* combined = StrCombine(tempCombine, file->name);
                    if (!FS_STUFF::WriteFileToFullPath(combined, (char*)file->driver_specific_data, file->size, true))
                        Panic("ADDING FILE FAILED!", true);
                    _Free(combined);
                }
            }

            _Free(tempCombine);
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

    tempBootInfo.bootImage = assets->bootImage;
    tempBootInfo.programs = assets->programs;

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