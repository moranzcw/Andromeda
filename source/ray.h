//
//  ray.h
//  RayTracing
//
//  Created by moranzcw on 2018/11/24.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class Ray {
public:
    Ray() {}
    Ray(const Vec3& a, const Vec3& b) { A = a; B = b; }
    Vec3 origin() const       { return A; }
    Vec3 direction() const    { return B; }
    
    // 与参数t对应的射线的终点
    Vec3 point_at_parameter(float t) const { return A + t*B; }
    
    Vec3 A; // 起点
    Vec3 B; // 方向
};

#endif /* RAY_H */
