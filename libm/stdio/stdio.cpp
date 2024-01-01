#include "stdio.h"
#include <libm/syscallManager.h>
#include <libm/msgPackets/msgPacket.h>
#include <libm/rnd/rnd.h>
#include <libm/cstrTools.h>
#include <libm/memStuff.h>
#include <libm/stubs.h>
#include "./stdargs.h"
#include <libm/cstr.h>
#include <libm/heap/heap.h>
#include <libm/list/list_basics.h>

namespace STDIO
{
    StdioInst::StdioInst(uint64_t pid)
    {
        this->pid = pid;
        readQueue = new Queue<uint8_t>(16);
    }

    void StdioInst::Free()
    {
        readQueue->Free();
        _Free(readQueue);
    }

    StdioInst* parent = NULL;

    void initStdio(bool needLoggerWindow)
    {
        parent = new StdioInst(getParentPid());

        // try to connect with parent
        GenericMessagePacket* packet = msgWaitConv(STDIO_INIT_CONVO_ID, 1500);

        if (packet != NULL && packet->Type == MessagePacketType::GENERIC_DATA && packet->Size == 8 && packet->FromPID == parent->pid)
        {
            parent->convoId = *(uint64_t*)packet->Data;

            packet->Free();
            _Free(packet);
        }
        else
        {
            if (packet != NULL)
            {
                packet->Free();
                _Free(packet);
            }

            // failed to connect with parent
            if (needLoggerWindow)
            {
                const char* wPath = getWorkingPath();
                uint64_t newPid = startProcess("bruh:programs/logger/logger.elf", 0, NULL, wPath);
                _Free(wPath);

                if (newPid == 0)
                {
                    parent->pid = 0;
                    parent->convoId = 0;
                    Panic("Failed to start logger!", true);
                }
                else
                {
                    parent->Free();
                    _Free(parent);
                    parent = initStdio(newPid);
                }
            }
        }
    }

    StdioInst* initStdioClient(uint64_t pid)
    {
        StdioInst* temp = new StdioInst(pid);

        // try to connect with parent
        GenericMessagePacket* packet = msgWaitConv(STDIO_INIT_CONVO_ID, 2000);

        if (packet != NULL && packet->Type == MessagePacketType::GENERIC_DATA && packet->Size == 8 && packet->FromPID == temp->pid)
        {
            temp->convoId = *(uint64_t*)packet->Data;

            packet->Free();
            _Free(packet);
        }
        else
        {
            if (packet != NULL)
            {
                packet->Free();
                _Free(packet);
            }

            temp->Free();
            _Free(temp);
            return NULL;
        }

        return temp;
    }

    StdioInst* initStdio(uint64_t pid)
    {
        StdioInst* other = new StdioInst(pid);
        other->convoId = RND::RandomInt();

        {
            GenericMessagePacket* packet = new GenericMessagePacket(MessagePacketType::GENERIC_DATA, (uint8_t*)&other->convoId, 8);
            msgSendConv(packet, other->pid, STDIO_INIT_CONVO_ID);
            packet->Free();
            _Free(packet);  
        }

        return other;
    }

    // Print to parent
    void print(const char* str)
    {
        print(str, parent);
    }

    void print(char chr)
    {
        char str[2];
        str[0] = chr;
        str[1] = '\0';
        print(str);
    }
    void println()
    {
        print("\n\r");
    }
    void println(char chr)
    {
        print(chr);
        println();
    }
    void println(const char* str)
    {
        print(str);
        println();
    }

    // Print to any
    void print(const char* str, StdioInst* other)
    {
        GenericMessagePacket* packet = new GenericMessagePacket(MessagePacketType::GENERIC_DATA, (uint8_t*)str, StrLen(str));
        msgSendConv(packet, other->pid, other->convoId);
        packet->Free();
        _Free(packet);
    }
    
    void print(char chr, StdioInst* other)
    {
        char str[2];
        str[0] = chr;
        str[1] = '\0';
        print(str, other);
    }
    void println(StdioInst* other)
    {
        print("\n\r", other);
    }
    void println(char chr, StdioInst* other)
    {
        print(chr, other);
        println(other);
    }
    void println(const char* str, StdioInst* other)
    {
        print(str, other);
        println(other);
    }

    void _printf(const char* str, va_list arg, StdioInst* other);

    void printlnf(const char* str, ...)
    {
        va_list arg;
        va_start(arg, str);
        _printf(str, arg, parent);
        va_end(arg);
        println(parent);
    }

    void printf(const char* str, ...)
    {
        va_list arg;
        va_start(arg, str);
        _printf(str, arg, parent);
        va_end(arg);
    }

    void printlnf(StdioInst* other, const char* str, ...)
    {
        va_list arg;
        va_start(arg, str);
        _printf(str, arg, other);
        va_end(arg);
        println(other);
    }

    void printf(StdioInst* other, const char* str, ...)
    {
        va_list arg;
        va_start(arg, str);
        _printf(str, arg, other);
        va_end(arg);
    }


