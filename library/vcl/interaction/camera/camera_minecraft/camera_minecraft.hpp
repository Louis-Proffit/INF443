#pragma once

#include "vcl/math/math.hpp"
#include "vcl/interaction/camera/camera_base/camera_base.hpp"

namespace vcl
{
	struct camera_minecraft : camera_base
	{
		camera_minecraft();

		rotation orientation_camera;
		//rotation orientation_camera_inverse;
		vec3 position_camera;
		float rotation_z;
		float rotation_orthogonal;

		vec3 position() const;
		rotation orientation() const;

		void manipulator_rotate_2_axis(float rotate_dorthogonal, float rotate_dz);
		void manipulator_set_translation(vcl::vec2 const &tr);
		void manipulator_set_altitude(float altitude);
	};
}