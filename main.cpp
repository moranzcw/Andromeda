//
//  main.cpp
//  RayTracing
//
//  Created by moranzcw on 2018/12/19.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <cfloat>
#include "sphere.h"
#include "triangle.h"
#include "cube.h"
#include "bvh.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define WIDTH 1280 // 宽
#define HEIGHT 720 // 高
// #define SAMPLE 2000 // 采样率
#define SAMPLE 100 // 采样率
#define DEPTH 50 // 迭代深度
#define LOOK_FROM vec3(4,6,15) // 相机位置
#define LOOK_AT vec3(0,1,0) // 相机朝向
// #define LOOK_FROM vec3(-200, 280, 1200) // 相机位置
// #define LOOK_AT vec3(170,320,0) // 相机朝向
#define DIST_TO_FOCUS (LOOK_AT-LOOK_FROM).length() // 对焦距离
#define APERTURE 0.0 // 光圈
#define FOV 20
// #define FOV 33
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
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered; // 散射光线
        vec3 attenuation; // 衰减
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p); //自发光
        // 若迭代次数没有达到上限，则计算散射光线和衰减，并递归计算颜色
        if (depth < DEPTH && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return emitted + attenuation*color(scattered, world, depth+1);
        }
        else {
            //若迭代次数已达上限，说明光线散射多次依然没有到达背景，返回黑色
            return emitted + vec3(0,0,0);
        }
    }
    else {
        // 若没有相交，则按y坐标计算背景颜色
        // vec3 unit_direction = unit_vector(r.direction());
        // float t = 0.5*(unit_direction.y() + 1.0);
        // return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
        return vec3(0,0,0);
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
    // 地面
    texture *checker = new checker_texture(new constant_texture(vec3(0.2,0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
    list[0] = new square(vec3(100,0,100), vec3(-100,0,100), vec3(-100,0,-100), vec3(100,0,-100), new lambertian(checker));
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
                    list[i++] = new sphere(center, 0.2, new lambertian(new constant_texture(vec3(0.9*drand48(), 0.3, 0.2))));
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
    list[i++] = new sphere(vec3(-3, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
    list[i++] = new sphere(vec3(3, 1, 0), 1.0, new dielectric(1.5));
    
    return new bvh_node(list,i);
}

// 生成场景
hitable* scene2() {
    // 四个球
    int n = 5;
    hitable **list = new hitable*[n];
    list[0] = new square(vec3(100,0,100), vec3(-100,0,100), vec3(-100,0,-100), vec3(100,0,-100), new lambertian(new constant_texture(vec3(0.7, 0.4, 0.4))));
    list[1] = new sphere(vec3(0, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.1, 0.2, 0.5))));
    list[2] = new sphere(vec3(2, 1, 0), 1.0, new metal(vec3(0.8, 0.6, 0.2), 0.0));
    list[3] = new sphere(vec3(-2, 1, 0), 1.0, new dielectric(1.5));
    list[4] = new sphere(vec3(-2, 1, 0), 0.8, new dielectric(1.5));
    return new bvh_node(list,n);
}

// 生成场景
hitable* scene3() {
    int n = 7;
    hitable **list = new hitable*[n];
    list[0] = new square(vec3(100,0,100), vec3(-100,0,100), vec3(-100,0,-100), vec3(100,0,-100), new lambertian(new constant_texture(vec3(0.7, 0.7, 0.6))));
    list[1] = new square(vec3(100,0,95), vec3(0,0,-5), vec3(0,100,-5), vec3(100,100,95), new metal(vec3(0.7, 0.6, 0.5), 0.0));
    list[2] = new square(vec3(-100,0,95), vec3(-100,100,95), vec3(0,100,-5), vec3(0,0,-5), new metal(vec3(0.7, 0.6, 0.5), 0.0));

    list[3] = new cube(vec3(-0.5,0.75,-1.5), 1.5, new metal(vec3(0.7, 0.6, 0.5), 0.0));
    list[4] = new sphere(vec3(-1.5,1,1.5), 1, new metal(vec3(0.8, 0.6, 0.2), 0.0));
    list[5] = new sphere(vec3(2,1,0), 1.0, new lambertian(new constant_texture(vec3(0.3, 0.5, 0.4))));
    list[6] = new sphere(vec3(-0.5,2.5,-1.5), 1.0, new dielectric(1.5));

    return new bvh_node(list,n);
}

hitable *two_spheres() {
    int n = 2;
    hitable **list = new hitable*[2];

    texture *checker = new checker_texture(new constant_texture(vec3(0.2,0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
    list[0] =  new sphere(vec3(0,-10, 0), 10, new lambertian(checker));
    list[1] =  new sphere(vec3(0, 10, 0), 10, new lambertian(checker));

    return new bvh_node(list,n);
}

hitable *perlin_spheres() {
    int n = 2;
    hitable **list = new hitable*[2];

    texture *pertext = new noise_texture(4);
    list[0] = new square(vec3(100,0,100), vec3(-100,0,100), vec3(-100,0,-100), vec3(100,0,-100),new lambertian( pertext ));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian( pertext ));
    
    return new bvh_node(list,n);
}

hitable *image_scene() {
    int n = 4;
    hitable **list = new hitable*[3];

    texture *checker = new checker_texture(new constant_texture(vec3(0.2,0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
    list[0] = new square(vec3(100,0,100), vec3(-100,0,100), vec3(-100,0,-100), vec3(100,0,-100),new lambertian( checker ));
    
    int nx, ny, nn;
    unsigned char *tex_data = stbi_load("../mars_map.jpg", &nx, &ny, &nn, 0);
    material *mat =  new lambertian(new image_texture(tex_data, nx, ny));
    list[1] = new sphere(vec3(-2.5, 1, 0), 1, mat);

    // texture *pertext = new noise_texture(4);
    // list[2] = new sphere(vec3(0, 1, 0), 1, new lambertian( pertext ));

    unsigned char *tex_data2 = stbi_load("../jupiter_map.jpg", &nx, &ny, &nn, 0);
    material *mat2 =  new lambertian(new image_texture(tex_data2, nx, ny));
    list[2] = new sphere(vec3(0, 1, 0), 1, mat2);

    unsigned char *tex_data3 = stbi_load("../marble.jpg", &nx, &ny, &nn, 0);
    material *mat3 =  new lambertian(new image_texture(tex_data3, nx, ny));
    list[3] = new cube(vec3(2.5, 1, 0), 1.5, mat3);

    return new bvh_node(list,n);
}

hitable *simple_light() {
    hitable **list = new hitable*[3];

    texture *checker = new checker_texture(new constant_texture(vec3(0.2,0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
    list[0] = new square(vec3(100,0,100), vec3(-100,0,100), vec3(-100,0,-100), vec3(100,0,-100),new lambertian( checker ));
    
    texture *pertext = new noise_texture(4);
    list[1] =  new sphere(vec3(0, 1, 0), 1, new lambertian( pertext ));

    list[2] =  new square(vec3(2,2,-1), vec3(2,2,1), vec3(2,0,1), vec3(2,0,-1), new diffuse_light(new constant_texture(vec3(4,4,4))));
    return new bvh_node(list,3);
}


// 生成场景
hitable* scene4() {
    int n = 8;
    hitable **list = new hitable*[n];
    int nx, ny, nn; 
    unsigned char *ground_tex = stbi_load("../marble.jpg", &nx, &ny, &nn, 0);
    material *ground =  new lambertian(new image_texture(ground_tex, nx, ny));
    list[0] = new square(vec3(10,0,10), vec3(-10,0,10), vec3(-10,0,-10), vec3(10,0,-10), ground);
    list[1] = new square(vec3(100,0,95), vec3(0,0,-5), vec3(0,100,-5), vec3(100,100,95), new metal(vec3(0.7, 0.6, 0.5), 0.0));
    list[2] = new square(vec3(-100,0,95), vec3(-100,100,95), vec3(0,100,-5), vec3(0,0,-5), new metal(vec3(0.7, 0.6, 0.5), 0.0));

    // 
    texture *checker = new checker_texture(new constant_texture(vec3(0.2,0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
    list[3] = new cube(vec3(-0.5,0.75,-1.5), 1.5, new lambertian(checker));
    // list[3] = new cube(vec3(-0.5,0.75,-1.5), 1.3, new metal(vec3(0.8, 0.8, 0.9), 0.0));
    list[4] = new sphere(vec3(-1.5,1,1.5), 1, new metal(vec3(0.8, 0.6, 0.2), 0.0));

    unsigned char *tex_data2 = stbi_load("../jupiter_map.jpg", &nx, &ny, &nn, 0);
    material *mat2 =  new lambertian(new image_texture(tex_data2, nx, ny));
    list[5] = new sphere(vec3(2,1,0), 1.0, mat2);
    list[6] = new sphere(vec3(-0.5,2.5,-1.5), 1.0, new dielectric(1.5));

    material *light = new diffuse_light( new constant_texture(vec3(7, 7, 7)) );
    list[7] = new cube(vec3(0.4,6,1.0), 2.5, light);

    return new hitable_list(list,n);
}


hitable *final() {
    hitable **list = new hitable*[500];
    int l = 0;

    material *light = new diffuse_light( new constant_texture(vec3(7, 7, 7)) );
    list[l++] = new square(vec3(-150,550,412), vec3(150,550,412), vec3(150,550,147), vec3(-150,550,147), light);

    int nx, ny, nn; 
    unsigned char *ground_tex = stbi_load("../marble.jpg", &nx, &ny, &nn, 0);
    material *ground =  new lambertian(new image_texture(ground_tex, nx, ny));
    int nb = 20;
    for (int i = 0; i < nb; i++) {
        for (int j = 0; j < nb; j++) {
            float w = 100;
            list[l++] = new cube(vec3(-1000+i*w+w/2, w*(drand48()+0.01)/2, -1000+j*w+w/2),100, ground);
        }
    }

    unsigned char *tex_data = stbi_load("../mars_map.jpg", &nx, &ny, &nn, 0);
    material *mat =  new lambertian(new image_texture(tex_data, nx, ny));
    list[l++] = new sphere(vec3(-50, 400, 70), 70, mat);

    unsigned char *tex_data2 = stbi_load("../jupiter_map.jpg", &nx, &ny, &nn, 0);
    material *mat2 =  new lambertian(new image_texture(tex_data2, nx, ny));
    list[l++] = new sphere(vec3(210, 270, 230), 80, mat2);


    texture *pertext = new noise_texture(0.1);
    list[l++] = new sphere(vec3(130, 450, 100), 80, new lambertian(pertext));
    
    list[l++] = new sphere(vec3(50, 195, 220), 100, new dielectric(1.5));
    list[l++] = new sphere(vec3(-190, 250, 145), 80, new metal(vec3(0.8, 0.8, 0.9), 0.1));

    list[l++] = new square(vec3(310,500,0), vec3(310,500,600), vec3(310,140,600), vec3(310,140,0), new metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new bvh_node(list,l);
}

std::mutex q1_mutex;
std::mutex q2_mutex;
std::queue<request> q1;
std::queue<pixel> q2;

void run(){
    while(true){
        // 从q1中取出渲染请求
        request r;
        if(q1.size() > 0){
            std::lock_guard<std::mutex> lock(q1_mutex);
            r = q1.front();
            q1.pop();
        }
        else
            break;

        // 渲染
        vec3 col = pixel_render(r.x_pos, r.y_pos, r.cam, r.world);
        pixel p(col, r.x_pos, r.y_pos);

        // 渲染结果写入q2
        std::lock_guard<std::mutex> lock(q2_mutex);
        q2.push(p);
    }
    return;
}

int main() {
    std::cout<<"Size:"<<WIDTH<<"*"<<HEIGHT<<", ";
    std::cout<<"Sample:"<<SAMPLE<<", ";
    std::cout<<"Depth:"<<DEPTH<<", ";
    std::cout<<"Thread:"<<THREAD_NUM<<std::endl;

    // hitable *world = scene1();
    // hitable *world = scene2();
    // hitable *world = scene3();
    hitable *world = scene4();
    // hitable *world = two_spheres();
    // hitable *world = perlin_spheres();
    // hitable *world = image_scene();
    // hitable *world = simple_light();
    // hitable *world = final();
    
    camera *cam = new camera(LOOK_FROM, LOOK_AT, vec3(0,1,0), FOV, float(WIDTH)/float(HEIGHT), APERTURE, DIST_TO_FOCUS);
    
    request r(cam, world, 0, 0);
    for (int y=0; y<HEIGHT; y++) {
        for (int x=0; x<WIDTH; x++) {
            r.x_pos = x;
            r.y_pos = y;
            q1.push(r);
        }
    }

    std::cout<<"Rendering..."<<std::flush;

    // 创建工作线程组，并开始
    std::vector<std::thread> thread_group;
    for(int i=0;i<THREAD_NUM;i++)
        thread_group.push_back(std::thread(run));
    
    // 根据q2元素数量显示进度
    int rate,last;
    while(q1.size()>0){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        rate = int(q2.size()/float(HEIGHT*WIDTH)*100);
        if(rate>last+4){
            std::cout<<"."<<std::flush;
            last = rate;
        }
    }
    std::cout<<"OK"<<std::endl;
    
    // 等待所有线程join
    for(auto &t:thread_group){
        t.join();
    }

    // 收到的数据可能乱序，输出到二维vector中
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
            col[0] = col[0]>1?1:col[0];
            col[1] = col[1]>1?1:col[1];
            col[2] = col[2]>1?1:col[2];
            fout << int(255.99*col[0]) << " " << int(255.99*col[1]) << " " << int(255.99*col[2]) << "\n";
        }
    }
    fout.close();
    std::cout<<"Complete."<<std::endl;
    return 0;
}
