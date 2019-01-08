//
//  skybox.h
//  RayTracing
//
//  Created by moranzcw on 2019/1/8.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef skybox_h
#define skybox_h

#include "ray.h"
#include "square.h"
#include <cfloat>

class skybox {
public:
    virtual vec3 get_background(const ray &r) const = 0;
    virtual ~skybox() {}
};

class black_skybox : public skybox {
public:
    virtual vec3 get_background(const ray &r) const {
        return vec3(0,0,0);
    }
};

class blue_skybox : public skybox {
public:
    virtual vec3 get_background(const ray &r) const {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
};


// 存放hitable对象的数组
class daylight_skybox : public skybox {
public:
    daylight_skybox(texture *tex) : tex_ptr(tex) {
        vec3 vertex[8]; // 8个顶点
        vertex[0] =  vec3(1,1,1);
        vertex[1] =  vec3(1,-1,1);
        vertex[2] =  vec3(-1,-1,1);
        vertex[3] =  vec3(-1,1,1);
        vertex[4] =  vec3(1,1,-1);
        vertex[5] =  vec3(1,-1,-1);
        vertex[6] =  vec3(-1,-1,-1);
        vertex[7] =  vec3(-1,1,-1);

        for(int i=0;i<8;i++)
            vertex[i] *= 5000;

        material *mat_ptr =  new lambertian(new constant_texture);

        // front
        squares[0] = square(vertex[0], vertex[3], vertex[2], vertex[1], mat_ptr);
        // up
        squares[1] = square(vertex[0], vertex[4], vertex[7], vertex[3], mat_ptr);
        // left
        squares[2] = square(vertex[3], vertex[7], vertex[6], vertex[2], mat_ptr);
        // right
        squares[3] = square(vertex[4], vertex[0], vertex[1], vertex[5], mat_ptr);
        // bottom
        squares[4] = square(vertex[2], vertex[6], vertex[5], vertex[1], mat_ptr);
        // back
        squares[5] = square(vertex[7], vertex[4], vertex[5], vertex[6], mat_ptr);
    }
    
    virtual vec3 get_background(const ray &r) const {
        hit_record rec, temp_rec;

        ray rr = r;
        rr.A = vec3(0,0,0);
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
    square squares[6]; // 6个面
    texture *tex_ptr; // 材质
};

#endif /* skybox_h */