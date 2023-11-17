#include "maabTask.h"
#include <libm/cstrTools.h>
#include <libm/cstr.h>
#include <libm/gui/guiStuff/components/text/textComponent.h>
#include <libm/gui/guiStuff/components/imageRect/imageRectangleComponent.h>
#include <libm/rnd/rnd.h>
#include <libm/stubs.h>
#include <libm/list/list_window.h>
#include <libm/rendering/Cols.h>
#include <libm/wmStuff/wmStuff.h>
#include <libm/window/windowAttr.h>
#include <libm/list/list_gui.h>
#include <libm/syscallManager.h>
#include <libm/stubs.h>


uint8_t* mem;
uint64_t memLen;

MAAB_MEM::MbMemHandler* memHandler;

uint64_t writeInputInto;

List<Window*>* windowsCreated;
List<GuiInstance*>* guisCreated;

Window* window;

int cyclesPerCall = 300;

uint64_t instrPointer = 0;
uint32_t defCol = Colors.white;

uint64_t subLastPos[100];
int64_t subDeepness = 0;
int64_t maxSubDeepness = 90;
bool programEnded = false;



const char* errMsg = "<NO ERRORS>";
int errCode = 0;
bool waitInput;
bool gotInput;
char memUserInput[500];
uint64_t memUserInputLen = 0;
bool done;



uint32_t foregroundColor = Colors.white;
uint32_t backgroundColor = Colors.black;
void Print(const char* str)
{
    serialPrint(str);
}
void Println()
{
    serialPrintLn("");
}
char ReadChar()
{
    while (true)
    {
        char c = serialReadChar();
        if (c != 0)
            return c;
    }
}
void Clear()
{
    serialPrintLn("<CLS>");
}
void Print(char chr)
{
    char str[2];
    str[0] = chr;
    str[1] = 0;
    Print(str);
}
void Println(const char* str)
{
    Print(str);
    Println();
}


int main(int argc, const char** argv)
{
    initWindowManagerStuff();


	if (argc == 0 || argv == NULL)
		return -1;
	
	const char* path = argv[0];
	uint64_t codeLen;
	uint8_t* code;

	if (!fsReadFile(path, (void**)&code, &codeLen))
		return - 1;

	programEnded = false;
	done = false;

	windowsCreated = new List<Window*>(10);
    guisCreated = new List<GuiInstance*>(10);

	memLen = 2000000;
	mem = (uint8_t*)_Malloc(memLen);
	for (int i = 0; i < memLen; i++)
		mem[i] = 0;

	for (int i = 0; i < codeLen; i++)
		mem[i] = code[i];

	memHandler = (MAAB_MEM::MbMemHandler*)_Malloc(sizeof(MAAB_MEM::MbMemHandler));
	*memHandler = MAAB_MEM::MbMemHandler((void*)((uint64_t)mem + codeLen), memLen - codeLen, codeLen);

	memUserInputLen = 0;
	for (int i = 0; i < 500; i++)
		memUserInput[i] = (char)0;

	waitInput = false;
	gotInput = false;
	programEnded = false;


	instrPointer = 0;
	errMsg = "<NO ERRORS>";
	errCode = 0;


	waitInput = false;
	writeInputInto = (uint64_t)0;
	defCol = Colors.white;

	subDeepness = 0;
	for (int i = 0; i < maxSubDeepness; i++)
		subLastPos[i] = 0;

    while (!done)
    {
        Do();

        for (int i = 0; i < guisCreated->GetCount(); i++)
        {
            GuiInstance* gui = guisCreated->ElementAt(i);
            gui->Update();
            gui->Render(false);
        }

        programYield();
    }
}


Window* FindWindowWithId(uint64_t winID)
{
    for (int i = 0; i < windowsCreated->GetCount(); i++)
    {
        Window* win = windowsCreated->ElementAt(i);
        if (win->ID == winID)
            return win;
    }
    return NULL;
}
GuiInstance* GetGuiFromWindow(Window* win)
{
    if (win == NULL)
        return NULL;
    
    if (guisCreated->GetCount() != windowsCreated->GetCount())
    {
        Println("WAAAA COUNTS DONT MATCH");
        programCrash();
    }
    

    for (int i = 0; i < windowsCreated->GetCount(); i++)
    {
        Window* window = windowsCreated->ElementAt(i);
        if (window == win)
            return guisCreated->ElementAt(i);
    }
}


void PrintMem()
{
	Println("Data:");
	for (int i = 0; i < 500; i++)
	{
        Print(to_string(mem[i]));
        Print(" ");
    }
	Println();
}


