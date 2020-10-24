// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>

struct vertex
{
  float x, y, z;
};

struct vertex g_vertices[100];
int           g_vertex_count = 0;

void model_load()
{
  FILE* modelfile = fopen("model.obj", "r");
  int   matchcount = 0;

  if(modelfile == 0)
  {
    printf("ERROR: model.obj could not be opened\n");
    return;
  }

  while(g_vertex_count < 100)
  {
    char *line = 0;
    matchcount = fscanf(modelfile, "%m[^\n]*", &line);
    fscanf(modelfile, "\n");
    if (matchcount == 1)
    {
      struct vertex* v = &g_vertices[g_vertex_count];
      if(3 == sscanf(line, "v %f %f %f", &v->x, &v->y, &v->z))
      {
        ++g_vertex_count;
      }
      free(line);
      line = 0;
    }
    else if (matchcount == EOF)
    {
      break;
    }
  }

  if(g_vertex_count == 0)
  {
    printf("ERROR: No model vertices were not loaded\n");
  }

  fclose(modelfile);
}

void model_render()
{
  glBegin (GL_TRIANGLES);

  for(int i = 0; i < g_vertex_count; ++i)
  {
    glVertex3f(g_vertices[i].x, g_vertices[i].y, g_vertices[i].z);
  }

  glEnd();
}
