# MaslOS-2
MaslOS but 2

This is very very WIP!

I plan on this being like MaslOS from the outside, but it being completely different internally.


Of course it is 64bit and boots using BIOS.
And it mostly works on real hardware.

## Goals
 + Basics
   - Scheduler (working)
   - Elf loading (working)
   - User Space (working)
   - Inter Process Communication (working)
   - Stdio using IPC (soon)
 + GUI
   - MaslOS GUI Framework port (working)
   - Mouse/Keyboard interaction (mostly working)
   - MaslOS Desktop / WM (mostly working)
   - Taskbar (mostly working)
 + Apps
   - Tic Tac Toe (working)
   - Flappy Rocc (soon)
   - Snake (soon)
   - Terminal (basics be working)
   - Explorer (working)
   - Notepad (basics be working)
   - Paint (mostly working)
   + MAAB interpreter (mostly working / output is currently routed to serial until i add stdio)
     - Pong
   - Doom (soon)
   - Other Apps ported from MaslOS and some extra ones (not yet)
 + Other
   - SMP (not added yet)
   - idk more stuff


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

## How to run (Windows)

Install QEMU if you haven't.

Add QEMU To your PATH variable. [If you need help, click here!](https://linuxhint.com/qemu-windows/)

Then download/clone the repo and run the `run.cmd` file


## Screenshots
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
 
