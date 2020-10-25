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

struct model
{
  struct vertex vertices[100];
  int           vertex_count;

  struct material materials[100];
  int             material_count;
  int             current_material;

  struct face faces[100];
  int         face_count;

  float x, y, z;
};

struct model g_models[10];
int          g_model_count;

void materials_load(struct model* pModel, const char* filename)
{
  FILE* materialsfile  = fopen(filename, "r");
  int   matchcount     = 0;
  int   material_count = 0;

  if(materialsfile == 0)
  {
    printf("ERROR: %s could not be opened\n", filename);
    return;
  }

  while(material_count < 100)
  {
    char *line = 0;
    matchcount = fscanf(materialsfile, "%m[^\n]*", &line);
    fscanf(materialsfile, "\n");
    if (matchcount == 1)
    {
      struct material* m = &pModel->materials[pModel->material_count + material_count];
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

  pModel->material_count = pModel->material_count + material_count;

  if(material_count == 0)
  {
    printf("ERROR: No materials were loaded from %s\n", filename);
  }

  fclose(materialsfile);
}

void model_load(const char* filename, float x, float y, float z)
{
  struct model* pModel = &g_models[g_model_count];
  pModel->vertex_count = 0;
  pModel->face_count = 0;
  pModel->x = x;
  pModel->y = y;
  pModel->z = z;

  pModel->materials[0].red   = 1;
  pModel->materials[0].green = 0;
  pModel->materials[0].blue  = 1;
  pModel->material_count = 1;
  pModel->current_material = 0;

  FILE* modelfile = fopen(filename, "r");
  int   matchcount = 0;

  if(modelfile == 0)
  {
    printf("ERROR: model.obj could not be opened\n");
    return;
  }

  while(pModel->vertex_count < 100)
  {
    char *line     = 0;
    char* filename = 0;

    matchcount = fscanf(modelfile, "%m[^\n]*", &line);
    fscanf(modelfile, "\n");
    if (matchcount == 1)
    {
      struct vertex* v = &pModel->vertices[pModel->vertex_count];
      if(3 == sscanf(line, "v %f %f %f", &v->x, &v->y, &v->z))
      {
        ++pModel->vertex_count;
      }

      struct face* f = &pModel->faces[pModel->face_count];
      if(3 == sscanf(line, "f %d %d %d", &f->v1, &f->v2, &f->v3))
      {
        f->material = pModel->current_material;
        ++pModel->face_count;
      }

      if(1 == sscanf(line, "mtllib %m[^\n]*", &filename))
      {
        materials_load(pModel, filename);

        free(filename);
        filename = 0;
      }

      int current_material = 0;
      if(1 == sscanf(line, "usemtl %d", &current_material))
      {
        pModel->current_material = current_material;
      }

      free(line);
      line = 0;
    }
    else if (matchcount == EOF)
    {
      break;
    }
  }

  if(pModel->vertex_count == 0)
  {
    printf("ERROR: No model vertices were loaded\n");
  }

  if(pModel->face_count == 0)
  {
    printf("ERROR: No model faces were loaded\n");
  }

  fclose(modelfile);

  ++g_model_count;
}

void model_render(float cam_x, float cam_y, float cam_z, float rotX, float rotY, float rotZ)
{
  glMatrixMode (GL_MODELVIEW);

  for(int modelId = 0; modelId < g_model_count; ++modelId)
  {
    struct model* pModel = &g_models[modelId];

    glLoadIdentity();
    glTranslatef(pModel->x, pModel->y, pModel->z);
    glRotatef (rotX, 1,0,0);
    glRotatef (rotY, 0,1,0);
    glRotatef (rotZ, 0,0,1);
    glTranslatef (-cam_x, -cam_y, -cam_z);

    glBegin (GL_TRIANGLES);

    for(int i = 0; i < pModel->face_count; ++i)
    {
      struct face*   f   = &pModel->faces[i];
      struct material* m = &pModel->materials[f->material];
      struct vertex* v1  = &pModel->vertices[f->v1 - 1];
      struct vertex* v2  = &pModel->vertices[f->v2 - 1];
      struct vertex* v3  = &pModel->vertices[f->v3 - 1];
      glColor3f(m->red, m->green, m->blue);
      glVertex3f(v1->x, v1->y, v1->z);
      glVertex3f(v2->x, v2->y, v2->z);
      glVertex3f(v3->x, v3->y, v3->z);
    }

    glEnd();
  }
}
