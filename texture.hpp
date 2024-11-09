#ifndef TEXTURE_H
#define TEXTURE_H

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
		: checker_texture(scale, make_shared<solid_colour>(c1), make_shared<solid_colour>(c2)) {}

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
	noise_texture(double scale) : scale(scale) {}

	colour value(double u, double v, const point3& p) const override {
		// Perlin interpolate can return negative values [-1, +1], so we must map to [0, 1]
		// return colour(1, 1, 1) * 0.5 * (1.0 + noise.noise(scale * p));
		
		// Perlin noise with turbulence
		// return colour(1, 1, 1) * noise.noise_turbulence(p, 7);

		// Marble like
		return colour(0.5, 0.5, 0.5) * (1 + std::sin(scale * p.z() + 10 * noise.noise_turbulence(p, 7)));
	}
private:
	perlin noise;
	double scale;
};


class mandelbrot_texture : public texture {
public:
	mandelbrot_texture() : max_limit(1000) {}
	mandelbrot_texture(int max_limit) : max_limit(max_limit) {}

	colour value(double u, double v, const point3& p) const override {
		// Mandelbrot set equation
		double x = (u * 4 - 2);
		double y = (v * 4 - 2);
		int value = mandle(x, y);
		double q = static_cast<double>(value) / max_limit;
		return colour(q, q, q);
	}
private:
	int max_limit;
	int mandle(double real, double imag) const {
		int limit = max_limit;
		double zReal = real;
		double zImag = imag;
		for (int i = 0; i < limit; ++i) {
			double r2 = zReal * zReal;
			double i2 = zImag * zImag;
			if (r2 + i2 > 4.0) return i;

			zImag = 2.0 * zReal * zImag + imag;
			zReal = r2 - i2 + real;
		}
		return limit;
	}
};

#endif