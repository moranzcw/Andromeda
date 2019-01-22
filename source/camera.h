//
//  camera.h
//  RayTracing
//
//  Created by moranzcw on 2018/11/25.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"

// 单位圆的区域内生成随机点
vec3 random_in_unit_disk();

class Camera
{
  public:
    Camera()
        : position(vec3(0, 6, 15)),
          directionPositon(vec3(0, 1, 0)),
          viewUp(vec3(0, 1, 0)),
          viewFov(40),
          aspect(16.0 / 9.0),
          aperture(0.0)
    {
        this->focusDist = (directionPositon - position).length();
        update();
    }

    // 构造函数，参数：相机位置，相机指向，视野上方方向，视角(垂直方向)，宽高比，光圈，焦平面到相机的距离
    Camera(vec3 pos, vec3 direct_pos, vec3 vup, float vfov, float aspect,
           float aperture, float focus_dist)
        : position(pos),
          directionPositon(direct_pos),
          viewUp(vup),
          viewFov(vfov),
          aspect(aspect),
          aperture(aperture),
          focusDist(focus_dist)
    {
        update();
    }

    // 获取一根光线
    ray get_ray(float s, float t) const
    {
        vec3 rd = lensRadius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y(); // 目标点的偏移量
        return ray(
            position + offset,
            lowerLeftCorner + s * horizontal + t * vertical -
                (position + offset)); // 起点添加了偏移，终点依然在原来的焦平面位置
    }

    // 在修改参数后，需要更新相机
    int update()
    {
        lensRadius = aperture / 2;                    // 光圈半径
        float theta = viewFov * M_PI / 180;           // 视角换算为弧度
        float half_height = tan(theta / 2);           // 焦平面高度与焦平面距离的比值的一半
        float half_width = aspect * half_height;      // 焦平面宽度与焦平面距离的比值的一半
        w = unit_vector(position - directionPositon); // 相机方向的负方向，单位向量
        u = unit_vector(cross(viewUp, w));            // 屏幕空间的x轴方向，单位向量
        v = cross(w, u);                              // 屏幕空间的y轴方向，单位向量
        lowerLeftCorner = position - half_width * focusDist * u -
                          half_height * focusDist * v -
                          focusDist * w;             // 焦平面左下角位置
        horizontal = 2 * half_width * focusDist * u; // 焦平面宽度
        vertical = 2 * half_height * focusDist * v;  // 焦平面高度
        return 0;
    }

    // 数据
    vec3 position;         // 相机位置
    vec3 directionPositon; // 对焦点位置
    vec3 viewUp;           // 视野上方方向
    float viewFov;         // 视角大小
    float aspect;          // 宽高比
    float aperture;        // 光圈大小
    float focusDist;       // 对焦距离

  private:
    vec3 lowerLeftCorner; // 焦平面左下角位置
    vec3 horizontal;      // 焦平面宽度
    vec3 vertical;        // 焦平面高度
    vec3 u, v, w;
    float lensRadius; // 光圈半径
};

vec3 random_in_unit_disk()
{
    vec3 p;
    do
    {
        p = 2.0 * vec3(drand48(), drand48(), 0) - vec3(1, 1, 0);
    } while (dot(p, p) >= 1.0);
    return p;
}

#endif /* CAMERA_H */
