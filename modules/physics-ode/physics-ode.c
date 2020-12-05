// SPDX-License-Identifier: GPL-3.0-or-later

#include <ode/ode.h>

dWorldID g_world;
dReal g_stepsize = 0.001;

struct physics_object
{
  dBodyID id;
  dMass   mass;
  float*  position;
};

#define MAX_PHYSICS_OBJECTS 10
const int             g_physics_objects_count_max = MAX_PHYSICS_OBJECTS;
int                   g_physics_objects_count     = 0;
struct physics_object g_physics_objects[MAX_PHYSICS_OBJECTS];

void physics_initialize()
{
  dInitODE2(0);

  g_world = dWorldCreate();

  dWorldSetGravity(g_world, 0, 0, -9.81);
}

void physics_processFrame()
{
  if(g_physics_objects_count <= 0)
  {
    return;
  }

  dWorldQuickStep(g_world, g_stepsize);

  for(int i = 0; i < g_physics_objects_count; i++)
  {
    struct physics_object* pObject   = &g_physics_objects[i];
    const dReal*           position  = dBodyGetPosition(pObject->id);

    pObject->position[0] = position[0];
    pObject->position[1] = position[2];
    pObject->position[2] = position[1];
  }
}

void physics_deinitialize()
{
  for(int i = 0; i < g_physics_objects_count; i++)
  {
    dBodyDestroy(g_physics_objects[i].id);
  }

  dWorldDestroy(g_world);
  dCloseODE();
}

struct physics_object* physics_createBox(float* position)
{
  struct physics_object* pObject = &g_physics_objects[g_physics_objects_count];
  pObject->position = position;

  pObject->id = dBodyCreate (g_world);
  dBodySetPosition(pObject->id, pObject->position[0], pObject->position[2], pObject->position[1]);
  dMassSetBox(&pObject->mass, 1, 1, 1, 1);
  dBodySetMass(pObject->id, &pObject->mass);

  ++g_physics_objects_count;

  return pObject;
}
