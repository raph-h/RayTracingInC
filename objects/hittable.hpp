#ifndef HITTABLE_H
#define HITTABLE_H

#include "aabb.hpp"

class material;

class hit_record {
public:
	point3 p;
	vec3 normal;
	//shared_ptr<material> mat;
	material* mat_ptr; // IMPROVED: By using a normal pointer, we improve performance
	double t; // t for calculating when the rays collide - the t value
	double u; // u and v for textures
	double v;
	bool front_face;

	void set_face_normal(const ray& r, const vec3& outward_normal) {
		// Sets the hit record normal vector
		// NOTE: the parameter 'outward_normal' is assumed to have unit length

		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable {
public:
	virtual ~hittable() = default;

	virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

	virtual aabb bounding_box() const = 0;

	virtual double pdf_value(const point3& origin, const vec3& direction) const {
		return 0.0;
	}

	virtual vec3 random(const point3& origin) const {
		return vec3(1, 0, 0);
	}
};
// TODO: Implement scaling - Surface normal transformations will need to be researched
class translate : public hittable {
public:
	translate(shared_ptr<hittable> object, const vec3& offset) : object(object), offset(offset) {
		bbox = object->bounding_box().translate(offset);
	}
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		// Move the ray backwards by the offset
		ray offset_r(r.origin() - offset, r.direction(), r.time());

		// Determine whether an intersection exists along the offset ray (and if so, where)
		if (!object->hit(offset_r, ray_t, rec))
			return false;

		// Move the intersection point forwards by the offset
		rec.p += offset;

		return true;
	}

	aabb bounding_box() const override { return bbox; }
private:
	shared_ptr<hittable> object;
	vec3 offset;
	aabb bbox;
};

class rotate_y : public hittable {
public:
	rotate_y(shared_ptr<hittable> object, double angle) : object(object) {
		double radians = degrees_to_radians(angle);
		sin_theta = std::sin(radians);
		cos_theta = std::cos(radians);
		bbox = object->bounding_box().rotate_y(angle);
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		// Transform the ray from world space to object space
		vec3 origin = vec3((cos_theta * r.origin().x()) - (sin_theta * r.origin().z()), r.origin().y(), (sin_theta * r.origin().x()) + (cos_theta * r.origin().z()));
		vec3 direction = vec3((cos_theta * r.direction().x()) - (sin_theta * r.direction().z()), r.direction().y(), (sin_theta * r.direction().x()) + (cos_theta * r.direction().z()));

		ray rotated_r(origin, direction, r.time());

		// Determine whether an intersection exists in object space (and if so, where)
		if (!object->hit(rotated_r, ray_t, rec))
			return false;

		// Transform the intersection point from object space to world space
		rec.p = vec3((cos_theta * rec.p.x()) + (sin_theta * rec.p.z()), rec.p.y(), (-sin_theta * rec.p.x()) + (cos_theta * rec.p.z()));

		// Transform the normal from object space to world space
		rec.normal = vec3((cos_theta * rec.normal.x()) + (sin_theta * rec.normal.z()), rec.normal.y(), (-sin_theta * rec.normal.x()) + (cos_theta * rec.normal.z()));

		return true;
	}

	aabb bounding_box() const override { return bbox; }
private:
	shared_ptr<hittable> object;
	double sin_theta;
	double cos_theta;
	aabb bbox;
};

#endif