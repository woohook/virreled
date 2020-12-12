// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>

extern float g_testobject_height;

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

  struct face faces[100];
  int         face_count;

  float* position;
  float rx, ry, rz;
};

#define      MODEL_COUNT_MAX 10
const int    g_model_count_max = MODEL_COUNT_MAX;
struct model g_models[MODEL_COUNT_MAX];
int          g_model_count = 0;

float* g_rx = &g_models[1].rx;
float* g_ry = &g_models[1].ry;
float* g_rz = &g_models[1].rz;

int    g_current_material = 0;

void model_set_rotation(float rx, float ry, float rz)
{
  struct model* pModel = &g_models[g_model_count - 1];
  pModel->rx = rx;
  pModel->ry = ry;
  pModel->rz = rz;
}

void model_create(float* position)
{
  struct model* pModel = &g_models[g_model_count];
  pModel->vertex_count = 0;
  pModel->face_count = 0;
  pModel->position = position;
  pModel->rx = 0;
  pModel->ry = 0;
  pModel->rz = 0;

  pModel->materials[0].red   = 1;
  pModel->materials[0].green = 0;
  pModel->materials[0].blue  = 1;
  pModel->material_count = 1;

  ++g_model_count;

  g_current_material = 0;
}

void model_add_vertex(float x, float y, float z)
{
  struct model* pModel = &g_models[g_model_count - 1];
  struct vertex* v = &pModel->vertices[pModel->vertex_count];
  v->x = x;
  v->y = y;
  v->z = z;
  ++pModel->vertex_count;
}

void model_add_face(int v1, int v2, int v3)
{
  struct model* pModel = &g_models[g_model_count - 1];
  struct face* f = &pModel->faces[pModel->face_count];
  f->v1 = v1;
  f->v2 = v2;
  f->v3 = v3;
  f->material = g_current_material;
  ++pModel->face_count;
}

void model_add_material(float red, float green, float blue)
{
  struct model* pModel = &g_models[g_model_count - 1];
  struct material* m = &pModel->materials[pModel->material_count];
  m->red = red;
  m->green = green;
  m->blue = blue;
  ++pModel->material_count;
}

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

  while(material_count < 100)
  {
    char line[1024];
    matchcount = fscanf(materialsfile, "%1023[^\n]*", &line);
    fscanf(materialsfile, "\n");
    if (matchcount == 1)
    {
      float red, green, blue;
      if(3 == sscanf(line, "Kd %f %f %f", &red, &green, &blue))
      {
        model_add_material(red, green, blue);
        ++material_count;
      }

      line[0] = 0;
    }
    else if (matchcount == EOF)
    {
      break;
    }
  }

  if(material_count == 0)
  {
    printf("ERROR: No materials were loaded from %s\n", filename);
  }

  fclose(materialsfile);
}

void model_load(const char* filename, float* position, float rx, float ry, float rz)
{
  FILE* modelfile = fopen(filename, "r");
  int   matchcount = 0;
  int   vertexcount = 0;
  int   facecount = 0;

  if(modelfile == 0)
  {
    printf("ERROR: %s could not be opened\n", filename);
    return;
  }

  model_create(position);
  model_set_rotation(rx, ry, rz);

  while(vertexcount < 100)
  {
    char line[1024];
    line[1023] = 0;
    char filename[1024];
    filename[1023] = 0;

    matchcount = fscanf(modelfile, "%1023[^\n]*", &line);
    fscanf(modelfile, "\n");
    if (matchcount == 1)
    {
      float v[3];
      if(3 == sscanf(line, "v %f %f %f", &v[0], &v[1], &v[2]))
      {
        model_add_vertex(v[0], v[1], v[2]);
        ++vertexcount;
      }

      int v1, v2, v3;
      if(3 == sscanf(line, "f %d %d %d", &v1, &v2, &v3))
      {
        model_add_face(v1, v2, v3);
        ++facecount;
      }

      if(1 == sscanf(line, "mtllib %1023[^\n]*", &filename))
      {
        materials_load(filename);

        filename[0] = 0;
      }

      int current_material = 0;
      if( 1 == sscanf(line, "usemtl %d", &current_material))
      {
        g_current_material = current_material;
      }

      line[0] = 0;
    }
    else if (matchcount == EOF)
    {
      break;
    }
  }

  if(vertexcount == 0)
  {
    printf("ERROR: No model vertices were loaded\n");
  }

  if(facecount == 0)
  {
    printf("ERROR: No model faces were loaded\n");
  }

  fclose(modelfile);
}

void model_render(float cam_x, float cam_y, float cam_z, float rotX, float rotY, float rotZ)
{
  glMatrixMode (GL_MODELVIEW);

  for(int modelId = 0; modelId < g_model_count; ++modelId)
  {
    struct model* pModel = &g_models[modelId];

    glLoadIdentity();

    // camera offset relative to object
    glTranslatef(-cam_x, -cam_y, -cam_z);

    // absolute camera rotation
    //glRotatef (rotZ, 0,0,1);
    //glRotatef (rotX, 1,0,0);
    //glRotatef (rotY, 0,1,0);

    // object relative camera rotation
    glRotatef (g_models[1].rz, 0,0,1);
    glRotatef (g_models[1].rx, 1,0,0);
    glRotatef (180 - g_models[1].ry, 0,1,0);

    // move world to view coordinates
    glTranslatef(-g_models[1].position[0], -g_models[1].position[1], -g_models[1].position[2]);

    // move model to world coordinates
    glTranslatef(pModel->position[0], pModel->position[1], pModel->position[2]);

    // rotate the model
    glRotatef (pModel->ry, 0,1,0);
    glRotatef (pModel->rx, 1,0,0);
    glRotatef (pModel->rz, 0,0,1);

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