void Do()
{
	if (waitInput)
	{
		if (!gotInput)
			return;
		waitInput = false;
		gotInput = false;

		void* toWrite = memHandler->MallocMem(memUserInputLen + 1);
		if (toWrite == NULL)
		{
			programEnded = true;
			errCode = 2;
			errMsg = "MALLOC FOR USER INPUT FAILED!";
			return;
		}

		for (int i = 0; i < memUserInputLen; i++)
			*((char*)((uint64_t)mem + (uint64_t)toWrite + i)) = memUserInput[i];
		*((char*)((uint64_t)mem + (uint64_t)toWrite + memUserInputLen)) = 0;

		*((uint64_t*)((uint64_t)mem + writeInputInto)) = (uint64_t)toWrite;

		memUserInputLen = 0;
		for (int i = 0; i < 500; i++)
			memUserInput[i] = (char)0;
	}

	if (instrPointer >= memLen)
	{
		programEnded = true;
		errCode = 1;
		errMsg = "INSTRUCTION POINTER OUT OF BOUNDS!";
		return;
	}

	for (int sI = 0; sI < cyclesPerCall; sI++)
	{
		// if (tempTask != NULL)
		// {
		// 	DoTask(tempTask);
		// 	if (tempTask->GetDone())
		// 	{
		// 		FreeTask(tempTask);
		// 		tempTask = NULL;
		// 	}
		// 	else
		// 		continue;
		// }

		if (waitInput)
			return;
		if (done)
			return;
		if (programEnded)
		{
			Println();
			Println();
			if (errCode == 0)
			{
				Println("Program exited with no errors!");
			}
			else
			{
				Print("Program exited with error code: ");
                Print(to_string(errCode));
                Println(" !");
				Println("Error Message:");
				Println(errMsg);
			}

			done = true;
			return;
		}
		if (errCode != 0)
		{
			programEnded = true;
			return;
		}

		uint8_t instr = mem[instrPointer];

		if (instr == 0) //nop
		{
			instrPointer++;
		}
		else if (instr == 1) //exit
		{
			programEnded = true;
			errCode = 0;
			return;
		}
		else if (instr == 2) //set fix mem to fix val
		{
			if (instrPointer + 20 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}
			uint8_t copySize = mem[instrPointer + 1];
			uint64_t copyAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 2));

			if (copyAddr + copySize >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "ADDR OUT OF BOUNDS!";
				return;
			}

			//Print("\n<COPY {} BYTES TO ADDR ", to_string(copySize), defCol);
			//Println("{}>", to_string(copyAddr), defCol);

			//Print("DATA ADDR TO: ");
			//ShowBytes(8, instrPointer + 2);
			//Println();
			//Print("DATA ADDR TO: ");
			//ShowBytes(8, copyAddr);
			//Println();
			//Print("DATA: ");
			//ShowBytes(copySize, instrPointer + 10);
			//Println();

			for (int i = 0; i < copySize; i++)
				mem[copyAddr + i] = mem[instrPointer + 10 + i];




			instrPointer += 10 + copySize;
		}
		else if (instr == 3) //copy fix mem to fix mem
		{
			if (instrPointer + 17 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}
			uint8_t copySize = mem[instrPointer + 1];
			uint64_t copyAddr1 = *((uint64_t*)((uint64_t)mem + instrPointer + 2));
			uint64_t copyAddr2 = *((uint64_t*)((uint64_t)mem + instrPointer + 10));

			if (copyAddr1 + copySize >= memLen || copyAddr2 + copySize >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "ADDR OUT OF BOUNDS!";
				return;
			}


			for (int i = 0; i < copySize; i++)
				mem[copyAddr2 + i] = mem[copyAddr1 + i];

			instrPointer += 18;
		}

		else if (instr == 16) // fix cast
		{
			if (instrPointer + 30 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}

			DatatypeNumber dataType1 = (DatatypeNumber)mem[instrPointer + 1];
			DatatypeNumber dataType2 = (DatatypeNumber)mem[instrPointer + 2];
			uint8_t size1 = datatypeSizes[(uint8_t)dataType1];

			uint64_t addr1 = instrPointer + 3;
			uint64_t addr2 = *((uint64_t*)((uint64_t)mem + instrPointer + 3 + size1));

			Cast(dataType1, addr1, dataType2, addr2);

			instrPointer += 11 + size1;
		}
		else if (instr == 15) // var cast
		{
			if (instrPointer + 30 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}

			DatatypeNumber dataType1 = (DatatypeNumber)mem[instrPointer + 1];
			DatatypeNumber dataType2 = (DatatypeNumber)mem[instrPointer + 2];

			uint64_t addr1 = *((uint64_t*)((uint64_t)mem + instrPointer + 3));
			uint64_t addr2 = *((uint64_t*)((uint64_t)mem + instrPointer + 11));

			Cast(dataType1, addr1, dataType2, addr2);

			instrPointer += 19;
		}

		else if (instr == 10) // calc
		{
			if (instrPointer + 30 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}

			OpNumber opNum = (OpNumber)mem[instrPointer + 1];
			DatatypeNumber dataType = (DatatypeNumber)mem[instrPointer + 2];
			uint8_t size = datatypeSizes[(uint8_t)dataType];

			if (opNum == OpNumber::BOOL_NOT || opNum == OpNumber::BIT_NOT)
			{
				uint64_t addr1 = instrPointer + 3;
				uint64_t resAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 3 + size));

				Math(opNum, dataType, addr1, 0, resAddr);
				instrPointer += 11 + size;
			}
			else
			{
				uint64_t addr1 = instrPointer + 3;
				uint64_t addr2 = instrPointer + 3 + size;
				uint64_t resAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 3 + size * 2));

				//Println();
				//ShowBytes(size, addr1);
				//Println();
				//ShowBytes(size, addr2);
				//Println();
				//ShowBytes(size, instrPointer + 3 + size * 2);
				//Println();
				//ShowBytes(size, resAddr);
				//Println();


				Math(opNum, dataType, addr1, addr2, resAddr);
				instrPointer += 11 + size * 2;
			}
		}

		else if (instr == 20) //jump
		{
			if (instrPointer + 8 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}
			uint64_t jumpAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 1));

			instrPointer = jumpAddr;
		}
		else if (instr == 40) //if jump
		{
			if (instrPointer + 9 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}
			bool jump = *((char*)((uint64_t)mem + instrPointer + 1));;
			uint64_t jumpAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 2));

			if (jump)
				instrPointer = jumpAddr;
			else
				instrPointer += 10;
		}

		else if (instr == 25) //sub
		{
			if (instrPointer + 8 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}
			uint64_t jumpAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 1));

			if (subDeepness > maxSubDeepness)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "SUBROUTINE STACK OVERFLOW!";
				return;
			}

			subLastPos[subDeepness++] = instrPointer + 9;
			instrPointer = jumpAddr;
		}
		else if (instr == 30) //return
		{
			if (subDeepness < 1)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "SUBROUTINE STACK UNDERFLOW!";
				return;
			}
			instrPointer = subLastPos[--subDeepness];
		}
		else if (instr == 45) //if_sub
		{
			if (instrPointer + 9 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}
			bool jump = *((char*)((uint64_t)mem + instrPointer + 1));;
			uint64_t jumpAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 2));

			if (subDeepness > maxSubDeepness)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "SUBROUTINE STACK OVERFLOW!";
				return;
			}

			subLastPos[subDeepness++] = instrPointer + 10;
			instrPointer = jumpAddr;
		}


		else if (instr == 50) //syscall
		{
			if (instrPointer + 2 >= memLen)
			{
				programEnded = true;
				errCode = 1;
				errMsg = "INSTRUCTION OUT OF BOUNDS!";
				return;
			}
			uint8_t syscall1 = mem[instrPointer + 1];
			uint8_t syscall2 = mem[instrPointer + 2];

			if (syscall1 == 1)
			{
				if (syscall2 == 1)
				{
					uint8_t byteToPrint = *((uint8_t*)((uint64_t)mem + instrPointer + 3));
					Print((char)byteToPrint);
					instrPointer += 3 + 1;
				}
				else if (syscall2 == 2)
				{
					DatatypeNumber dataType1 = (DatatypeNumber)mem[instrPointer + 3];
					uint8_t size1 = datatypeSizes[(uint8_t)dataType1];

					uint64_t addr1 = instrPointer + 4;

					PrintVal(dataType1, addr1);
					instrPointer += 3 + 1 + size1;
				}
				else if (syscall2 == 3)
				{
					uint64_t printAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 3));
					//Println("<PRINTING STR AT {}>", to_string(printAddr), defCol);
					for (int i = 0; printAddr + i < memLen && mem[printAddr + i] != 0; i++)
					{
						Print((char)mem[printAddr + i]);
					}
					instrPointer += 3 + 8;
				}
				else if (syscall2 == 4)
				{
					uint64_t rAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 3));

					//Println("\n<FREE GO BRRT>");

					//bool fRes = memHandler->FreeMem((void*)fAddr);

					if (rAddr + 8 >= memLen)
					{
						programEnded = true;
						errCode = 1;
						errMsg = "ADDRESS OUT OF BOUNDS!";
						return;
					}

					instrPointer += 3 + 8;
					gotInput = false;
					waitInput = true;
					writeInputInto = rAddr;
					break;
				}
				else if (syscall2 == 5)
				{
					uint32_t col = *((uint32_t*)((uint64_t)mem + instrPointer + 3));
					//Println("<PRINTING STR AT {}>", to_string(printAddr), defCol);

					foregroundColor = col;

					instrPointer += 3 + 4;
				}
				else if (syscall2 == 6)
				{
					uint32_t col = *((uint32_t*)((uint64_t)mem + instrPointer + 3));
					//Println("<PRINTING STR AT {}>", to_string(printAddr), defCol);

					backgroundColor = col;

					instrPointer += 3 + 4;
				}
				else if (syscall2 == 7)
				{
					Clear();
					instrPointer += 3 + 0;
				}
				else if (syscall2 == 8)
				{
					int32_t amt = *((int32_t*)((uint64_t)mem + instrPointer + 3));
					//Println("<PRINTING STR AT {}>", to_string(printAddr), defCol);

					programWait(amt);

					instrPointer += 3 + 4;
				}

				else
				{
					programEnded = true;
					errCode = 1;
					errMsg = "CONSOLE SYSCALL IS NOT SUPPORTED";
					return;
				}
			}
			else if (syscall1 == 2)
			{
				if (syscall2 == 1)
				{
					uint32_t mSize = *((uint32_t*)((uint64_t)mem + instrPointer + 3));
					uint64_t mAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 7));

					if (mAddr + 8 >= memLen)
					{
						programEnded = true;
						errCode = 1;
						errMsg = "ADDRESS OUT OF BOUNDS!";
						return;
					}
					
					//Println("\n<MALLOC GO BRRT>");

					void* mRes = memHandler->MallocMem(mSize);
					
					if (mRes == NULL)
					{
						programEnded = true;
						errCode = 2;
						errMsg = "MALLOC FAILED!";
						return;
					}

					*((uint64_t*)(mem + mAddr)) = (uint64_t)mRes;


					instrPointer += 3 + 12;
				}
				else if (syscall2 == 2)
				{
					uint64_t fAddr = *((uint64_t*)((uint64_t)mem + instrPointer + 3));

					//Println("\n<FREE GO BRRT>");

					bool fRes = memHandler->FreeMem((void*)fAddr);

					if (!fRes)
					{
						programEnded = true;
						errCode = 2;
						errMsg = "FREE FAILED!";
						return;
					}

					instrPointer += 3 + 8;
				}



				else
				{
					programEnded = true;
					errCode = 1;
					errMsg = "MEMORY SYSCALL IS NOT SUPPORTED";
					return;
				}
			}
			else if (syscall1 == 3)
			{
				if (syscall2 == 1)
				{
					int64_t id = *((int64_t*)((uint64_t)mem + instrPointer + 3));


                    Window* window = requestWindow(id);

                    if (window != NULL)
                    {
                        windowsCreated->Add(window);

                        GuiInstance* guiInst = new GuiInstance(window);
                        guiInst->Init();
                        setWindow(window);

                        guisCreated->Add(guiInst);
                    }

					instrPointer += 3 + 8;
				}
				else if (syscall2 == 2)
				{
					int64_t id = *((int64_t*)((uint64_t)mem + instrPointer + 3));

                    int indx1 = windowsCreated->GetIndexOf(FindWindowWithId(id));
                    if (indx1 != -1)
                    {
                        Window* win = windowsCreated->ElementAt(indx1);
                        GuiInstance* gui = guisCreated->ElementAt(indx1);
                        windowsCreated->RemoveAt(indx1);
                        guisCreated->RemoveAt(indx1);

                        deleteWindow(id);

                        win->Free();
                        _Free(win);
                        gui->Free();
                        _Free(gui);
                    }

					instrPointer += 3 + 8;
				}
				else if (syscall2 == 3)
				{
					int64_t id = *((int64_t*)((uint64_t)mem + instrPointer + 3));
					int32_t attr = *((int32_t*)((uint64_t)mem + instrPointer + 11));
					uint64_t val = *((uint64_t*)((uint64_t)mem + instrPointer + 15));
					if ((WindowAttributeType)attr == WindowAttributeType::TITLE_TEXT)
					{	
						if (val >= memLen)
						{
							programEnded = true;
							errCode = 3;
							errMsg = "STRING IS OUT OF BOUNDS!";
							return;
						}

						Window* tWin = FindWindowWithId(id);
						if (tWin != NULL)
						{
							const char* newThing = StrCopy((const char*)(mem + val));
							const char* oldThing = tWin->Title;
							tWin->Title = newThing;

							_Free((void*)oldThing);
						}

					}
					else
					{
                        Window* tWin = FindWindowWithId(id);

                        if (tWin != NULL)
                        {
                            SetWindowAttribute(tWin, (WindowAttributeType)attr, val);
                            setWindow(tWin);
                        } 
                    }

					instrPointer += 3 + 8 + 4 + 8;
				}
				else if (syscall2 == 4)
				{
					// TODO: ADD WINDOW MODIFIYNG

					int64_t id = *((int64_t*)((uint64_t)mem + instrPointer + 3));
					int32_t attr = *((int32_t*)((uint64_t)mem + instrPointer + 11));
					uint64_t addr = *((uint64_t*)((uint64_t)mem + instrPointer + 15));

					if (addr >= memLen)
					{
						programEnded = true;
						errCode = 3;
						errMsg = "ADDR IS OUT OF BOUNDS!";
						return;
					}

					if ((WindowAttributeType)attr == WindowAttributeType::TITLE_TEXT)
					{	
						Window* tWin = FindWindowWithId(id);
						if (tWin != NULL)
						{
							int len = StrLen(tWin->Title);
							uint64_t nAddr = (uint64_t)memHandler->MallocMem(len + 1);
							for (int i = 0; i < len; i++)
								mem[nAddr + i] = tWin->Title[i];
							mem[nAddr + len] = 0;

							*((uint64_t*)(mem + addr)) = (uint64_t)nAddr;
						}
					}
					else
					{
                        Window* tWin = FindWindowWithId(id);
						uint64_t bleh = GetWindowAttribute(tWin, (WindowAttributeType)attr);
						int amt = GetWindowAttributeSize((WindowAttributeType)attr);
						for (int i = 0; i < amt; i++)
							*((uint8_t*)(mem + addr + i)) = ((uint8_t*)&bleh)[i];
					}

					instrPointer += 3 + 8 + 4 + 8;
				}
				else if (syscall2 == 5)
				{
					int64_t winID = *((int64_t*)((uint64_t)mem + instrPointer + 3));
					int64_t scrID = *((int64_t*)((uint64_t)mem + instrPointer + 11));
					
					Window* tWin = FindWindowWithId(winID);
                    GuiInstance* gui = GetGuiFromWindow(tWin);
                    if (gui != NULL)
                    {
                        gui->SetActiveScreenFromId(scrID);
                    }

					instrPointer += 3 + 8 + 8;
				}
				else if (syscall2 == 6)
				{
					int64_t winID = *((int64_t*)((uint64_t)mem + instrPointer + 3));
					uint64_t addr = *((uint64_t*)((uint64_t)mem + instrPointer + 11));
					
					if (addr >= memLen)
					{
						programEnded = true;
						errCode = 3;
						errMsg = "ADDR IS OUT OF BOUNDS!";
						return;
					}

					Window* tWin = FindWindowWithId(winID);
                    GuiInstance* gui = GetGuiFromWindow(tWin);
                    if (gui != NULL)
                    {
                        if (gui->screen != NULL)
                        {
                            *((int64_t*)(mem + addr)) = (int64_t)gui->screen->id;
                        }                        
                    }

					instrPointer += 3 + 8 + 8;
				}
















				else
				{
					programEnded = true;
					errCode = 1;
					errMsg = "WINDOW SYSCALL IS NOT SUPPORTED";
					return;
				}
			}
			else if (syscall1 == 4)
			{
				if (syscall2 == 1)
				{
					int64_t winId = *((int64_t*)((uint64_t)mem + instrPointer + 3));
					int64_t compId = *((int64_t*)((uint64_t)mem + instrPointer + 11));
					int32_t compType = *((int32_t*)((uint64_t)mem + instrPointer + 19));
					int64_t parentId = *((int64_t*)((uint64_t)mem + instrPointer + 23));
					
					Window* tWin = FindWindowWithId(winId);
                    GuiInstance* gui = GetGuiFromWindow(tWin);
                    if (gui != NULL)
                    {
                        if (parentId == -1)
                        {
                            parentId = gui->screen->id;
                        }

                        if (!gui->CreateComponentWithIdAndParent(compId, (GuiComponentStuff::ComponentType)compType, parentId))
                        {
                            // programEnded = true;
                            // errCode = 4;
                            // errMsg = "COMPONENT CREATION FAILED!";
                            // return;
                        }


                        gui->Render(true);
                    }
					
					instrPointer += 3 + 8 + 8 + 4 + 8;
				}
				else if (syscall2 == 2)
				{
					int64_t winId = *((int64_t*)((uint64_t)mem + instrPointer + 3));
					int64_t compId = *((int64_t*)((uint64_t)mem + instrPointer + 11));
					bool delChild = *((bool*)((uint64_t)mem + instrPointer + 19));

					Window* tWin = FindWindowWithId(winId);
                    GuiInstance* gui = GetGuiFromWindow(tWin);
                    if (gui != NULL)
                    {
                        if (!gui->DeleteComponentWithId(compId, delChild))
                        {
                            // programEnded = true;
                            // errCode = 4;
                            // errMsg = "COMPONENT DELETION FAILED!";
                            // return;
                        }

                        gui->Render(true);

                        // GuiComponentStuff::BaseComponent* c = gui->GetComponentFromId(compId);
                        // if (c != NULL)
                        // {
                        // 	Println("BRUH COMP STILL EXISTS!", "", Colors.bred);
                        // }
                        // else
                        // 	Println("NICE!", "", Colors.bgreen);                        
                    }

					instrPointer += 3 + 8 + 8 + 1;
				}
				else if (syscall2 == 3)
				{
					int64_t winId = *((int64_t*)((uint64_t)mem + instrPointer + 3));
					int64_t compId = *((int64_t*)((uint64_t)mem + instrPointer + 11));
					int32_t attrType = *((int32_t*)((uint64_t)mem + instrPointer + 19));
					int64_t val = *((int64_t*)((uint64_t)mem + instrPointer + 23));

					Window* tWin = FindWindowWithId(winId);
                    GuiInstance* gui = GetGuiFromWindow(tWin);
                    if (gui != NULL)
                    {
                        gui->SetBaseComponentAttribute(compId, (GuiInstanceBaseAttributeType)attrType, val);
                        gui->Render(true);
                    }

					instrPointer += 3 + 8 + 8 + 4 + 8;
				}
				else if (syscall2 == 4)
				{
					int64_t winId = *((int64_t*)((uint64_t)mem + instrPointer + 3));
					int64_t compId = *((int64_t*)((uint64_t)mem + instrPointer + 11));
					int32_t attrType = *((int32_t*)((uint64_t)mem + instrPointer + 19));
					int64_t to = *((int64_t*)((uint64_t)mem + instrPointer + 23));

					if (to >= memLen)
					{
						programEnded = true;
						errCode = 4;
						errMsg = "ADDR IS OUT OF BOUNDS!";
						return;
					}

					Window* tWin = FindWindowWithId(winId);
                    GuiInstance* gui = GetGuiFromWindow(tWin);
                    if (gui != NULL)
                    {
                        uint64_t bleh = gui->GetBaseComponentAttribute(compId, (GuiInstanceBaseAttributeType)attrType);
                        int amt = GetBaseComponentAttributeSize((GuiInstanceBaseAttributeType)attrType);
                        for (int i = 0; i < amt; i++)
                            *((uint8_t*)(mem + to + i)) = ((uint8_t*)&bleh)[i];                        
                    }
					
					instrPointer += 3 + 8 + 8 + 4 + 8;
				}
				else if (syscall2 == 5)
				{
					int64_t winId = *((int64_t*)((uint64_t)mem + instrPointer + 3));
					int64_t compId = *((int64_t*)((uint64_t)mem + instrPointer + 11));
					int32_t attrType = *((int32_t*)((uint64_t)mem + instrPointer + 19));
					int64_t val = *((int64_t*)((uint64_t)mem + instrPointer + 23));

					Window* tWin = FindWindowWithId(winId);
                    GuiInstance* gui = GetGuiFromWindow(tWin);
                    if (gui != NULL)
                    {
                        GuiComponentStuff::BaseComponent* bruhus = gui->GetComponentFromId(compId);
                        if (bruhus != NULL)
                        {
                            if (bruhus->componentType == GuiComponentStuff::ComponentType::TEXT && attrType == 20)
                            {
                                GuiComponentStuff::TextComponent* bleh = ((GuiComponentStuff::TextComponent*)bruhus);

                                if (val >= memLen)
                                {
                                    programEnded = true;
                                    errCode = 3;
                                    errMsg = "STRING IS OUT OF BOUNDS!";
                                    return;
                                }

                                const char* newThing = StrCopy((const char*)(mem + val));
                                const char* oldThing = bleh->text;
                                bleh->text = newThing;

                                _TryFree((void*)oldThing);
                            }
                            else if (bruhus->componentType == GuiComponentStuff::ComponentType::IMAGE_RECT && attrType == 10)
                            {
                                GuiComponentStuff::ImageRectangleComponent* bleh = ((GuiComponentStuff::ImageRectangleComponent*)bruhus);

                                if (val >= memLen)
                                {
                                    programEnded = true;
                                    errCode = 3;
                                    errMsg = "STRING IS OUT OF BOUNDS!";
                                    return;
                                }

                                const char* newThing = StrCopy((const char*)(mem + val));
                                const char* oldThing = bleh->imagePath;
                                bleh->imagePath = newThing;

                                _TryFree((void*)oldThing);
                            }
                            else
                            {
                                gui->SetSpecificComponentAttribute(compId, attrType, val);
                                gui->Render(true);
                            }
                        }                        
                    }

					instrPointer += 3 + 8 + 8 + 4 + 8;
				}
				else if (syscall2 == 6)
				{
					int64_t winId = *((int64_t*)((uint64_t)mem + instrPointer + 3));
					int64_t compId = *((int64_t*)((uint64_t)mem + instrPointer + 11));
					int32_t attrType = *((int32_t*)((uint64_t)mem + instrPointer + 19));
					int64_t to = *((int64_t*)((uint64_t)mem + instrPointer + 23));

					if (to >= memLen)
					{
						programEnded = true;
						errCode = 4;
						errMsg = "ADDR IS OUT OF BOUNDS!";
						return;
					}


                    Window* tWin = FindWindowWithId(winId);
                    GuiInstance* gui = GetGuiFromWindow(tWin);
                    if (gui != NULL)
                    {
                        GuiComponentStuff::BaseComponent* bruhus = gui->GetComponentFromId(compId);
                        if (bruhus != NULL)
                        {
                            if (bruhus->componentType == GuiComponentStuff::ComponentType::TEXT && attrType == 20)
                            {
                                GuiComponentStuff::TextComponent* bleh = ((GuiComponentStuff::TextComponent*)bruhus);

                                int len = StrLen(bleh->text);
                                uint64_t nAddr = (uint64_t)memHandler->MallocMem(len + 1);
                                for (int i = 0; i < len; i++)
                                    mem[nAddr + i] = bleh->text[i];
                                mem[nAddr + len] = 0;

                                *((uint64_t*)(mem + to)) = (uint64_t)nAddr;
                            }
                            else if (bruhus->componentType == GuiComponentStuff::ComponentType::IMAGE_RECT && attrType == 10)
                            {
                                GuiComponentStuff::ImageRectangleComponent* bleh = ((GuiComponentStuff::ImageRectangleComponent*)bruhus);

                                int len = StrLen(bleh->imagePath);
                                uint64_t nAddr = (uint64_t)memHandler->MallocMem(len + 1);
                                for (int i = 0; i < len; i++)
                                    mem[nAddr + i] = bleh->imagePath[i];
                                mem[nAddr + len] = 0;

                                *((uint64_t*)(mem + to)) = (uint64_t)nAddr;
                            }
                            else
                            {
                                uint64_t bleh = gui->GetSpecificComponentAttribute(compId, attrType);
                                int amt = gui->GetSpecificComponentAttributeSize(compId, attrType);
                                for (int i = 0; i < amt; i++)
                                    *((uint8_t*)(mem + to + i)) = ((uint8_t*)&bleh)[i];
                            }
                        }
                    }
	
					instrPointer += 3 + 8 + 8 + 4 + 8;
				}
				


				else
				{
					programEnded = true;
					errCode = 1;
					errMsg = "WINDOW SYSCALL IS NOT SUPPORTED";
					return;
				}
			}
			else if (syscall1 == 5)
			{
				if (syscall2 == 1)
				{
					uint64_t to = *((uint64_t*)((uint64_t)mem + instrPointer + 3));

					if (to >= memLen)
					{
						programEnded = true;
						errCode = 5;
						errMsg = "ADDR IS OUT OF BOUNDS!";
						return;
					}

					*((uint64_t*)(mem + to)) = RND::RandomInt();

					instrPointer += 3 + 8;
				}
				else if (syscall2 == 2)
				{
					uint64_t to = *((uint64_t*)((uint64_t)mem + instrPointer + 3));

					if (to >= memLen)
					{
						programEnded = true;
						errCode = 5;
						errMsg = "ADDR IS OUT OF BOUNDS!";
						return;
					}

					*((double*)(mem + to)) = RND::RandomDouble();

					instrPointer += 3 + 8;
				}
				else if (syscall2 == 3)
				{
					int8_t scan = *((uint8_t*)((uint64_t)mem + instrPointer + 3));
					uint64_t to = *((uint64_t*)((uint64_t)mem + instrPointer + 4));

					if (to >= memLen)
					{
						programEnded = true;
						errCode = 4;
						errMsg = "ADDR IS OUT OF BOUNDS!";
						return;
					}

					*((bool*)(mem + to)) = (bool)envGetKeyState(scan);
					
					instrPointer += 3 + 1 + 8;
				}
				else if (syscall2 == 4)
				{
					int8_t stat = *((uint8_t*)((uint64_t)mem + instrPointer + 3));
					uint64_t to = *((uint64_t*)((uint64_t)mem + instrPointer + 4));

					if (to >= memLen)
					{
						programEnded = true;
						errCode = 4;
						errMsg = "ADDR IS OUT OF BOUNDS!";
						return;
					}

					if (stat >= 0 && stat <= 2)
					{
						MouseState* mState = envGetMouseState();
						if (mState != NULL)
						{
							bool state[3] = {mState->Left, mState->Middle, mState->Right};
							*((bool*)(mem + to)) = state[stat];
							_Free(mState);
						}
					}
					else if (stat >= 3 && stat <= 4)
					{
						int res = 0;
                        MouseState* mState = envGetMouseState();
                        if (mState != NULL)
                        {
                            if (stat == 3)
                                res = mState->MouseX;
                            else if (stat == 4)
                                res = mState->MouseY;

                            _Free(mState);
                        }
						

						*((int*)(mem + to)) = res;
					}

					instrPointer += 3 + 1 + 8;
				}





				else
				{
					programEnded = true;
					errCode = 1;
					errMsg = "OTHER SYSCALL IS NOT SUPPORTED";
					return;
				}
			}







			else
			{
				programEnded = true;
				errCode = 1;
				errMsg = "SYSCALL IS NOT SUPPORTED";
				return;
			}

			//uint64_t jumpAddr = (uint64_t)((void*)mem[instrPointer + 1]);

			//instrPointer = jumpAddr;
		}



		else
		{
			Print("\n<INVALID OPCODE: ");
            Print(to_string(instr));
            Print("!>\n");
			instrPointer++;
		}

	}



}

