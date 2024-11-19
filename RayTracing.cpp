// RayTracing.cpp : Defines the entry point for the application.

#include <chrono>

#include "RayTracing.hpp"
//#include "scene.hpp"

#include "objects/bvh.hpp"
#include "camera.hpp"
#include "objects/constant_medium.hpp"
#include "objects/hittable.hpp"
#include "objects/hittable_list.hpp"
#include "material.hpp"
#include "objects/quad.hpp"
#include "objects/sphere.hpp"
#include "objects/box.hpp"
#include "texture.hpp"


int main(int argc, char *argv[])
{
	// Time
	const auto start_time = std::chrono::steady_clock::now();
	/*scene scene = SCENE_H::cornell_box2();;
	
	if (argc >= 2) {
		int value = atoi(argv[1]);
		switch (value)
		{
		case 1:
			scene = SCENE_H::many_spheres(false);
			break;
		case 2:
			scene = SCENE_H::many_spheres(true);
			break;
		case 3:
			scene = SCENE_H::checkered_spheres();
			break;
		case 4:
			scene = SCENE_H::earth();
			break;
		case 5:
			scene = SCENE_H::perlin_spheres();
			break;
		case 6:
			scene = SCENE_H::quads();
			break;
		case 7:
			scene = SCENE_H::quads2();
			break;
		case 8:
			scene = SCENE_H::mandelbrot();
			break;
		case 9:
			scene = SCENE_H::simple_light();
			break;
		case 10:
			scene = SCENE_H::cornell_box();
			break;
		case 11:
			scene = SCENE_H::cornell_box2();
			break;
		case 12:
			scene = SCENE_H::cornell_box3();
			break;
		case 13:
			scene = SCENE_H::cornell_smoke();
			break;
		case 14:
			scene = SCENE_H::final_scene();
			break;
		case 15:
			scene = SCENE_H::glass_boxes();
			break;
		default:
			scene = SCENE_H::three_spheres();
			break;
		}
	}
	if (argc >= 3) {
		int value = atoi(argv[2]);
		scene.high_res_render(value);
	}
	else {
		scene.render();
	}
	*/
	hittable_list world;
	hittable_list lights;

	auto red = make_shared<lambertian>(colour(.65, .05, .05));
	auto white = make_shared<lambertian>(colour(.73, .73, .73));
	auto green = make_shared<lambertian>(colour(.12, .45, .15));
	auto light = make_shared<diffuse_light>(colour(15, 15, 15));

	// Cornell box sides
	world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 0, 555), vec3(0, 555, 0), green));
	world.add(make_shared<quad>(point3(0, 0, 555), vec3(0, 0, -555), vec3(0, 555, 0), red));
	world.add(make_shared<quad>(point3(0, 555, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
	world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 0, -555), white));
	world.add(make_shared<quad>(point3(555, 0, 555), vec3(-555, 0, 0), vec3(0, 555, 0), white));

	// Light
	world.add(make_shared<quad>(point3(213, 554, 227), vec3(130, 0, 0), vec3(0, 0, 105), light));

	// Box
	auto aluminium = make_shared<metal>(colour(.73, .73, .73), 0.0);

	shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), aluminium);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));
	world.add(box1);


	/*shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
	box2 = make_shared<rotate_y>(box2, -18);
	box2 = make_shared<translate>(box2, vec3(130, 0, 65));
	world.add(box2);*/
	// Glass Sphere
	auto glass = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(190, 90, 190), 90, glass));

	// Light Sources
	auto empty_material = shared_ptr<material>();
	lights.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), empty_material));

	camera cam;

	cam.aspect_ratio = 1.0;
	cam.image_width = 600;
	cam.samples_per_pixel = 1000;
	cam.max_depth = 50;
	cam.background_bottom = colour(0, 0, 0);
	cam.background_top = colour(0, 0, 0);

	cam.vfov = 40;
	cam.lookfrom = point3(278, 278, -800);
	cam.lookat = point3(278, 278, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0;

	cam.render(world, lights);

	const auto end_time = std::chrono::steady_clock::now();
	const auto elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	std::clog << "elapsed time: " << elapsed_seconds / 1000.0 << "s\n";

	return 0;
}
