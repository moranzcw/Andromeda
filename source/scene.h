//
//  scene.h
//  RayTracing
//
//  Created by moranzcw on 2018/12/28.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef scene_h
#define scene_h

#include <vector>
#include "hitable.h"
#include "camera.h"
#include "bvh.h"
#include "skybox.h"

class scene {
public:
    scene() {}
    scene(std::vector<hitable*> l, camera cam, skybox *b) :list(l), bvh_tree(l), cam(cam), skb(b) {};
    ~scene() {
        for(auto p:list)
            if(p) delete p;
        if(skb)
            delete skb;
    }

    bool add_object(hitable *obj){
        list.push_back(obj);
        reset_tree();
        return true;
    };

    bool reset_tree(){
        bvh_tree = bvh_node(list);
        return true;
    }

    std::vector<hitable *> list;
    bvh_node bvh_tree;
    camera cam;
    skybox *skb;
};

#endif /* scene_h */