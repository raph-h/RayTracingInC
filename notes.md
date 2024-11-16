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

Volumes - Uses differential equations

Monte Carlo - Good for applications where great accuracy is not needed
- Gives a statistical estimate of an answer, getting more and more accurate the longer it is run
- At a certain point, we can decide that the answer is accurate enough and stop it
Las Vegas - Will always eventually arrive at an exact answer, but cannot say beforehand how long

Law of diminishing returns - Worst part of Monte Carlo
- Mititaged by stratifying samples (jittering), by taking a grid and taking a sample within each cell
- Stratified methods converge at a better asymptotic rate, however the advantage decreases with the dimension of the problem (Curse of Dimensionality)
	- As ray tracing is a very high-dimension algorithm (Each reflection adds two new dimensions), the output reflection angle won't be stratified (Too complex)
	- However, the locations of the samples around each pixel can be sampled

Stratification
- Sharper contrast at locations which have a high frequency of change - Also thought as high information density
- High information density locations include edges
- Always stratify when doing a single-reflection or shadowing or strictly 2D problems

PDF for importance sampling
- A non-uniform PDF steers more samples to where the PDF is big and fewer to where the PDF is small, this can be used to steer samples to areas which have a higher noise, we can reduce the total noise of the scene with fewer samples, speeding things up
- Choosing a incorrect PDF will not ruin the render, but an incorrect scattering function will ruin a render
- Most difficult bug in a Monte Carlo program - A bug that produces reasonable outputs


Potential improvements
Triangles - done, however polygons should be imported to test, Parallelist, Shadow rays

Potential additions

Bidirectional method, metropolis methods, photon mapping
Add pdf support for other objects - not just quads (spheres) etc