void PrintVal(DatatypeNumber numType, uint64_t numAddr)
{
	//Println("\n<PRINT>\n", to_string(0), defCol);
	//Println("+ TYPE: {}", to_string((uint8_t)numType), defCol);
	//Println("+ SIZE: {}", to_string(datatypeSizes[(uint8_t)numType]), defCol);
	//Println("+ ADDR: {}", to_string((uint64_t)numAddr), defCol);

	//Print("DATA: ");
	//ShowBytes(datatypeSizes[(uint8_t)numType], numAddr);
	//Println();

	if (numAddr + datatypeSizes[(uint8_t)numType] >= memLen)
	{
		programEnded = true;
		errCode = 1;
		errMsg = "ADDR OUT OF BOUNDS!";
		return;
	}

	if (numType == DatatypeNumber::CHAR)
	{
		char chr = (char)mem[numAddr];
		Print(chr);
	}
	else
	{
		numAddr += (uint64_t)mem;
		if (numType == DatatypeNumber::INT)
			Print(to_string(*((int32_t*)numAddr)));//*((int32_t*)((uint64_t)mem + addrFrom)) = from;
		else if (numType == DatatypeNumber::UINT)
		{
			uint32_t blehus = *((uint32_t*)numAddr);
			Print(to_string((uint64_t)blehus));
		}

		else if (numType == DatatypeNumber::SHORT)
			Print(to_string(*((int16_t*)numAddr)));
		else if (numType == DatatypeNumber::USHORT)
			Print(to_string(*((uint16_t*)numAddr)));

		else if (numType == DatatypeNumber::LONG)
			Print(to_string(*((int64_t*)numAddr)));
		else if (numType == DatatypeNumber::ULONG)
			Print(to_string(*((uint64_t*)numAddr)));

		else if (numType == DatatypeNumber::BOOL)
		{
			if (*((bool*)numAddr))
				Print("true");
			else
				Print("false");
		}

		else if (numType == DatatypeNumber::FLOAT)
			Print(to_string(*((float*)numAddr)));
		else if (numType == DatatypeNumber::DOUBLE)
			Print(to_string(*((double*)numAddr)));
	}
}

