//
//  sphere.h
//  RayTracing
//
//  Created by moranzcw on 2018/11/25.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef sphere_h
#define sphere_h

#include "hitable.h"

// 球，继承自hitable，实现光线相交的方法
class sphere: public hitable {
public:
    // 构造函数
    sphere() {}
    // 构造函数，参数为球心，半径，材质
    sphere(vec3 cen, float r, material *m) : center(cen), radius(r), mat_ptr(m)  {};
    // 相交检测
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
    
    // 数据
    vec3 center; // 球心
    float radius; // 半径
    material *mat_ptr; // 材质
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    // 设ray端点到球心的距离等于半径，即可解出交点
    // 即dot((A + t*B - C),(A + t*B - C)) = R*R
    // 展开得，dot(B,B)*t^2 + 2*dot(A-C,A-C)*t + dot(C,C) - R*R = 0
    // 按照二元一次方程求根公式求解t，即t = (-b +/- sqrt(b^2-4ac)) / 2a
    // 其中, a = dot(B,B)，b = 2*dot(A-C,A-C)，c = dot(C,C) - R*R
    vec3 oc = r.origin() - center; // 视点到球心，即A-C
    float a = dot(r.direction(), r.direction()); //
    float b = 2 * dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - 4*a*c; // 判断是否有根，即b^2-4ac是否大于等于0
    if (discriminant > 0) {
        // 求解t，t是距离，有两个根（两个交点）的情况下，先取小t，若小t不合法，则取大t
        // 这样做是因为小t离相机近，大t会被遮挡。
        float temp = (-b - sqrt(discriminant)) / (2*a);
        if (temp < t_max && temp > t_min) {
            rec.t = temp; // t
            rec.p = r.point_at_parameter(rec.t); // 交点
            rec.normal = (rec.p - center) / radius; // 法线，rec.p - center求出法线方向，除以radius归一化
            rec.mat_ptr = mat_ptr; // 材质
            return true;
        }
        temp = (-b + sqrt(discriminant)) / (2*a);
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}

#endif /* sphere_h */
