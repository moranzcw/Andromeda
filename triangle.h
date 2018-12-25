//
//  triangle.h
//  RayTracing
//
//  Created by moranzcw on 2018/12/25.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef triangle_h
#define triangle_h

#include "hitable.h"

class triangle : public hitable {
public:
    // 构造函数
    triangle() {}
    // 构造函数，参数为三个顶点，材质
    triangle(vec3 v1, vec3 v2, vec3 v3, material *m) : normal(unit_vector(cross(v3-v1,v2-v1))), mat_ptr(m) {
        vertex[0] = v1;
        vertex[1] = v2;
        vertex[2] = v3;
    };
    // 相交检测
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
    
    // 数据
    vec3 vertex[3]; // 顶点
    vec3 normal; // 法线
    material *mat_ptr; // 材质
};

// Determine whether a ray intersect with a triangle
// Parameters
// orig: origin of the ray
// dir: direction of the ray
// v0, v1, v2: vertices of triangle
// t(out): weight of the intersection for the ray
// u(out), v(out): barycentric coordinate of intersection

bool IntersectTriangle(const vec3& orig, const vec3& dir,
    const vec3& v0, const vec3& v1, const vec3& v2,
    float* t, float* u, float* v)
{
    // E1
    vec3 E1 = v1 - v0;

    // E2
    vec3 E2 = v2 - v0;

    // P
    vec3 P = cross(dir, E2);

    // determinant
    float det = dot(E1, P);

    // keep det > 0, modify T accordingly
    vec3 T;
    if( det >0 )
    {
        T = orig - v0;
    }
    else
    {
        T = v0 - orig;
        det = -det;
    }

    // If determinant is near zero, ray lies in plane of triangle
    if( det < 0.0001f )
        return false;

    // Calculate u and make sure u <= 1
    *u = dot(T, P);
    if( *u < 0.0f || *u > det )
        return false;

    // Q
    vec3 Q = cross(T, E1);

    // Calculate v and make sure u + v <= 1
    *v = dot(dir, Q);
    if( *v < 0.0f || *u + *v > det )
        return false;

    // Calculate t, scale parameters, ray intersects triangle
    *t = dot(E2, Q);

    float fInvDet = 1.0f / det;
    *t *= fInvDet;
    *u *= fInvDet;
    *v *= fInvDet;

    return true;
}

bool triangle::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    float t,u,v;
    bool is_hit = false;
    is_hit = IntersectTriangle(r.origin(), r.direction(), vertex[0], vertex[1], vertex[2], &t, &u, &v);
    if(is_hit && t < t_max && t > t_min){
        rec.t = t;
        rec.p = r.point_at_parameter(rec.t);
        rec.normal = normal;
        rec.mat_ptr = mat_ptr;
        return true;
    }
    return false;
}

#endif /* triangle_h */