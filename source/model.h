//
//  model.h
//  RayTracing
//
//  Created by moranzcw on 2019/2/7.
//  Copyright © 2019 moranzcw. All rights reserved.
//

#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "object.h"
#include "triangle.h"

class Model : public Object
{
  public:
    // 构造函数
    Model() {}
    Model(const std::vector<Triangle> &t) : triangles(t){};

    // 相交检测
    virtual bool hit(const Ray &r, float tmin, float tmax, HitRecord &rec) const;
    virtual bool boundingBox(AABB &box) const;

    // 数据
    std::vector<Triangle> triangles;
};

bool Model::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const
{
    HitRecord temp_rec;
    bool hit_anything = false;
    double closest_so_far = t_max;

    // 批量调用每个triangle对象的hit函数，仅保留距离视点最近的一组hit信息
    for (int i = 0; i < triangles.size(); i++)
    {
        if (triangles[i].hit(r, t_min, closest_so_far, temp_rec))
        {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;

            //  计算uv坐标
            // Vec3 e1 = vertex[2] - vertex[3];
            // Vec3 e2 = vertex[0] - vertex[3];
            // Vec3 pp = rec.p - vertex[3];
            // rec.u = dot(pp, e1) / (e1.length() * e1.length());
            // rec.v = dot(pp, e2) / (e2.length() * e2.length());
        }
    }
    return hit_anything;
}

bool Model::boundingBox(AABB &box) const
{
    AABB temp;
    for (int i = 0; i < triangles.size(); i++)
    {
        triangles[i].boundingBox(temp);
        box = AABB::SurroundingBox(box, temp);
    }
    return true;
}

#endif /* MODEL_H */