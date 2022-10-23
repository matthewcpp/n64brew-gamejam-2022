#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"
#include "framework64/animation_data.h"

typedef struct {
    fw64Engine* engine;
    fw64Node* node;
    fw64Mesh* mesh;
    fw64AnimationData* animation_data;
    fw64AnimationController animation_controller;
    fw64Allocator* allocator;
} Catherine;

void catherine_init(Catherine* catherine, fw64Engine* engine, fw64Node* node, fw64Allocator* allocator);
void catherine_update(Catherine* catherine);
void catherine_draw(Catherine* catherine);
void catherine_uninit(Catherine* catherine);