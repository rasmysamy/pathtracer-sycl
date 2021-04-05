////
//// Created by sysgen on 3/22/21.
////
//#include "Random.h"
//#include <CL/sycl.hpp>
////From GPUGems 3 chapter 37, chained LCG + Tausworthe Generator
//
//
//
//uint TausStep(uint z, int S1, int S2, int S3, uint M)
//{
//    uint b = (((z << S1) ^ z) >> S2);
//    return (((z & M) << S3) ^ b);
//}
//
//uint LCGStep(uint z, uint A, uint C)
//{
//    return (A * z + C);
//}
//
//RandomResult Random(sc::uint4 state)
//{
//    unsigned int x = TausStep(state.x(), 13, 19, 12, 4294967294);
//    unsigned int y = TausStep(state.y(), 2, 25, 4, 4294967288);
//    unsigned int z = TausStep(state.z(), 3, 11, 17, 4294967280);
//    unsigned int w = LCGStep(state.w(), 1664525, 1013904223);
//
//    state = sc::uint4(x, y, z, w);
//
//    RandomResult result;
//    result.state = state;
//    result.value = ((2.3283064365387e-10) * (state.x() ^ state.y() ^ state.z() ^ state.w()));
//
//    return result;
//}
//
//
