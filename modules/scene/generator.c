// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define GENERATOR_SHAPE_UNDEFINED 0
#define GENERATOR_SHAPE_BOX       1

struct entity* entity_create();
float* entity_getPosition(struct entity*);

void model_create(float* position);
void model_add_vertex(float x, float y, float z);
void model_add_face(int v1, int v2, int v3);
void model_add_material(float red, float green, float blue);
void model_select_material(int materialIndex);

struct physics_object* physics_createBox(float* position, float* extents, float mass);

void generator_createEntity(const char* parameters)
{
  struct entity* pEntity = entity_create();
  float* position = entity_getPosition(pEntity);
  position[0] = 0;
  position[1] = 0;
  position[2] = 0;
  float extents[3] = {1,1,1};
  float color[]    = {1,0,1};

  float mass = 0;
  int shape = GENERATOR_SHAPE_UNDEFINED;

  int  matchcount  = 0;
  char property[1024];
  property[1023] = 0;

  float model_x, model_y, model_z;
  float rx = 0, ry = 0, rz = 0;

  char* paramStr = 0;

  paramStr = strstr(parameters, "position=(");
  if(paramStr != 0)
  {
    float x, y, z;
    if(3 == sscanf(paramStr, "position=(x=%f, y=%f, z=%f)", &x, &y, &z))
    {
      position[0] = x;
      position[1] = y;
      position[2] = z;
    }
  }

  paramStr = strstr(parameters, "extents=(");
  if(paramStr != 0)
  {
    float x, y, z;
    if(3 == sscanf(paramStr, "extents=(x=%f, y=%f, z=%f)", &x, &y, &z))
    {
      extents[0] = x;
      extents[1] = y;
      extents[2] = z;
    }
  }

  paramStr = strstr(parameters, "color=(");
  if(paramStr != 0)
  {
    float red, green, blue;
    if(3 == sscanf(paramStr, "color=(red=%f, green=%f, blue=%f)", &red, &green, &blue))
    {
      color[0] = red;
      color[1] = green;
      color[2] = blue;
    }
  }

  paramStr = strstr(parameters, "mass=");
  if(paramStr != 0)
  {
    float mass_read;
    if(1 == sscanf(paramStr, "mass=%f", &mass_read))
    {
      mass = mass_read;
    }
  }

  paramStr = strstr(parameters, "shape=");
  if(paramStr != 0)
  {
    if(1 == sscanf(paramStr, "shape=%1023[^ ]", &property))
    {
      if(0 == strncmp(property, "box", 3))
      {
        shape = GENERATOR_SHAPE_BOX;
      }
    }
  }

  if(shape != GENERATOR_SHAPE_UNDEFINED)
  {
    model_create(position);

    model_add_material(color[0], color[1], color[2]);
    model_select_material(1);

    model_add_vertex(0.5, 0.5,  -0.5);
    model_add_vertex(0.5, 0.5,   0.5);
    model_add_vertex(0.5, -0.5,  0.5);
    model_add_vertex(0.5, -0.5, -0.5);

    model_add_vertex(-0.5, 0.5,  -0.5);
    model_add_vertex(-0.5, 0.5,   0.5);
    model_add_vertex(-0.5, -0.5,  0.5);
    model_add_vertex(-0.5, -0.5, -0.5);

    model_add_face(1, 2, 3);
    model_add_face(3, 4, 1);
    model_add_face(7, 6, 5);
    model_add_face(8, 7, 5);
    model_add_face(2, 6, 7);
    model_add_face(7, 3, 2);
    model_add_face(5, 1, 4);
    model_add_face(4, 8, 5);
    model_add_face(6, 2, 1);
    model_add_face(6, 5, 1);
    model_add_face(7, 3, 4);
    model_add_face(8, 7, 4);

    physics_createBox(position, extents, mass);
  }
}
