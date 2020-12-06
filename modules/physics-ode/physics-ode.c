// SPDX-License-Identifier: GPL-3.0-or-later

#include <ode/ode.h>

#include "virreled/time.h"

dWorldID      g_world;
dSpaceID      g_space;
dJointGroupID g_contact_joint_group;
dReal g_stepsize = 0.001;

struct physics_object
{
  dBodyID id;
  dMass   mass;
  dGeomID geometry;
  float*  position;
};

#define MAX_PHYSICS_OBJECTS 10
const int             g_physics_objects_count_max = MAX_PHYSICS_OBJECTS;
int                   g_physics_objects_count     = 0;
struct physics_object g_physics_objects[MAX_PHYSICS_OBJECTS];

#define MAX_CONTACT_JOINTS 10

void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
  dContact contacts[MAX_CONTACT_JOINTS];
  int contact_point_count = dCollide(o1, o2, MAX_CONTACT_JOINTS, &contacts[0].geom,sizeof(dContact));
  for (int i = 0; i < contact_point_count; i++)
  {
    dJointID contactJoint = dJointCreateContact(g_world, g_contact_joint_group, &contacts[i]);
    dJointAttach(contactJoint, dGeomGetBody(contacts[i].geom.g1), dGeomGetBody(contacts[i].geom.g2));
  }
}

void physics_initialize()
{
  dInitODE2(0);

  g_world = dWorldCreate();
  g_space = dHashSpaceCreate(0);

  g_contact_joint_group = dJointGroupCreate(0);

  dWorldSetGravity(g_world, 0, 0, -9.81);
}

void physics_processFrame()
{
  if(g_physics_objects_count <= 0)
  {
    return;
  }

  int frameDurationMillis = time_getIntervalMilliseconds();
  int stepDurationMillis  = g_stepsize * 1000;
  for(int i = 0; i < frameDurationMillis; i += stepDurationMillis)
  {
    dSpaceCollide(g_space, 0, nearCallback);
    dWorldQuickStep(g_world, g_stepsize);

    dJointGroupEmpty(g_contact_joint_group);
  }

  for(int i = 0; i < g_physics_objects_count; i++)
  {
    struct physics_object* pObject   = &g_physics_objects[i];

    if(pObject->id == 0) continue;

    const dReal*           position  = dBodyGetPosition(pObject->id);

    pObject->position[0] = position[0];
    pObject->position[1] = position[2];
    pObject->position[2] = position[1];
  }
}

void physics_deinitialize()
{
  dJointGroupDestroy(g_contact_joint_group);
  dSpaceDestroy(g_space);
  dWorldDestroy(g_world);
  dCloseODE();
}

struct physics_object* physics_createBox(float* position, float mass)
{
  struct physics_object* pObject = &g_physics_objects[g_physics_objects_count];
  pObject->position = position;
  pObject->id = 0;

  pObject->geometry = dCreateBox(g_space, 1, 1, 1);
  if(mass > 0)
  {
    pObject->id = dBodyCreate (g_world);
    dBodySetPosition(pObject->id, pObject->position[0], pObject->position[2], pObject->position[1]);
    dMassSetBoxTotal(&pObject->mass, mass, 1, 1, 1);
    dBodySetMass(pObject->id, &pObject->mass);
    dGeomSetBody(pObject->geometry, pObject->id);
  }
  else
  {
    dGeomSetPosition(pObject->geometry, pObject->position[0], pObject->position[2], pObject->position[1]);
  }

  ++g_physics_objects_count;

  return pObject;
}
