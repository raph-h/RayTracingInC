#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable {
public:
	// Stationary Sphere
	sphere(const point3& center, double radius, shared_ptr<material> mat)
		: start_center(center), radius(fmax(0, radius)), mat(mat), is_moving(false) {}
	
	// Moving Sphere
	sphere(const point3& start_center, const point3& end_center, double radius, shared_ptr<material> mat)
		: start_center(start_center), radius(fmax(0, radius)), mat(mat), is_moving(true) {
		center_vec = end_center - start_center;
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		point3 center = is_moving ? sphere_center(r.time()) : start_center;
		vec3 oc = start_center - r.origin();
		// Mathematically vec3.length_squared() is the same as dot of a vec3 with itself
		double a = r.direction().length_squared();
		double h = dot(r.direction(), oc); // The -2.0 will simplify itself out
		double c = oc.length_squared() - radius * radius;
		double discriminant = h * h - a * c; // Simplified out because of -2.0
		if (discriminant < 0) {
			return false;
		}
		double sqrtd = sqrt(discriminant);

		// Find the nearest root that lies in the acceptable range
		double root = (h - sqrtd) / a;
		if (!ray_t.surrounds(root)) {
			root = (h + sqrtd) / a;
			if (!ray_t.surrounds(root))
				return false;
		}

		rec.t = root;
		rec.p = r.at(rec.t);
		vec3 outward_normal = (rec.p - center) / radius;
		rec.set_face_normal(r, outward_normal);
		rec.mat_ptr = mat.get();

		return true;
	}
private:
	point3 start_center;
	double radius;
	shared_ptr<material> mat;

	bool is_moving;
	vec3 center_vec;

	point3 sphere_center(double time) const {
		// Linearly interpolate from start_center to end_center according to time, where t=0 yields start_center and t=1 yields end_center
		return start_center + time * center_vec;
	}
};

#endif