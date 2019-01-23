//
//  material.h
//  RayTracing
//
//  Created by moranzcw on 2018/11/25.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef MATERIAL_H
#define MATERIAL_H

struct HitRecord;

#include "ray.h"
#include "object.h"
#include "texture.h"

float schlick(float cosine, float ref_idx);
bool refract(const Vec3 &v, const Vec3 &n, float ni_over_nt, Vec3 &refracted);
Vec3 reflect(const Vec3 &v, const Vec3 &n); // 反射
Vec3 random_in_unit_sphere(); // 单位球内的随机点

// 材质，基类
class Material
{
  public: 
    // 散射函数，虚函数，需要在子类中实现，参数：入射光线，交点信息，衰减，散射
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Vec3 &attenuation, Ray &scattered) const = 0;
    // 自发光
    virtual Vec3 emitted(float u, float v, const Vec3 &p) const
    {
        return Vec3(0, 0, 0);
    }
};

// 漫射光源材质
class DiffuseLight : public Material
{
  public:
    DiffuseLight(Texture *a) : emit(a) {}
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Vec3 &attenuation, Ray &scattered) const { return false; }
    virtual Vec3 emitted(float u, float v, const Vec3 &p) const { return emit->value(u, v, p); }
    Texture *emit;
};

// 漫反射材质
class Lambertian : public Material
{
  public:
    // 构造函数，传入反射率
    Lambertian(Texture *a) : albedo(a) {}

    // 散射函数
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Vec3 &attenuation, Ray &scattered) const
    {
        Vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = Ray(rec.p, target - rec.p);
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

    Texture *albedo; // 反射率
};

// 金属
class Metal : public Material
{
  public:
    Metal(const Vec3 &a, float f) : albedo(a)
    {
        if (f < 1)
            fuzz = f;
        else
            fuzz = 1;
    }
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Vec3 &attenuation, Ray &scattered) const
    {
        Vec3 reflected = reflect(makeUnit(r_in.direction()), rec.normal);
        scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
    Vec3 albedo;
    float fuzz;
};

// 玻璃
class Dielectric : public Material
{
  public:
    Dielectric(float ri) : ref_idx(ri) {}
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Vec3 &attenuation, Ray &scattered) const
    {
        Vec3 outward_normal;
        Vec3 reflected = reflect(r_in.direction(), rec.normal);
        float ni_over_nt;
        attenuation = Vec3(1.0, 1.0, 1.0);
        Vec3 refracted;
        float reflect_prob;
        float cosine;
        if (dot(r_in.direction(), rec.normal) > 0)
        {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            //         cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
            cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
            cosine = sqrt(1 - ref_idx * ref_idx * (1 - cosine * cosine));
        }
        else
        {
            outward_normal = rec.normal;
            ni_over_nt = 1.0 / ref_idx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
            reflect_prob = schlick(cosine, ref_idx);
        else
            reflect_prob = 1.0;
        if (drand48() < reflect_prob)
            scattered = Ray(rec.p, reflected);
        else
            scattered = Ray(rec.p, refracted);
        return true;
    }

    float ref_idx;
};

float schlick(float cosine, float ref_idx)
{
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

bool refract(const Vec3 &v, const Vec3 &n, float ni_over_nt, Vec3 &refracted)
{
    Vec3 uv = makeUnit(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
    if (discriminant > 0)
    {
        refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    }
    else
        return false;
}

Vec3 reflect(const Vec3 &v, const Vec3 &n)
{
    return v - 2 * dot(v, n) * n;
}

Vec3 random_in_unit_sphere()
{
    Vec3 p;
    do
    {
        p = 2.0 * Vec3(drand48(), drand48(), drand48()) - Vec3(1, 1, 1);
    } while (p.squareLength() >= 1.0);
    return p;
}

#endif /* MATERIAL_H */
