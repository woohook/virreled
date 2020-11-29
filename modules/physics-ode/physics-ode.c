// SPDX-License-Identifier: GPL-3.0-or-later

#include <ode/ode.h>

dWorldID g_world;
dReal g_stepsize = 0.01;

void physics_initialize()
{
  dInitODE2(0);

  g_world = dWorldCreate();

  dWorldSetGravity(g_world, 0, 0, -9.81);
}

void physics_processFrame()
{
  dWorldQuickStep(g_world, g_stepsize);
}

void physics_deinitialize()
{
  dWorldDestroy(g_world);
  dCloseODE();
}
