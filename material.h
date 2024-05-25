#ifndef  MATERIAL_H
#define MATERIAL_H

#include "RayTracing.h"

class hit_record;

class material {
public:
	virtual ~material() = default;

	virtual bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const {
		return false;
	}
};

class uniform : public material {
public:
	uniform(const colour& albedo) : albedo(albedo) {}

	bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
		vec3 scatter_direction = random_on_hemisphere(rec.normal);

		scattered = ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true;

		/*if (random_double() < scatterProbability) { // Scatter with fixed probability p
			attenuation = albedo / scatterProbability; // Attenuation be albedo / p
			scattered = ray(rec.p, scatter_direction);
			return true;
		}
		return false;*/
	}

private:
	colour albedo;
};

class lambertian : public material {
public:

	lambertian(const colour& albedo) : albedo(albedo) {}

	bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
		vec3 scatter_direction = rec.normal + random_unit_vector();
		
		// Catch degenerate scatter direction
		if (scatter_direction.near_zero())
			scatter_direction = rec.normal;

		scattered = ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true;
		/*if (random_double() < scatterProbability) { // Scatter with fixed probability p
			attenuation = albedo / scatterProbability; // Attenuation be albedo / p
			scattered = ray(rec.p, scatter_direction);
			return true;
		}
		return false;*/
	}

private:
	colour albedo;
};

class metal : public material {
public:
	metal(const colour& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

	bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
		scattered = ray(rec.p, reflected);
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}

private:
	colour albedo;
	double fuzz;
};

#endif