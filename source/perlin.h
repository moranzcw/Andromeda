//
//  perlin.h
//  RayTracing
//
//  Created by moranzcw on 2018/12/30.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef PERLIN_H
#define PERLIN_H

#include "vec3.h"

inline float perlin_interp(Vec3 c[2][2][2], float u, float v, float w);

class Perlin
{
  public:
    float noise(const Vec3 &p) const
    {
        float u = p.x() - floor(p.x());
        float v = p.y() - floor(p.y());
        float w = p.z() - floor(p.z());
        int i = floor(p.x());
        int j = floor(p.y());
        int k = floor(p.z());
        Vec3 c[2][2][2];
        for (int di = 0; di < 2; di++)
            for (int dj = 0; dj < 2; dj++)
                for (int dk = 0; dk < 2; dk++)
                    c[di][dj][dk] = ranvec[permX[(i + di) & 255] ^ permY[(j + dj) & 255] ^ permZ[(k + dk) & 255]];
        return perlin_interp(c, u, v, w);
    }
    float turb(const Vec3 &p, int depth = 7) const
    {
        float accum = 0;
        Vec3 temp_p = p;
        float weight = 1.0;
        for (int i = 0; i < depth; i++)
        {
            accum += weight * noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }
        return fabs(accum);
    }
    static Vec3 *ranvec;
    static int *permX;
    static int *permY;
    static int *permZ;
};

inline float perlin_interp(Vec3 c[2][2][2], float u, float v, float w)
{
    float uu = u * u * (3 - 2 * u);
    float vv = v * v * (3 - 2 * v);
    float ww = w * w * (3 - 2 * w);
    float accum = 0;
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            for (int k = 0; k < 2; k++)
            {
                Vec3 weight_v(u - i, v - j, w - k);
                accum += (i * uu + (1 - i) * (1 - uu)) *
                         (j * vv + (1 - j) * (1 - vv)) *
                         (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], weight_v);
            }
    return accum;
}

static Vec3 *perlin_generate()
{
    Vec3 *p = new Vec3[256];
    for (int i = 0; i < 256; ++i)
        p[i] = makeUnit(Vec3(-1 + 2 * drand48(), -1 + 2 * drand48(), -1 + 2 * drand48()));
    return p;
}

void permute(int *p, int n)
{
    for (int i = n - 1; i > 0; i--)
    {
        int target = int(drand48() * (i + 1));
        int tmp = p[i];
        p[i] = p[target];
        p[target] = tmp;
    }
    return;
}

static int *perlin_generate_perm()
{
    int *p = new int[256];
    for (int i = 0; i < 256; i++)
        p[i] = i;
    permute(p, 256);
    return p;
}

Vec3 *Perlin::ranvec = perlin_generate();
int *Perlin::permX = perlin_generate_perm();
int *Perlin::permY = perlin_generate_perm();
int *Perlin::permZ = perlin_generate_perm();

#endif /* PERLIN_H */
