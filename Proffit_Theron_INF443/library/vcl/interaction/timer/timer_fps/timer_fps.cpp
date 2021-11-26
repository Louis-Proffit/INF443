#include "timer_fps.hpp"

#include <GLFW/glfw3.h>

namespace vcl
{
    timer_fps::timer_fps(float update_fps_period)
        :timer_event_periodic(update_fps_period), fps(0), counter(0)
    {}

    float timer_fps::update()
    {
        float const elapsed_time = t_periodic;

        float const dt = timer_event_periodic::update();

        fps = 1.0f / dt;

        return dt;
    }



}