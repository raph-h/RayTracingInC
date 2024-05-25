// RayTracing.cpp : Defines the entry point for the application.


#include "RayTracing.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

int main()
{
	// World
	hittable_list world;

	shared_ptr<material> material_ground = make_shared<lambertian>(colour(0.8, 0.8, 0.0));
	shared_ptr<material> material_center = make_shared<lambertian>(colour(0.1, 0.2, 0.5));
	shared_ptr<material> material_left = make_shared<metal>(colour(0.8, 0.8, 0.8), 0.3);
	shared_ptr<material> material_right = make_shared<metal>(colour(0.8, 0.6, 0.2), 1.0);

	world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
	world.add(make_shared<sphere>(point3(0.0, 0.0, -1.2), 0.5, material_center));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

	// Camera
	camera cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400; // 400, 1000
	cam.samples_per_pixel = 100; // 100, 5000
	cam.max_depth = 20; // 50, 500

	// Render
	cam.render(world);
	return 0;
}
