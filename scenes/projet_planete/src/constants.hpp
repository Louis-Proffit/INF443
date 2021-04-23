#pragma once

#include "vcl/vcl.hpp"

// Scene constants
#define planet_radius 1.0f
#define plane_altitude 1.1f
#define satelite_altitude 2.0f

#define plane_body_length 1.0f
#define plane_body_radius 0.1f
#define plane_nose_length 0.2f
#define plane_rear_length 0.2f
#define plane_wing_span 0.8f
#define plane_wing_inner_length 0.3f
#define plane_wing_outer_length 0.1f
#define plane_wing_fleche 0.3f
#define plane_wing_advance_body_proportion 0.8f
#define plane_rear_wing_span 0.3f
#define plane_rear_wing_inner_length 0.2f
#define plane_rear_wing_outer_length 0.1f
#define plane_rear_wing_fleche 0.15f
#define plane_fin_span 0.3f
#define plane_fin_inner_length 0.2f
#define plane_fin_outer_length 0.1f
#define plane_fin_fleche 0.15f
#define plane_scale 0.05f
#define plane_rotation_speed 0.0002f

// Satelite graphics
#define satelite_body_width 0.8f
#define satelite_body_height 0.8f
#define satelite_link_length 0.5f
#define satelite_link_radius 0.1f
#define satelite_panel_height 1.0f
#define satelite_panel_width 3.0f
#define satelite_scale 0.01f
#define satelite_rotation_speed 0.0001f

// boat graphics
#define boat_altitude 1.01f
#define boat_scale 0.0003f
#define boat_rotation_speed 0.0001f

// Planet graphics
#define sphere_division_steps 5
#define sphere_radius 1.0f
#define sphere_rotation_speed 0.0f

// islands constants
#define island_radius 0.4f
#define number_of_islands 10

#define color_sea_low vec3(float(0) / 255, float(191) / 255, float(255) / 255);
#define color_sea_high vec3(float(30) / 255, float(144) / 255, float(255) / 255);
#define color_mountain_low vec3(float(20) / 255, float(20) / 255, float(20) / 255);
#define color_mountain_high vec3(float(40) / 255, float(40) / 255, float(40) / 255);
#define color_city_low vec3(float(150) / 255, float(150) / 255, float(150) / 255);
#define color_city_high vec3(float(190) / 255, float(190) / 255, float(190) / 255);
#define color_forest_low vec3(float(50) / 255, float(205) / 255, float(50) / 255);
#define color_forest_high vec3(float(34) / 255, float(139) / 255, float(34) / 255);
#define color_red vec3(float(255) / 255, float(0) / 255, float(0) / 255);
#define color_desert_low vec3(float(204) / 255, float(204) / 255, float(0) / 255);
#define color_desert_high vec3(float(255) / 255, float(255) / 255, float(0) / 255);
#define color_field_low vec3(float(51) / 255, float(25) / 255, float(0) / 255);
#define color_field_high vec3(float(102) / 255, float(51) / 255, float(0) / 255);