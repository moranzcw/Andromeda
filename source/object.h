//
//  object.h
//  RayTracing
//
//  Created by moranzcw on 2018/11/25.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef OBJECT_H
#define OBJECT_H

#include "aabb.h"

class Material;

// 保存一个相交的记录
struct HitRecord
{
    float t; // 视点到交点的距离（即ray中的参数t）
    float u; // 纹理坐标u
    float v; // 纹理坐标v
    Vec3 p; // 交点
    Vec3 normal; // 法线
    Material *mat_ptr; // 材质
};

// 所有物体继承此类，用于实现检测光线与物体相交的方法
class Object {
public:
    // 纯虚函数，由子类自己实现，参数：光线，t的范围，hit_record用于保存相交信息
    virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const = 0;
    // 纯虚函数，返回该对象的包围盒
    virtual bool boundingBox(AABB& box) const = 0;

    // 虚析构函数
    virtual ~Object() {}
};

#endif /* OBJECT_H */
