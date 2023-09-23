#include "minirt.h"

static t_vec3	get_color_from_material(t_material *material)
{
	if (material->type == MATERIAL_DIELECTRIC)
		return ((t_vec3){1, 1, 1});
	else if (material->type == MATERIAL_LAMBERTIAN)
		return (material->u.lambertian.albedo);
	else
		return (material->u.metal.albedo);
}

t_vec3	get_ambient_color(t_scene *scene, t_hit *hit)
{
	return (get_color_from_material(&hit->material)
		* scene->ambient.color
		* hit->material.u.lambertian.ka);
}

t_vec3	get_diffuse_color(t_light *light, t_hit *hit, t_ray *light_ray)
{
	return (light->color
		* LIGHT_INTENSITY
		* fmaxf(vec3_dot(light_ray->direction, hit->normal), 0)
		/ (light_ray->distance * light_ray->distance)
		* get_color_from_material(&hit->material)
		* hit->material.u.lambertian.kd);
}

t_vec3	get_specular_color(t_light *light, t_hit *hit,
		t_ray *ray, t_ray *light_ray)
{
	const t_vec3	reflection_direction = reflect(
			-light_ray->direction, hit->normal);

	return (light->color
		* LIGHT_INTENSITY
		* hit->material.u.lambertian.ks
		/ light_ray->distance / light_ray->distance * powf(fmaxf(0,
				-vec3_dot(reflection_direction, vec3_unit(ray->direction))),
			hit->material.u.lambertian.specular_exponent));
}