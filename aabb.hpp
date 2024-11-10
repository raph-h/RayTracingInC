#ifndef AABB_H
#define AABB_H

class aabb {
public:
	interval x, y, z;

	aabb() {} // The default AABB is empty, since intervals are empty by default

	aabb(const interval& x, const interval& y, const interval& z) : x(x), y(y), z(z) {}

	aabb(const point3& a, const point3& b) {
		// Treat the two points a and b as extrema for the bounding box, so we don't require a particular minimum / maximum coordinate order
		x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
		y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
		z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
	}

	aabb(const aabb& box1, const aabb& box2) {
		x = interval(box1.x, box2.x);
		y = interval(box1.y, box2.y);
		z = interval(box1.z, box2.z);
	}

	const interval& axis_interval(int n) const {
		if (n == 1) return y;
		if (n == 2) return z;
		return x;
	}

	bool hit(const ray& r, interval ray_t) const {
		const point3& ray_orig = r.origin();
		const vec3& ray_dir = r.direction();
		
		for (int axis = 0; axis < 3; axis++) {
			const interval& ax = axis_interval(axis);
			const double adinv = 1.0 / ray_dir[axis];

			double t0 = (ax.min - ray_orig[axis]) * adinv;
			double t1 = (ax.max - ray_orig[axis]) * adinv;
			
			// Somehow also slower
			//if (adinv < 0)
			//	std::swap(t0, t1);
			//ray_t.min = (t0 > ray_t.min) ? t0 : ray_t.min;
			//ray_t.max = (t1 < ray_t.max) ? t1 : ray_t.max;
			
			if (t0 < t1) {
				if (t0 > ray_t.min) ray_t.min = t0;
				if (t1 < ray_t.max) ray_t.max = t1;
			}
			else {
				if (t1 > ray_t.min) ray_t.min = t1;
				if (t0 < ray_t.max) ray_t.max = t0;
			}

			if (ray_t.max <= ray_t.min)
				return false;
		}
		return true;
		/*
		// ALTERNATE: "Faster" method using vector instructions
		// TESTED on 28/5/2024 - Took 264.061s vs 84.556s ~ 3 times slower
		// This method is a bit faster because of modern CPUs (vector instructions) and newer compilers (doing auto-vectorization)
		// Adapted from: https://medium.com/@bromanz/another-view-on-the-classic-ray-aabb-intersection-algorithm-for-bvh-traversal-41125138b525
		const double t0[3] = {
			(x.min - ray_orig[0]) / r.direction()[0],
			(y.min - ray_orig[1]) / r.direction()[1],
			(z.min - ray_orig[2]) / r.direction()[2]
		};

		const double t1[3] = {
			(x.max - ray_orig[0]) / r.direction()[0],
			(y.max - ray_orig[1]) / r.direction()[1],
			(z.max - ray_orig[2]) / r.direction()[2]
		};

		const double t_smaller[3] = {
			std::min(t0[0], t1[0]),
			std::min(t0[1], t1[1]),
			std::min(t0[2], t1[2])
		};

		const double t_larger[3] = {
			std::max(t0[0], t1[0]),
			std::max(t0[1], t1[1]),
			std::max(t0[2], t1[2])
		};

		ray_t.min = std::max({ ray_t.min, t_smaller[0], t_smaller[1], t_smaller[2] });
		ray_t.max = std::min({ ray_t.max, t_larger[0], t_larger[1], t_larger[2] });
		return (ray_t.min < ray_t.max);
		*/
	}

	int longest_axis() const {
		// Returns the index of the longest axis of the bounding box
		if (x.size() > y.size())
			return x.size() > z.size() ? 0 : 2;
		else
			return y.size() > z.size() ? 1 : 2;
	}

	void pad_to_minimums() {
		// Adjust the AABB so that no side is narrower than some delta, padding if necessary
		double delta = 0.0001;
		if (x.size() < delta)
			x = x.expand(delta);
		if (y.size() < delta)
			y = y.expand(delta);
		if (z.size() < delta)
			z = z.expand(delta);
	}
	
	static const aabb empty, universe;
};

const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);

aabb operator +(const aabb& bbox, const vec3& offset) {
	return aabb(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}

aabb operator +(const vec3& offset, const aabb& bbox) {
	return bbox + offset;
}

#endif