#include <limits>
#include <vector>
#include "geometry.h"
#include "our_gl.h"
#include "tgaimage.h"

int main(int argc, char** argv) {
    if (2>argc) {

        return 1;
    }

    constexpr int width  = 800;      // output image size
    constexpr int height = 800;
    constexpr vec3 light_dir{ 1,1,1 };
    constexpr vec3 eye{ 1,1,3 };
    constexpr vec3 center{ 0,0,0 };
    constexpr vec3 up{ 0,1,0 };

    lookat(eye, center, up);                            // build the ModelView matrix
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4); // build the Viewport matrix
    projection(norm(eye - center));                       // build the Projection matrix
    std::vector<double> zbuffer(width * height, std::numeric_limits<double>::max());

    TGAImage framebuffer(width, height, TGAImage::RGB);
    for (int m = 1; m < argc; m++)
    {

    }
    return 0;
}

