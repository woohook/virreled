#include <stdlib.h>
#include <stdio.h>

extern const int g_model_count_max;
extern int       g_model_count;
extern float     g_cam_x, g_cam_y, g_cam_z;
extern float     g_cam_rx, g_cam_ry, g_cam_rz;

void model_load(const char* filename, float* position, float rx, float ry, float rz);

struct entity* entity_create();
float* entity_getPosition(struct entity*);

void entity_load(const char* filename, float x, float y, float z)
{
  struct entity* pEntity = entity_create();
  float* position = entity_getPosition(pEntity);
  position[0] = x;
  position[1] = y;
  position[2] = z;

  FILE* file        = fopen(filename, "r");
  int   matchcount  = 0;

  if(file == 0)
  {
    printf("ERROR: %s could not be opened\n", filename);
    return;
  }

  while(1)
  {
    char line [1024];
    line[1023] = 0;
    matchcount = fscanf(file, "%1023[^\n]", &line);
    fscanf(file, "\n");
    if (matchcount == 1)
    {
      float model_x, model_y, model_z;
      float rx = 0, ry = 0, rz = 0;
      char model_filename[1024];
      int offset = 0;

      if(4 == sscanf(line, " model %1023[^ ] x=%f y=%f z=%f%n", &model_filename, &model_x, &model_y, &model_z, &offset))
      {
        sscanf(&line[offset], " rx=%f ry=%f rz=%f", &rx, &ry, &rz);
        model_load(model_filename, position, rx, ry, rz);
      }

      line[0] = 0;
    }
    else if (matchcount == EOF)
    {
      break;
    }
  }

  fclose(file);
}

void scene_load(const char* filename)
{
  FILE* file        = fopen(filename, "r");
  int   matchcount  = 0;
  int   model_count = 0;
  int   cam_count   = 0;

  if(file == 0)
  {
    printf("ERROR: %s could not be opened\n", filename);
    return;
  }

  while(g_model_count < g_model_count_max)
  {
    char line [1024];
    line[1023] = 0;
    matchcount = fscanf(file, "%1023[^\n]", &line);
    fscanf(file, "\n");
    if (matchcount == 1)
    {
      float x, y, z;
      float rx = 0, ry = 0, rz = 0;
      char model_filename[1024];
      char entity_filename[1024];
      int offset = 0;

      if(4 == sscanf(line, " entity %1023[^ ] x=%f y=%f z=%f%n", &entity_filename, &x, &y, &z, &offset))
      {
        entity_load(entity_filename, x, y, z);
      }

      if(6 == sscanf(line, " camera x=%f y=%f z=%f rx=%f ry=%f rz=%f", &x, &y, &z, &rx, &ry, &rz))
      {
        if(cam_count > 0)
        {
          printf("INFO: Overriding previous camera position from %s\n", filename);
        }
        ++cam_count;
        g_cam_x = x;
        g_cam_y = y;
        g_cam_z = z;
        g_cam_rx = rx;
        g_cam_ry = ry;
        g_cam_rz = rz;
      }

      line[0] = 0;
    }
    else if (matchcount == EOF)
    {
      break;
    }
  }

  if(model_count == 0)
  {
    printf("ERROR: No models were loaded from %s\n", filename);
  }
  if(cam_count == 0)
  {
    printf("WARN: No camera position loaded from %s\n", filename);
  }

  fclose(file);
}
