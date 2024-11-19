#ifndef COLOUR_H
#define COLOUR_H

#include "interval.hpp"
#include "vec3.hpp"

using colour = vec3;

inline double linear_to_gamma(double linear_component) {
	if (linear_component > 0)
		return std::sqrt(linear_component);
	return 0;
}

void write_colour(std::ostream& out, const colour& pixel_colour) {
	double r = pixel_colour.x();
	double g = pixel_colour.y();
	double b = pixel_colour.z();

	// Replace NaN components with zero (NaN never equals itself) (this is a quick fix to remove acne)
	if (r != r) { r = 0.0; }
	if (g != g) { g = 0.0; }
	if (b != b) { b = 0.0; }

	// Apply a linear to gamma transform for gamma 2
	r = linear_to_gamma(r);
	g = linear_to_gamma(g);
	b = linear_to_gamma(b);

	// Translate the [0, 1] component values to the byte range [0, 255]
	static const interval intensity(0.000, 0.999);
	int rbyte = int(256 * intensity.clamp(r));
	int gbyte = int(256 * intensity.clamp(g));
	int bbyte = int(256 * intensity.clamp(b));

	// Write out the pixel colour components
	out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif