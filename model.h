#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <string>
#include "geometry.h"

class Triangle
{
public:
  ~Triangle();
  std::vector<int> pos_indices;
  std::vector<int> tex_indices;
};

class Model
{
private:
  std::vector<Vec3f> verts_;
  std::vector<Triangle> tris_;
  std::vector<Vec2f> uvs_;

public:
  Model(const char *filename);
  ~Model();
  int nverts();
  int nfaces();
  Vec3f vert(int i);
  Vec2f uv(int i);
  std::vector<int> uv_indices(int tri_index);
  std::vector<int> tri_indices(int index);
  // std::string print_uvs();
};

#endif
