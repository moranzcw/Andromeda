//
//  scenes.h
//  RayTracing
//
//  Created by moranzcw on 2018/12/28.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef SCENES_H
#define SCENES_H

#include <vector>
#include "model.h"
#include "sphere.h"
#include "cube.h"
#include "material.h"
#include "bvh.h"
#include "scene.h"
#include "skybox.h"
#include "resource.h"
#define STB_IMAGE_IMPLEMENTATION
#include "tools/stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tools/tiny_obj_loader.h"
#include <iostream>

// 生成场景
Scene scene1()
{
    std::vector<Object *> l;
    // 地面
    Texture *checker = new CheckerTexture(new ConstantTexture(Vec3(0.2, 0.3, 0.1)), new ConstantTexture(Vec3(0.9, 0.9, 0.9)));
    l.push_back(new Square(Vec3(100, 0, 100), Vec3(-100, 0, 100), Vec3(-100, 0, -100), Vec3(100, 0, -100), new Lambertian(checker)));

    // 在-11<x<11，-11<z<11的区域内生成n个小球
    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            float choose_mat = drand48();
            // z坐标和x坐标加一些随机偏移，y坐标为0.2，即球心离地0.2，刚好等于半径
            Vec3 center(a + 0.9 * drand48(), 0.2, b + 0.9 * drand48());
            // 避开大球坐标
            if ((center - Vec3(3, 0.2, 0)).length() > 0.9 && (center - Vec3(0, 0.2, 0)).length() > 0.9 && (center - Vec3(-3, 0.2, 0)).length() > 0.9)
            {
                if (choose_mat < 0.6)
                { // 0.6概率的漫反射材质
                    l.push_back(new Sphere(center, 0.2, new Lambertian(new ConstantTexture(Vec3(0.9 * drand48(), 0.3, 0.2)))));
                }
                else if (choose_mat < 0.9)
                { // 0.3概率的金属材质
                    l.push_back(new Sphere(center, 0.2,
                                           new Metal(Vec3(0.5 * (1 + drand48()), 0.5 * (1 + drand48()), 0.3 * (1 + drand48())), 0.2 * drand48())));
                }
                else
                { // 0.1概率的玻璃材质
                    l.push_back(new Sphere(center, 0.2, new Dielectric(1.5)));
                }
            }
        }
    }

    // 三个大球
    l.push_back(new Sphere(Vec3(0, 1, 0), 1.0, new Metal(Vec3(0.7, 0.6, 0.5), 0.0)));
    l.push_back(new Sphere(Vec3(-3, 1, 0), 1.0, new Lambertian(new ConstantTexture(Vec3(0.4, 0.2, 0.1)))));
    l.push_back(new Sphere(Vec3(3, 1, 0), 1.0, new Dielectric(1.5)));

    Camera cam(Vec3(0, 1, 15), Vec3(0, 1, 0), Vec3(0, 1, 0), 30, 16.0 / 9.0, 0.0, 15);

    int nx, ny, nn;
    unsigned char *tex_data = stbi_load(SKYBOX_TEXTURE_DAYLIGHT_2, &nx, &ny, &nn, 0);
    SkyBox *skb = new DaylightSkyBox(new ImageTexture(tex_data, nx, ny));
    return Scene(l, cam, skb);
}

// 生成场景
Scene scene2()
{
    std::vector<Object *> l;
    // 地面
    Texture *checker = new CheckerTexture(new ConstantTexture(Vec3(0.2, 0.3, 0.1)), new ConstantTexture(Vec3(0.9, 0.9, 0.9)));
    // l.push_back(new Square(Vec3(100, 0, 100), Vec3(-100, 0, 100), Vec3(-100, 0, -100), Vec3(100, 0, -100), new Lambertian(checker)));

    //
    const char *filename = "../resource/models/tr-and-d-issue-43.obj";
    const char *basepath = "../resource/models/";
    std::cout << "Loading " << filename << std::endl;

    tinyobj::attrib_t attrib;                   // 包含所有顶点坐标，法线，纹理坐标数据
    std::vector<tinyobj::shape_t> shapes;       // 形状
    std::vector<tinyobj::material_t> materials; // 材质

    // 使用 tinyobjloader 加载模型
    std::string warn; // 警告
    std::string err;  // 错误
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename,
                                basepath, true);

    // 警告
    if (!warn.empty())
    {
        std::cout << "WARN: " << warn << std::endl;
    }

    // 错误
    if (!err.empty())
    {
        std::cerr << "ERR: " << err << std::endl;
    }

    // 加载失败
    if (!ret)
    {
        std::cerr << "Failed to load/parse .obj." << std::endl;
    }

    // 顶点，法线，纹理坐标
    std::cout << "# of vertices  : " << (attrib.vertices.size() / 3) << std::endl;
    std::cout << "# of normals   : " << (attrib.normals.size() / 3) << std::endl;
    std::cout << "# of texcoords : " << (attrib.texcoords.size() / 2) << std::endl;

    // 形状，纹理
    std::cout << "# of shapes    : " << shapes.size() << std::endl;
    std::cout << "# of materials : " << materials.size() << std::endl;

    // 输出顶点坐标
    for (size_t v = 0; v < attrib.vertices.size() / 3; v++)
    {
        printf("  v[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
               static_cast<const double>(attrib.vertices[3 * v + 0]),
               static_cast<const double>(attrib.vertices[3 * v + 1]),
               static_cast<const double>(attrib.vertices[3 * v + 2]));
    }

    // 输出法线
    for (size_t v = 0; v < attrib.normals.size() / 3; v++)
    {
        printf("  n[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
               static_cast<const double>(attrib.normals[3 * v + 0]),
               static_cast<const double>(attrib.normals[3 * v + 1]),
               static_cast<const double>(attrib.normals[3 * v + 2]));
    }

    // 输出纹理坐标
    for (size_t v = 0; v < attrib.texcoords.size() / 2; v++)
    {
        printf("  uv[%ld] = (%f, %f)\n", static_cast<long>(v),
               static_cast<const double>(attrib.texcoords[2 * v + 0]),
               static_cast<const double>(attrib.texcoords[2 * v + 1]));
    }

    std::vector<Triangle> triangles;
    // 对每个shape
    for (size_t i = 0; i < shapes.size(); i++)
    {
        // shape名
        printf("shape[%ld].name = %s\n", static_cast<long>(i),
               shapes[i].name.c_str());
        // 顶点总数
        printf("Size of shape[%ld].mesh.indices: %lu\n", static_cast<long>(i),
               static_cast<unsigned long>(shapes[i].mesh.indices.size()));
        // printf("Size of shape[%ld].path.indices: %lu\n", static_cast<long>(i),
        //        static_cast<unsigned long>(shapes[i].path.indices.size()));

        size_t index_offset = 0;

        assert(shapes[i].mesh.num_face_vertices.size() == shapes[i].mesh.material_ids.size());
        assert(shapes[i].mesh.num_face_vertices.size() == shapes[i].mesh.smoothing_group_ids.size());

        printf("shape[%ld].num_faces: %lu\n", static_cast<long>(i),
               static_cast<unsigned long>(shapes[i].mesh.num_face_vertices.size()));

        // 对每个face
        for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++)
        {
            size_t fnum = shapes[i].mesh.num_face_vertices[f];

            printf("  face[%ld].fnum = %ld\n", static_cast<long>(f),
                   static_cast<unsigned long>(fnum));

            Vec3 position[3];
            // 对每个顶点
            for (size_t v = 0; v < fnum; v++)
            {
                tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
                printf("    face[%ld].v[%ld].idx = %d/%d/%d, ", static_cast<long>(f),
                       static_cast<long>(v), idx.vertex_index, idx.normal_index,
                       idx.texcoord_index);
                position[v] = Vec3(static_cast<float>(attrib.vertices[idx.vertex_index * 3]),
                             static_cast<float>(attrib.vertices[idx.vertex_index * 3 + 1]),
                             static_cast<float>(attrib.vertices[idx.vertex_index * 3 + 2]));
                std::cout<<position[v]<<std::endl;
            }

            triangles.push_back(Triangle(position[0],position[1],position[2],new Lambertian(checker)));
            // printf("  face[%ld].material_id = %d\n", static_cast<long>(f),
            //        shapes[i].mesh.material_ids[f]);
            // printf("  face[%ld].smoothing_group_id = %d\n", static_cast<long>(f),
            //        shapes[i].mesh.smoothing_group_ids[f]);

            index_offset += fnum;
        }
    }

    l.push_back(new Model(triangles));
    Camera cam(Vec3(-6, 5, 20), Vec3(0, 0, 0), Vec3(0, 1, 0), 30, 16.0 / 9.0, 0.0, 15);

    int nx, ny, nn;
    unsigned char *tex_data = stbi_load(SKYBOX_TEXTURE_DAYLIGHT_2, &nx, &ny, &nn, 0);
    SkyBox *skb = new BlueSkyBox();
    return Scene(l, cam, skb);
}

// 生成场景
Scene skybox_scene()
{
    // skybox
    std::vector<Object *> l;
    l.push_back(new Sphere(Vec3(0, 1, 0), 1.0, new Dielectric(1.5)));
    l.push_back(new Sphere(Vec3(0, 1, 0), 0.8, new Dielectric(1.5)));

    Camera cam(Vec3(0, 5, 10), Vec3(0, 1, 0), Vec3(0, 1, 0), 110, 16.0 / 9.0, 0.01, 2.5);

    int nx, ny, nn;
    unsigned char *tex_data = stbi_load(SKYBOX_TEXTURE_DAYLIGHT_2, &nx, &ny, &nn, 0);
    SkyBox *skb = new DaylightSkyBox(new ImageTexture(tex_data, nx, ny));
    return Scene(l, cam, skb);
}

Scene two_Spheres()
{
    std::vector<Object *> l;

    Texture *checker = new CheckerTexture(new ConstantTexture(Vec3(0.2, 0.3, 0.1)), new ConstantTexture(Vec3(0.9, 0.9, 0.9)));
    l.push_back(new Sphere(Vec3(0, -10, 0), 10, new Lambertian(checker)));
    l.push_back(new Sphere(Vec3(0, 10, 0), 10, new Lambertian(checker)));

    Camera cam(Vec3(0, 1, 15), Vec3(0, 1, 0), Vec3(0, 1, 0), 30, 16.0 / 9.0, 0.2, 15);
    SkyBox *skb = new BlueSkyBox();
    return Scene(l, cam, skb);
}

Scene perlin_Spheres()
{
    std::vector<Object *> l;

    Texture *pertext = new NoiseTexture(4);
    l.push_back(new Square(Vec3(100, 0, 100), Vec3(-100, 0, 100), Vec3(-100, 0, -100), Vec3(100, 0, -100), new Lambertian(pertext)));
    l.push_back(new Sphere(Vec3(0, 2, 0), 2, new Lambertian(pertext)));

    Camera cam(Vec3(0, 1, 15), Vec3(0, 1, 0), Vec3(0, 1, 0), 30, 16.0 / 9.0, 0.2, 15);
    SkyBox *skb = new BlueSkyBox();
    return Scene(l, cam, skb);
}

Scene image_tex()
{
    std::vector<Object *> l;

    Texture *checker = new CheckerTexture(new ConstantTexture(Vec3(0.2, 0.3, 0.1)), new ConstantTexture(Vec3(0.9, 0.9, 0.9)));
    l.push_back(new Square(Vec3(100, 0, 100), Vec3(-100, 0, 100), Vec3(-100, 0, -100), Vec3(100, 0, -100), new Lambertian(checker)));

    int nx, ny, nn;
    unsigned char *tex_data = stbi_load(TEXTURE_MARS_MAP, &nx, &ny, &nn, 0);
    Material *mat = new Lambertian(new ImageTexture(tex_data, nx, ny));
    l.push_back(new Sphere(Vec3(-2.5, 1, 0), 1, mat));

    unsigned char *tex_data2 = stbi_load(TEXTURE_JUPITER_MAP, &nx, &ny, &nn, 0);
    Material *mat2 = new Lambertian(new ImageTexture(tex_data2, nx, ny));
    l.push_back(new Sphere(Vec3(0, 1, 0), 1, mat2));

    unsigned char *tex_data3 = stbi_load(TEXTURE_MARBLE_MAP, &nx, &ny, &nn, 0);
    Material *mat3 = new Lambertian(new ImageTexture(tex_data3, nx, ny));
    l.push_back(new Cube(Vec3(2.5, 1, 0), 1.5, mat3));

    Camera cam(Vec3(0, 1, 15), Vec3(0, 1, 0), Vec3(0, 1, 0), 30, 16.0 / 9.0, 0.2, 15);
    SkyBox *skb = new BlueSkyBox();
    return Scene(l, cam, skb);
}

Scene simple_light()
{
    std::vector<Object *> l;

    Texture *checker = new CheckerTexture(new ConstantTexture(Vec3(0.2, 0.3, 0.1)), new ConstantTexture(Vec3(0.9, 0.9, 0.9)));
    l.push_back(new Square(Vec3(100, 0, 100), Vec3(-100, 0, 100), Vec3(-100, 0, -100), Vec3(100, 0, -100),
                           new Lambertian(checker)));

    Texture *pertext = new NoiseTexture(4);
    l.push_back(new Sphere(Vec3(0, 1, 0), 1, new Lambertian(pertext)));

    l.push_back(new Square(Vec3(2, 2, -1), Vec3(2, 2, 1), Vec3(2, 0, 1), Vec3(2, 0, -1),
                           new DiffuseLight(new ConstantTexture(Vec3(4, 4, 4)))));
    Camera cam(Vec3(0, 1, 15), Vec3(0, 1, 0), Vec3(0, 1, 0), 30, 16.0 / 9.0, 0.2, 15);
    SkyBox *skb = new BlueSkyBox();
    return Scene(l, cam, skb);
}

