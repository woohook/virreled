// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>

struct vertex
{
  float x, y, z;
};

struct material
{
  float red, green, blue;
};

struct face
{
  int v1, v2, v3, material;
};

struct vertex g_vertices[100];
int           g_vertex_count = 0;

struct material g_materials[100];
int             g_material_count   = 0;
int             g_current_material = 0;

struct face g_faces[100];
int         g_face_count = 0;

void materials_load(const char* filename)
{
  FILE* materialsfile  = fopen(filename, "r");
  int   matchcount     = 0;
  int   material_count = 0;

  if(materialsfile == 0)
  {
    printf("ERROR: %s could not be opened\n", filename);
    return;
  }

  while(g_material_count < 100)
  {
    char *line = 0;
    matchcount = fscanf(materialsfile, "%m[^\n]*", &line);
    fscanf(materialsfile, "\n");
    if (matchcount == 1)
    {
      struct material* m = &g_materials[g_material_count + material_count];
      if(3 == sscanf(line, "Kd %f %f %f", &m->red, &m->green, &m->blue))
      {
        ++material_count;
      }

      free(line);
      line = 0;
    }
    else if (matchcount == EOF)
    {
      break;
    }
  }

  g_material_count = g_material_count + material_count;

  if(material_count == 0)
  {
    printf("ERROR: No materials were loaded from %s\n", filename);
  }

  fclose(materialsfile);
}

void model_load()
{
  if(g_material_count == 0)
  {
    g_materials[0].red = 1;
    g_materials[0].green = 0;
    g_materials[0].blue  = 1;
    ++g_material_count;
  }

  FILE* modelfile = fopen("model.obj", "r");
  int   matchcount = 0;

  if(modelfile == 0)
  {
    printf("ERROR: model.obj could not be opened\n");
    return;
  }

  while(g_vertex_count < 100)
  {
    char *line     = 0;
    char* filename = 0;

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
        f->material = g_current_material;
        ++g_face_count;
      }

      if(1 == sscanf(line, "mtllib %m[^\n]*", &filename))
      {
        materials_load(filename);

        free(filename);
        filename = 0;
      }

      int current_material = 0;
      if(1 == sscanf(line, "usemtl %d", &current_material))
      {
        g_current_material = current_material;
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
    struct face*   f   = &g_faces[i];
    struct material* m = &g_materials[f->material];
    struct vertex* v1  = &g_vertices[f->v1 - 1];
    struct vertex* v2  = &g_vertices[f->v2 - 1];
    struct vertex* v3  = &g_vertices[f->v3 - 1];
    glColor3f(m->red, m->green, m->blue);
    glVertex3f(v1->x, v1->y, v1->z);
    glVertex3f(v2->x, v2->y, v2->z);
    glVertex3f(v3->x, v3->y, v3->z);
  }

  glEnd();
}
