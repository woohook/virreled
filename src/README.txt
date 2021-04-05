Simcar 5.2.0 - Car simulation with software 3D rendering (no 3D
               acceleration required or available)

Copyright 2010-2014 Matei Petrescu


The files here, except for "src/COPYING", are available under the GNU GPL license,
version 3 (see the file "src/COPYING" for details). 
The SDL library is available at www.libsdl.org
under the GNU LGPL license, version 2 (www.gnu.org).

The recommendations below are for GNU/Linux, but should work for any
Unix-type system which has the GNU tools installed.

     1. SOME OBSERVATIONS
     2. INSTALLATION
     3. RUNNING


1. SOME OBSERVATIONS

  * Accelerating while the car is in the air pushes the front of the
    car up and the rear down (i.e. if the wheels are pushed in one
    direction, the car is pushed in the other direction). This is
    the only way in which the car can be controlled during jumps and
    is necessary for obtaining a good position at landing

  * This game looks and plays relatively similar to "Skunks" from
    http://skunks.sf.net, which is based on Open Dynamics Engine
    ( http://www.ode.org ) but there are some significant differences.


2. INSTALLATION

SDL 2 library is required. Make sure you have it installed.
You should install it from the sources available at www.libsdl.org or,
at least, get a "sdl-devel" or "sdl2-devel" package for your
distribution.

Then open a terminal/console window and change the current
directory to "simcar-5.2.0", using the command "cd", and type:

./comp2

.. if you have SDL 2. If it doesn't work, try:

chmod 755 *

.. or check the contents of the file "comp2". You can modify the
compiler flags in "comp2", according to your system. If you have an
old processor (Pentium 2), remove "-mfpmath=sse" and add "-march=pentium2".

For more options,the file "src/config.h" may be edited, which
facilitates the selection of the display resolution, viewing angle,
colours and other parameters.


3. RUNNING

First, open a terminal/console window and change the current
directory to "simcar-5.2.0", using the command "cd".

After compilation, you can start the game by typing:

./simcar CARFILE TRACKFILE

Examples:

./simcar data/car0/car0 tracks/track1

./simcar data/car0/car0 tracks/track6

22 tracks are available in the directory 'tracks'.

There is currently one car available:

car0: front wheel drive

All cars are electric and therefore don't need gearboxes.

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
