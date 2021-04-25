# virreled

I was looking for a title for a little 3D game, something that sounded like "world". And I ended up with "virreled".

Currently virreled is just a fork of Victor Matei Petrescu's [simcar](https://sourceforge.net/projects/simple3d/) game
* without sound
* without replays
* without the high-detail TORCS/Speed Dreams models
* without dependencies to the SDL library

for Linux and Windows

## Compiling
### On Linux
You need a C compiler (e.g. gcc), the C standard library headers (e.g. libc-dev or glibc-headers), and the Xlib headers (libx11-dev or -devel).

Compile with:

> _gcc src/virreled.c -o virreled -lm -lX11 -O3_

or use the build.sh script.

### On Windows
You need a C compiler (e.g. tcc), the C standard library headers, and the windows headers.

Compile with:
> tcc.exe src/virreled.c

## Running the game
Start the game by specifying car and track:

### On Linux
> ./virreled cars/car4 tracks/track22

### On Windows
> virreled.exe cars/car4 tracks/track22

### Setting the window size
You can optionally specify the window size by appending the desired width and height in pixels as parameters
> virreled cars/car4 tracks/track22 1024 768
