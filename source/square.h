//
//  square.h
//  RayTracing
//
//  Created by moranzcw on 2018/12/26.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef SQUARE_H
#define SQUARE_H

#include "triangle.h"

class square : public Object {
public:
    // 构造函数
    square() {}
    // 构造函数，参数为三个顶点，材质
    square(vec3 v1, vec3 v2, vec3 v3, vec3 v4, material *m) : mat_ptr(m) {
        vertex[0] = v1;
        vertex[1] = v2;
        vertex[2] = v3;
        vertex[3] = v4;
        triangles[0] = triangle(v1,v2,v3,mat_ptr);
        triangles[1] = triangle(v1,v3,v4,mat_ptr);
    };
    // 相交检测
    virtual bool Hit(const ray& r, float tmin, float tmax, HitRecord& rec) const;
    virtual bool BoundingBox(AABB& box) const;

    // 数据
    vec3 vertex[4];
    triangle triangles[2];
    material *mat_ptr; // 材质
};

bool square::Hit(const ray& r, float t_min, float t_max, HitRecord& rec) const {
    HitRecord temp_rec;
    bool hit_anything = false;
    double closest_so_far = t_max;
    
    // 批量调用每个triangle对象的hit函数，仅保留距离视点最近的一组hit信息
    for (int i = 0; i < 2; i++) {
        if (triangles[i].Hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;

            //  计算uv坐标
            vec3 e1 = vertex[2]-vertex[3];
            vec3 e2 = vertex[0]-vertex[3];
            vec3 pp = rec.p-vertex[3];
            rec.u = dot(pp,e1)/(e1.length()*e1.length());
            rec.v = dot(pp,e2)/(e2.length()*e2.length());
        }
    }
    return hit_anything;
}

bool square::BoundingBox(AABB& box) const{
    AABB temp1, temp2;
    triangles[0].BoundingBox(temp1);
    triangles[1].BoundingBox(temp2);
    box = AABB::SurroundingBox(temp1, temp2);
    return true;
}

#endif /* SQUARE_H */