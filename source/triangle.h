//
//  triangle.h
//  RayTracing
//
//  Created by moranzcw on 2018/12/25.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object.h"

class Vertex
{
  public:
    Vec3 position;
    Vec3 normal;
    float u;
    float v;
};

class Triangle : public Object
{
  public:
    // 构造函数
    Triangle() {}
    Triangle(Vec3 v1, Vec3 v2, Vec3 v3, Material *m)
        : mat_ptr(m)
    {
        vertex[0].position = v1;
        vertex[1].position = v2;
        vertex[2].position = v3;
        Vec3 normal = makeUnit(cross(v3 - v1, v2 - v1));
        vertex[0].normal = normal;
        vertex[0].normal = normal;
        vertex[0].normal = normal;
    };
    Triangle(Vertex v1, Vertex v2, Vertex v3, Material *m)
        : mat_ptr(m)
    {
        vertex[0] = v1;
        vertex[1] = v2;
        vertex[2] = v3;
    };

    virtual bool hit(const Ray &r, float tmin, float tmax, HitRecord &rec) const;
    virtual bool boundingBox(AABB &box) const;

    // 数据
    Vertex vertex[3];
    Material *mat_ptr; // 材质
};

bool Triangle::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const
{
    float t, u, v;
    bool is_hit = false;

    // E1
    Vec3 E1 = vertex[1].position - vertex[0].position;
    // E2
    Vec3 E2 = vertex[2].position - vertex[0].position;
    // P
    Vec3 P = cross(r.direction(), E2);
    // determinant
    float det = dot(E1, P);

    // keep det > 0, modify T accordingly
    Vec3 T;
    if (det > 0)
    {
        T = r.origin() - vertex[0].position;
    }
    else
    {
        T = vertex[0].position - r.origin();
        det = -det;
    }

    //  determinant 接近0时，表示r射线与平面几乎平行
    if (det < 0.0001f)
        return false;

    // 计算 u, 保证 u <= 1
    u = dot(T, P);
    if (u < 0.0f || u > det)
        return false;

    // Q
    Vec3 Q = cross(T, E1);

    // 计算 v, 保证 u + v <= 1
    v = dot(r.direction(), Q);
    if (v < 0.0f || u + v > det)
        return false;

    // 计算 t, 射线的缩放参数, 表示与三角形相交的点
    t = dot(E2, Q);

    float fInvDet = 1.0f / det;
    rec.t = t * fInvDet;
    if (rec.t <= t_min || rec.t >= t_max)
        return false;
    rec.p = r.point_at_parameter(rec.t);

    u = u * fInvDet;
    v = v * fInvDet;

    // 纹理uv坐标插值
    rec.u = vertex[0].u * (1 - u - v) + vertex[1].u * v + vertex[2].u * v;
    rec.v = vertex[0].v * (1 - u - v) + vertex[1].v * v + vertex[2].v * v;

    // 法线插值
    rec.normal = vertex[0].normal * (1 - u - v) + vertex[1].normal * v + vertex[2].normal * v;
    // rec.normal = vertex[2].normal * (1 - u - v) + vertex[1].normal * v + vertex[0].normal * v;

    // 纹理
    rec.mat_ptr = mat_ptr;

    return true;
}

bool Triangle::boundingBox(AABB &box) const
{
    Vec3 _min, _max;
    _min[0] = ffmin(vertex[0].position.x(), ffmin(vertex[1].position.x(), vertex[2].position.x()));
    _min[1] = ffmin(vertex[0].position.y(), ffmin(vertex[1].position.y(), vertex[2].position.y()));
    _min[2] = ffmin(vertex[0].position.z(), ffmin(vertex[1].position.z(), vertex[2].position.z()));

    // 加0.1是为了防止盒子没有体积（模型是平行于坐标轴的平面）
    _max[0] = ffmax(vertex[0].position.x(), ffmax(vertex[1].position.x(), vertex[2].position.x())) + 0.1;
    _max[1] = ffmax(vertex[0].position.y(), ffmax(vertex[1].position.y(), vertex[2].position.y())) + 0.1;
    _max[2] = ffmax(vertex[0].position.z(), ffmax(vertex[1].position.z(), vertex[2].position.z())) + 0.1;

    box = AABB(_min, _max);
    return true;
}

#endif /* TRIANGLE_H */