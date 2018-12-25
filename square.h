//
//  square.h
//  RayTracing
//
//  Created by moranzcw on 2018/12/26.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef square_h
#define square_h

#include "triangle.h"

class square : public hitable {
public:
    // 构造函数
    square() {}
    // 构造函数，参数为三个顶点，材质
    square(vec3 v1, vec3 v2, vec3 v3, vec3 v4, material *m) : mat_ptr(m) {
        triangles[0] = triangle(v1,v2,v3,mat_ptr);
        triangles[1] = triangle(v1,v3,v4,mat_ptr);
    };
    // 相交检测
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
    
    // 数据
    triangle triangles[2];
    material *mat_ptr; // 材质
};

bool square::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    double closest_so_far = t_max;
    
    // 批量调用每个triangle对象的hit函数，仅保留距离视点最近的一组hit信息
    for (int i = 0; i < 2; i++) {
        if (triangles[i].hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

#endif /* square_h */