void ShowBytes(uint8_t amount, uint64_t addr)
{
	Print("<");
	for (int i = 0; i < amount; i++)
	{
        Print(to_string(*((uint8_t*)((uint64_t)mem + addr + i))));
        Print(" ");
    }
	Print(">");
}

void Cast(DatatypeNumber typeFrom, uint64_t addrFrom, DatatypeNumber typeTo, uint64_t addrTo)
{
	//Println("\n<CAST>\n", to_string(0), defCol);
	//Println("+ FROM:      {}", to_string((uint8_t)typeFrom), defCol);
	//Println("+ FROM SIZE: {}", to_string(datatypeSizes[(uint8_t)typeFrom]), defCol);
	//Println("+ FROM ADDR: {}", to_string((uint64_t)addrFrom), defCol);
	//Println();
	//Println("+ TO:        {}", to_string((uint8_t)typeTo), defCol);
	//Println("+ TO ADDR:   {}", to_string((uint64_t)addrTo), defCol);
	//Println("+ TO SIZE:   {}", to_string(datatypeSizes[(uint8_t)typeTo]), defCol);

	//Print("FROM: ");
	//ShowBytes(datatypeSizes[(uint8_t)typeFrom], addrFrom);
	//Println();

	if (addrFrom + datatypeSizes[(uint8_t)typeFrom] >= memLen ||
		addrTo + datatypeSizes[(uint8_t)typeTo] >= memLen)
	{
		programEnded = true;
		errCode = 1;
		errMsg = "ADDR OUT OF BOUNDS!";
		return;
	}

#pragma region INT
	if (typeFrom == DatatypeNumber::INT)
	{
		int32_t from = *((int32_t*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region UINT
	else if (typeFrom == DatatypeNumber::UINT)
	{
		uint32_t from = *((uint32_t*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region SHORT
	else if (typeFrom == DatatypeNumber::SHORT)
	{
		int16_t from = *((int16_t*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region USHORT
	else if (typeFrom == DatatypeNumber::USHORT)
	{
		uint16_t from = *((uint16_t*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region LONG
	else if (typeFrom == DatatypeNumber::LONG)
	{
		int64_t from = *((int64_t*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region ULONG
	else if (typeFrom == DatatypeNumber::ULONG)
	{
		uint64_t from = *((uint64_t*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region FLOAT
	else if (typeFrom == DatatypeNumber::FLOAT)
	{
		float from = *((float*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region DOUBLE
	else if (typeFrom == DatatypeNumber::DOUBLE)
	{
		double from = *((double*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region CHAR
	else if (typeFrom == DatatypeNumber::CHAR)
	{
		char from = *((char*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion
#pragma region BOOL
	else if (typeFrom == DatatypeNumber::BOOL)
	{
		bool from = *((bool*)((uint64_t)mem + addrFrom));

		if (typeTo == DatatypeNumber::INT)
			*((int32_t*)((uint64_t)mem + addrTo)) = (int32_t)from;
		else if (typeTo == DatatypeNumber::UINT)
			*((uint32_t*)((uint64_t)mem + addrTo)) = (uint32_t)from;

		else if (typeTo == DatatypeNumber::SHORT)
			*((int16_t*)((uint64_t)mem + addrTo)) = (int16_t)from;
		else if (typeTo == DatatypeNumber::USHORT)
			*((uint16_t*)((uint64_t)mem + addrTo)) = (uint16_t)from;

		else if (typeTo == DatatypeNumber::LONG)
			*((int64_t*)((uint64_t)mem + addrTo)) = (int64_t)from;
		else if (typeTo == DatatypeNumber::ULONG)
			*((uint64_t*)((uint64_t)mem + addrTo)) = (uint64_t)from;

		else if (typeTo == DatatypeNumber::CHAR)
			*((char*)((uint64_t)mem + addrTo)) = (char)from;
		else if (typeTo == DatatypeNumber::BOOL)
			*((bool*)((uint64_t)mem + addrTo)) = (bool)from;

		else if (typeTo == DatatypeNumber::FLOAT)
			*((float*)((uint64_t)mem + addrTo)) = (float)from;
		else if (typeTo == DatatypeNumber::DOUBLE)
			*((double*)((uint64_t)mem + addrTo)) = (double)from;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN DATATYPE TO INT!";
			return;
		}
	}
#pragma endregion

	else
	{
		programEnded = true;
		errCode = 1;
		errMsg = "UNKNOWN DATATYPE!";
		return;
	}


	//Print("TO: ");
	//ShowBytes(datatypeSizes[(uint8_t)typeTo], addrTo);
	//Println();
}

void Math(OpNumber opNum, DatatypeNumber typeNum, uint64_t addr1, uint64_t addr2, uint64_t addrRes)
{
	if (addr1 + datatypeSizes[(uint8_t)typeNum] >= memLen ||
		addr1 + datatypeSizes[(uint8_t)typeNum] >= memLen ||
		addrRes + datatypeSizes[(uint8_t)typeNum] >= memLen)
	{
		programEnded = true;
		errCode = 1;
		errMsg = "ADDR OUT OF BOUNDS!";
		return;
	}


	//Println("\n<MATH>\n", to_string(0), defCol);
	//Println("+ OP TYPE:  {}", to_string((uint8_t)opNum), defCol);
	//Println("+ NUM TYPE: {}", to_string((uint8_t)typeNum), defCol);
	//Println("+ NUM SIZE: {}", to_string(datatypeSizes[(uint8_t)typeNum]), defCol);
	//Println("+ ADDR 1:   {}", to_string((uint64_t)addr1), defCol);
	//Println("+ ADDR 2:   {}", to_string((uint64_t)addr2), defCol);
	//Println("+ ADDR RES: {}", to_string((uint64_t)addrRes), defCol);


	//Print("DATA ADDR 1: ");
	//ShowBytes(datatypeSizes[(uint8_t)typeNum], addr1);
	//Println();
	//Print("DATA ADDR 2: ");
	//ShowBytes(datatypeSizes[(uint8_t)typeNum], addr2);
	//Println();


	addr1 += (uint64_t)mem;
	addr2 += (uint64_t)mem;
	addrRes += (uint64_t)mem;


#pragma region INT
	if (typeNum == DatatypeNumber::INT)
	{
		int32_t a = *((int32_t*)addr1);
		int32_t b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((int32_t*)addr2);

		if (opNum == OpNumber::PLUS)
			*((int32_t*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((int32_t*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((int32_t*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
			*((int32_t*)addrRes) = a / b;
		else if (opNum == OpNumber::MOD)
			*((int32_t*)addrRes) = a % b;
		else if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((bool*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((bool*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((bool*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((bool*)addrRes) = a <= b;
		else if (opNum == OpNumber::BIT_AND)
			*((int32_t*)addrRes) = a & b;
		else if (opNum == OpNumber::BIT_OR)
			*((int32_t*)addrRes) = a | b;
		else if (opNum == OpNumber::BIT_NOT)
			*((int32_t*)addrRes) = ~a;
		else if (opNum == OpNumber::BITSHIFT_LEFT)
			*((int32_t*)addrRes) = a << b;
		else if (opNum == OpNumber::BITSHIFT_RIGHT)
			*((int32_t*)addrRes) = a >> b;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion
#pragma region UINT
	else if (typeNum == DatatypeNumber::UINT)
	{
		uint32_t a = *((uint32_t*)addr1);
		uint32_t b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((uint32_t*)addr2);

		if (opNum == OpNumber::PLUS)
			*((uint32_t*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((uint32_t*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((uint32_t*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
			*((uint32_t*)addrRes) = a / b;
		else if (opNum == OpNumber::MOD)
			*((uint32_t*)addrRes) = a % b;
		else if (opNum == OpNumber::BIT_AND)
			*((uint32_t*)addrRes) = a & b;
		else if (opNum == OpNumber::BIT_OR)
			*((uint32_t*)addrRes) = a | b;
		else if (opNum == OpNumber::BIT_NOT)
			*((uint32_t*)addrRes) = ~a;
		else if (opNum == OpNumber::BITSHIFT_LEFT)
			*((uint32_t*)addrRes) = a << b;
		else if (opNum == OpNumber::BITSHIFT_RIGHT)
			*((uint32_t*)addrRes) = a >> b;
		else if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((bool*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((bool*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((bool*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((bool*)addrRes) = a <= b;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion

#pragma region SHORT
	else if (typeNum == DatatypeNumber::SHORT)
	{
		int16_t a = *((int16_t*)addr1);
		int16_t b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((int16_t*)addr2);

		if (opNum == OpNumber::PLUS)
			*((int16_t*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((int16_t*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((int16_t*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
			*((int16_t*)addrRes) = a / b;
		else if (opNum == OpNumber::MOD)
			*((int16_t*)addrRes) = a % b;
		else if (opNum == OpNumber::BIT_AND)
			*((int16_t*)addrRes) = a & b;
		else if (opNum == OpNumber::BIT_OR)
			*((int16_t*)addrRes) = a | b;
		else if (opNum == OpNumber::BIT_NOT)
			*((int16_t*)addrRes) = ~a;
		else if (opNum == OpNumber::BITSHIFT_LEFT)
			*((int16_t*)addrRes) = a << b;
		else if (opNum == OpNumber::BITSHIFT_RIGHT)
			*((int16_t*)addrRes) = a >> b;
		else if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((bool*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((bool*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((bool*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((bool*)addrRes) = a <= b;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion
#pragma region USHORT
	else if (typeNum == DatatypeNumber::USHORT)
	{
		uint16_t a = *((uint16_t*)addr1);
		uint16_t b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((uint16_t*)addr2);

		if (opNum == OpNumber::PLUS)
			*((uint16_t*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((uint16_t*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((uint16_t*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
			*((uint16_t*)addrRes) = a / b;
		else if (opNum == OpNumber::MOD)
			*((uint16_t*)addrRes) = a % b;
		else if (opNum == OpNumber::BIT_AND)
			*((uint16_t*)addrRes) = a & b;
		else if (opNum == OpNumber::BIT_OR)
			*((uint16_t*)addrRes) = a | b;
		else if (opNum == OpNumber::BIT_NOT)
			*((uint16_t*)addrRes) = ~a;
		else if (opNum == OpNumber::BITSHIFT_LEFT)
			*((uint16_t*)addrRes) = a << b;
		else if (opNum == OpNumber::BITSHIFT_RIGHT)
			*((uint16_t*)addrRes) = a >> b;
		else if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((bool*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((bool*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((bool*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((bool*)addrRes) = a <= b;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion

#pragma region LONG
	else if (typeNum == DatatypeNumber::LONG)
	{
		int64_t a = *((int64_t*)addr1);
		int64_t b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((int64_t*)addr2);

		if (opNum == OpNumber::PLUS)
			*((int64_t*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((int64_t*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((int64_t*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
			*((int64_t*)addrRes) = a / b;
		else if (opNum == OpNumber::MOD)
			*((int64_t*)addrRes) = a % b;
		else if (opNum == OpNumber::BIT_AND)
			*((int64_t*)addrRes) = a & b;
		else if (opNum == OpNumber::BIT_OR)
			*((int64_t*)addrRes) = a | b;
		else if (opNum == OpNumber::BIT_NOT)
			*((int64_t*)addrRes) = ~a;
		else if (opNum == OpNumber::BITSHIFT_LEFT)
			*((int64_t*)addrRes) = a << b;
		else if (opNum == OpNumber::BITSHIFT_RIGHT)
			*((int64_t*)addrRes) = a >> b;
		else if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((bool*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((bool*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((bool*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((bool*)addrRes) = a <= b;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion
#pragma region ULONG
	else if (typeNum == DatatypeNumber::ULONG)
	{
		uint64_t a = *((uint64_t*)addr1);
		uint64_t b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((uint64_t*)addr2);

		if (opNum == OpNumber::PLUS)
			*((uint64_t*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((uint64_t*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((uint64_t*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
			*((uint64_t*)addrRes) = a / b;
		else if (opNum == OpNumber::MOD)
			*((uint64_t*)addrRes) = a % b;
		else if (opNum == OpNumber::BIT_AND)
			*((uint64_t*)addrRes) = a & b;
		else if (opNum == OpNumber::BIT_OR)
			*((uint64_t*)addrRes) = a | b;
		else if (opNum == OpNumber::BIT_NOT)
			*((uint64_t*)addrRes) = ~a;
		else if (opNum == OpNumber::BITSHIFT_LEFT)
			*((uint64_t*)addrRes) = a << b;
		else if (opNum == OpNumber::BITSHIFT_RIGHT)
			*((uint64_t*)addrRes) = a >> b;
		else if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((bool*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((bool*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((bool*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((bool*)addrRes) = a <= b;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion

#pragma region FLOAT
	else if (typeNum == DatatypeNumber::FLOAT)
	{
		float a = *((float*)addr1);
		float b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((float*)addr2);

		if (opNum == OpNumber::PLUS)
			*((float*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((float*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((float*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
		{
			if (b != 0)
				*((float*)addrRes) = a / b;
		}
		else if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((bool*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((bool*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((bool*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((bool*)addrRes) = a <= b;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion
#pragma region DOUBLE
	else if (typeNum == DatatypeNumber::DOUBLE)
	{
		double a = *((double*)addr1);
		double b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((double*)addr2);

		if (opNum == OpNumber::PLUS)
			*((double*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((double*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((double*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
		{
			if (b != 0)
				*((double*)addrRes) = a / b;
		}
		else if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((bool*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((bool*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((bool*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((bool*)addrRes) = a <= b;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion


#pragma region CHAR
	else if (typeNum == DatatypeNumber::CHAR)
	{
		int8_t a = *((int8_t*)addr1);
		int8_t b = 0;
		if (opNum != OpNumber::BIT_NOT)
			b = *((int8_t*)addr2);

		if (opNum == OpNumber::PLUS)
			*((int8_t*)addrRes) = a + b;
		else if (opNum == OpNumber::MINUS)
			*((int8_t*)addrRes) = a - b;
		else if (opNum == OpNumber::MULT)
			*((int8_t*)addrRes) = a * b;
		else if (opNum == OpNumber::DIVIDE)
			*((int8_t*)addrRes) = a / b;
		else if (opNum == OpNumber::MOD)
			*((int8_t*)addrRes) = a % b;
		else if (opNum == OpNumber::BIT_AND)
			*((int8_t*)addrRes) = a & b;
		else if (opNum == OpNumber::BIT_OR)
			*((int8_t*)addrRes) = a | b;
		else if (opNum == OpNumber::BIT_NOT)
			*((int8_t*)addrRes) = ~a;
		else if (opNum == OpNumber::BITSHIFT_LEFT)
			*((int8_t*)addrRes) = a << b;
		else if (opNum == OpNumber::BITSHIFT_RIGHT)
			*((int8_t*)addrRes) = a >> b;
		else if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::GREATER)
			*((bool*)addrRes) = a > b;
		else if (opNum == OpNumber::GREATER_EQUALS)
			*((bool*)addrRes) = a >= b;
		else if (opNum == OpNumber::LESS)
			*((bool*)addrRes) = a < b;
		else if (opNum == OpNumber::LESS_EQUAL)
			*((bool*)addrRes) = a <= b;


		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion

#pragma region BOOL
	else if (typeNum == DatatypeNumber::BOOL)
	{
		bool a = *((bool*)addr1);
		bool b = false;
		if (opNum != OpNumber::BOOL_NOT)
			b = *((bool*)addr2);

		if (opNum == OpNumber::EQUALS)
			*((bool*)addrRes) = a == b;
		else if (opNum == OpNumber::NOT_EQUALS)
			*((bool*)addrRes) = a != b;
		else if (opNum == OpNumber::BOOL_AND)
			*((bool*)addrRes) = a && b;
		else if (opNum == OpNumber::BOOL_OR)
			*((bool*)addrRes) = a || b;
		else if (opNum == OpNumber::BOOL_NOT)
			*((bool*)addrRes) = !a;

		else
		{
			programEnded = true;
			errCode = 1;
			errMsg = "UNKNOWN OPERATION!";
			return;
		}
	}
#pragma endregion

	else
	{
		programEnded = true;
		errCode = 1;
		errMsg = "UNKNOWN DATATYPE!";
		return;
	}


	//Print("DATA RES:    ");
	//ShowBytes(8, addrRes - (uint64_t)mem);
	//Println();
}

