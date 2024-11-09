#ifndef QUAD_H
#define QUAD_H

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
		bbox.pad_to_minimums();
	}

	aabb bounding_box() const override { return bbox; }

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		double denom = dot(normal, r.direction());

		// No hit if the ray is parallel to the plane
		if (std::fabs(denom) < 1e-8)
			return false;

		// Return false if the hit point parameter t is outside the ray interval
		// n.v = D, n.(P+td)=D, n.P + n.td = D, t = (D - n.P)/(n.d)
		double t = (D - dot(normal, r.origin())) / denom;
		if (!ray_t.contains(t))
			return false;

		// Determine the hit point lies within the planar shape using its plane coordinates
		const point3 intersection = r.at(t);
		// See derivatation in part 6.5
		const vec3 planar_hitpt_vector = intersection - Q;
		const double alpha = dot(w, cross(planar_hitpt_vector, v));
		const double beta = dot(w, cross(u, planar_hitpt_vector));
		if (!is_interior(alpha, beta, rec, intersection))
			return false;
		
		// Ray hits the 2D shape, set the rest of the hit record data and return true
		rec.t = t;
		rec.p = intersection;
		rec.mat_ptr = mat.get();
		rec.set_face_normal(r, normal);
		return true;
	}

	virtual bool is_interior(double a, double b, hit_record& rec, point3 intersection) const {
		interval unit_interval = interval(0, 1);
		// Given the hit point in plane coordinates, return false if it is outside the primitive, otherwise set the hit record UV coordinates and return true

		if (!unit_interval.contains(a) || !unit_interval.contains(b))
			return false;
		rec.u = a;
		rec.v = b;
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

class triangle : public quad {
public:
	triangle(const point3& origin, const vec3& a, const vec3& b, shared_ptr <material> mat) : quad(origin, a, b, mat) {}

	virtual bool is_interior(double a, double b, hit_record& rec, point3 intersection) const override {
		if ((a + b) > 1 || a < 0 || b < 0)
			return false;
		rec.u = a;
		rec.v = b;
		return true;
	}
};

class ellipse : public quad {
public:
	ellipse(const point3& centre, const vec3& a, const vec3& b, shared_ptr <material> mat) : quad(centre, a, b, mat) {}

	virtual bool is_interior(double a, double b, hit_record& rec, point3 intersection) const override {
		if ((a * a + b * b) > 1)
			return false;
		rec.u = a / 2 + 0.5;
		rec.v = b / 2 + 0.5;
		return true;
	}
};

class annulus : public quad {
public:
	annulus(const point3& center, const vec3& a, const vec3& b, double radius, shared_ptr<material> mat) : quad(center, a, b, mat), radius(radius) {}
	
	virtual bool is_interior(double a, double b, hit_record& rec, point3 intersection) const override {
		double distance = a * a + b * b;
		if (distance > 1 || distance < radius * radius)
			return false;
		rec.u = a / 2 + 0.5;
		rec.v = b / 2 + 0.5;
		return true;
	}
private:
	double radius;
};

class texture_quad : public quad {
public:
	texture_quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<texture> tex, shared_ptr<material> mat) : quad(Q, u, v, mat), tex(tex) {}

	virtual bool is_interior(double a, double b, hit_record& rec, point3 intersection) const override {
		interval unit_interval = interval(0, 1);

		if (tex->value(a, b, intersection).length_squared() < 0.5 || !unit_interval.contains(a) || !unit_interval.contains(b)) // Does not hit if the texture is not white
			return false;
		rec.u = a;
		rec.v = b;
		return true;
	}
private:
	shared_ptr<texture> tex;
};
#endif