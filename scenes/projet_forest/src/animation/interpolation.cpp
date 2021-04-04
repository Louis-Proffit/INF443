#include "interpolation.hpp"

using namespace vcl;

/** Compute the linear interpolation p(t) between p1 at time t1 and p2 at time t2*/
vcl::vec3 linear_interpolation(float t, float t1, float t2, const vcl::vec3 &p1, const vcl::vec3 &p2);

/** Compute the cardinal spline interpolation p(t) with the polygon [p0,p1,p2,p3] at time [t0,t1,t2,t3]
*  - Assume t \in [t1,t2] */
vcl::vec3 cardinal_spline_interpolation(float t, float t0, float t1, float t2, float t3, vcl::vec3 const &p0, vcl::vec3 const &p1, vcl::vec3 const &p2, vcl::vec3 const &p3, float K);

/** Find the index k such that intervals[k] < t < intervals[k+1] 
* - Assume intervals is a sorted array of N time values
* - Assume t \in [ intervals[0], intervals[N-1] [       */
size_t find_index_of_interval(float t, vcl::buffer<float> const &intervals);

vec3 const interpolation(float t, buffer<vec3> const &key_positions, buffer<float> const &key_times)
{
    // Find idx such that key_times[idx] < t < key_times[idx+1]
    int const idx = find_index_of_interval(t, key_times);
    size_t const N = key_times.size();
    // Parameters used to compute the linear interpolation
    float const t1 = key_times[idx];           // = t_i
    float const t2 = key_times[(idx + 1) % N]; // = t_{i+1}
    float const t3 = key_times[(idx + 2) % N];
    float const t0 = key_times[(idx - 1) % N];

    vec3 const &p1 = key_positions[idx];           // = p_i
    vec3 const &p2 = key_positions[(idx + 1) % N]; // = p_{i+1}
    vec3 const &p0 = key_positions[(idx - 1) % N];
    vec3 const &p3 = key_positions[(idx + 2) % N];
    float K = 0.5;

    //vec3 const p = linear_interpolation(t, t1,t2, p1,p2);
    vec3 const p = cardinal_spline_interpolation(t, t0, t1, t2, t3, p0, p1, p2, p3, K);

    return p;
}

vec3 linear_interpolation(float t, float t1, float t2, const vec3 &p1, const vec3 &p2)
{
    float const alpha = (t - t1) / (t2 - t1);
    vec3 const p = (1 - alpha) * p1 + alpha * p2;

    return p;
}

vec3 cardinal_spline_interpolation(float t, float t0, float t1, float t2, float t3, vec3 const &p0, vec3 const &p1, vec3 const &p2, vec3 const &p3, float K)
{
    // To do: fill the function to compute p(t) as a cardinal spline interpolation
    float s = (t - t1) / (t2 - t1);
    vec3 di = 2 * K * (p2 - p0) / (t2 - t0);
    vec3 dj = 2 * K * (p3 - p1) / (t3 - t1);
    float s2 = pow(s, 2);
    float s3 = pow(s, 3);
    vec3 p = (2 * s3 - 3 * s2 + 1) * p1 + (s3 - 2 * s2 + s) * di + (-2 * s3 + 3 * s2) * p2 + (s3 - s2) * dj;
    return p;
}

size_t find_index_of_interval(float t, buffer<float> const &intervals)
{
    size_t const N = intervals.size();
    bool error = false;
    if (intervals.size() < 2)
    {
        std::cout << "Error: Intervals should have at least two values; current size=" << intervals.size() << std::endl;
        error = true;
    }
    if (N > 0 && t < intervals[0])
    {
        std::cout << "Error: current time t is smaller than the first time of the interval" << std::endl;
        error = true;
    }
    if (N > 0 && t > intervals[N - 1])
    {
        std::cout << "Error: current time t is greater than the last time of the interval" << std::endl;
        error = true;
    }
    if (error == true)
    {
        std::string const error_str = "Error trying to find interval for t=" + str(t) + " within values: [" + str(intervals) + "]";
        error_vcl(error_str);
    }

    size_t k = 0;
    while (intervals[k + 1] < t)
        ++k;
    return k;
}