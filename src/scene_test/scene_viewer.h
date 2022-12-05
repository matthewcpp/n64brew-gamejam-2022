#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"
#include "framework64/util/bump_allocator.h"
#include "fly_camera.h"

typedef struct {
    fw64Engine* engine;
    fw64Scene* scene;
    fw64Font* font;
    fw64FlyCamera fly_cam;
    fw64BumpAllocator allocator;
} fw64SceneViewer;

void fw64_scene_viewer_init(fw64SceneViewer* viewer, fw64Engine* engine, fw64Font* font, uint32_t bump_allocator_size);

int fw64_scene_viewer_load(fw64SceneViewer* viewer, int asset_id);
void fw64_scene_viewer_unload(fw64SceneViewer* viewer);
void fw64_scene_viewer_update(fw64SceneViewer* viewer);
void fw64_scene_viewer_draw(fw64SceneViewer* viewer);