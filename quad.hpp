#ifndef QUAD_H
#define QUAD_H

#include "RayTracing.hpp"
#include "hittable.hpp"

class quad : public hittable {
public:
	quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) : Q(Q), u(u), v(v), mat(mat) {
		vec3 n = cross(u, v);
		normal = unit_vector(n);
		D = dot(normal, Q);

		w = n / dot(n, n);

		set_bounding_box();
	}

	virtual void set_bounding_box() {
		// Compute the bounding box of all four vertices
		aabb bbox_diagonal1 = aabb(Q, Q + u + v);
		aabb bbox_diagonal2 = aabb(Q + u, Q + v);
		bbox = aabb(bbox_diagonal1, bbox_diagonal2);
	}

	aabb bounding_box() const override { return bbox; }

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		double denom = dot(normal, r.direction());

		// No hit if the ray is parallel to the plane
		if (fabs(denom) < 1e-8)
			return false;

		// Return false if the hit point parameter t is outside the ray interval
		// n.v = D, n.(P+td)=D, n.P + n.td = D, t = (D - n.P)/(n.d)
		double t = (D - dot(normal, r.origin())) / denom;
		if (!ray_t.contains(t))
			return false;

		point3 intersection = r.at(t);

		rec.t = t;
		rec.p = intersection;
		rec.mat_ptr = mat.get();
		rec.set_face_normal(r, normal);
		return true;
	}
private:
	point3 Q;
	vec3 u, v;
	vec3 w;
	shared_ptr<material> mat;
	aabb bbox;
	vec3 normal;
	double D; //Ax+By+Cz=D, where n = nxv
};

#endif