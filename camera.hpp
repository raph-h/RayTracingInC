#ifndef CAMERA_H
#define CAMERA_H

#include "objects/hittable.hpp"
#include "material.hpp"
#include "pdf.hpp"

class camera {
public:
	double aspect_ratio = 1; // Ratio of image width over height
	int image_width = 100; // Rendered image width in pixel count
	int samples_per_pixel = 10; // Count of random samples for each pixel
	int max_depth = 10; // Maximum number of ray bounces into scene (prevents recusion)
	colour background_bottom = colour(1.0, 1.0, 1.0); // Scene background colour on the bottom
	colour background_top = colour(0.5, 0.7, 1.0); // Scene background colour on the top

	double vfov = 90; // Verticla view angle (field of view)
	point3 lookfrom = point3(0, 0, 0); // Point camera is looking from
	point3 lookat = point3(0, 0, -1); // Point camera is looking at
	vec3 vup = vec3(0, 1, 0); // Camera-relative "up" direction

	double defocus_angle = 0; // Variation angle of rays through each pixel
	double focus_dist = 10; // Distance from camera lookfrom point to plane of perfect focus

	void render(const hittable& world, const hittable& lights) {
		initialize();

		std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
		for (int j = 0; j < image_height; j++)
		{
			std::clog << "\rScanlines remaining: " << (image_height - j) << " / " << image_height << " (" << (j * 100 / image_height) << "%)     " << std::flush;
			for (int i = 0; i < image_width; i++)
			{
				colour pixel_colour(0, 0, 0);
				// Stratified to improve render quality
				for (int s_j = 0; s_j < sqrt_spp; s_j++) {
					for (int s_i = 0; s_i < sqrt_spp; s_i++) {
						ray r = get_ray(i, j, s_i, s_j);
						pixel_colour += ray_colour(r, max_depth, world, lights);
					}
				}
				write_colour(std::cout, pixel_samples_scale * pixel_colour);
			}
		}
		std::clog << "\rDone.                              \n";
	}
private:
	int image_height; // Rendered image height
	double pixel_samples_scale; // Colour scale factor for a sum of pixel samples
	int sqrt_spp; // Square root of number of samples per pixel
	double reciprocal_sqrt_spp; // 1 / sqrt_spp
	
	point3 center; // Camera center
	vec3 pixel_delta_u; // Offset to pixel to the right
	vec3 pixel_delta_v; // Offset to pixel below
	point3 pixel00_loc; // Location of pixel 0, 0
	vec3 u, v, w; // Camera frame basis vectors
	vec3 defocus_disk_u; // Defocus disk horizontal radius
	vec3 defocus_disk_v; // Defocus disk vertial radius

	void initialize() {
		// Calculate the image height, and ensure that it's at least 1
		image_height = int(image_width / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;

		sqrt_spp = int(std::sqrt(samples_per_pixel));
		reciprocal_sqrt_spp = 1.0 / sqrt_spp;
		pixel_samples_scale = 1.0 / (sqrt_spp * sqrt_spp);

		center = lookfrom;

		// Determine viewport dimensions
		double h = std::tan(degrees_to_radians(vfov) / 2);
		double viewport_height = 2 * h * focus_dist; // Viewport widths less than one are ok since they are real valued
		double viewport_width = viewport_height * (double(image_width) / image_height);

		// Calculate the u, v, w unit basis vectors for the camera coordinate frame
		w = unit_vector(lookfrom - lookat); // Unit vector opposite of looking direction
		u = unit_vector(cross(vup, w)); // Unit vector perpendicular to up direction and opposite of looking direction
		v = cross(w, u);

		// Calculate the vectors across the horizontal and down the vertical viewport edges
		vec3 viewport_u = viewport_width * u; // Vector across viewport horizontal edge
		vec3 viewport_v = viewport_height * -v; // Vector down viewport vertical edge

		// Calculate the horizontal and verical delta vectors from pixel to pixel
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		// Calculate the location of the upper left pixel
		vec3 viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
	
		// Calculate the camera defocus disk basis vectors
		double defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;
	}

	ray get_ray(int i, int j, int s_i, int s_j) const {
		// Construct a camera ray originating from the defocus disk and directed at randomly sampled points around the pixel location i, j for stratified sample square s_i, s_j
		vec3 offset = sample_square_stratified(s_i, s_j); //TODO: Can also use sample_disk(0.5);, or sample_square();
		vec3 pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);
		vec3 ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
		vec3 ray_direction = pixel_sample - ray_origin;
		double ray_time = random_double();

		return ray(ray_origin, ray_direction, ray_time);
	}

	vec3 sample_square_stratified(int s_i, int s_j) const {
		// Returns the vector to a random point in the square sub-pixel specified by grid indices s_i and s_j, for an idealized unit square pixel [-0.5, -0.5] to [0.5, 0.5]
		double px = ((s_i + random_double()) * reciprocal_sqrt_spp) - 0.5;
		double py = ((s_j + random_double()) * reciprocal_sqrt_spp) - 0.5;
		return vec3(px, py, 0);
	}

	vec3 sample_square() const {
		// Returns the vector to a random point in the [-0.5, -0.5]-[+0.5, +0.5] unit square
		return vec3(random_double() - 0.5, random_double() - 0.5, 0);
	}

	vec3 sample_disk(double radius) const {
		// Returns a random point in the unit (radius 0.5) disk centered at the origin
		return radius * random_in_unit_disk();
	}

	point3 defocus_disk_sample() const {
		// Returns a random point in the camera defocus disk
		vec3 p = random_in_unit_disk();
		return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
	}

	colour ray_colour(const ray& r, int depth, const hittable& world, const hittable& lights) const {
		// If we've exceeded the ray bounce limit, no more light is gathered
		if (depth <= 0)
			return colour(0, 0, 0);
		
		hit_record rec;
		// If the ray hits nothing, return the background colour
		if (!world.hit(r, interval(0.001, infinity), rec)) {
			vec3 unit_direction = unit_vector(r.direction());
			double a = 0.5 * (unit_direction.y() + 1.0);
			// Linear interpolation between background bottom and top
			return (1.0 - a) * background_bottom + a * background_top;
		}

		scatter_record srec;
		colour emission_colour = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);
		if (!rec.mat_ptr->scatter(r, rec, srec)) {
			return emission_colour;
		}

		if (srec.skip_pdf) {
			return srec.attenuation * ray_colour(srec.skip_pdf_ray, depth - 1, world, lights); // TODO: what about emission_colour
		}
		shared_ptr<hittable_pdf> light_ptr = make_shared<hittable_pdf>(lights, rec.p);
		mixture_pdf p(light_ptr, srec.pdf_ptr);

		ray scattered = ray(rec.p, p.generate(), r.time());
		double pdf_value = p.value(scattered.direction());

		double scattering_pdf = rec.mat_ptr->scattering_pdf(r, rec, scattered);
		
		colour sample_colour = ray_colour(scattered, depth - 1, world, lights);
		colour scatter_colour = (srec.attenuation * scattering_pdf * sample_colour) / pdf_value;

		return emission_colour + scatter_colour;
	}
};

#endif