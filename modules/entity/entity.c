// SPDX-License-Identifier: GPL-3.0-or-later

float g_identity[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};

struct entity
{
  float transform[16];
};

#define MAX_ENTITIES 10
const int     g_entities_count_max = MAX_ENTITIES;
int           g_entities_count     = 0;
struct entity g_entities[MAX_ENTITIES];

struct entity* entity_create()
{
  struct entity* pEntity = &g_entities[g_entities_count];
  for(int i=0; i<16; ++i)
  {
    pEntity->transform[i] = g_identity[i];
  }

  ++g_entities_count;

  return pEntity;
}

float* entity_getPosition(struct entity* pEntity)
{
  return &pEntity->transform[12];
}

float* entity_getTransform(struct entity* pEntity)
{
  return pEntity->transform;
}
