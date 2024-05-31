#ifndef SCENE_H
#define SCENE_H

#include "RayTracing.hpp"

#include "bvh.hpp"
#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "quad.hpp"
#include "sphere.hpp"
#include "texture.hpp"

void three_spheres() { //TODO Fix it so it looks the same as original
	// World
	hittable_list world;

	shared_ptr<material> material_ground = make_shared<lambertian>(colour(0.5, 0.5, 0.5));
	world.add(make_shared<sphere>(point3(0.0, -100000.5, -1.0), 100000.0, material_ground));

	shared_ptr<material> material_center = make_shared<lambertian>(colour(0.1, 0.2, 0.5));
	shared_ptr<material> material_left = make_shared<dielectric>(1.50);
	shared_ptr<material> material_bubble = make_shared<dielectric>(1.00 / 1.50);
	shared_ptr<material> material_right = make_shared<metal>(colour(0.8, 0.6, 0.2), 1.0);

	world.add(make_shared<sphere>(point3(0.0, 0.0, -1.2), 0.5, material_center));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.4, material_bubble));
	world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

	// Camera
	camera cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;

	cam.vfov = 90;
	cam.lookfrom = point3(0, 0, 0);
	cam.lookat = point3(0, 0, -1);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0.0;
	cam.focus_dist = 1.0;

	// Render
	cam.render(world);
}

void many_spheres(bool bouncing) {
	// World
	hittable_list world;

	shared_ptr<material> material_ground = make_shared<lambertian>(colour(0.5, 0.5, 0.5));
	world.add(make_shared<sphere>(point3(0.0, -100000, -1.0), 100000.0, material_ground));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			double choose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
			if ((center - point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphere_material;
				if (choose_mat < 0.8) {
					// Diffuse
					colour albedo = colour::random() * colour::random();
					sphere_material = make_shared<lambertian>(albedo);
					if (bouncing) {
						point3 end_center = center + vec3(0, random_double(0, 0.5), 0);
						world.add(make_shared<sphere>(center, end_center, 0.2, sphere_material));
					}
					else {
						world.add(make_shared<sphere>(center, 0.2, sphere_material));
					}
				}
				else if (choose_mat < 0.95) {
					// Metal
					colour albedo = colour::random(0.5, 1);
					double fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else {
					// Glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	shared_ptr<dielectric> material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	shared_ptr<lambertian> material2 = make_shared<lambertian>(colour(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	shared_ptr<metal> material3 = make_shared<metal>(colour(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	world = hittable_list(make_shared<bvh_node>(world));

	// Camera
	camera cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;

	cam.vfov = 20;
	cam.lookfrom = point3(13, 2, 3);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0.6;
	cam.focus_dist = 10.0;

	// Render
	cam.render(world);
}

void checkered_spheres() {
	// World
	hittable_list world;

	shared_ptr<checker_texture> checker = make_shared<checker_texture>(0.32, colour(0.2, 0.3, 0.1), colour(0.9, 0.9, 0.9));
	world.add(make_shared<sphere>(point3(0.0, -10.0, 0.0), 10.0, make_shared<lambertian>(checker)));
	world.add(make_shared<sphere>(point3(0.0, 10.0, 0.0), 10.0, make_shared<lambertian>(checker)));

	// Camera
	camera cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;

	cam.vfov = 20;
	cam.lookfrom = point3(13, 2, 3);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0.0;

	// Render
	cam.render(world);
}

void earth() {
	// World
	shared_ptr<image_texture> earth_texture = make_shared<image_texture>("earthmap.jpg");
	shared_ptr<lambertian> earth_surface = make_shared<lambertian>(earth_texture);
	shared_ptr<sphere> globe = make_shared<sphere>(point3(0.0, 0.0, 0.0), 2, earth_surface);

	// Camera
	camera cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 500;
	cam.max_depth = 50;

	cam.vfov = 20;
	cam.lookfrom = point3(0, 0, 12);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0.0;

	// Render
	cam.render(hittable_list(globe));
}

void perlin_spheres() {
	// World
	hittable_list world;

	shared_ptr<noise_texture> perlin_texture = make_shared<noise_texture>(4.0);
	shared_ptr<lambertian> perlin_surface = make_shared<lambertian>(perlin_texture);
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, perlin_surface));
	world.add(make_shared<sphere>(point3(0, 2, 0), 2, perlin_surface));

	// Camera
	camera cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 500;
	cam.max_depth = 50;

	cam.vfov = 20;
	cam.lookfrom = point3(13, 2, 3);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0.0;

	// Render
	cam.render(world);
}

void quads() {
	// World
	hittable_list world;

	// Materials
	shared_ptr<lambertian> left_red = make_shared<lambertian>(colour(1.0, 0.2, 0.2));
	shared_ptr<lambertian> back_green = make_shared<lambertian>(colour(0.2, 1.0, 0.2));
	shared_ptr<lambertian> right_blue = make_shared<lambertian>(colour(0.2, 0.2, 1.0));
	shared_ptr<lambertian> upper_orange = make_shared<lambertian>(colour(1.0, 0.5, 0.0));
	shared_ptr<lambertian> lower_teal = make_shared<lambertian>(colour(0.2, 0.8, 0.8));

	// Quads
	world.add(make_shared<quad>(point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red));
	world.add(make_shared<quad>(point3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
	world.add(make_shared<quad>(point3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
	world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
	world.add(make_shared<quad>(point3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4), lower_teal));

	// Camera
	camera cam;
	cam.aspect_ratio = 1.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 500;
	cam.max_depth = 50;

	cam.vfov = 80;
	cam.lookfrom = point3(0, 0, 9);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0.0;

	// Render
	cam.render(world);
}

void quads2() {
	// World
	hittable_list world;

	shared_ptr<noise_texture> perlin_texture = make_shared<noise_texture>(4.0);

	// Materials
	shared_ptr<lambertian> left_red = make_shared<lambertian>(colour(1.0, 0.2, 0.2));
	shared_ptr<lambertian> back_green = make_shared<lambertian>(colour(0.2, 1.0, 0.2));
	shared_ptr<lambertian> right_blue = make_shared<lambertian>(colour(0.2, 0.2, 1.0));
	shared_ptr<lambertian> upper_orange = make_shared<lambertian>(colour(1.0, 0.5, 0.0));
	shared_ptr<lambertian> lower_teal = make_shared<lambertian>(colour(0.2, 0.8, 0.8));

	// Quads
	world.add(make_shared<quad>(point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red));
	world.add(make_shared<triangle>(point3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
	world.add(make_shared<ellipse>(point3(3, 0, 1), vec3(0, 0, 2), vec3(0, 2, 0), right_blue));
	world.add(make_shared<annulus>(point3(0, 3, 1), vec3(1, 1, 0), vec3(0, 0, 1), 0.2, upper_orange));
	world.add(make_shared<texture_quad>(point3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4), perlin_texture, lower_teal));

	// Camera
	camera cam;
	cam.aspect_ratio = 1.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 500;
	cam.max_depth = 50;

	cam.vfov = 80;
	cam.lookfrom = point3(0, 0, 9);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0.0;

	// Render
	cam.render(world);
}
#endif