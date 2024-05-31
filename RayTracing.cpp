// RayTracing.cpp : Defines the entry point for the application.

#include <chrono>

#include "RayTracing.hpp"
#include "scene.hpp"

int main(int argc, char *argv[])
{
	// Time
	const auto start_time = std::chrono::steady_clock::now();

	if (argc >= 2) {
		int value = atoi(argv[1]);
		switch (value)
		{
		case 1:
			SCENE_H::many_spheres(false);
			break;
		case 2:
			SCENE_H::many_spheres(true);
			break;
		case 3:
			SCENE_H::checkered_spheres();
			break;
		case 4:
			SCENE_H::earth();
			break;
		case 5:
			SCENE_H::perlin_spheres();
			break;
		case 6:
			SCENE_H::quads();
			break;
		case 7:
			SCENE_H::quads2();
			break;
		default:
			SCENE_H::three_spheres();
			break;
		}
	}
	else {
		SCENE_H::three_spheres();
	}

	const auto end_time = std::chrono::steady_clock::now();
	const auto elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	std::clog << "elapsed time: " << elapsed_seconds / 1000.0 << "s\n";

	return 0;
}
