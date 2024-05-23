#ifndef COLOUR_H
#define COLOUR_H

#include "vec3.h"

#include "RayTracing.h"

using colour = vec3;

void write_colour(std::ostream& out, const colour& pixel_colour) {
	double r = pixel_colour.x();
	double g = pixel_colour.y();
	double b = pixel_colour.z();

	// Translate the [0, 1] component values to the byte range [0, 255]
	int rbyte = int(255.999 * r);
	int gbyte = int(255.999 * g);
	int bbyte = int(255.999 * b);
	out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif