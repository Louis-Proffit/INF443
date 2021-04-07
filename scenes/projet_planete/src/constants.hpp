#pragma once

#include "vcl/vcl.hpp"

// Scene constants
const float planete_radius = 1.0f;
const float plane_altitude = 1.1f;
const float satelite_altitude = 2.0f;

// Plane graphics
const float plane_body_length = 1.0f;
const float plane_body_radius = 0.1f;
const float plane_nose_length = 0.2f;
const float plane_rear_length = 0.2f;
const float plane_wing_span = 0.8f;
const float plane_wing_inner_length = 0.3f;
const float plane_wing_outer_length = 0.1f;
const float plane_wing_fleche = 0.3f;
const float plane_wing_advance_body_proportion = 0.8f;
const float plane_rear_wing_span = 0.3f;
const float plane_rear_wing_inner_length = 0.2f;
const float plane_rear_wing_outer_length = 0.1f;
const float plane_rear_wing_fleche = 0.15f;
const float plane_fin_span = 0.3f;
const float plane_fin_inner_length = 0.2f;
const float plane_fin_outer_length = 0.1f;
const float plane_fin_fleche = 0.15f;
const float plane_scale = 0.05f;
const float plane_rotation_speed = 0.0002f;

// Satelite graphics
const float satelite_body_width = 0.8f;
const float satelite_body_height = 0.8f;
const float satelite_link_length = 0.5f;
const float satelite_link_radius = 0.1f;
const float satelite_panel_height = 1.0f;
const float satelite_panel_width = 3.0f;
const float satelite_scale = 0.01f;
const float satelite_rotation_speed = 0.0001f;

// Planet graphics
const int sphere_division_steps = 4;
const float sphere_radius = 1.0f;
const float sphere_rotation_speed = 0.0001f;

const vcl::vec3 color_sea_low = vcl::vec3(float(0) / 255, float(191) / 255, float(255) / 255);
const vcl::vec3 color_sea_high = vcl::vec3(float(30) / 255, float(144) / 255, float(255) / 255);
const vcl::vec3 color_mountain_low = vcl::vec3(float(20) / 255, float(20) / 255, float(20) / 255);
const vcl::vec3 color_mountain_high = vcl::vec3(float(40) / 255, float(40) / 255, float(40) / 255);
const vcl::vec3 color_city_low = vcl::vec3(float(150) / 255, float(150) / 255, float(150) / 255);
const vcl::vec3 color_city_high = vcl::vec3(float(190) / 255, float(190) / 255, float(190) / 255);
const vcl::vec3 color_forest_low = vcl::vec3(float(50) / 255, float(205) / 255, float(50) / 255);
const vcl::vec3 color_forest_high = vcl::vec3(float(34) / 255, float(139) / 255, float(34) / 255);
const vcl::vec3 color_red = vcl::vec3(float(255) / 255, float(0) / 255, float(0) / 255);
const vcl::vec3 color_desert_low = vcl::vec3(float(204) / 255, float(204) / 255, float(0) / 255);
const vcl::vec3 color_desert_high = vcl::vec3(float(255) / 255, float(255) / 255, float(0) / 255);
const vcl::vec3 color_field_low = vcl::vec3(float(51) / 255, float(25) / 255, float(0) / 255);
const vcl::vec3 color_field_high = vcl::vec3(float(102) / 255, float(51) / 255, float(0) / 255);