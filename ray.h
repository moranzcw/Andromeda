//
//  ray.h
//  RayTracing
//
//  Created by moranzcw on 2018/11/24.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef ray_h
#define ray_h

#include "vec3.h"

class ray {
public:
    ray() {}
    ray(const vec3& a, const vec3& b) { A = a; B = b; }
    vec3 origin() const       { return A; }
    vec3 direction() const    { return B; }
    
    // 与参数t对应的射线的终点
    vec3 point_at_parameter(float t) const { return A + t*B; }
    
    vec3 A; // 起点
    vec3 B; // 方向
};

#endif /* ray_h */
