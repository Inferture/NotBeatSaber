# NotBeatSaber
Port of the [rythm game I made in processing](https://github.com/Inferture/NotBeatSaber-processing) in C with SDL2, for PC and Switch.


# Usage


## Playing:
This mode is made to play an existing map.

For the music to be available, there must be a file music.mp3 and a pattern file music.pattern in the data folder.

For example, if the music is **"MyAwesomeMusic"**, there must be a .mp3 **"MyAwesomeMusic.mp3"** and a pattern **"MyAwesomeMusic.pattern"** in the data folder. 

![](https://media.giphy.com/media/lN9geFOynllLuaEC7S/giphy.gif)




## Saving:
This game is made to create a map.

For this, the  music must be a valid .mp3 inside the data folder.

For example, if the music is **"MeltyBlood"**, there must be a .mp3 **"MeltyBlood.mp3"** in the data folder. 

![](https://media.giphy.com/media/XgYhMA1uUcpjhx648r/giphy.gif)

You should play the music with the beat (Press a button exactly on beat so that on Playing mode, the player should also play it on beat).


## PC setup

Just download the release.zip and put your MP3s in the data folder. 

## Switch setup

Put the data folder at the root of your sdcard and launch the NotBeatSaber.nro. 
If you are using Ryujinx, use File-> Open Ryujinx folder and place the data folder inside the sdcard folder (the hierarchy will be *Ryujinx folder*/sdcard/data).

Note: for now, patterns made in the PC version don't seem to work on the switch but the ones made on the switch seem to work on PC.

## Controls:

| Action                  | Switch        | PC  |
| ------------------------|:-------------:|:--------:|
| Up (menu)               | DPAD UP       | Up arrow   |
| Down (menu)             | DPAD DOWN     | Down arrow |
| Select (menu)           | B BUTTON      | Enter      |
| Right block             | A BUTTON      | Right arrow|
| Top-Right block         | X BUTTON      | Up arrow   |
| Bottom-Right block      | B BUTTON      | Down arrow |
| Left block              | DPAD LEFT     | Q / A      |
| Top-Left block          | DPAD UP       | Z / W      |
| Bottom-Left block       | DPAD DOWN     | S          |
| Rewind (Creating mode)  | LEFT TRIGGER  | Left arrow |
| Save (Creating mode)    | PLUS BUTTON   | G          |

Block actions mean slicing the block in Playing mode and adding the block in Creating mode.





## Add a music:
Add a file .mp3 in the data folder and map it by selecting "Make a pattern" in the menu and choosing the name of your file.
Don't forget to save with g (PC) / + (Switch) when you finish the mapping !



# Compilation

## PC
To compile it on PC you will need the libraries: SDL2, SDL2_mixer, SDL2_ttf and SDL2_gfx.
You can either open the project with code blocks (which should be setup with SDL2) or use the Makefile (command: make).
Should you use the Makefile, make sure you change the variables SDL_INCLUDE and SDL_LIB at the top to fit your SDL2 installation.
I used mingw-32g++.exe to compile. If you use another compiler, make sure you have the appropriate libraries (and dlls) and change the compiler in the Makefile.

If you compile with codeblocks, the executable will be in bin/Release or bin/Debug depending on wether you're in Release or Debug mode.
If you compile with the Makefile, it will be in bin/PC.

## Switch
You will need to use [devkitPro](https://devkitpro.org/wiki/Getting_Started)
Once it's installed, you can launch Msys2 and using pacman, install switch-sdl2, switch-sdl2_gfx, switch-sdl2_mixer, switch-sdl2_ttf libraries.
Still in Msys2, use make -f Makefile.switch to compile for the switch.
The executable will be in bin/switch (you should have the executable and the data in bin/switch/NotBeatSaber).

