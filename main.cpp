#include <algorithm>
#include "tgaimage.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void draw_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;

    // The line is "steep" if it changes more in y than in x
    // This can be used to make sure that lines are drawn without holes
    // and also with as few iterations as needed.
    bool is_steep = (std::abs(dx) < std::abs(dy));

    // If the line is steep, we want to iterate over y to avoid holes
    // Rather than branching, we'll just swap x and y here.
    if (is_steep)
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    // If the first input point is to the right of (or above if is_steep) the second,
    // swap the point order so that it's always correct to increment positively
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int error = 0;
    if (is_steep)
    {
        for (int x = x0; x <= x1; x++)
        {
            //  In this case, y and x were transposed, so untranspose them here

            image.set(y, x, color);
        }
    }

    // Precompute to save time
    // double divisor = 1.0 / (x1 - x0);

    // // x isn't always on the x axis, it is on the y axis if `is_steep`
    // for (int x = x0; x <= x1; x++)
    // {
    //     // t is how far along the line (from 0 to 1) we are
    //     double t = (x - x0) * divisor;
    //     // Lerp y from y0 to y1 based on t
    //     int y = y0 * (1.0 - t) + y1 * t;

    //     if (is_steep)
    //     {
    //         image.set(y, x, color);
    //     }
    //     else
    //     {
    //         image.set(x, y, color);
    //     }
    // }
}

int main(int argc, char **argv)
{
    TGAImage image(100, 100, TGAImage::RGB);
    image.set(52, 41, red);
    // normal line, is_steep
    draw_line(10, 20, 40, 70, image, white);
    // normal line, !is_steep
    draw_line(10, 50, 40, 70, image, TGAColor(255, 255, 0, 255));
    // horizontal line
    draw_line(10, 30, 40, 30, image, TGAColor(255, 0, 255, 255));
    // vertical line
    draw_line(60, 40, 60, 20, image, TGAColor(0, 255, 255, 255));
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}
