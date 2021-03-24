//
// Created by sysgen on 3/20/21.
//

#ifndef PATHTRACER_SYCL_RANDOM_H
#define PATHTRACER_SYCL_RANDOM_H

#include <limits>
#include <CL/sycl.hpp>

const float PI = 3.14156;

struct RandomResult
{
    sycl::uint4 state;
    float value;
};

SYCL_EXTERNAL RandomResult Random(sycl::uint4 state);
inline int fastrand(int *g_seed) {
    *g_seed = (214013*(*g_seed)+2531011);
    return (*g_seed>>16)&0x7FFF;
}
inline sycl::float4 BoxMueller(sycl::float4 uniformRandomUnit){
    float a = uniformRandomUnit.x();
    float b = uniformRandomUnit.y();
    float c = uniformRandomUnit.z();
    float d = uniformRandomUnit.w();
    return sycl::float4({sycl::sqrt(-2*sycl::log(a))*sycl::cos(2*PI*b),
                         sycl::sqrt(-2*sycl::log(a))*sycl::sin(2*PI*b),
                         sycl::sqrt(-2*sycl::log(c))*sycl::cos(2*PI*d),
                         sycl::sqrt(-2*sycl::log(c))*sycl::sin(2*PI*d)});
}

inline sycl::float3 BoxMueller3(sycl::float4 uniformRandomUnit){
    float a = uniformRandomUnit.x();
    float b = uniformRandomUnit.y();
    float c = uniformRandomUnit.z();
    float d = uniformRandomUnit.w();
    return sycl::float3({sycl::sqrt(-2*sycl::log(a))*sycl::cos(2*PI*b),
                         sycl::sqrt(-2*sycl::log(a))*sycl::sin(2*PI*b),
                         sycl::sqrt(-2*sycl::log(c))*sycl::cos(2*PI*d)});
}


#endif //PATHTRACER_SYCL_RANDOM_H
