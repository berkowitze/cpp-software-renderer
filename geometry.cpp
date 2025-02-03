#include "geometry.h"
#include <iostream>

std::ostream &operator<<(std::ostream &s, Matrix &m)
{
  for (int i = 0; i < m.rows; i++)
  {
    for (int j = 0; j < m.cols; j++)
    {
      s << m[i][j];
      if (j < m.cols - 1)
        s << "\t";
    }
    s << "\n";
  }
  return s;
}