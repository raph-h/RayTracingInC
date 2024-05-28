### What I learnt
PPM is a cool image format
vector class new operations - inline vec3 operator \[insert operator here](parameters)

Normals - Can be made to point out of a surface (geometrically determined) or always against incident ray (determined during colouring)

shared_ptr<type> - Shared pointer to an allocated type - less performant than a normal pointer, so hit_record uses non-shared pointers for increased performance_
make_shared<thing>(thing_constructor_params ...) - Allocates a new instance of type thing
e.g. shared_ptr<double> double_ptr = make_shared<double>(0.37);

Have a common header file containing std usings, constants, utility functions and common headers to make each file have a shorter include section

C++'s built-in random is bad, use cstdlib's rand()

Getting a random unit vector -> Get a unit vector in a cube, and repeat until the vector is within a unit sphere, then unit vector it

Diffuse - Main ways learnt - Equal reflection & Lambertian
We brute force find a random vector in a unit sphere by creating a random vector in a unit cube, then checking if it has a length lower than 1

Recursion (when used) can be guarded by giving a maximum depth, by calling the function with depth - 1

Gamma & Linear - Images are gamma corrected - Pixel colour is transformed
Images written wthout being transformed - Linear space. When viewed in a gamma space, it will appear darker
Images transformed - Gamma space

Pointers used to solve circular reference issues - the material class takes in a hit_record pointer, and the hit_record takes in a material pointer

Defocus blur - Depth of field

Files should be .hpp as .h is more for C while .hpp is for C++

BVH really improved render speeds, I achieved a 10 times faster render
And with improvements to the bounding box axis determination, about a 11.8 times faster render

Potential improvements
Triangles, Parallelist, Shadow rays