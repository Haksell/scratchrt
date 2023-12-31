#include "minirt.h"

static bool	get_root(t_ray *ray, const t_sphere *sphere, t_interval interval,
	float *root)
{
	const t_vec3	oc = ray->origin - sphere->center;
	const float		aa = vec3_length_squared(ray->direction);
	const float		minus_half_b = -vec3_dot(oc, ray->direction);
	const float		cc = vec3_length_squared(oc)
		- sphere->radius * sphere->radius;
	float			discriminant;

	discriminant = minus_half_b * minus_half_b - aa * cc;
	if (discriminant < 0)
		return (false);
	discriminant = sqrt(discriminant);
	*root = (minus_half_b - discriminant) / aa;
	if (in_interval(interval, *root))
		return (true);
	*root = (minus_half_b + discriminant) / aa;
	return (in_interval(interval, *root));
}

static t_vec3	img_pix_get(const t_image *img, int x, int y)
{
	char	*pixel;
	int		color;
	t_vec3	color_vec;

	pixel = img->addr + (y * img->line_length + x * (img->bpp / 8));
	color = *((int *) pixel);
	color_vec[X] = ((color >> 16) & 0xFF) / 255.0;
	color_vec[Y] = ((color >> 8) & 0xFF) / 255.0;
	color_vec[Z] = (color & 0xFF) / 255.0;
	return (color_vec);
}

static	t_vec3	get_hit_color_sphere(const t_hit *hit, const t_sphere *sphere)
{
	float	u;
	float	v;
	t_vec3	corrected_hit_point;

	corrected_hit_point = hit->point - sphere->center;
	u = atan2(corrected_hit_point[X],
			corrected_hit_point[Z]) / (2 * M_PI) + 0.5;
	v = acos(corrected_hit_point[Y] / sphere->radius) / M_PI;
	if (sphere->texture.type == TEXTURE_NONE)
		return (sphere->material.albedo);
	else if (sphere->texture.type == TEXTURE_CHECKERED)
	{
		if (((int)floor(u * sphere->texture.u.checkered.squares_width)
				+ (int)floor(v * sphere->texture.u.checkered.squares_height))
			% 2 == 0)
			return (sphere->texture.u.checkered.color1);
		else
			return (sphere->texture.u.checkered.color2);
	}
	else if (sphere->texture.type == TEXTURE_IMAGE)
	{
		u = floor(u * sphere->texture.u.image.width);
		v = floor(v * sphere->texture.u.image.height);
		return (img_pix_get(&sphere->texture.u.image, u, v));
	}
	return (sphere->material.albedo);
}

bool	hit_sphere(t_hit *hit, const t_sphere *sphere, t_ray *ray,
	t_interval interval)
{
	t_vec3	outward_normal;
	float	root;

	if (!get_root(ray, sphere, interval, &root))
		return (false);
	hit->t = root;
	hit->point = ray_at(*ray, hit->t);
	hit->material = sphere->material;
	hit->hit_color = get_hit_color_sphere(hit, sphere);
	outward_normal = (hit->point - sphere->center) / sphere->radius;
	set_face_normal(hit, ray, &outward_normal);
	return (true);
}
