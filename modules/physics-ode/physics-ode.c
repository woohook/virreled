// SPDX-License-Identifier: GPL-3.0-or-later

#include <ode/ode.h>

dWorldID g_world;
dReal g_stepsize = 0.001;

dBodyID g_testobject_id;
dMass   g_testobject_mass;

float g_testobject_height = 0;

void physics_initialize()
{
  dInitODE2(0);

  g_world = dWorldCreate();

  dWorldSetGravity(g_world, 0, 0, -9.81);

  g_testobject_id = dBodyCreate (g_world);
  dBodySetPosition(g_testobject_id, 0, 0, 3.0);
  dMassSetSphere(&g_testobject_mass, 1, 1);
  dBodySetMass(g_testobject_id, &g_testobject_mass);
}

void physics_processFrame()
{
  dWorldQuickStep(g_world, g_stepsize);

  const dReal* testobject_position = dBodyGetPosition(g_testobject_id);
  g_testobject_height              = testobject_position[2];
}

void physics_deinitialize()
{
  dBodyDestroy(g_testobject_id);
  dWorldDestroy(g_world);
  dCloseODE();
}
