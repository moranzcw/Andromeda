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
#include "camera.h"
#include "scenes.h"

#define WIDTH 1280   // 宽
#define HEIGHT 720   // 高
#define SAMPLE 100    // 采样率
#define DEPTH 50     // 迭代深度
#define THREAD_NUM 4 // 线程数

// 着色，返回该光线的色彩，参数：光线，环境物品列表，迭代次数
Vec3 color(const Ray &r, const Scene &s, int depth)
{
    HitRecord rec;
    // 相交检测，rec带回相交信息
    if (s.bvh_tree.hit(r, 0.001, FLT_MAX, rec))
    {
        Ray scattered;                                            // 散射光线
        Vec3 attenuation;                                         // 衰减
        Vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p); //自发光
        // 若迭代次数没有达到上限，且相交的物体可以散射，则计算散射光线和衰减，并递归计算颜色
        if (depth < DEPTH && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        {
            return emitted + attenuation * color(scattered, s, depth + 1);
        }
        //若迭代次数已达上限，说明光线散射多次依然没有到达背景，返回黑色
        return emitted + Vec3(0, 0, 0);
    }
    // 天空盒子
    return s.skb->getBackgroundColor(r);
}

// 为像素着色，实现了抗锯齿
Vec3 pixel_render(int x_pos, int y_pos, const Scene &scene)
{
    Vec3 col(0, 0, 0);
    for (int s = 0; s < SAMPLE; s++)
    {
        // u,v坐标添加像素范围内的随机偏移，用于抗锯齿
        float u = float(x_pos + drand48()) / float(WIDTH);
        float v = float(y_pos + drand48()) / float(HEIGHT);
        Ray r = scene.cam.getRay(u, v);
        //      Vec3 p = r.point_at_parameter(2.0);
        // 累加所有采样的结果
        col += color(r, scene, 0);
    }
    col /= float(SAMPLE); // 对结果求平均
    col = Vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
    return col;
}

class pixel
{
  public:
    pixel(){};
    pixel(Vec3 c, int x, int y) : color(c), x_pos(x), y_pos(y){};
    Vec3 color;
    int x_pos;
    int y_pos;
};

class request
{
  public:
    request(){};
    request(const Scene *w, int x, int y) : scene(w), x_pos(x), y_pos(y){};
    const Scene *scene;
    int x_pos;
    int y_pos;
};

std::mutex q1_mutex;
std::mutex q2_mutex;
std::queue<request> q1;
std::queue<pixel> q2;

void run()
{
    while (true)
    {
        // 从q1中取出渲染请求
        request r;
        if (q1.size() > 0)
        {
            std::lock_guard<std::mutex> lock(q1_mutex);
            r = q1.front();
            q1.pop();
        }
        else
            break;

        // 渲染
        Vec3 col = pixel_render(r.x_pos, r.y_pos, *r.scene);
        pixel p(col, r.x_pos, r.y_pos);

        // 渲染结果写入q2
        std::lock_guard<std::mutex> lock(q2_mutex);
        q2.push(p);
    }
    return;
}

int main()
{
    std::cout << "Size:" << WIDTH << "*" << HEIGHT << ", ";
    std::cout << "Sample:" << SAMPLE << ", ";
    std::cout << "Depth:" << DEPTH << ", ";
    std::cout << "Thread:" << THREAD_NUM << std::endl;

    // Scene s = scene1();
    Scene s = scene2();
    // Scene s = skybox_scene();
    // Scene s = two_Spheres();
    // Scene s = perlin_spheres();
    // Scene s = image_tex();
    // Scene s = simple_light();
    // Scene s = dark1();
    // Scene s = dark2();

    request r(&s, 0, 0);
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            r.x_pos = x;
            r.y_pos = y;
            q1.push(r);
        }
    }

    std::cout << "Rendering..." << std::flush;

    // 创建工作线程组，并开始
    std::vector<std::thread> thread_group;
    for (int i = 0; i < THREAD_NUM; i++)
        thread_group.push_back(std::thread(run));

    // 根据q2元素数量显示进度
    int rate, last;
    while (q1.size() > 0)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        rate = int(q2.size() / float(HEIGHT * WIDTH) * 100);
        if (rate > last + 4)
        {
            std::cout << "." << std::flush;
            last = rate;
        }
    }
    std::cout << "OK" << std::endl;

    // 等待所有线程join
    for (auto &t : thread_group)
    {
        t.join();
    }

    // 收到的数据可能乱序，输出到二维vector中
    std::vector<std::vector<Vec3>> pic(HEIGHT, std::vector<Vec3>(WIDTH));
    while (q2.size() > 0)
    {
        pixel p = q2.front();
        q2.pop();
        pic[p.y_pos][p.x_pos] = p.color;
    }

    std::cout << "Writing Data..." << std::endl;
    // 写入文件
    std::ofstream fout("image.ppm");
    fout << "P3\n"
         << WIDTH << " " << HEIGHT << "\n255\n";
    for (int y = HEIGHT - 1; y >= 0; y--)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            Vec3 &col = pic[y][x];
            col[0] = col[0] > 1 ? 1 : col[0];
            col[1] = col[1] > 1 ? 1 : col[1];
            col[2] = col[2] > 1 ? 1 : col[2];
            fout << int(255.99 * col[0]) << " " << int(255.99 * col[1]) << " " << int(255.99 * col[2]) << "\n";
        }
    }
    fout.close();
    std::cout << "Complete." << std::endl;
    return 0;
}
