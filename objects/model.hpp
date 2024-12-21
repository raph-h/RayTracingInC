#ifndef MODEL_H
#define MODEL_H

#include <filesystem>
#include <string>
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
// Creates a box from the two points specified and material
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

#define MAX_VERTICES 131068

// Imports a model
inline shared_ptr<bvh_node> model(const char filePath[], const float scale, shared_ptr<material> mat)
{
	hittable_list finalModel;

	std::ifstream file(filePath);

	if (file.is_open()) {
		std::string buffer;
		int vertexCount = 0;

		std::clog << "Opened file '" << filePath << "'\n";

		// First round to count number of vertices
		while (file.good()) {
			file >> buffer;
			if (!buffer.compare("v")) {
				vertexCount++;
			}
		}
		if (vertexCount <= MAX_VERTICES) {
			file.clear();
			file.seekg(0);

			vec3* vertexArray = new vec3[vertexCount];
			vec3 vec3Buffer = vec3();
			int index = 0;
			int v1 = 0;
			int v2 = 0;
			int v3 = 0;

			// Second round to store data
			while (file.good()) {
				file >> buffer;
				if (!buffer.compare("v")) {
					file >> buffer;
					vec3Buffer[0] = stof(buffer) * scale;
					file >> buffer;
					vec3Buffer[1] = stof(buffer) * scale;
					file >> buffer;
					vec3Buffer[2] = stof(buffer) * scale;
					if (index < vertexCount) {
						vertexArray[index] = vec3Buffer;
					}
					
					index++;
				}
				else if (!buffer.compare("f")) {
					// Subtract 1 due to first vertex counted as 1
					file >> buffer;
					v1 = stoi(buffer) - 1;
					file >> buffer;
					v2 = stoi(buffer) - 1;
					file >> buffer;
					v3 = stoi(buffer) - 1;
					if (v1 < vertexCount && v2 < vertexCount && v3 < vertexCount) {
						//TODO: fix math errors here
						finalModel.add(make_shared<quad>(vertexArray[v1], vertexArray[v2] - vertexArray[v1], vertexArray[v3] - vertexArray[v1], mat));
					}
					else {
						std::clog << "More vertices referenced in face than counted\n";
					}
				}
			}
			delete[] vertexArray;
		}
		else {
			std::clog << "More than max vertices loaded: " << vertexCount << "\n";
		}
		file.close();
	}
	else {
		std::clog << "Couldn't open file '" << filePath << "'\n";
		// TODO: Not optimised as it converts char* to string to use some functions
		const size_t last_slash_idx = std::string(filePath).rfind('/');
		if (std::string::npos != last_slash_idx)
		{
			std::clog << "Here are a list of files in that folder\n";
			for (const auto& entry : std::filesystem::directory_iterator(std::string(filePath).substr(0, last_slash_idx)))
				std::clog << entry.path() << std::endl;
		}
	}
	// Just incase nothing is loaded, it doesn't cause a massive crash
	if (finalModel.objects.size() == 0) {
		finalModel.add(make_shared<quad>(vec3(0, 0, 0), vec3(1, 1, 1), vec3(0, 1, 1), mat));
	}

	return make_shared<bvh_node>(finalModel);
}
#endif