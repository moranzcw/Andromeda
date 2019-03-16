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
#include <cfloat>
#include "object.h"
#include "triangle.h"
#include "bvh.h"

class Model : public Object
{
  public:
    // 构造函数
    Model() {}
    Model(const std::vector<Triangle> &t) : triangles(t)
    {
        std::vector<Object *> list;
        for (int i = 0; i < t.size(); i++)
        {
            Triangle *tt = new Triangle(t[i]);
            list.push_back(tt);
        }
        bvh_tree = BVHNode(list);
    };

    // 相交检测
    virtual bool hit(const Ray &r, float tmin, float tmax, HitRecord &rec) const;
    virtual bool boundingBox(AABB &box) const;

    // 数据
    std::vector<Triangle> triangles;
    BVHNode bvh_tree;
};

bool Model::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const
{
    // 相交检测，rec带回相交信息
    return bvh_tree.hit(r, 0.001, FLT_MAX, rec);
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