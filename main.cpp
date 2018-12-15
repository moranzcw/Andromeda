//
//  main.cpp
//  RayTracing
//
//  Created by 张成悟 on 2018/11/24.
//  Copyright © 2018 张成悟. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"

#define WIDTH 1280 // 宽
#define HEIGHT 720 // 高
#define SAMPLE 64 // 采样率
#define DEPTH 50 // 迭代深度
#define LOOK_FROM vec3(0,2,15) // 相机位置
#define LOOK_AT vec3(0,1,0) // 相机朝向
#define DIST_TO_FOCUS (LOOK_AT-LOOK_FROM).length() // 对焦距离
#define APERTURE 0.3 // 光圈


// 着色，返回该光线的色彩，参数：光线，环境物品列表，迭代次数
vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    // 相交检测，rec带回相交信息
    if (world->hit(r, 0.001, MAXFLOAT, rec)) {
        ray scattered; // 散射光线
        vec3 attenuation; // 衰减
        // 若迭代次数没有达到上限，则计算散射光线和衰减，并递归计算颜色
        if (depth < DEPTH && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation*color(scattered, world, depth+1);
        }
        else {
            //若迭代次数已达上限，说明光线散射多次依然没有到达背景，返回黑色
            return vec3(0,0,0);
        }
    }
    else {
        // 若没有相交，则按y坐标计算背景颜色
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

// 生成场景
hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable*[n+1];
    list[0] =  new sphere(vec3(0,-1000,0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    // 在-11<x<11，-11<z<11的区域内生成n个小球
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = drand48();
            // z坐标和x坐标加一些随机偏移，y坐标为0.2，即球心离地0.2，刚好等于半径
            vec3 center(a+0.9*drand48(),0.2,b+0.9*drand48());
            // 避开大球坐标
            if ((center-vec3(4, 0.2, 0)).length() > 0.9
                && (center-vec3(0, 0.2, 0)).length() > 0.9
                && (center-vec3(-4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.6) {  // 0.6概率的漫反射材质
                    list[i++] = new sphere(center, 0.2, new lambertian(vec3(0.9*drand48(), 0.3, 0.2)));
                }
                else if (choose_mat < 0.9) { // 0.3概率的金属材质
                    list[i++] = new sphere(center, 0.2,
                                           new metal(vec3(0.5*(1 + drand48()), 0.5*(1 + drand48()), 0.3*(1 + drand48())),  0.2*drand48()));
                }
                else {  // 0.1概率的玻璃材质
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }
    
    // 三个大球
    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new dielectric(1.5));
    
    return new hitable_list(list,i);
}

int main() {
    std::ofstream fout("t.ppm");
    fout << "P3\n" << WIDTH << " " << HEIGHT << "\n255\n";
    
    // 五个球的场景
    hitable *list[5];
//    float R = cos(M_PI/4);
    list[0] = new sphere(vec3(0, 1, 0), 1.0, new lambertian(vec3(0.1, 0.2, 0.5)));
    list[1] = new sphere(vec3(0,-100,0), 100, new lambertian(vec3(0.7, 0.4, 0.4)));
    list[2] = new sphere(vec3(2, 1, 0), 1.0, new metal(vec3(0.8, 0.6, 0.2), 0.0));
    list[3] = new sphere(vec3(-2, 1, 0), 1.0, new dielectric(1.5));
    list[4] = new sphere(vec3(-2, 1, 0), 0.8, new dielectric(1.5));
    hitable *world = new hitable_list(list,5);
    world = random_scene();

    
    camera cam(LOOK_FROM, LOOK_AT, vec3(0,1,0), 20, float(WIDTH)/float(HEIGHT), APERTURE, DIST_TO_FOCUS);
    

    std::cout<<"Rendering ..."<<std::endl;
    // 从上到下渲染
    for (int j = HEIGHT-1; j >= 0; j--) {
        std::cout<<(HEIGHT-j)/(HEIGHT*1.0)*100<<"%"<<std::endl;
        // 从左到右
        for (int i = 0; i < WIDTH; i++) {
            vec3 col(0, 0, 0); // 渲染结果
            for (int s=0; s < SAMPLE; s++) {
                // u,v坐标添加像素范围内的随机偏移，用于抗锯齿
                float u = float(i + drand48()) / float(WIDTH);
                float v = float(j + drand48()) / float(HEIGHT);
                ray r = cam.get_ray(u, v);
//                vec3 p = r.point_at_parameter(2.0);
                // 累加所有采样的结果
                col += color(r, world,0);
            }
            col /= float(SAMPLE); // 对结果求平均
            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            
            // 写入文件
            fout << ir << " " << ig << " " << ib << "\n";
        }
    }
    fout.close();
    std::cout<<"Complete."<<std::endl;
    return 0;
}
