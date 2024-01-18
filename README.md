# MaslOS-2
MaslOS but 2

This is essentially MaslOS on the outside, but completely different on the inside.

The main difference being a working scheduler and support for ELF execution!

(Ofc still WIP but a bunch of stuff works already!)


Of course it is 64bit and boots using BIOS.
And it mostly works on real hardware.

## Features
 + Basics
   - Scheduler (working)
   - Elf loading (working, rarely crashes at the start for no apparent reason?)
   - User Space (working)
   - Inter Process Communication (working)
   - Stdio using IPC (working)
 + GUI
   - MaslOS GUI Framework port (working)
   - Mouse/Keyboard interaction (working)
   - MaslOS Desktop / WM (mostly working)
   - Taskbar (mostly working)
   - Start Menu (mostly working)
 + Apps
   - Tic Tac Toe (working)
   - Flappy Rocc (working)
   - Snake (soon)
   - Terminal (basics be working)
   - Explorer (working)
   - Image Viewer (working)
   + MAAB interpreter (working)
     - Pong
     - Maalbrot
   - Notepad (basics be working)
   - Paint (mostly working)
   - Doom (it exists, but no input yet)
   - MicroEdit (somewhat working)
   - 3D Cube Renderer
   + Dialogs
     - Open File Dialog (working)
     - Save File Dialog (soon)
     - Generic Information Dialog (soon)
     - Generic Text Input Dialog (soon)
     - Generic Information YES/NO Dialog (soon)
   - Other Apps ported from MaslOS and some extra ones (not yet)
 + Other
   - SMP (not added yet)
   - idk more stuff

## How to use
You can click on the start menu in the bottom left corner to see a list of things you can launch

There also some debug shortcuts:
 - Press F10 to open the explorer
 - Press F12 to open a shell (there is a help command)
 - Press F11 to redraw the desktop (eg. If you get an error)
 - Press F5 to launch an empty elf
 - Press F6 to launch a mini window test
 - Press ALT + F4 to force close a program
 

## How to build (Linux)
 - Install `qemu-system` if you haven't.
 - Install `make` if you haven't.
 - Install `gcc` if you haven't.
 - Install `g++` if you haven't.
 - Install `binutils` if you haven't.
 - Install `nasm` if you haven't.
 - Install `xorriso` if you haven't.


Go into the MaslOS2 Folder and run the `CRUN.sh` file

If you are trying this and getting weird compiler/linker errors, please let me know!

gcc and ld like to break things in newer versions for some reason.  *:D*

## How to run (No need to build)

### How to run (Linux) 
Install QEMU if you haven't.

Download/clone the repo.

Run the `cDisk.sh` file to generate an empty disk so QEMU can boot correctly.

Run the `JUST RUN OS.sh` file to run it!

### How to run (Windows)

Install QEMU if you haven't.

Add QEMU To your PATH variable. [If you need help, click here!](https://linuxhint.com/qemu-windows/)

Then download/clone the repo and run the `run.cmd` file

## Contributing
I am still working on quite a few things that need to be done for MaslOS2 to be in a good state.

Meaning that not everything is 100% implemented yet and some needed things could be missing.

The Docs can be found [here](https://github.com/marceldobehere/MaslOS-2/wiki/Application-Development-for-MaslOS2).

If you want to contribute by adding your own little apps/tools/games/ports I would be very happy!

If you need any help / have any questions or are curious about MaslOS2, feel free to contact me per discord. (@marceldarcel)






## Screenshots
![Showing off some apps](/images/demo.png)
![The start menu](/images/wm%207.png)
![Doom](/images/doom-test.png)
![More apps ported](/images/some%20stuff.jpg)
![Working more on the terminal](/images/wm%206.png)
![Working on the terminal](/images/terminal%201.gif)
![Taskbar yes](/images/wm%205.png)
![WM almost on par with MaslOS](/images/wm%204.png)
![More WM progress](/images/wm%203.png)
![WM progress](/images/wm%202.png)
![Beginning to work on WM](/images/start%20of%20wm%201.png)
![A Bootscreen yes](/images/img1.png)




## Thanks to:
 - [MaslOS](https://github.com/marceldobehere/MaslOS)
 - [VisualOS](https://github.com/nothotscott/VisualOS)
 - [MicroOS](https://github.com/Glowman554/MicroOS)
 - [SAF](https://github.com/chocabloc/saf)
 
