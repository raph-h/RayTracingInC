#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.hpp"

class sphere : public hittable {
public:
	// Stationary Sphere
	sphere(const point3& center, double radius, shared_ptr<material> mat)
		: start_center(center), radius(std::max(0.0, radius)), mat(mat), is_moving(false) {
		vec3 rvec = vec3(radius, radius, radius);
		bbox = aabb(start_center - rvec, start_center + rvec);
	}
	
	// Moving Sphere
	sphere(const point3& start_center, const point3& end_center, double radius, shared_ptr<material> mat)
		: start_center(start_center), radius(std::max(0.0, radius)), mat(mat), is_moving(true) {
		center_vec = end_center - start_center;

		vec3 rvec = vec3(radius, radius, radius);
		aabb box1(start_center - rvec, start_center + rvec);
		aabb box2(end_center - rvec, end_center + rvec);
		bbox = aabb(box1, box2);
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		point3 center = is_moving ? sphere_center(r.time()) : start_center;
		vec3 oc = center - r.origin();
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
		rec.p = r.at(root);
		vec3 outward_normal = (rec.p - center) / radius;
		rec.set_face_normal(r, outward_normal);
		get_sphere_uv(outward_normal, rec.u, rec.v); // Set's the u and v values of rec
		rec.mat_ptr = mat.get();

		return true;
	}

	aabb bounding_box() const override {
		return bbox;
	}
private:
	point3 start_center;
	double radius;
	shared_ptr<material> mat;

	bool is_moving;
	vec3 center_vec;

	aabb bbox; // Bounding box

	point3 sphere_center(double time) const {
		// Linearly interpolate from start_center to end_center according to time, where t=0 yields start_center and t=1 yields end_center
		return start_center + time * center_vec;
	}

	static void get_sphere_uv(const point3& p, double& u, double& v) {
		// p: a given point on the sphere of radius one, centered at the origin
		// u: returned value [0, 1] of angle around the Y axis from X = -1
		// v: returned value [0, 1] of angle from Y = -1 to Y = +1
		// <1 0 0> yields <0.50 0.50>	<-1  0  0> yields <0.00 0.50>
		// <0 1 0> yields <0.50 1.00>	< 0 -1  0> yields <0.50 0.00>
		// <0 0 1> yields <0.25 0.50>	< 0  0 -1> yields <0.75 0.50>
		double theta = acos(-p.y());
		double phi = atan2(-p.z(), p.x()) + pi;

		u = phi / (2 * pi);
		v = theta / pi;
	}
};

#endif