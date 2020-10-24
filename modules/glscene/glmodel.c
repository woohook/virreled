// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>

struct vertex
{
  float x, y, z;
};

struct face
{
  int v1, v2, v3;
};

struct vertex g_vertices[100];
int           g_vertex_count = 0;

struct face g_faces[100];
int         g_face_count = 0;

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

      struct face* f = &g_faces[g_face_count];
      if(3 == sscanf(line, "f %d %d %d", &f->v1, &f->v2, &f->v3))
      {
        ++g_face_count;
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
    printf("ERROR: No model vertices were loaded\n");
  }

  if(g_face_count == 0)
  {
    printf("ERROR: No model faces were loaded\n");
  }

  fclose(modelfile);
}

void model_render()
{
  glBegin (GL_TRIANGLES);

  for(int i = 0; i < g_face_count; ++i)
  {
    struct face*   f  = &g_faces[i];
    struct vertex* v1 = &g_vertices[f->v1 - 1];
    struct vertex* v2 = &g_vertices[f->v2 - 1];
    struct vertex* v3 = &g_vertices[f->v3 - 1];
    glVertex3f(v1->x, v1->y, v1->z);
    glVertex3f(v2->x, v2->y, v2->z);
    glVertex3f(v3->x, v3->y, v3->z);
  }

  glEnd();
}
