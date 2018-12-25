//
//  hitable_list.h
//  RayTracing
//
//  Created by moranzcw on 2018/11/25.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef hitable_list_h
#define hitable_list_h

#include "hitable.h"

// 存放hitable对象的数组
class hitable_list: public hitable  {
public:
    // 构造函数
    hitable_list() {}
    hitable_list(hitable **l, int n) {list = l; list_size = n; }
    
    //与数组中所有hitable对象进行hit操作，求出距离最近的一组记录
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
    
    // 数据
    hitable **list; // 存放hitable对象的数组
    int list_size; // 尺寸
};

bool hitable_list::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    double closest_so_far = t_max;
    
    // 批量调用每个hitable对象的hit函数，仅保留距离视点最近的一组hit信息
    for (int i = 0; i < list_size; i++) {
        if (list[i]->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

#endif /* hitable_list_h */
