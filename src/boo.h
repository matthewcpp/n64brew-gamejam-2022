#pragma once

#include "player.h"
#include "framework64/node.h"

typedef enum {
    BOO_WAITING,
    BOO_APPEARING
} BooState;

typedef struct {
    fw64Engine* engine;
    Player* player;
    fw64Node* node;
    fw64Mesh* mesh;
    fw64Scene* scene;
    BooState state;
} Boo;

void boo_init(Boo* boo, fw64Engine* engine, Player* player, fw64Scene* scene);
void boo_update(Boo* boo);

