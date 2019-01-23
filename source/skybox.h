//
//  skybox.h
//  RayTracing
//
//  Created by moranzcw on 2019/1/8.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef SKYBOX_H
#define SKYBOX_H

#include "ray.h"
#include "square.h"
#include "texture.h"
#include "material.h"
#include <cfloat>

class SkyBox {
public:
    virtual Vec3 getBackgroundColor(const Ray &r) const = 0;
    virtual ~SkyBox() {}
};

class BlackSkyBox : public SkyBox {
public:
    virtual Vec3 getBackgroundColor(const Ray &r) const {
        return Vec3(0,0,0);
    }
};

class BlueSkyBox : public SkyBox {
public:
    virtual Vec3 getBackgroundColor(const Ray &r) const {
        Vec3 unit_direction = makeUnit(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*Vec3(1.0, 1.0, 1.0) + t*Vec3(0.5, 0.7, 1.0);
    }
};


// 存放Object对象的数组
class DaylightSkyBox : public SkyBox {
public:
    DaylightSkyBox(Texture *tex) : tex_ptr(tex) {
        Vec3 vertex[8]; // 8个顶点
        vertex[0] =  Vec3(1,1,1);
        vertex[1] =  Vec3(1,-1,1);
        vertex[2] =  Vec3(-1,-1,1);
        vertex[3] =  Vec3(-1,1,1);
        vertex[4] =  Vec3(1,1,-1);
        vertex[5] =  Vec3(1,-1,-1);
        vertex[6] =  Vec3(-1,-1,-1);
        vertex[7] =  Vec3(-1,1,-1);

        for(int i=0;i<8;i++)
            vertex[i] *= 5000;

        Material *mat_ptr =  new Lambertian(new ConstantTexture);

        // front
        squares[0] = Square(vertex[0], vertex[3], vertex[2], vertex[1], mat_ptr);
        // up
        squares[1] = Square(vertex[0], vertex[4], vertex[7], vertex[3], mat_ptr);
        // left
        squares[2] = Square(vertex[3], vertex[7], vertex[6], vertex[2], mat_ptr);
        // right
        squares[3] = Square(vertex[4], vertex[0], vertex[1], vertex[5], mat_ptr);
        // bottom
        squares[4] = Square(vertex[2], vertex[6], vertex[5], vertex[1], mat_ptr);
        // back
        squares[5] = Square(vertex[7], vertex[4], vertex[5], vertex[6], mat_ptr);
    }
    
    virtual Vec3 getBackgroundColor(const Ray &r) const {
        HitRecord rec, temp_rec;

        Ray rr = r;
        rr.A = Vec3(0,0,0);
        double closest_so_far = FLT_MAX;
        // 批量调用每个square对象的hit函数，仅保留距离视点最近的一组hit信息
        int i;
        for (i = 0; i < 6; i++) {
            if (squares[i].hit(rr, 0.001, closest_so_far, temp_rec)) {
                closest_so_far = temp_rec.t;
                rec = temp_rec;
                break;
            }
        }
        rec.u /= 4.0;
        rec.v /= 3.0;
        switch(i){
            case 0:
                rec.u += 1/4.0 * 0;
                rec.v += 1/3.0 * 1;
                break;
            case 1:
                rec.u += 1/4.0 * 1;
                rec.v += 1/3.0 * 2;
                break;
            case 2:
                rec.u += 1/4.0 * 1;
                rec.v += 1/3.0 * 1;
                break;
            case 3:
                rec.u += 1/4.0 * 3;
                rec.v += 1/3.0 * 1;
                break;
            case 4:
                rec.u += 1/4.0 * 1;
                rec.v += 1/3.0 * 0;
                break;
            case 5:
                rec.u += 1/4.0 * 2;
                rec.v += 1/3.0 * 1;
                break;
        }
        return tex_ptr->value(rec.u, rec.v, rec.p);;
    }
    // 数据
    Square squares[6]; // 6个面
    Texture *tex_ptr; // 纹理
};

#endif /* SKYBOX_H */