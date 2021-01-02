// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>

extern float g_testobject_height;

struct vertex
{
  float x, y, z;
};

struct texcoord
{
  float u, v;
};

struct material
{
  float red, green, blue;
  GLuint texture_id;
  GLubyte texture[10240];
  int texture_width;
  int texture_height;
};

struct face
{
  int v1, v2, v3, material;
  int vt[3];
};

struct model
{
  struct vertex vertices[100];
  int           vertex_count;

  struct material materials[100];
  int             material_count;

  struct texcoord texcoords[128];
  int             texcoord_count;

  struct face faces[100];
  int         face_count;

  float* transform;
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

#define g_textureWidth 2
#define g_textureHeight 2
static GLubyte g_textureData[] = {0,255,255,255, 255,0,255,255, 255,255,0,255, 0,0,255,255};

void texture_initialize(GLuint* textureId, int width, int height, GLubyte* data)
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, textureId);
  glBindTexture(GL_TEXTURE_2D, *textureId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void model_set_rotation(float rx, float ry, float rz)
{
  struct model* pModel = &g_models[g_model_count - 1];
  pModel->rx = rx;
  pModel->ry = ry;
  pModel->rz = rz;
}

void model_create(float* transform)
{
  struct model* pModel = &g_models[g_model_count];
  pModel->vertex_count = 0;
  pModel->face_count = 0;
  pModel->transform = transform;
  pModel->rx = 0;
  pModel->ry = 0;
  pModel->rz = 0;

  pModel->materials[0].red   = 1;
  pModel->materials[0].green = 0;
  pModel->materials[0].blue  = 1;
  pModel->materials[0].texture_id     = 0;
  pModel->materials[0].texture_width  = 0;
  pModel->materials[0].texture_height = 0;
  pModel->material_count = 1;

  pModel->texcoord_count = 0;

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

void model_add_texcoord(float u, float v)
{
  struct model* pModel = &g_models[g_model_count - 1];
  struct texcoord* uv = &pModel->texcoords[pModel->texcoord_count];
  uv->u = u;
  uv->v = v;
  ++pModel->texcoord_count;
}

void model_add_face(int v1, int v2, int v3)
{
  struct model* pModel = &g_models[g_model_count - 1];
  struct face* f = &pModel->faces[pModel->face_count];
  f->v1 = v1;
  f->v2 = v2;
  f->v3 = v3;
  f->vt[0] = -1;
  f->vt[1] = -1;
  f->vt[2] = -1;
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
  m->texture_id = 0;
  m->texture_width = 0;
  m->texture_height = 0;
  ++pModel->material_count;
}

void model_select_material(int materialIndex)
{
  g_current_material = materialIndex;
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

void model_load(const char* filename, float* transform, float rx, float ry, float rz)
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

  model_create(transform);
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

      float uv[2];
      if(2 == sscanf(line, "vt %f %f", &uv[0], &uv[1]))
      {
        model_add_texcoord(uv[0], uv[1]);
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
        model_select_material(current_material);
      }

      line[0] = 0;
    }
    else if (matchcount == EOF)
    {
      break;
    }
  }

  if(g_model_count == 6)
  {
    struct model* pModel = &g_models[g_model_count - 1];
    pModel->materials[0].texture_width = g_textureWidth;
    pModel->materials[0].texture_height = g_textureHeight;
    for(int i=0; i < g_textureWidth * g_textureHeight * 4; ++i)
    {
      pModel->materials[0].texture[i] = g_textureData[i];
    }
    texture_initialize(&pModel->materials[0].texture_id, pModel->materials[0].texture_width, pModel->materials[0].texture_height, pModel->materials[0].texture);
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
    glTranslatef(-g_models[1].transform[12], -g_models[1].transform[13], -g_models[1].transform[14]);
/*
    GLfloat x[16];
    x[0] = g_models[4].transform[0];
    x[1] = g_models[4].transform[4];
    x[2] = g_models[4].transform[8];
    x[3] = 0;

    x[4] = g_models[4].transform[1];
    x[5] = g_models[4].transform[5];
    x[6] = g_models[4].transform[9];
    x[7] = 0;

    x[8] = g_models[4].transform[2];
    x[9] = -g_models[4].transform[6];
    x[10] = g_models[4].transform[10];
    x[11] = 0;

    x[12] = -g_models[4].transform[12];
    x[13] = -g_models[4].transform[13];
    x[14] = -g_models[4].transform[14];
    x[15] = 1;

    glMultMatrixf(x);
*/
    // apply the model transformation matrix
    GLfloat transform[16];
    for(int j=0;j<16;++j)
    {
      transform[j] = pModel->transform[j];
    }
    glMultMatrixf(transform);

    struct material* modelMaterial = &pModel->materials[0];
    if(modelMaterial->texture_width > 0)
    {
      glEnable(GL_TEXTURE_2D);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
      glBindTexture(GL_TEXTURE_2D, modelMaterial->texture_id);
    }

    glBegin (GL_TRIANGLES);

    for(int i = 0; i < pModel->face_count; ++i)
    {
      struct face*   f   = &pModel->faces[i];
      struct material* m = &pModel->materials[f->material];
      struct vertex* v1  = &pModel->vertices[f->v1 - 1];
      struct vertex* v2  = &pModel->vertices[f->v2 - 1];
      struct vertex* v3  = &pModel->vertices[f->v3 - 1];
      glColor3f(m->red, m->green, m->blue);
      glTexCoord2f(0.0, 0.0);
      glVertex3f(v1->x, v1->y, v1->z);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(v2->x, v2->y, v2->z);
      glTexCoord2f(1.0, 1.0);
      glVertex3f(v3->x, v3->y, v3->z);
    }

    glEnd();

    if(modelMaterial->texture_width > 0)
    {
      glDisable(GL_TEXTURE_2D);
    }
  }
}
