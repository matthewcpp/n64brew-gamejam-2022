#include "level.h"

#include <string.h>

void fw64_level_init(fw64Level* level) {
    memset(level, 0, sizeof(fw64Level));
}

void fw64_level_add_dyanmic_node(fw64Level* level, fw64Node* node) {
    if (level->dynamic_node_count == LEVEL_DYNAMIC_NODE_SIZE)
        return;

    level->dynamic_nodes[level->dynamic_node_count++] = node;
}

void fw64_level_remove_dynamic_node(fw64Level* level, fw64Node* node) {
    for (int i = 0; i < level->dynamic_node_count; i++) {
        if (level->dynamic_nodes[i] == node) {
            level->dynamic_node_count -= 1;
            level->dynamic_nodes[i] = level->dynamic_nodes[level->dynamic_node_count];
            
            return;
        }
    }
}

uint32_t fw64_level_get_dynamic_node_count(fw64Level* level) {
    return level->dynamic_node_count;
}

fw64Node* fw64_level_get_dynamic_node(fw64Level* level, uint32_t index) {
    return level->dynamic_nodes[index];
}