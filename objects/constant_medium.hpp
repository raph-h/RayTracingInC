#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.hpp"
#include "../material.hpp"
#include "../texture.hpp"

// Assumes the boundary shape is convex, so this implementation will work for boxes, spheres etc
// WARNING: This will not work with toruses or shapes that contain voids
// TODO: Implement arbitrary shape handling

class constant_medium : public hittable {
public:
	constant_medium(shared_ptr<hittable> boundary, double density, shared_ptr<texture> tex) : boundary(boundary), neg_inv_density(-1 / density), phase_function(make_shared<isotropic>(tex)) {}
	constant_medium(shared_ptr<hittable> boundary, double density, const colour& albedo) : boundary(boundary), neg_inv_density(-1 / density), phase_function(make_shared<isotropic>(albedo)) {}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		hit_record rec1, rec2;

		if (!boundary->hit(r, interval::universe, rec1))
			return false;
		
		if (!boundary->hit(r, interval(rec1.t + 0.0001, infinity), rec2))
			return false;

		if (rec1.t < ray_t.min) rec1.t = ray_t.min;
		if (rec2.t > ray_t.max) rec2.t = ray_t.max;

		if (rec1.t >= rec2.t)
			return false;

		if (rec1.t < 0)
			rec1.t = 0;

		double ray_length = r.direction().length();
		double distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
		double hit_distance = neg_inv_density * std::log(random_double());

		if (hit_distance > distance_inside_boundary)
			return false;

		rec.t = rec1.t + hit_distance / ray_length;
		rec.p = r.at(rec.t);

		rec.normal = vec3(1, 0, 0); // Arbitrary
		rec.front_face = true; // Arbitrary
		rec.mat_ptr = phase_function.get();

		return true;
	}

	aabb bounding_box() const override { return boundary->bounding_box(); }

	double pdf_value(const point3& origin, const vec3& direction) const override {
		return 0.0;
	}

	vec3 random(const point3& origin) const override {
		return vec3(1, 0, 0);
	}
private:
	shared_ptr<hittable> boundary;
	double neg_inv_density;
	shared_ptr<material> phase_function;
};

#endif