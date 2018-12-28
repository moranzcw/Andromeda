//
//  hitable.h
//  RayTracing
//
//  Created by moranzcw on 2018/12/28.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef bvh_h
#define bvh_h

#include "hitable.h"

class bvh_node : public hitable  {
public:
    bvh_node() {}
    bvh_node(hitable **l, int n);
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
    virtual bool bounding_box(aabb& box) const;
    
    // 左子树
    hitable *left;
    // 右子树
    hitable *right;
    // 包围盒
    aabb box;
};

// 比较两个包围盒的坐标
int box_x_compare(const void * a, const void * b);
int box_y_compare(const void * a, const void * b);
int box_z_compare(const void * a, const void * b);

bvh_node::bvh_node(hitable **l, int n) {
    // 从x,y,z随机选择一个坐标轴排序
    int axis = int(3*drand48());
    if (axis == 0)
       qsort(l, n, sizeof(hitable *), box_x_compare);
    else if (axis == 1)
       qsort(l, n, sizeof(hitable *), box_y_compare);
    else
       qsort(l, n, sizeof(hitable *), box_z_compare);
    
    // 生成二叉树
    if (n == 1) { // 只有一个对象
        left = right = l[0];
    }
    else if (n == 2) { // 有两个对象
        left = l[0];
        right = l[1];
    }
    else { // 大于两个对象，递归生成子树
        left = new bvh_node(l, n/2);
        right = new bvh_node(l + n/2, n - n/2);
    }

    // 求包围盒
    aabb box_left, box_right;
    if(!left->bounding_box(box_left) || !right->bounding_box(box_right))
        std::cerr << "no bounding box in bvh_node constructor\n"; 
    box = surrounding_box(box_left, box_right);
}

bool bvh_node::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    if (box.hit(r, t_min, t_max)) {
        hit_record left_rec, right_rec;
        bool hit_left = left->hit(r, t_min, t_max, left_rec);
        bool hit_right = right->hit(r, t_min, t_max, right_rec);
        if (hit_left && hit_right) {
            if (left_rec.t < right_rec.t) 
                rec = left_rec;
            else 
                rec = right_rec;
            return true;
        }
        else if (hit_left) {
            rec = left_rec;
            return true;
        }
        else if (hit_right) {
            rec = right_rec;
            return true;
        }
        else 
            return false;
    }
    else return false;
}

bool bvh_node::bounding_box(aabb& b) const {
    b = box;
    return true;
}

int box_x_compare(const void * a, const void * b) {
    aabb box_left, box_right;
    hitable *ah = *(hitable**)a;
    hitable *bh = *(hitable**)b;
    if(!ah->bounding_box(box_left) || !bh->bounding_box(box_right))
                    std::cerr << "no bounding box in bvh_node constructor\n";
    if ( box_left.min().x() - box_right.min().x() < 0.0  )
        return -1;
    else
        return 1;
}

int box_y_compare(const void * a, const void * b) {
    aabb box_left, box_right;
    hitable *ah = *(hitable**)a;
    hitable *bh = *(hitable**)b;
    if(!ah->bounding_box(box_left) || !bh->bounding_box(box_right))
                    std::cerr << "no bounding box in bvh_node constructor\n";
    if ( box_left.min().y() - box_right.min().y() < 0.0  )
        return -1;
    else
        return 1;
}

int box_z_compare(const void * a, const void * b) {
    aabb box_left, box_right;
    hitable *ah = *(hitable**)a;
    hitable *bh = *(hitable**)b;
    if(!ah->bounding_box(box_left) || !bh->bounding_box(box_right))
                    std::cerr << "no bounding box in bvh_node constructor\n";
    if ( box_left.min().z() - box_right.min().z() < 0.0  )
        return -1;
    else
        return 1;
}

#endif /* bvh_h */

