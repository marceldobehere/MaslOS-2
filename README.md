# MaslOS-2
MaslOS but 2

This is very very WIP!

I plan on this being like MaslOS from the outside, but it being completely different internally.


Of course it is 64bit and boots using BIOS.
And it mostly works on real hardware.

## Goals
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
 + Apps
   - Tic Tac Toe (working)
   - Flappy Rocc (working)
   - Snake (soon)
   - Terminal (basics be working)
   - Explorer (working)
   + MAAB interpreter (working)
     - Pong
     - Maalbrot
   - Notepad (basics be working)
   - Paint (mostly working)
   - Doom (soon)
   - MicroEdit (somewhat working)
   - 3D Cube Renderer
   - Other Apps ported from MaslOS and some extra ones (not yet)
 + Other
   - SMP (not added yet)
   - idk more stuff

## How to use
 - Press F10 to open the explorer
 - Press F12 to open a shell (there is a help command)
 - Press F11 to redraw the desktop (eg. If you get an error)

## How to build (Linux)
 - Install QEMU if you haven't.
 - Install GNU MAKE if you haven't.
 - Install GCC/LD if you haven't.
 - Install NASM if you haven't.


Go into the MaslOS2 Folder and run the `CRUN.sh` file

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


If you want to contribute by adding your own little apps/tools/games/ports I would be very happy!

If you need any help / have any questions or are curious about MaslOS2, feel free to contact me per discord. (@marceldarcel)




## Screenshots
![Showing off some apps](/images/demo.png)
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
 
