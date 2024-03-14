#ifndef MENUS_H
#define MENUS_H

#include "game.h"
#include <raylib.h>

void UpdateDrawPauseMenu(void);
void UpdateDrawStart(void);
void UpdateDrawGameOver(void);
void UpdateDrawConfigMenu(GameConfig *config, GameScreen previousScreen);
void UpdateDrawDebugMenu(GameScreen previousScreen);

#endif
