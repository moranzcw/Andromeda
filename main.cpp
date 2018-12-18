//
//  main.cpp
//  RayTracing
//
//  Created by 张成悟 on 2018/12/19.
//  Copyright © 2018 张成悟. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <unistd.h> // sleep函数
#include "float.h"
#include "sphere.h"
#include "hitable_list.h"
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
#define THREAD_NUM 4


class pixel{
public:
    pixel() {};
    pixel(vec3 c, int x, int y) : color(c), x_pos(x), y_pos(y) {};
    vec3 color;
    int x_pos;
    int y_pos;
};

class request{
public:
    request() {};
    request(const camera *c, const hitable *w, int x, int y) : cam(c), world(w), x_pos(x), y_pos(y) {};
    const camera *cam;
    const hitable *world;
    int x_pos;
    int y_pos;
};

// 着色，返回该光线的色彩，参数：光线，环境物品列表，迭代次数
vec3 color(const ray& r, const hitable *world, int depth) {
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

// 为像素着色，实现了抗锯齿
vec3 pixel_render(int x_pos, int y_pos, const camera *cam, const hitable *world){
    vec3 col(0,0,0);
    for (int s=0; s < SAMPLE; s++) {
        // u,v坐标添加像素范围内的随机偏移，用于抗锯齿
        float u = float(x_pos + drand48()) / float(WIDTH);
        float v = float(y_pos + drand48()) / float(HEIGHT);
        ray r = cam->get_ray(u, v);
//      vec3 p = r.point_at_parameter(2.0);
        // 累加所有采样的结果
        col += color(r, world,0);
    }
    col /= float(SAMPLE); // 对结果求平均
    col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
    return col;
}

// 生成场景
hitable* scene1() {
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

// 生成场景
hitable* scene2() {
    // 五个球
    int n = 5;
    hitable **list = new hitable*[n];
    list[0] = new sphere(vec3(0, 1, 0), 1.0, new lambertian(vec3(0.1, 0.2, 0.5)));
    list[1] = new sphere(vec3(0,-100,0), 100, new lambertian(vec3(0.7, 0.4, 0.4)));
    list[2] = new sphere(vec3(2, 1, 0), 1.0, new metal(vec3(0.8, 0.6, 0.2), 0.0));
    list[3] = new sphere(vec3(-2, 1, 0), 1.0, new dielectric(1.5));
    list[4] = new sphere(vec3(-2, 1, 0), 0.8, new dielectric(1.5));
    return new hitable_list(list,n);
}

std::mutex g_lock;
std::queue<request> q1;
std::queue<pixel> q2;

void run(){
    while(q1.size() > 0){
        g_lock.lock();
        request r;
        if(q1.size() > 0){
            r = q1.front();
            q1.pop();
        }
        else{
            g_lock.unlock();
            return;
        }
        g_lock.unlock();
        vec3 col = pixel_render(r.x_pos, r.y_pos, r.cam, r.world);
        pixel p(col, r.x_pos, r.y_pos);

        g_lock.lock();
        q2.push(p);
        g_lock.unlock();
    }
}

int main() {
    std::cout<<"Size: "<<WIDTH<<"*"<<HEIGHT<<std::endl;
    std::cout<<"Sample: "<<SAMPLE<<std::endl;
    std::cout<<"Depth: "<<DEPTH<<std::endl;
    std::cout<<"Thread: "<<THREAD_NUM<<std::endl;

    hitable *world = scene1();
    // hitable *world = scene2();
    
    camera *cam = new camera(LOOK_FROM, LOOK_AT, vec3(0,1,0), 20, float(WIDTH)/float(HEIGHT), APERTURE, DIST_TO_FOCUS);
    
    request r(cam, world, 0, 0);
    for (int y=0; y<HEIGHT; y++) {
        for (int x=0; x<WIDTH; x++) {
            r.x_pos = x;
            r.y_pos = y;
            q1.push(r);
        }
    }

    std::cout<<"Rendering..."<<std::endl;

    // 创建初始化线程
    std::vector<std::thread> threads;
    for(int i=0;i<THREAD_NUM;i++)
        threads.push_back(std::thread(run));
    
    // 显示进度
    while(q1.size()>0){
        sleep(1);
        std::cout<<100-q1.size()/float(HEIGHT*WIDTH)*100.0<<"%"<<std::endl;
    }

    // 等待线程join
    for(auto &t:threads){
        t.join();
    }
    
    std::cout<<"Render Complete."<<std::endl;


    std::vector<std::vector<vec3> > pic(HEIGHT, std::vector<vec3>(WIDTH));
    while(q2.size() > 0){
        pixel p = q2.front();
        q2.pop();
        pic[p.y_pos][p.x_pos] = p.color;
    }

    std::cout<<"Writing Data..."<<std::endl;
    // 写入文件
    std::ofstream fout("image.ppm");
    fout << "P3\n" << WIDTH << " " << HEIGHT << "\n255\n";
    for (int y = HEIGHT-1; y >= 0; y--) {
        for (int x = 0; x < WIDTH; x++) {
            vec3 &col = pic[y][x];
            fout << int(255.99*col[0]) << " " << int(255.99*col[1]) << " " << int(255.99*col[2]) << "\n";
        }
    }
    fout.close();

    std::cout<<"Complete."<<std::endl;
    return 0;
}
