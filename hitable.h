//
//  hitable.h
//  RayTracing
//
//  Created by moranzcw on 2018/11/25.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef hitable_h
#define hitable_h

#include "aabb.h"

class material;

// 保存一个相交的记录
struct hit_record
{
    float t; // 视点到交点的距离（即ray中的参数t）
    float u; // 纹理坐标u
    float v; // 纹理坐标v
    vec3 p; // 交点
    vec3 normal; // 法线
    material *mat_ptr; // 材质
};

// 所有物体继承此类，用于实现检测光线与物体相交的方法
class hitable {
public:
    // 纯虚函数，由子类自己实现，参数：光线，t的范围，hit_record用于保存相交信息
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
    // 纯虚函数，返回该对象的包围盒
    virtual bool bounding_box(aabb& box) const = 0;

    // 虚析构函数
    virtual ~hitable() {}
};

#endif /* hitable_h */
