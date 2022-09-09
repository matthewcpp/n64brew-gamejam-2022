#include "trigger.h"

void trigger_box_init_from_node(TriggerBox* trigger_box, fw64Node* node) {
    trigger_box->trigger_func = NULL;
    trigger_box->trigger_func_arg = NULL;

    box_set_center_extents(&trigger_box->box, &node->transform.position, &node->transform.scale);
    trigger_box_set_func(trigger_box, NULL, NULL);
    trigger_box_set_target(trigger_box, NULL, 0);
}

void trigger_box_set_target(TriggerBox* trigger_box, fw64Node* target, int trigger_count) {
    trigger_box->target = target;
    trigger_box->target_in_trigger = 0;
    trigger_box->trigger_count = trigger_count;
}

void trigger_box_set_func(TriggerBox* trigger_box, TriggerFunc trigger_func, void* trigger_func_arg) {
    trigger_box->trigger_func = trigger_func;
    trigger_box->trigger_func_arg = trigger_func_arg;
}

void trigger_box_update(TriggerBox* trigger_box) {
    if (trigger_box->target == NULL)
        return;

    int was_in_box = trigger_box->target_in_trigger;
    trigger_box->target_in_trigger = box_contains_point(&trigger_box->box, &trigger_box->target->transform.position);
    int entered_trigger = !was_in_box && trigger_box->target_in_trigger;

    if (!trigger_box->trigger_func || !entered_trigger || trigger_box->trigger_count == 0)
        return;

    trigger_box->trigger_func(trigger_box, trigger_box->trigger_func_arg);
    trigger_box->trigger_count -= 1;
}