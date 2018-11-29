//
//  hitable.h
//  RayTracing
//
//  Created by 张成悟 on 2018/11/25.
//  Copyright © 2018 张成悟. All rights reserved.
//

#ifndef hitable_h
#define hitable_h

#include "ray.h"

class material;

// 保存一个相交的记录
struct hit_record
{
    float t; // 视点到交点的距离（即ray中的参数t）
    vec3 p; // 交点
    vec3 normal; // 法线
    material *mat_ptr; // 材质
};

// 所有物体继承此类，用于实现检测光线与物体相交的方法
class hitable  {
public:
    // 虚函数，由子类自己实现，参数：光线，t的范围，hit_record用于保存相交信息
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};

#endif /* hitable_h */
