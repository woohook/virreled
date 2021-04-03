Simcar 5.2.0 - Car simulation with software 3D rendering (no 3D
               acceleration required or available)

Copyright 2010-2014 Matei Petrescu


The files here, except for "SDL.dll", "src/COPYING" and the files
from directory "newdata", are available under the GNU GPL license,
version 3 (see the file "src/COPYING" for details). The programs
use the "SDL 1.2" library, available at www.libsdl.org, which is
distributed under the GNU LGPL license, version 2 (www.gnu.org).
The files from directory "newdata" can be modified and redistributed
according to the terms of the Free Art License. More details are in
"newdata/COPYING.txt"

The recommendations below are for GNU/Linux, but should work for any
Unix-type system which has the GNU tools installed.

Some executable files are already available for an operating system
used by many people (whose lives would be better without computers):
wsimcar.exe, wsim-js.exe and wreplay.exe. They were compiled with
Wine (http://winehq.org) and OpenWatcom (http://openwatcom.org) and
tested on GNU/Linux (Fedora 9) with Wine 1.5.17. If you're using
GNU/Linux or any other Unix-type system, just delete them.


     1. SOME OBSERVATIONS
     2. INSTALLATION
     3. RUNNING
     4. JOYSTICK / STEERING WHEEL
     5. REPLAY
     6. MAKING NEW TRACKS OR CARS


1. SOME OBSERVATIONS

  * The replay application has no sound. This is a design feature.

  * Accelerating while the car is in the air pushes the front of the
    car up and the rear down (i.e. if the wheels are pushed in one
    direction, the car is pushed in the other direction). This is
    the only way in which the car can be controlled during jumps and
    is necessary for obtaining a good position at landing (watch the
    replays for an illustration of that).

  * This game looks and plays relatively similar to "Skunks" from
    http://skunks.sf.net, which is based on Open Dynamics Engine
    ( http://www.ode.org ) but there are some significant differences.


2. INSTALLATION

SDL 1.2 or SDL 2 library is required. Make sure you have it installed.
You should install it from the sources available at www.libsdl.org or,
at least, get a "sdl-devel" or "sdl2-devel" package for your
distribution.

Then open a terminal/console window and change the current
directory to "simcar-5.2.0", using the command "cd", and type:

./comp

.. if you have SDL 1.2, or:

./comp2

.. if you have SDL 2. If it doesn't work, try:

chmod 755 *

.. or check the contents of the file "comp". You can modify the
compiler flags in "comp", according to your system. If you have an
old processor (Pentium 2), remove "-mfpmath=sse" and replace
"pentium3" with "pentium2".

For more options,the file "src/config.h" may be edited, which
facilitates the selection of the display resolution, viewing angle,
colours and other parameters.


3. RUNNING

First, open a terminal/console window and change the current
directory to "simcar-5.2.0", using the command "cd".

After compilation, you can start the game by typing:

./simcar CARFILE TRACKFILE

Examples:

./simcar cars/car1 tracks/track1

./simcar cars/car3 tracks/track6

28 tracks are available in the directory 'tracks'.

Tracks 23 ... 28 were imported from TORCS ( http://torcs.sf.net , more
details in directory "newdata").

The parameters of the cars can easily be modified. Below are a few
details about the 4 cars:

car1: open wheel, rear wheel drive, with wings which generate downforce
car2: open wheel, rear wheel drive, without wings
car3: rear wheel drive
car4: 4 wheel drive

All cars are electric and therefore don't need gearboxes. The 3D models
were imported from TORCS ( http://torcs.org ) and Speed Dreams
( http://speed-dreams.org ) and can be modified and redistributed
according to the terms of the Free Art License.

The buttons which control the game are:

Q or UP - accelerate
A or DOWN - brake
O or LEFT - left
P or RIGHT - right
SPACE - handbrake
R - reverse
C - change camera
N and M - rotate camera
ESCAPE - exit


4. JOYSTICK / STEERING WHEEL

If you want to use a joystick or steering wheel, start the game
with:

./sim-js CARFILE TRACKFILE

The joystick configuration is in the file "config/joystick". The
settings there should be appropriate for the case in which you have
a joystick and want to use buttons 1 and 2 for acceleration and
brake. If you have a steering wheel and want to use the pedals, the
following modifications are suggested:

sensx 1
sensy 1
deadx 600
deady 3000
nonlinear 0

I only tested joystick support in Fedora Core 6 - Fedora 12, where
the following commands could be used for calibration:

jscal -c /dev/js0

jscal -c /dev/input/js0


5. REPLAY

A replay is recorded in 'replays/rep1'. To keep it from being
overwritten, you can rename it to something else.

To watch a replay:

./replay REPLAYFILE

Example:

./replay replays/rep1

Only buttons C, N, M and ESCAPE can be used during replays.

6 replays recorded by me are in the directory 'replays' (rep2 ... rep7).
Watch them.


6. MAKING NEW TRACKS OR CARS

Look in the file "README" from directory "src/aux/newtrax" for details
about making new tracks. It's easier than with any track editor of any
other game.

The files which define the parameters of the cars can also be edited.
After "objects n", the numbers on each of the next n lines have the
following significances:

1st: object type, from the list above;

2nd: object function (1 - body of vehicle, 3 - motor wheel,
     4 - steering wheel, 5 - motor and steering wheel, 6 - wheel);

3rd, 4th and 5th: coordinates on X, Y and Z;

last: friction coefficient.

Axes X, Y and Z are like this (not like in OpenGL):

^ X   __
|     /| Z
|    /
|   /
|  /
| /      Y
|-------->
