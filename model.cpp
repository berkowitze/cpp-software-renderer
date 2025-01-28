#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

// Rewritten following the sample code (not copied):
// https://github.com/ssloy/tinyrenderer/blob/f6fecb7ad493264ecd15e230411bfb1cca539a12/model.cpp

Model::Model(const char *filename) : verts_(), faces_()
{
  std::ifstream in; // input file stream
  in.open(filename, std::ifstream::in);
  if (in.fail())
  {
    std::cout << "Failed to open file" << std::endl;
    return;
  }

  std::string line;
  while (!in.eof())
  {
    std::getline(in, line);
    std::istringstream iss(line); // convert string to stream
    char trash;
    // line.compare returns 0 if the substring is an exact match, so this condition is
    // first 2 characters === "v "
    if (!line.compare(0, 2, "v "))
    {
      // '>>' is whitespace-delimited (can be customized in the getline call, default is space)
      // So this "throws out" the "v" at the beginning
      // vertex lines are formatted `v pos_x_float pos_y_float pos_z_float`
      iss >> trash;
      Vec3f v;
      for (int i = 0; i < 3; i++)
      {
        iss >> v.raw[i];
      }
      verts_.push_back(v);
    }
    else if (!line.compare(0, 2, "f "))
    {
      // face lines are formatted `f vert_index_0/texture_index_0/normal_index_0 vert_index_1/...`
      // f contains the indices of the face (should have 3)
      std::vector<int> f;
      int itrash, idx;
      iss >> trash; // throw out "f"
      // slashes are written to `trash`, unused indices are written to `itrash`
      // so the first index is written to idx, and the rest is thrown out
      while (iss >> idx >> trash >> itrash >> trash >> itrash)
      {
        idx--; // in obj files, indices are 1-indexed for some reason
        f.push_back(idx);
      }
      faces_.push_back(f);
    }
  }
  std::cerr << "#vertices: " << verts_.size() << ", #faces " << faces_.size() << std::endl;
}

Model::~Model() {}

int Model::nverts()
{
  return (int)verts_.size();
}

int Model::nfaces()
{
  return (int)faces_.size();
}

std::vector<int> Model::face(int idx)
{
  return faces_[idx];
}

Vec3f Model::vert(int idx)
{
  return verts_[idx];
}