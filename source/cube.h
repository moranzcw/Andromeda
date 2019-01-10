//
//  cube.h
//  RayTracing
//
//  Created by moranzcw on 2018/12/26.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef cube_h
#define cube_h

#include "triangle.h"
#include "square.h"

// 存放hitable对象的数组
class cube: public hitable {
public:
    // 构造函数
    cube() {}
    cube(vec3 c, float s, material *m) : center(c), scale(s), mat_ptr(m) {
        vec3 vertex[8]; // 8个顶点
        vertex[0] =  center + 0.5 * scale * vec3(1,1,1);
        vertex[1] =  center + 0.5 * scale * vec3(1,-1,1);
        vertex[2] =  center + 0.5 * scale * vec3(-1,-1,1);
        vertex[3] =  center + 0.5 * scale * vec3(-1,1,1);
        vertex[4] =  center + 0.5 * scale * vec3(1,1,-1);
        vertex[5] =  center + 0.5 * scale * vec3(1,-1,-1);
        vertex[6] =  center + 0.5 * scale * vec3(-1,-1,-1);
        vertex[7] =  center + 0.5 * scale * vec3(-1,1,-1);

        // front
        squares[0] = square(vertex[3], vertex[0], vertex[1], vertex[2], mat_ptr);
        // up
        squares[1] = square(vertex[7], vertex[4], vertex[0], vertex[3], mat_ptr);
        // left
        squares[2] = square(vertex[7], vertex[3], vertex[2], vertex[6], mat_ptr);
        // right
        squares[3] = square(vertex[0], vertex[4], vertex[5], vertex[1], mat_ptr);
        // bottom
        squares[4] = square(vertex[2], vertex[1], vertex[5], vertex[6], mat_ptr);
        // back
        squares[5] = square(vertex[4], vertex[7], vertex[6], vertex[5], mat_ptr);
    }
    
    //与所有triangle对象进行hit操作，求出距离最近的一组记录
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
    virtual bool bounding_box(aabb& box) const;
    // 数据
    vec3 center; // 中心
    float scale; // 边长
    square squares[6]; // 6个面
    material *mat_ptr; // 材质
};

bool cube::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    double closest_so_far = t_max;
    
    // 批量调用每个square对象的hit函数，仅保留距离视点最近的一组hit信息
    for (int i = 0; i < 6; i++) {
        if (squares[i].hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

#include <iostream>
using namespace std;
bool cube::bounding_box(aabb& box) const{
    aabb temp_box;
    bool flag = squares[0].bounding_box(temp_box);
    if (!flag)
        return false;
    else 
        box = temp_box;
    
    // 与所有对象求包围盒
    for (int i=1; i<6; i++) {
        if(squares[i].bounding_box(temp_box)) {
            box = surrounding_box(box, temp_box);
        }
        else
            return false;
    }
    return true;
}
#endif /* cube_h */