    // %s -> string
    // %c -> char
    // %d/i -> int (32 bit)
    // %D/I -> int (64 bit)
    // %x -> hex (32 bit)
    // %X -> hex (64 bit)
    // %b -> byte
    // %B -> bool
    // %f -> float
    // %F -> double
    // %% -> %
    void _printf(const char* str, va_list arg, StdioInst* other)
    {
        int len = StrLen(str);

        for (int i = 0; i < len; i++)
        {
            if (str[i] == '%' && i + 1 < len)
            {
                i++;
                if (str[i] == 's')
                {
                    char* argStr = va_arg(arg, char*);
                    if (argStr != NULL)
                        print(argStr, other);
                    else
                        print("(null)", other);
                }
                else if (str[i] == 'c')
                {
                    char argChar = va_arg(arg, int);
                    print(argChar, other);
                }
                else if (str[i] == 'd' || str[i + 1] == 'i')
                {
                    int argInt = va_arg(arg, int);
                    print(to_string(argInt), other);
                }
                else if (str[i] == 'D' || str[i + 1] == 'I')
                {
                    uint64_t argInt = va_arg(arg, uint64_t);
                    print(to_string(argInt), other);
                }
                else if (str[i] == 'x')
                {
                    uint32_t argInt = va_arg(arg, uint32_t);
                    print(ConvertHexToString(argInt), other);
                }
                else if (str[i] == 'X')
                {
                    uint64_t argInt = va_arg(arg, uint64_t);
                    print(ConvertHexToString(argInt), other);
                }
                else if (str[i] == 'b')
                {
                    uint8_t argInt = (uint8_t)va_arg(arg, int);
                    print(to_string(argInt), other);
                }
                else if (str[i] == 'B')
                {
                    bool argInt = (bool)va_arg(arg, int);
                    print(to_string(argInt), other);
                }
                else if (str[i] == 'f')
                {
                    // compiler be trolling me
                    // float argFloat = va_arg(arg, float);
                    // Write(to_string(argFloat));
                    

                    double argDouble = va_arg(arg, double);
                    print(to_string(argDouble), other);
                }
                else if (str[i] == 'F')
                {
                    double argDouble = va_arg(arg, double);
                    print(to_string(argDouble), other);
                }
                else if (str[i] == '%')
                {
                    print('%', other);
                }
                else
                {
                    print(str[i], other);
                }
            }
            else
            {
                print(str[i], other);
            }
        }
    }

    // Clear parent
    void clear()
    {
        clear(parent);
    }

    // Clear any
    void clear(StdioInst* other)
    {
        print("\033[2J", other);
    }


    // Read from parent
    int read()
    {
        return read(parent);
    }
    
    bool available()
    {
        return available(parent);
    }

    int bytesAvailable()
    {
        return bytesAvailable(parent);
    }

    void CheckReadQueue(StdioInst* other)
    {
        while (true)
        {
            GenericMessagePacket* packet = msgGetConv(other->convoId);
            if (packet == NULL)
                return;
            
            if (packet->Type != MessagePacketType::GENERIC_DATA)
            {
                packet->Free();
                _Free(packet);
                return;
            }

            for (int i = 0; i < packet->Size; i++)
                other->readQueue->Enqueue(packet->Data[i]);
                
            packet->Free();
            _Free(packet);
        }
    }

    // Read from any
    int read(StdioInst* other)
    {
        CheckReadQueue(other);
        if (other->readQueue->GetCount() > 0)
            return other->readQueue->Dequeue();
        return -1;
    }

    bool available(StdioInst* other)
    {
        CheckReadQueue(other);
        return other->readQueue->GetCount() > 0;
    }

    int bytesAvailable(StdioInst* other)
    {
        CheckReadQueue(other);
        return other->readQueue->GetCount();
    }

    void sendBytes(uint8_t* bytes, uint64_t size)
    {
        sendBytes(bytes, size, parent);
    }

    uint64_t readBytes(uint8_t* bytes, uint64_t size)
    {
        return readBytes(bytes, size, parent);
    }
    
    const char* readLine()
    {
        return readLine(parent);
    }
    
    
    void sendBytes(uint8_t* bytes, uint64_t size, StdioInst* other)
    {
        GenericMessagePacket* packet = new GenericMessagePacket(MessagePacketType::GENERIC_DATA, bytes, size);
        msgSendConv(packet, other->pid, other->convoId);
        packet->Free();
        _Free(packet);
    }



    uint64_t readBytes(uint8_t* bytes, uint64_t size, StdioInst* other)
    {
        for (int i = 0; i < size; i++)
        {
            while (true)
            {
                CheckReadQueue(other);
                if (other->readQueue->GetCount() < 1)
                    programWaitMsg();
                else
                    break;
            }

            bytes[i] = other->readQueue->Dequeue();
        }

        return size;
    }


    const char* readLine(StdioInst* other)
    {
        List<char>* line = new List<char>();
        while (true)
        {
            while (true)
            {
                CheckReadQueue(other);
                if (other->readQueue->GetCount() < 1 && pidExists(other->pid))
                    //programWaitMsg();
                    programWait(50);
                else
                    break;
            }

            if (other->readQueue->GetCount() < 1 && !pidExists(other->pid))
                break;

            char c = other->readQueue->Dequeue();
            if (c == '\n')
                break;
            if (c == '\r')
                continue;
            line->Add(c);
        }

        // Handle \b by deleting it and the previous element
        for (int i = 0; i < line->GetCount(); i++)
        {
            if (line->ElementAt(i) == '\b')
            {
                line->RemoveAt(i);
                if (i > 0)
                {
                    i--;
                    line->RemoveAt(i);
                }
                i--;
                continue;
            }
        }

        int len = line->GetCount();
        char* str = (char*)_Malloc(len + 1);

        for (int i = 0; i < len; i++)
            str[i] = line->ElementAt(i);
        str[len] = '\0';

        line->Free();
        _Free(line);

        return str;
    }
};