// 生成场景
Scene dark1()
{
    std::vector<Object *> l;

    int nx, ny, nn;
    unsigned char *ground_tex = stbi_load(TEXTURE_MARBLE_MAP, &nx, &ny, &nn, 0);
    Material *ground = new Lambertian(new ImageTexture(ground_tex, nx, ny));
    l.push_back(new Square(Vec3(10, 0, 10), Vec3(-10, 0, 10), Vec3(-10, 0, -10), Vec3(10, 0, -10), ground));
    l.push_back(new Square(Vec3(100, 0, 95), Vec3(0, 0, -5), Vec3(0, 100, -5), Vec3(100, 100, 95),
                           new Metal(Vec3(0.7, 0.6, 0.5), 0.0)));
    l.push_back(new Square(Vec3(-100, 0, 95), Vec3(-100, 100, 95), Vec3(0, 100, -5), Vec3(0, 0, -5),
                           new Metal(Vec3(0.7, 0.6, 0.5), 0.0)));

    //
    Texture *checker = new CheckerTexture(new ConstantTexture(Vec3(0.2, 0.3, 0.1)), new ConstantTexture(Vec3(0.9, 0.9, 0.9)));
    l.push_back(new Cube(Vec3(-0.5, 0.75, -1.5), 1.5, new Lambertian(checker)));

    l.push_back(new Sphere(Vec3(-1.5, 1, 1.5), 1, new Metal(Vec3(0.8, 0.6, 0.2), 0.0)));

    unsigned char *tex_data2 = stbi_load(TEXTURE_JUPITER_MAP, &nx, &ny, &nn, 0);
    Material *mat2 = new Lambertian(new ImageTexture(tex_data2, nx, ny));
    l.push_back(new Sphere(Vec3(2, 1, 0), 1.0, mat2));
    l.push_back(new Sphere(Vec3(-0.5, 2.5, -1.5), 1.0, new Dielectric(1.5)));

    Material *light = new DiffuseLight(new ConstantTexture(Vec3(7, 7, 7)));
    l.push_back(new Cube(Vec3(0.4, 6, 1.0), 2.5, light));

    Camera cam(Vec3(4, 6, 15), Vec3(0, 1, 0), Vec3(0, 1, 0), 20, 16.0 / 9.0, 0.0, 15);
    SkyBox *skb = new BlackSkyBox();
    return Scene(l, cam, skb);
}

Scene dark2()
{
    std::vector<Object *> l;

    Material *light = new DiffuseLight(new ConstantTexture(Vec3(7, 7, 7)));
    l.push_back(new Square(Vec3(-150, 550, 412), Vec3(150, 550, 412), Vec3(150, 550, 147), Vec3(-150, 550, 147), light));

    int nx, ny, nn;
    unsigned char *ground_tex = stbi_load(TEXTURE_MARBLE_MAP, &nx, &ny, &nn, 0);
    Material *ground = new Lambertian(new ImageTexture(ground_tex, nx, ny));
    int nb = 20;
    for (int i = 0; i < nb; i++)
    {
        for (int j = 0; j < nb; j++)
        {
            float w = 100;
            l.push_back(new Cube(Vec3(-1000 + i * w + w / 2, w * (drand48() + 0.01) / 2, -1000 + j * w + w / 2), 100, ground));
        }
    }

    unsigned char *tex_data = stbi_load(TEXTURE_MARS_MAP, &nx, &ny, &nn, 0);
    Material *mat = new Lambertian(new ImageTexture(tex_data, nx, ny));
    l.push_back(new Sphere(Vec3(-50, 400, 70), 70, mat));

    unsigned char *tex_data2 = stbi_load(TEXTURE_JUPITER_MAP, &nx, &ny, &nn, 0);
    Material *mat2 = new Lambertian(new ImageTexture(tex_data2, nx, ny));
    l.push_back(new Sphere(Vec3(210, 270, 230), 80, mat2));

    Texture *pertext = new NoiseTexture(0.1);
    l.push_back(new Sphere(Vec3(130, 450, 100), 80, new Lambertian(pertext)));

    l.push_back(new Sphere(Vec3(50, 195, 220), 100, new Dielectric(1.5)));
    l.push_back(new Sphere(Vec3(-190, 250, 145), 80, new Metal(Vec3(0.8, 0.8, 0.9), 0.1)));

    l.push_back(new Square(Vec3(310, 500, 0), Vec3(310, 500, 600), Vec3(310, 140, 600), Vec3(310, 140, 0), new Metal(Vec3(0.7, 0.6, 0.5), 0.0)));

    Camera cam(Vec3(-200, 280, 1200), Vec3(170, 320, 0), Vec3(0, 1, 0), 33, 16.0 / 9.0, 0.0, 15);
    SkyBox *skb = new BlackSkyBox();
    return Scene(l, cam, skb);
}

#endif /* SCENES_H */