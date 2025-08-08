#include <limits>
#include "geometry.h"

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

    return 0;
}

