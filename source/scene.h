//
//  scene.h
//  RayTracing
//
//  Created by moranzcw on 2018/12/28.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "object.h"
#include "camera.h"
#include "bvh.h"
#include "skybox.h"

class Scene
{
  public:
    Scene() {}
    Scene(std::vector<Object *> l, Camera cam, SkyBox *b) : list(l),
                                                            bvh_tree(l),
                                                            cam(cam),
                                                            skb(b){};
    ~Scene()
    {
        if (skb)
            delete skb;
    }

    bool add_object(Object *obj)
    {
        list.push_back(obj);
        reset_tree();
        return true;
    };

    bool reset_tree()
    {
        bvh_tree = BVHNode(list);
        return true;
    }

    std::vector<Object *> list;
    BVHNode bvh_tree;
    Camera cam;
    SkyBox *skb;
};

#endif /* SCENE_H */