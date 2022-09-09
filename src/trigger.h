#pragma once

#include "framework64/audio.h"
#include "framework64/box.h"
#include "framework64/node.h"

typedef struct TriggerBox TriggerBox;

typedef void(*TriggerFunc)(struct TriggerBox*, void*);

struct TriggerBox{
    TriggerFunc trigger_func;
    void* trigger_func_arg;
    fw64Node* target;
    int target_in_trigger;
    Box box;
    int trigger_count;
};

/** Creates a box centered at the node's position with extens equal to it's scale. */
void trigger_box_init_from_node(TriggerBox* trigger_box, fw64Node* node);
void trigger_box_set_func(TriggerBox* trigger_box, TriggerFunc trigger_func, void* trigger_func_arg);
void trigger_box_set_target(TriggerBox* trigger_box, fw64Node* target, int trigger_count);
void trigger_box_update(TriggerBox* trigger_box);