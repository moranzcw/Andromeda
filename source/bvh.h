//
//  bvh.h
//  RayTracing
//
//  Created by moranzcw on 2018/12/28.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef BVH_H
#define BVH_H

#include <vector>
#include <algorithm>
#include "object.h"

class BVHNode : public Object
{
  public:
    BVHNode() {}
    BVHNode(std::vector<Object *> l);
    virtual ~BVHNode()
    {
        // if(left) delete left;
        // if(right) delete right;
    }
    virtual bool hit(const Ray &r, float tmin, float tmax, HitRecord &rec) const;
    virtual bool boundingBox(AABB &box) const;

    // 左子树
    Object *left;
    // 右子树
    Object *right;
    // 包围盒
    AABB box;
};

// 比较两个包围盒的坐标
bool box_x_compare(const Object *a, const Object *b);
bool box_y_compare(const Object *a, const Object *b);
bool box_z_compare(const Object *a, const Object *b);

BVHNode::BVHNode(std::vector<Object *> l)
{
    // 从x,y,z随机选择一个坐标轴排序
    int axis = int(3 * drand48());
    if (axis == 0)
        std::sort(l.begin(), l.end(), box_x_compare);
    else if (axis == 1)
        std::sort(l.begin(), l.end(), box_y_compare);
    else
        std::sort(l.begin(), l.end(), box_z_compare);

    // 生成二叉树
    if (l.size() == 1)
    { // 只有一个对象
        left = right = l[0];
    }
    else if (l.size() == 2)
    { // 有两个对象
        left = l[0];
        right = l[1];
    }
    else
    { // 大于两个对象，递归生成子树
        left = new BVHNode(std::vector<Object *>(l.begin(), l.begin() + l.size() / 2));
        right = new BVHNode(std::vector<Object *>(l.begin() + l.size() / 2, l.end()));
    }

    // 求包围盒
    AABB box_left, box_right;
    if (!left->boundingBox(box_left) || !right->boundingBox(box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    box = AABB::SurroundingBox(box_left, box_right);
}

bool BVHNode::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const
{
    if (box.hit(r, t_min, t_max))
    {
        HitRecord left_rec, right_rec;
        bool hit_left = left->hit(r, t_min, t_max, left_rec);
        bool hit_right = right->hit(r, t_min, t_max, right_rec);
        if (hit_left && hit_right)
        {
            if (left_rec.t < right_rec.t)
                rec = left_rec;
            else
                rec = right_rec;
            return true;
        }
        else if (hit_left)
        {
            rec = left_rec;
            return true;
        }
        else if (hit_right)
        {
            rec = right_rec;
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

bool BVHNode::boundingBox(AABB &b) const
{
    b = box;
    return true;
}

bool box_x_compare(const Object *a, const Object *b)
{
    AABB box_left, box_right;
    if (!a->boundingBox(box_left) || !b->boundingBox(box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    return box_left.min().x() - box_right.min().x() < 0.0;
}

bool box_y_compare(const Object *a, const Object *b)
{
    AABB box_left, box_right;
    if (!a->boundingBox(box_left) || !b->boundingBox(box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    return box_left.min().y() - box_right.min().y() < 0.0;
}

bool box_z_compare(const Object *a, const Object *b)
{
    AABB box_left, box_right;
    if (!a->boundingBox(box_left) || !b->boundingBox(box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    return box_left.min().z() - box_right.min().z() < 0.0;
}

#endif /* BVH_H */
