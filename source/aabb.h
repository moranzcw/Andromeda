//
//  aabb.h
//  RayTracing
//
//  Created by moranzcw on 2018/12/28.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef AABB_H
#define AABB_H

#include "ray.h"

inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }

class AABB {
public:
    AABB() {}
    AABB(const vec3& a, const vec3& b) { _min = a; _max = b;}  

    vec3 min() const {return _min; }
    vec3 max() const {return _max; }

    bool hit(const ray& r, float tmin, float tmax) const {
        for (int i=0; i<3; i++) {
            float t0 = ffmin((_min[i]-r.origin()[i])/r.direction()[i], (_max[i]-r.origin()[i])/r.direction()[i]);
            float t1 = ffmax((_min[i]-r.origin()[i])/r.direction()[i], (_max[i]-r.origin()[i])/r.direction()[i]);
            tmin = ffmax(t0, tmin);
            tmax = ffmin(t1, tmax);
            if (tmax <= tmin)
                return false;
        }
        return true;
    }
    
    // 求包含两个盒子的盒子
    static AABB SurroundingBox(AABB box0, AABB box1) {
        vec3 small(fmin(box0.min().x(), box1.min().x()), fmin(box0.min().y(), box1.min().y()), fmin(box0.min().z(), box1.min().z()));
        vec3 big(fmax(box0.max().x(), box1.max().x()), fmax(box0.max().y(), box1.max().y()), fmax(box0.max().z(), box1.max().z()));
        return AABB(small,big);
    }

private:
    // 与坐标轴平行的盒子可以用两个点表示
    vec3 _min; // 小x,y,z
    vec3 _max; // 大x,y,z
};
#endif /* AABB_H */

