#include "scene_viewer.h"

#include <stdio.h>

void fw64_scene_viewer_init(fw64SceneViewer* viewer, fw64Engine* engine, fw64Font* font, uint32_t bump_allocator_size) {
    viewer->engine = engine;
    viewer->font = font;
    viewer->scene = NULL;

    fw64_bump_allocator_init(&viewer->allocator, bump_allocator_size);
    fw64_fly_camera_init(&viewer->fly_cam, engine->input);
    viewer->fly_cam.movement_speed *= 2.0f;
}

void fw64_scene_viewer_unload(fw64SceneViewer* viewer) {
    if (!viewer->scene)
        return;

    fw64_scene_delete(viewer->engine->assets, viewer->scene, &viewer->allocator.interface);
    fw64_bump_allocator_reset(&viewer->allocator);
    viewer->scene = NULL;
}

int fw64_scene_viewer_load(fw64SceneViewer* viewer, int asset_id) {
    fw64_scene_viewer_unload(viewer);

    viewer->scene = fw64_scene_load(viewer->engine->assets, asset_id, &viewer->allocator.interface);

    if (viewer->scene == NULL) {
        fw64_bump_allocator_reset(&viewer->allocator);
        return 0;
    }

    return 1;
}

void fw64_scene_viewer_update(fw64SceneViewer* viewer) {
    fw64_fly_camera_update(&viewer->fly_cam, viewer->engine->time->time_delta);
}

void fw64_scene_viewer_draw(fw64SceneViewer* viewer) {
    fw64Renderer* renderer = viewer->engine->renderer;
    char buffer[32];
    sprintf(buffer, "Mem: %d/%d", fw64_bump_allocator_committed(&viewer->allocator), viewer->allocator.size);

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(renderer, &viewer->fly_cam.camera);

    if (viewer->scene) {
        //fw64Frustum frustum;
        //fw64_camera_extract_frustum_planes(&viewer->fly_cam.camera, &frustum);
        //fw64_scene_draw_frustrum(viewer->scene, renderer, &frustum);
        fw64_scene_draw_all(viewer->scene, renderer);
    }
        

    fw64_renderer_draw_text(renderer, viewer->font, 10, 10, buffer);
    
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}