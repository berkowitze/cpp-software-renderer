#include <algorithm>
#include "tgaimage.h"
#include <iostream>
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

int image_width = 800;
int image_height = 800;

Model *model = NULL;

Matrix vector_to_matrix(Vec3f v)
{
    Matrix m(4, 1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.0f;
    return m;
}

Vec3f matrix_to_vector(Matrix m)
{
    // Assumes 4x1 matrix representing (x, y, z, w)
    // Return vector that is x/y/z divided by w
    return Vec3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
}

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

void triangle(
    Vec3f p0, Vec3f p1, Vec3f p2, Vec2f uv0, Vec2f uv1, Vec2f uv2, float *zbuffer, TGAImage &image, TGAImage &texture, TGAColor color, bool show_bounding_box)
{
    // outline
    // draw_line(p0.x, p0.y, p1.x, p1.y, image, color);
    // draw_line(p1.x, p1.y, p2.x, p2.y, image, color);
    // draw_line(p2.x, p2.y, p0.x, p0.y, image, color);

    Vec2f bbox_min;
    Vec2f bbox_max;

    bbox_min.x = std::max(0.f, std::min(p0.x, std::min(p1.x, p2.x)));
    bbox_min.y = std::max(0.f, std::min(p0.y, std::min(p1.y, p2.y)));

    bbox_max.x = std::min(image.get_width() - 1.f, std::max(p0.x, std::max(p1.x, p2.x)));
    bbox_max.y = std::min(image.get_height() - 1.f, std::max(p0.y, std::max(p1.y, p2.y)));

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

    Vec3f P;
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

            // Calculate z based on linear combination of the barycentric coordinates
            P.z = p0.z * barycentric.x + p1.z * barycentric.y + p2.z * barycentric.z;

            float u_new = uv0.x * barycentric.x + uv1.x * barycentric.y + uv2.x * barycentric.z;
            float v_new = uv0.y * barycentric.x + uv1.y * barycentric.y + uv2.y * barycentric.z;
            TGAColor color = texture.get((int)(u_new * texture.get_width()), (int)((1.0 - v_new) * texture.get_height()));
            if (zbuffer[(int)(P.x + P.y * image.get_width())] >= P.z)
            {
                continue;
            }
            image.set(P.x, P.y, color);
            zbuffer[(int)(P.x + P.y * image.get_width())] = P.z;
        }
    }
}

const int depth = 255;

Matrix viewport(int x, int y, int w, int h)
{
    /*

    Viewport matrix:
    | w/2 0  0  x+w/2 |
    | 0  h/2 0  y+h/2 |
    | 0  0  d/2 d/2   |
    | 0  0  0   1     |

    */
    Matrix m = Matrix::identity(4);
    m[0][3] = x + w / 2.0f;
    m[1][3] = y + h / 2.0f;
    m[2][3] = depth / 2.0f;

    m[0][0] = w / 2.0f;
    m[1][1] = h / 2.0f;
    m[2][2] = depth / 2.0f;
    return m;
}

const Vec3f camera(0, 0, 3);

