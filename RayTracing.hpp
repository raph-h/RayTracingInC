// RayTracing.h : Include file for standard system include files,
// or project specific include files.
#ifndef RAYTRACING_H
#define RAYTRACING_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <limits>
#include <memory>


// C++ STD usings

using std::make_shared;
using std::shared_ptr;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}

inline double random_double() {
	// Returns a random real in [0, 1)
	return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
	// Returns a random real in [min, max)
	return min + (max - min) * random_double();
}

inline int random_int(int min, int max) {
	// Returns a random integer in [min, max]
	return int(random_double(min, max + 1));
}

// Common Headers

#include "colour.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include "vec3.hpp"

#endif