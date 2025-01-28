#include <algorithm>
#include "tgaimage.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void draw_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    // The line is "steep" if it changes more in y than in x
    // This can be used to make sure that lines are drawn without holes
    // and also with as few iterations as needed.
    bool is_steep = std::abs(x0 - x1) < std::abs(y0 - y1);

    if (is_steep)
    {
        // if the line is steep, we're going to reflect it over y=x so we can treat it
        // like a non-steep line and always iterate over "x". Then we'll have to untranspose it
        // with x=y when actually writing to the image.
        // We can also use the fact that the slope is less than 1 to our advantage.
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    // Make it always left-to-right
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    /*  less efficient approach since you have to do floating point stuff
    // derror (Delta Error) is how much error accumulates with each iteration of x
    // error is really just how much y changes as x changes (abs slope), I'm just calling it "error"
    // to be consistent with the tutorial
    // float derror = std::abs(dy / (float)dx);
    */
    // instead of doing error = dy/dx, let's do new_error = 2 * dy = 2 * dx * old_error
    int dxderror2 = std::abs(dy) * 2;
    // error is how much cumulative error has accumulated
    int error = 0;
    int y_increment = (y1 > y0) ? 1 : -1;

    int y = y0;
    // This could be a single loop with an if statement, but branching inside
    if (is_steep)
    {
        for (int x = x0; x <= x1; x++)
        {
            image.set(y, x, color);

            error += dxderror2;
            if (error > dx)
            {
                // if the cumulative error is over 0.5, we want to move y up/down to the next pixel
                y += y_increment;
                error -= dx * 2;
            }
        }
    }
    else
    {
        for (int x = x0; x <= x1; x++)
        {
            image.set(x, y, color);
            error += dxderror2;
            if (error > dx)
            {
                // if the cumulative error is over 0.5, we want to move y up/down to the next pixel
                y += y_increment;
                error -= dx * 2;
            }
        }
    }
}

int main(int argc, char **argv)
{
    TGAImage image(100, 100, TGAImage::RGB);
    image.set(52, 41, red);
    // normal line, is_steep
    draw_line(10, 10, 50, 50, image, white);
    // normal line, !is_steep
    draw_line(10, 50, 40, 70, image, TGAColor(255, 255, 0, 255));
    // horizontal lines
    draw_line(10, 30, 40, 30, image, TGAColor(255, 0, 255, 255));
    draw_line(40, 40, 10, 40, image, TGAColor(255, 100, 255, 255));
    // vertical lines
    draw_line(60, 40, 60, 20, image, TGAColor(255, 100, 100, 255));
    draw_line(70, 20, 70, 40, image, TGAColor(255, 0, 0, 255));
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}
