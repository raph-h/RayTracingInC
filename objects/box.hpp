#ifndef BOX_H
#define BOX_H

#include "bvh.hpp"
/*
class box : public hittable {
public:
	box(const vec3& a, const vec3& b, shared_ptr<material> mat) : mat(mat) {
		point3 min = point3(std::min(a.x(), b.x()), std::min(a.y(), b.y()), std::min(a.z(), b.z()));
		point3 max = point3(std::max(a.x(), b.x()), std::max(a.y(), b.y()), std::max(a.z(), b.z()));

		vec3 dx = vec3(max.x() - min.x(), 0, 0);
		vec3 dy = vec3(0, max.y() - min.y(), 0);
		vec3 dz = vec3(0, 0, max.z() - min.z());

		
		sides.add(make_shared<quad>(point3(min.x(), min.y(), max.z()),  dx,  dy, mat)); // front
		sides.add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz,  dy, mat)); // right
		sides.add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx,  dy, mat)); // back
		sides.add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dz,  dy, mat)); // left
		sides.add(make_shared<quad>(point3(min.x(), max.y(), max.z()),  dx, -dz, mat)); // top
		sides.add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dx,  dz, mat)); // bottom
		hierarchy = make_shared<bvh_node>(sides);

		bbox = aabb(box_min, box_max);
		bbox.pad_to_minimums();
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		// IMPROVED: Faster method as using stack memory (instead of dynamic), less memory, more cache friendly
		// Code obtained from https://github.com/define-private-public/PSRayTracing/blob/r1/src/Objects/Box.cpp
		return hierarchy->hit(r, ray_t, rec);
	}
	aabb bounding_box() const override { return bbox; }

private:
	vec3 box_min;
	vec3 box_max;
	aabb bbox;
	shared_ptr<material> mat;
	hittable_list sides{};
	shared_ptr<bvh_node> hierarchy;
};
*/

inline shared_ptr<bvh_node> box(const point3& a, const point3& b, shared_ptr<material> mat)
{
	// Returns the 3D box (six sides) that contains the two opposite vertices a & b.

	hittable_list sides;

	// Construct the two opposite vertices with the minimum and maximum coordinates.
	point3 min = point3(std::fmin(a.x(), b.x()), std::fmin(a.y(), b.y()), std::fmin(a.z(), b.z()));
	point3 max = point3(std::fmax(a.x(), b.x()), std::fmax(a.y(), b.y()), std::fmax(a.z(), b.z()));

	vec3 dx = vec3(max.x() - min.x(), 0, 0);
	vec3 dy = vec3(0, max.y() - min.y(), 0);
	vec3 dz = vec3(0, 0, max.z() - min.z());

	sides.add(make_shared<quad>(point3(min.x(), min.y(), max.z()), dx, dy, mat)); // front
	sides.add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz, dy, mat)); // right
	sides.add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx, dy, mat)); // back
	sides.add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dz, dy, mat)); // left
	sides.add(make_shared<quad>(point3(min.x(), max.y(), max.z()), dx, -dz, mat)); // top
	sides.add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dx, dz, mat)); // bottom

	return make_shared<bvh_node>(sides);
}
#endif