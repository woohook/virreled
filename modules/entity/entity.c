// SPDX-License-Identifier: GPL-3.0-or-later

struct entity
{
  float position[3];
};

#define MAX_ENTITIES 10
const int     g_entities_count_max = MAX_ENTITIES;
int           g_entities_count     = 0;
struct entity g_entities[MAX_ENTITIES];

struct entity* entity_create()
{
  struct entity* pEntity = &g_entities[g_entities_count];
  pEntity->position[0] = 0;
  pEntity->position[1] = 0;
  pEntity->position[2] = 0;

  ++g_entities_count;

  return pEntity;
}

float* entity_getPosition(struct entity* pEntity)
{
  return pEntity->position;
}
