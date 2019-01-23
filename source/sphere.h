//
//  sphere.h
//  RayTracing
//
//  Created by moranzcw on 2018/11/25.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"
#include "material.h"

// 球
class Sphere : public Object
{
  public:
    Sphere() {}
    Sphere(Vec3 cen, float r, Material *m) : center(cen), radius(r), mat_ptr(m){};
    virtual bool hit(const Ray &r, float tmin, float tmax, HitRecord &rec) const;
    virtual bool boundingBox(AABB &box) const;

    Vec3 center;       // 球心
    float radius;      // 半径
    Material *mat_ptr; // 材质
};

bool Sphere::boundingBox(AABB &box) const
{
    box = AABB(center - Vec3(radius, radius, radius), center + Vec3(radius, radius, radius));
    return true;
}

// 获取球体的uv坐标
void get_sphere_uv(const Vec3 &p, float &u, float &v)
{
    float phi = atan2(p.z(), p.x());
    float theta = asin(p.y());
    u = 1 - (phi + M_PI) / (2 * M_PI);
    v = (theta + M_PI / 2) / M_PI;
}

bool Sphere::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const
{
    // 设ray端点到球心的距离等于半径，即可解出交点
    // 即dot((A + t*B - C),(A + t*B - C)) = R*R
    // 展开得，dot(B,B)*t^2 + 2*dot(A-C,A-C)*t + dot(C,C) - R*R = 0
    // 按照二元一次方程求根公式求解t，即t = (-b +/- sqrt(b^2-4ac)) / 2a
    // 其中, a = dot(B,B)，b = 2*dot(A-C,A-C)，c = dot(C,C) - R*R
    Vec3 oc = r.origin() - center;               // 视点到球心，即A-C
    float a = dot(r.direction(), r.direction()); //
    float b = 2 * dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c; // 判断是否有根，即b^2-4ac是否大于等于0
    if (discriminant > 0)
    {
        // 求解t，t是距离，有两个根（两个交点）的情况下，先取小t，若小t不合法，则取大t
        // 这样做是因为小t离相机近，大t会被遮挡。
        float temp = (-b - sqrt(discriminant)) / (2 * a);
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;                                           // t
            rec.p = r.point_at_parameter(rec.t);                    // 交点
            get_sphere_uv((rec.p - center) / radius, rec.u, rec.v); // 纹理坐标
            rec.normal = (rec.p - center) / radius;                 // 法线，rec.p - center求出法线方向，除以radius归一化
            rec.mat_ptr = mat_ptr;                                  // 材质
            return true;
        }
        temp = (-b + sqrt(discriminant)) / (2 * a);
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}

#endif /* SPHERE_H */
