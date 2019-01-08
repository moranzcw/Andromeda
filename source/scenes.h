//
//  scenes.h
//  RayTracing
//
//  Created by moranzcw on 2018/12/28.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef scenes_h
#define scenes_h

#include <vector>
#include "sphere.h"
#include "cube.h"
#include "material.h"
#include "bvh.h"
#include "scene.h"
#include "skybox.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// 生成场景
scene scene1() {
    std::vector<hitable*> l;
    // 地面
    texture *checker = new checker_texture(new constant_texture(vec3(0.2,0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
    l.push_back(new square(vec3(100,0,100), vec3(-100,0,100), vec3(-100,0,-100), vec3(100,0,-100), new lambertian(checker)));
    int i = 1;

    // 在-11<x<11，-11<z<11的区域内生成n个小球
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = drand48();
            // z坐标和x坐标加一些随机偏移，y坐标为0.2，即球心离地0.2，刚好等于半径
            vec3 center(a+0.9*drand48(),0.2,b+0.9*drand48());
            // 避开大球坐标
            if ((center-vec3(3, 0.2, 0)).length() > 0.9
                && (center-vec3(0, 0.2, 0)).length() > 0.9
                && (center-vec3(-3, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.6) {  // 0.6概率的漫反射材质
                    l.push_back(new sphere(center, 0.2, new lambertian(new constant_texture(vec3(0.9*drand48(), 0.3, 0.2)))));
                }
                else if (choose_mat < 0.9) { // 0.3概率的金属材质
                    l.push_back(new sphere(center, 0.2,
                                new metal(vec3(0.5*(1 + drand48()), 0.5*(1 + drand48()), 0.3*(1 + drand48())),  0.2*drand48())));
                }
                else {  // 0.1概率的玻璃材质
                    l.push_back(new sphere(center, 0.2, new dielectric(1.5)));
                }
            }
        }
    }
    
    // 三个大球
    l.push_back(new sphere(vec3(0, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0)));
    l.push_back(new sphere(vec3(-3, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1)))));
    l.push_back(new sphere(vec3(3, 1, 0), 1.0, new dielectric(1.5)));
    
    camera cam(vec3(0,1,15), vec3(0,1,0), vec3(0,1,0), 30, 16.0/9.0, 0.2, 15);
    skybox *skb = new blue_skybox();
    return scene(l, cam, skb);
}

// 生成场景
scene scene2() {
    // 四个球
    std::vector<hitable*> l;
    l.push_back(new square(vec3(100,0,100), vec3(-100,0,100), vec3(-100,0,-100), vec3(100,0,-100), 
                new lambertian(new constant_texture(vec3(0.7, 0.4, 0.4)))));
    l.push_back(new sphere(vec3(0, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.1, 0.2, 0.5)))));
    l.push_back(new sphere(vec3(2, 1, 0), 1.0, new metal(vec3(0.8, 0.6, 0.2), 0.0)));
    l.push_back(new sphere(vec3(-2, 1, 0), 1.0, new dielectric(1.5)));
    l.push_back(new sphere(vec3(-2, 1, 0), 0.8, new dielectric(1.5)));

    camera cam(vec3(0,1,15), vec3(0,1,0), vec3(0,1,0), 30, 16.0/9.0, 0.2, 15); 
    skybox *skb = new blue_skybox();
    return scene(l, cam, skb);
}

