// RayTracing.cpp : Defines the entry point for the application.

#include <chrono>

#include "RayTracing.hpp"
#include "scene.hpp"

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
	scene scene = SCENE_H::cornell_box2();;
	
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
		case 13: // Black
			scene = SCENE_H::cornell_smoke();
			break;
		case 14: // Black
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

	const auto end_time = std::chrono::steady_clock::now();
	const auto elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	std::clog << "elapsed time: " << elapsed_seconds / 1000.0 << "s\n";

	return 0;
}
