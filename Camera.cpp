//
// Created by sysgen on 2/12/21.
//

#include "Camera.h"

//inline Ray Camera::getRay(int x, int y) const {
//    sc::float3 horizontalComp = (horizontalVector*(x-(X/2.0f))*virtualRecScale);
//    sc::float3 verticalComp = (upVector*(y-(Y/2.0f))*virtualRecScale);
//    sc::float3 forwardComp = principalRay.getDirection()*virtualRecDistance;
//    return Ray(principalRay.getOrigin(),
//               sc::normalize(horizontalComp+verticalComp+forwardComp));
//}
//
//std::vector<std::vector<Ray>> Camera::getRays() const {
//    std::vector<std::vector<Ray>> rays;
//    rays.resize(X, std::vector<Ray>(Y));
//    for (int i = 0; i < X; i++){
//        for(int j = 0; j < Y; j++){
//            rays[i][j] = getRay(i,j);
//        }
//    }
//    return rays;
//}
