/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yguaye <yguaye@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/26 21:46:57 by yguaye            #+#    #+#             */
/*   Updated: 2018/08/23 07:44:48 by jloro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft_base/stringft.h>
#include <libft_base/io.h>
#include "rt.h"

static t_mat_props	mat_props(char *str)
{
	if (ft_strequ(str, "reflective"))
		return (MAT_REFLECTIVE);
	else if (ft_strequ(str, "refractive"))
		return (MAT_REFRACTIVE);
	else if (ft_strequ(str, "portal"))
		return (MAT_PORTAL);
	else if (ft_strequ(str, "negative"))
		return (MAT_NEGATIVE);
	else
		return (MAT_NONE);
}

static int			obj_parse_portal(t_object *obj, const t_json_object *data)
{
	if (!vec3f_from_json(json_obj_get(data, "output_pos"), &obj->mat.p_out))
	{
		ft_putendl_fd("Invalid portal output position", STDERR_FILENO);
		return (0);
	}
	return (1);
}

static int			parse_material2(t_object *object,
		const t_json_object *data)
{
	t_json_value	*tmp;

	if ((tmp = json_obj_get(data, "has_shadow")))
	{
		if (tmp->bol.type != JSON_BOOL)
		{
			ft_putendl_fd("\"has_shadow\" should be a boolean", STDERR_FILENO);
			return (0);
		}
		object->mat.has_shadow = tmp->bol.value;
	}
	if ((tmp = json_obj_get(data, "perturbation")))
	{
		if (tmp->obj.type != JSON_OBJECT || !parse_noise(&tmp->obj,
					&object->mat.noise))
			return (0);
		object->mat.has_noise = 1;
	}
	else
		object->mat.has_noise = 0;
	return (1);
}

int					parse_material(t_object *object, const t_json_object *data)
{
	t_json_value	*tmp;

	if ((tmp = json_obj_get(data, "property")) && tmp->obj.type == JSON_STRING)
		object->mat.props = mat_props(tmp->str.value);
	else
		object->mat.props = MAT_NONE;
	if (!(float_from_json(json_obj_get(data, "coefficient"),
					&object->mat.props_coef)))
		object->mat.props_coef = 0.f;
	if (object->mat.props_coef < 0.f || object->mat.props_coef > 1.f)
		object->mat.props_coef = ((object->mat.props_coef < 0.f) ? 0.f : 1.f);
	if (!(float_from_json(json_obj_get(data, "refractive_index"),
					&object->mat.refractive_index)))
		object->mat.refractive_index = 1.f;
	if (object->mat.props == MAT_PORTAL && !obj_parse_portal(object, data))
		return (0);
	object->mat.has_shadow = 1;
	if (!parse_material2(object, data))
		return (0);
	return (1);
}
