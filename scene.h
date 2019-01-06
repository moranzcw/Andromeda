//
//  scene.h
//  RayTracing
//
//  Created by moranzcw on 2018/12/28.
//  Copyright © 2018 moranzcw. All rights reserved.
//

#ifndef scene_h
#define scene_h

#include "hitable.h"
#include "camera.h"
#include "bvh.h"

class scene {
public:
    scene() {}
    scene(std::vector<hitable*> l, camera cam) :list(l), bvh_tree(l), cam(cam) {};
    ~scene() {
        for(auto p:list)
            if(p) delete p;
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
};

#endif /* scene_h */