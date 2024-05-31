#ifndef TEXTURE_H
#define TEXTURE_H

#include "RayTracing.hpp"

#include "perlin.hpp"
#include "rtw_stb_image.hpp"

class texture {
public:
	virtual ~texture() = default;

	virtual colour value(double u, double v, const point3& p) const = 0;
};

class solid_colour : public texture {
public:
	solid_colour(const colour& albedo) : albedo(albedo) {}

	solid_colour(double red, double green, double blue) : solid_colour(colour(red, green, blue)) {}

	colour value(double u, double v, const point3& p) const override {
		return albedo;
	}

private:
	colour albedo;
};

class checker_texture : public texture {
public:
	checker_texture(double scale, shared_ptr<texture> even, shared_ptr<texture> odd) : inv_scale(1.0 / scale), even(even), odd(odd) {}

	checker_texture(double scale, const colour& c1, const colour& c2)
		: inv_scale(1.0 / scale), even(make_shared<solid_colour>(c1)), odd(make_shared<solid_colour>(c2)) {}

	colour value(double u, double v, const point3& p) const override {
		int xInteger = int(std::floor(inv_scale * p.x()));
		int yInteger = int(std::floor(inv_scale * p.y()));
		int zInteger = int(std::floor(inv_scale * p.z()));

		bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

		return isEven ? even->value(u, v, p) : odd->value(u, v, p);
	}
private:
	double inv_scale;
	shared_ptr<texture> even;
	shared_ptr<texture> odd;
};

class image_texture : public texture {
public:
	image_texture(const char* filename) : image(filename) {}

	colour value(double u, double v, const point3& p) const override {
		// If we have no texture data, then return solid cyan as a debugging aid
		if (image.height() <= 0) return colour(0, 1, 1);

		// Clamp input texture coordinates to [0, 1] x [1, 0]
		u = interval(0, 1).clamp(u);
		v = 1.0 - interval(0, 1).clamp(v); // Flip V to image coordinates

		int i = int(u * image.width());
		int j = int(v * image.height());
		const unsigned char* pixel = image.pixel_data(i, j);

		double colour_scale = 1.0 / 255.0;
		return colour(colour_scale * pixel[0], colour_scale * pixel[1], colour_scale * pixel[2]);
	}
private:
	rtw_image image;
};

class noise_texture : public texture {
public:
	noise_texture() : scale(1.0) {}

	noise_texture(double scale) : scale(scale) {}

	colour value(double u, double v, const point3& p) const override {
		// Perlin interpolate can return negative values [-1, +1], so we must map to [0, 1]
		// return colour(1, 1, 1) * 0.5 * (1.0 + noise.noise(scale * p));
		
		// Perlin noise with turbulence
		// return colour(1, 1, 1) * noise.noise_turbulence(p, 7);

		// Marble like
		return colour(0.5, 0.5, 0.5) * (1 + sin(scale * p.z() + 10 * noise.noise_turbulence(p, 7)));
	}
private:
	perlin noise;
	double scale;
};


class mandelbrot_texture : public texture { // TODO: FIX THIS
public:
	mandelbrot_texture() : max_iterations(1000) {}

	mandelbrot_texture(int max_iterations) : max_iterations(max_iterations) {}

	colour value(double u, double v, const point3& p) const override {
		// Mandelbrot set equation
		double x0 = (u * 5 - 2.5);
		double y0 = (v * 5 - 2.5);
		double x = 0.0;
		double y = 0.0;
		int iteration = 0;
		while (x * x + y * y <= 2 * 2 && iteration < max_iterations) {
			double xtmp = x * x + y * y + x0;
			y = 2 * x * y + y0;
			x = xtmp;
			iteration++;
		}
		return colour(log(iteration) / 10.0, 1, 1);
	}
private:
	int max_iterations;
};

#endif