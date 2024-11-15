#ifndef  MATERIAL_H
#define MATERIAL_H

#include "objects/hittable.hpp"
#include "texture.hpp"

class material {
public:
	virtual ~material() = default;

	virtual colour emitted(double u, double v, const point3& p) const {
		return colour(0, 0, 0);
	}

	virtual bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const {
		return false;
	}
};
/*
class uniform : public material {
public:
	uniform(const colour& albedo) : albedo(albedo) {}

	bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
		vec3 scatter_direction = random_on_hemisphere(rec.normal);

		scattered = ray(rec.p, scatter_direction, r_in.time());
		attenuation = albedo;
		return true;

		if (random_double() < scatterProbability) { // Scatter with fixed probability p
			attenuation = albedo / scatterProbability; // Attenuation be albedo / p
			scattered = ray(rec.p, scatter_direction);
			return true;
		}
		return false;
	}

private:
	colour albedo;
};*/

class lambertian : public material {
public:
	lambertian(const colour& albedo) : tex(make_shared<solid_colour>(albedo)) {}
	lambertian(shared_ptr<texture> tex) : tex(tex) {}

	bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
		vec3 scatter_direction = rec.normal + random_unit_vector();
		
		// Catch degenerate scatter direction
		if (scatter_direction.near_zero())
			scatter_direction = rec.normal;

		scattered = ray(rec.p, scatter_direction, r_in.time());
		attenuation = tex->value(rec.u, rec.v, rec.p);
		return true;
		/*if (random_double() < scatterProbability) { // Scatter with fixed probability p
			attenuation = albedo / scatterProbability; // Attenuation be albedo / p
			scattered = ray(rec.p, scatter_direction);
			return true;
		}
		return false;*/
	}

private:
	shared_ptr<texture> tex;
};

class metal : public material {
public:
	metal(const colour& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

	bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
		scattered = ray(rec.p, reflected, r_in.time());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}

private:
	colour albedo;
	double fuzz;
};

class dielectric : public material {
public:
	dielectric(double refraction_index) : refraction_index(refraction_index) {}

	bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
		double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;
		
		vec3 unit_direction = unit_vector(r_in.direction());
		double cos_theta = std::min(dot(-unit_direction, rec.normal), 1.0);
		double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

		bool cannot_refract = ri * sin_theta > 1.0;
		vec3 direction;
		
		if (cannot_refract || reflectance(cos_theta, ri) > random_double())
			direction = reflect(unit_direction, rec.normal);
		else
			direction = refract(unit_direction, rec.normal, ri);

		scattered = ray(rec.p, direction, r_in.time());
		attenuation = colour(1.0, 1.0, 1.0);
		return true;
	}

private:
	// Refractive index in vacuum or air, or the ratio of the material's refractive index over the refractive index of the enclosing media
	double refraction_index;

	static double reflectance(double cosine, double refraction_index) {
		// Use Schlick's approximation for reflectance
		// IMPROVED: Instead of using pow, we will hand write out all the multiples as pow is slow
		// Also utilises computer vector instructions for performance
		// However does not seem to make a difference
		const double alpha = 1 - cosine;
		const double beta = alpha * alpha * alpha * alpha * alpha;

		double r = (1 - refraction_index) / (1 + refraction_index);
		r = r * r;
		return r + (1 - r) * beta; // std::pow((1 - cosine), 5);
	}
};

class diffuse_light : public material {
public:
	diffuse_light(shared_ptr<texture> tex) : tex(tex) {}
	diffuse_light(const colour& emit) : tex(make_shared<solid_colour>(emit)) {}

	colour emitted(double u, double v, const point3& p) const override {
		return tex->value(u, v, p);
	}
private:
	shared_ptr<texture> tex;
};

class isotropic : public material {
public:
	isotropic(const colour& albedo) : tex(make_shared<solid_colour>(albedo)) {}
	isotropic(shared_ptr<texture> tex) : tex(tex) {}

	bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
		scattered = ray(rec.p, random_unit_vector(), r_in.time());
		attenuation = tex->value(rec.u, rec.v, rec.p);
		return true;
	}
	
private:
	shared_ptr<texture> tex;
};

#endif