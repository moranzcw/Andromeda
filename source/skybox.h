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

#endif /* skybox_h */