scene two_spheres() {
    std::vector<hitable*> l;

    texture *checker = new checker_texture(new constant_texture(vec3(0.2,0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
    l.push_back(new sphere(vec3(0,-10, 0), 10, new lambertian(checker)));
    l.push_back(new sphere(vec3(0, 10, 0), 10, new lambertian(checker)));

    camera cam(vec3(0,1,15), vec3(0,1,0), vec3(0,1,0), 30, 16.0/9.0, 0.2, 15); 
    skybox *skb = new blue_skybox();
    return scene(l, cam, skb);
}

scene perlin_spheres() {
    std::vector<hitable*> l;

    texture *pertext = new noise_texture(4);
    l.push_back(new square(vec3(100,0,100), vec3(-100,0,100), vec3(-100,0,-100), vec3(100,0,-100),new lambertian( pertext )));
    l.push_back(new sphere(vec3(0, 2, 0), 2, new lambertian( pertext )));
    
    camera cam(vec3(0,1,15), vec3(0,1,0), vec3(0,1,0), 30, 16.0/9.0, 0.2, 15); 
    skybox *skb = new blue_skybox();
    return scene(l, cam, skb);
}

scene image_tex() {
    std::vector<hitable*> l;

    texture *checker = new checker_texture(new constant_texture(vec3(0.2,0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
    l.push_back(new square(vec3(100,0,100), vec3(-100,0,100), vec3(-100,0,-100), vec3(100,0,-100),new lambertian( checker )));
    
    int nx, ny, nn;
    unsigned char *tex_data = stbi_load("../mars_map.jpg", &nx, &ny, &nn, 0);
    material *mat =  new lambertian(new image_texture(tex_data, nx, ny));
    l.push_back(new sphere(vec3(-2.5, 1, 0), 1, mat));

    unsigned char *tex_data2 = stbi_load("../jupiter_map.jpg", &nx, &ny, &nn, 0);
    material *mat2 =  new lambertian(new image_texture(tex_data2, nx, ny));
    l.push_back(new sphere(vec3(0, 1, 0), 1, mat2));

    unsigned char *tex_data3 = stbi_load("../marble.jpg", &nx, &ny, &nn, 0);
    material *mat3 =  new lambertian(new image_texture(tex_data3, nx, ny));
    l.push_back(new cube(vec3(2.5, 1, 0), 1.5, mat3));

    camera cam(vec3(0,1,15), vec3(0,1,0), vec3(0,1,0), 30, 16.0/9.0, 0.2, 15); 
    skybox *skb = new blue_skybox();
    return scene(l, cam, skb);
}

scene simple_light() {
    std::vector<hitable*> l;

    texture *checker = new checker_texture(new constant_texture(vec3(0.2,0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
    l.push_back(new square(vec3(100,0,100), vec3(-100,0,100), vec3(-100,0,-100), vec3(100,0,-100),
                new lambertian( checker )));
    
    texture *pertext = new noise_texture(4);
    l.push_back(new sphere(vec3(0, 1, 0), 1, new lambertian( pertext )));

    l.push_back(new square(vec3(2,2,-1), vec3(2,2,1), vec3(2,0,1), vec3(2,0,-1), 
                new diffuse_light(new constant_texture(vec3(4,4,4)))));
    camera cam(vec3(0,1,15), vec3(0,1,0), vec3(0,1,0), 30, 16.0/9.0, 0.2, 15); 
    skybox *skb = new blue_skybox();
    return scene(l, cam, skb);
}


// 生成场景
scene dark1() {
    std::vector<hitable*> l;

    int nx, ny, nn; 
    unsigned char *ground_tex = stbi_load("../marble.jpg", &nx, &ny, &nn, 0);
    material *ground =  new lambertian(new image_texture(ground_tex, nx, ny));
    l.push_back(new square(vec3(10,0,10), vec3(-10,0,10), vec3(-10,0,-10), vec3(10,0,-10), ground));
    l.push_back(new square(vec3(100,0,95), vec3(0,0,-5), vec3(0,100,-5), vec3(100,100,95), 
                new metal(vec3(0.7, 0.6, 0.5), 0.0)));
    l.push_back(new square(vec3(-100,0,95), vec3(-100,100,95), vec3(0,100,-5), vec3(0,0,-5), 
                new metal(vec3(0.7, 0.6, 0.5), 0.0)));

    // 
    texture *checker = new checker_texture(new constant_texture(vec3(0.2,0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
    l.push_back(new cube(vec3(-0.5,0.75,-1.5), 1.5, new lambertian(checker)));
     l.push_back(new sphere(vec3(-1.5,1,1.5), 1, new metal(vec3(0.8, 0.6, 0.2), 0.0)));

    unsigned char *tex_data2 = stbi_load("../jupiter_map.jpg", &nx, &ny, &nn, 0);
    material *mat2 =  new lambertian(new image_texture(tex_data2, nx, ny));
    l.push_back(new sphere(vec3(2,1,0), 1.0, mat2));
    l.push_back(new sphere(vec3(-0.5,2.5,-1.5), 1.0, new dielectric(1.5)));

    material *light = new diffuse_light( new constant_texture(vec3(7, 7, 7)) );
    l.push_back(new cube(vec3(0.4,6,1.0), 2.5, light));

    camera cam(vec3(4,6,15), vec3(0,1,0), vec3(0,1,0), 20, 16.0/9.0, 0.0, 15); 
    skybox *skb = new black_skybox();
    return scene(l, cam, skb);
}


scene dark2() {
    std::vector<hitable*> l;

    material *light = new diffuse_light( new constant_texture(vec3(7, 7, 7)) );
    l.push_back(new square(vec3(-150,550,412), vec3(150,550,412), vec3(150,550,147), vec3(-150,550,147), light));

    int nx, ny, nn; 
    unsigned char *ground_tex = stbi_load("../marble.jpg", &nx, &ny, &nn, 0);
    material *ground =  new lambertian(new image_texture(ground_tex, nx, ny));
    int nb = 20;
    for (int i = 0; i < nb; i++) {
        for (int j = 0; j < nb; j++) {
            float w = 100;
            l.push_back(new cube(vec3(-1000+i*w+w/2, w*(drand48()+0.01)/2, -1000+j*w+w/2),100, ground));
        }
    }

    unsigned char *tex_data = stbi_load("../mars_map.jpg", &nx, &ny, &nn, 0);
    material *mat =  new lambertian(new image_texture(tex_data, nx, ny));
    l.push_back(new sphere(vec3(-50, 400, 70), 70, mat));

    unsigned char *tex_data2 = stbi_load("../jupiter_map.jpg", &nx, &ny, &nn, 0);
    material *mat2 =  new lambertian(new image_texture(tex_data2, nx, ny));
    l.push_back(new sphere(vec3(210, 270, 230), 80, mat2));


    texture *pertext = new noise_texture(0.1);
    l.push_back(new sphere(vec3(130, 450, 100), 80, new lambertian(pertext)));
    
    l.push_back(new sphere(vec3(50, 195, 220), 100, new dielectric(1.5)));
    l.push_back(new sphere(vec3(-190, 250, 145), 80, new metal(vec3(0.8, 0.8, 0.9), 0.1)));

    l.push_back(new square(vec3(310,500,0), vec3(310,500,600), vec3(310,140,600), vec3(310,140,0), new metal(vec3(0.7, 0.6, 0.5), 0.0)));

    camera cam(vec3(-200, 280, 1200), vec3(170,320,0), vec3(0,1,0), 33, 16.0/9.0, 0.0, 15); 
    skybox *skb = new black_skybox();
    return scene(l, cam, skb);
}

#endif /* scenes_h */