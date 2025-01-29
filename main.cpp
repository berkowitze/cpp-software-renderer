#include <algorithm>
#include "tgaimage.h"
#include <iostream>
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

Model *model = NULL;

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

/*
void triangle_linesweep(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage &image, TGAColor color)
{
    draw_line(p0.x, p0.y, p1.x, p1.y, image, color);
    draw_line(p1.x, p1.y, p2.x, p2.y, image, color);
    draw_line(p2.x, p2.y, p0.x, p0.y, image, color);

    // First, sort p0, p1, p2 so that they are in order of increasing y
    // p0 is the "lowest", p2 is the "highest"
    if (p0.y > p1.y)
    {
        std::swap(p0, p1);
    }
    if (p1.y > p2.y)
    {
        std::swap(p1, p2);
    }
    if (p0.y > p1.y)
    {
        std::swap(p1, p2);
    }

    // Render the bottom half of the triangle
    for (int y = p0.y; y < p1.y; y++)
    {
        // First, figure out where along the (p0, p1) edge we are
        int dy_01 = (p1.y - p0.y);
        float p01_t = dy_01 == 0 ? 1.0 : (y - p0.y) / (float)(p1.y - p0.y);
        float p01_x = (p1.x - p0.x) * p01_t + p0.x;

        // Same for (p0, p2) edge
        int dy_02 = (p2.y - p0.y);
        float p02_t = dy_02 == 0 ? 1.0 : (y - p0.y) / (float)(p2.y - p0.y);
        float p02_x = (p2.x - p0.x) * p02_t + p0.x;

        // Left boundary is the min of these 2 x's, right is the max
        int left_boundary = std::min(p01_x, p02_x);
        int right_boundary = std::max(p01_x, p02_x);
        draw_line(left_boundary, y, right_boundary, y, image, color);
    }

    // Render the top half
    for (int y = p1.y; y <= p2.y; y++)
    {
        // First, figure out where along the (p1, p2) edge we are
        int dy_12 = (p2.y - p1.y);
        float p12_t = dy_12 == 0 ? 1.0 : (y - p1.y) / (float)(p2.y - p1.y);
        float p12_x = (p2.x - p1.x) * p12_t + p1.x;

        // Same for (p0, p2) edge
        int dy_02 = (p2.y - p0.y);
        float p02_t = dy_02 == 0 ? 1.0 : (y - p0.y) / (float)(p2.y - p0.y);
        float p02_x = (p2.x - p0.x) * p02_t + p0.x;

        // Left boundary is the min of these 2 x's, right is the max
        int left_boundary = std::min(p12_x, p02_x);
        int right_boundary = std::max(p12_x, p02_x);
        draw_line(left_boundary, y, right_boundary, y, image, color);
    }
}
*/

TGAColor bbox_color(125, 125, 100, 255);

void triangle(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage &image, TGAColor color, bool show_bounding_box)
{
    draw_line(p0.x, p0.y, p1.x, p1.y, image, color);
    draw_line(p1.x, p1.y, p2.x, p2.y, image, color);
    draw_line(p2.x, p2.y, p0.x, p0.y, image, color);

    Vec2i bbox_min;
    Vec2i bbox_max;

    bbox_min.x = std::max(0, std::min(p0.x, std::min(p1.x, p2.x)));
    bbox_min.y = std::max(0, std::min(p0.y, std::min(p1.y, p2.y)));

    bbox_max.x = std::min(image.get_width() - 1, std::max(p0.x, std::max(p1.x, p2.x)));
    bbox_max.y = std::min(image.get_height() - 1, std::max(p0.y, std::max(p1.y, p2.y)));

    if (show_bounding_box)
    {
        // draw bounding box for debugging
        // left
        draw_line(bbox_min.x, bbox_min.y, bbox_min.x, bbox_max.y, image, bbox_color);
        // right
        draw_line(bbox_max.x, bbox_min.y, bbox_max.x, bbox_max.y, image, bbox_color);
        // top
        draw_line(bbox_min.x, bbox_max.y, bbox_max.x, bbox_max.y, image, bbox_color);
        // bottom
        draw_line(bbox_min.x, bbox_min.y, bbox_max.x, bbox_min.y, image, bbox_color);
    }

    Vec2i P;
    for (int x = bbox_min.x; x <= bbox_max.x; x++)
    {
        P.x = x;
        for (int y = bbox_min.y; y <= bbox_max.y; y++)
        {
            P.y = y;
            Vec3f u = Vec3f(
                          p2.x - p0.x,
                          p1.x - p0.x,
                          p0.x - P.x) ^
                      Vec3f(
                          p2.y - p0.y,
                          p1.y - p0.y,
                          p0.y - P.y);
            if (std::abs(u.z) < 1)
            {
                // degenerate triangle
                continue;
            }
            Vec3f barycentric(
                1.0f - (u.x + u.y) / u.z,
                u.y / u.z,
                u.x / u.z);
            if (barycentric.x < 0 || barycentric.y < 0 || barycentric.z < 0)
            {
                // Not inside triangle
                continue;
            }
            image.set(P.x, P.y, color);
        }
    }
}

void flat_model(TGAImage &image)
{
    model = new Model("./head.obj");
    Vec3f light_dir(0.0, 0.0, -1.0);
    light_dir.normalize();
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3]; // vertices of the triangle in screen coordinates
        Vec3f world_coords[3];  // vertices of the triangle in world coordinates
        for (int j = 0; j < 3; j++)
        {
            Vec3f world_coord = model->vert(face[j]);
            screen_coords[j].x = (int)((world_coord.x + 1.0) / 2.0 * image.get_width());
            screen_coords[j].y = (int)((world_coord.y + 1.0) / 2.0 * image.get_height());

            world_coords[j] = world_coord;
        }

        Vec3f normal = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        normal.normalize();
        float brightness = normal * light_dir;
        if (brightness < 0)
            continue;

        // std::cout << color;
        triangle(
            screen_coords[0],
            screen_coords[1],
            screen_coords[2],
            image,
            TGAColor(brightness * 255, brightness * 255, brightness * 255, 255),
            false);
    }
}

int image_width = 800;
int image_height = 800;
int main(int argc, char **argv)
{
    TGAImage image(image_width, image_height, TGAImage::RGB);
    // lines(image);
    // wireframe(image);
    // triangle_test(image)
    flat_model(image);
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}

void triangle_test(TGAImage &image)
{
    // Create a few triangles
    Vec2i t0[3] = {Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

    triangle(t0[0], t0[1], t0[2], image, white, true);
    triangle(t1[0], t1[1], t1[2], image, TGAColor(255, 255, 0, 255), true);
    triangle(t2[0], t2[1], t2[2], image, red, true);
}

void wireframe(TGAImage &image)
{
    model = new Model("./head.obj");
    // For each face, draw all of its edges
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        // draw 3 edges, between vert j and vert j+1
        for (int j = 0; j < 3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            // obj file vertex values seem to lie between (-1, +1)
            int x0 = (v0.x + 1.0) / 2.0 * image_width;
            int y0 = (v0.y + 1.0) / 2.0 * image_height;
            int x1 = (v1.x + 1.0) / 2.0 * image_width;
            int y1 = (v1.y + 1.0) / 2.0 * image_height;
            draw_line(x0, y0, x1, y1, image, white);
        }
    }
}

void lines(TGAImage &image)
{
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
}