void flat_model(TGAImage &image, TGAImage &texture)
{
    Matrix Projection = Matrix::identity(4);
    Projection[3][2] = -1.f / camera.z;

    float *zbuffer = new float[image_width * image_height];
    for (int i = 0; i < image_width * image_height; i++)
    {
        zbuffer[i] = std::numeric_limits<int>::min();
    }
    model = new Model("./head.obj");
    std::cout << "model loaded" << std::endl;
    Vec3f light_dir(0.0, 0.0, -1.0);
    light_dir.normalize();
    Matrix Viewport = viewport(image.get_width() / 8.0f, image.get_height() / 8.0f, image.get_width() * 3.0f / 4.0f, image.get_height() * 3.0 / 4.0f);
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> pos_indices = model->tri_indices(i);
        std::vector<int> tex_indices = model->uv_indices(i);
        Vec3f screen_coords[3]; // vertices of the triangle in screen coordinates
        Vec3f world_coords[3];  // vertices of the triangle in world coordinates
        Vec2f uvs[3];
        for (int j = 0; j < 3; j++)
        {
            Vec3f world_coord = model->vert(pos_indices[j]);
            // float z = original_world_coord.z;
            // float denom = -1.0f / (z - camera.z);

            // Vec3f world_coord(
            //     original_world_coord.x,
            //     original_world_coord.y,
            //     original_world_coord.z);

            // screen_coords[j].x = ((world_coord.x + 1.0) / 2.0 * image.get_width());
            // screen_coords[j].y = ((world_coord.y + 1.0) / 2.0 * image.get_height());
            // screen_coords[j].z = world_coord.z;
            screen_coords[j] = matrix_to_vector(Viewport * Projection * vector_to_matrix(world_coord));

            Vec2f uv = model->uv(tex_indices[j]);
            uvs[j] = uv;

            world_coords[j] = world_coord;
        }

        std::cout << Viewport << std::endl;
        std::cout << Projection << std::endl;
        std::cout << screen_coords[0] << std::endl;
        std::cout << world_coords << std::endl;

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
            uvs[0],
            uvs[1],
            uvs[2],
            zbuffer,
            image,
            texture,
            TGAColor(brightness * 255, brightness * 255, brightness * 255, 255),
            false);
    }
}

// void triangle_test(TGAImage &image)
// {
//     float *zbuffer = new float[image.get_width() * image.get_height()];
//     // Create a few triangles
//     Vec2i t0[3] = {Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
//     Vec2i t1[3] = {Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
//     Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

//     triangle(t0[0], t0[1], t0[2], zbuffer, image, white, true);
//     triangle(t1[0], t1[1], t1[2], zbuffer, image, TGAColor(255, 255, 0, 255), true);
//     triangle(t2[0], t2[1], t2[2], zbuffer, image, red, true);

//     triangle(
//         Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80),
//         image, white);
// }

void wireframe(TGAImage &image)
{
    model = new Model("./head.obj");
    // For each face, draw all of its edges
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> pos_indices = model->tri_indices(i);
        // draw 3 edges, between vert j and vert j+1
        for (int j = 0; j < 3; j++)
        {
            Vec3f v0 = model->vert(pos_indices[j]);
            Vec3f v1 = model->vert(pos_indices[(j + 1) % 3]);
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

void matrix_test()
{
    Matrix x(4, 3);
    x[0][1] = 4.0f;
    x[1][1] = 5.0f;
    x[2][1] = 3.0f;
    x[0][2] = 1.0f;
    x[1][2] = 2.0f;
    x[2][2] = 3.0f;
    std::cout << x << std::endl;

    Matrix y(3, 5);
    y[0][1] = -1.0f;
    y[2][4] = 5.0f;
    y[0][2] = 2.0f;
    y[1][1] = 3.0f;
    y[2][3] = 4.0f;
    y[2][4] = 5.0f;
    y[2][2] = 6.0f;
    std::cout << y << std::endl;
    std::cout << "Multiplying " << x.rows << "x" << x.cols << " and " << y.rows << "x" << y.cols << " matrices" << std::endl;
    Matrix z = x * y;
    std::cout << "=" << std::endl;
    std::cout << z << std::endl;
    std::cout << "Transposing " << x.rows << "x" << x.cols << " matrix" << std::endl;
    Matrix x_t = x.transpose();
    std::cout << x_t << std::endl;
    std::cout << "Transposing " << y.rows << "x" << y.cols << " matrix" << std::endl;
    Matrix y_t = y.transpose();
    std::cout << y_t << std::endl;
}

int main(int argc, char **argv)
{
    // matrix_test();
    // return 0;
    TGAImage texture;
    bool success = texture.read_tga_file("african_head_diffuse.tga");
    if (!success)
    {
        std::cerr << "Failed to load texture" << std::endl;
        return 1;
    }
    TGAImage image(image_width, image_height, TGAImage::RGB);
    // lines(image);
    // wireframe(image);
    // triangle_test(image);
    flat_model(image, texture);
    image.flip_vertically();
    // write to a file called out/output_<current_date_time>.tga
    image.write_tga_file(("out/output_" + std::to_string(std::time(0)) + ".tga").c_str());
    std::cout << "out/output_" << std::to_string(std::time(0)) << ".tga";
    return 0;
}
