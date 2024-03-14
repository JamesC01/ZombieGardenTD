#include "menus.h"
#include "game.h"
#include "ui.h"
#include "zombie.h"
#include "sun.h"
#include <raylib.h>
#include <raymath.h>
#include "assets.h"
#include <stdio.h>

void DrawTitleText(char *text)
{
    const int titleFontSize = 50;
    DrawTextWithShadow(bigFont, text, GetCenteredTextX(bigFont, titleFontSize, text, 0, virtualScreenWidth), 64, titleFontSize, 4, WHITE);
}

void UpdateDrawStart(void)
{
    // TODO: Clean this up using DrawTitleText()
    char *titleText1 = "Zombies";
    char *titleText2 = "In Your";
    char *titleText3 = "Garden";
    int tfSize = 75;
    DrawTextWithShadow(bigFont, titleText1, GetCenteredTextX(bigFont, tfSize, titleText1, 0, virtualScreenWidth), 32, tfSize, 4, WHITE);
    DrawTextWithShadow(bigFont, titleText2, GetCenteredTextX(bigFont, tfSize/2, titleText2, 0, virtualScreenWidth), 32+tfSize, tfSize/2.0f, 4, WHITE);
    DrawTextWithShadow(bigFont, titleText3, GetCenteredTextX(bigFont, tfSize, titleText3, 0, virtualScreenWidth), 32+tfSize*1.5f, tfSize, 4, WHITE);


    TextOptions tOpt = {
        .font = smallFont,
        .size = 40,
        .shadowOffset = 2,
        .colour = WHITE
    };

    ButtonOptions bOpt = defaultButtonOptions;
    bOpt.centered = true;
    bOpt.shadowOffset = 4;

    // Buttons
    TextButton startButton = {.buttonOptions=bOpt, .textOptions=tOpt, .text="Start Game!"};
    startButton.buttonOptions.colour = GREEN;

    TextButton configButton = {.buttonOptions=bOpt, .textOptions=tOpt, .text="Game Config"};
    configButton.buttonOptions.colour = LIGHTGRAY;

    TextButton quitButton = {.buttonOptions=bOpt, .textOptions=tOpt, .text="Quit Game"};
    quitButton.buttonOptions.colour = RED;

    // Button positioning
    int y = virtualScreenHeight/2;
    int gap = 6;
    int height = GetButtonHeight(bOpt, tOpt) + gap;

    if (UpdateDrawTextButton(&startButton, 0, y)) {
        ChangeGameScreen(GAME_SCREEN_PLAYING);
        InitializeGame();
    }

    y += height;
    if (UpdateDrawTextButton(&configButton, 0, y)) {
        ChangeGameScreen(GAME_SCREEN_CONFIG_MENU);
    }

    y += height;
    if (UpdateDrawTextButton(&quitButton, 0, y)) {
        ChangeGameScreen(GAME_SCREEN_EXIT);
    }


    char *creditText = "(c) James Czekaj 2024";
    int cfSize = 25;
    DrawTextWithShadow(smallFont, creditText, GetCenteredTextX(smallFont, cfSize, creditText, 0, virtualScreenWidth), virtualScreenHeight-32, cfSize, 2, WHITE);

    char *versionText = VERSION_STRING;
    int vfSize = 25;
    DrawTextWithShadow(smallFont, versionText, 8, 5, cfSize, 2, WHITE);
}

void UpdateDrawConfigMenu(GameConfig *config, GameScreen previousScreen)
{
    DrawTitleText("Game Config");


    TextOptions tOpt = {
        .font = smallFont,
        .size = 40,
        .shadowOffset = 2,
        .colour = WHITE
    };

    ButtonOptions bOpt = defaultButtonOptions;
    bOpt.shadowOffset = 4;
    bOpt.centered = true;
    bOpt.minWidth = 250;

    int y = virtualScreenHeight/2-80;
    int gap = 6;
    int height = GetButtonHeight(bOpt, tOpt) + gap;

    bOpt.colour = LIGHTGRAY;
    const int btnShadow = 4;
    if (TextButton(bOpt, tOpt, "<-- Back", 0, y)) {
        ChangeGameScreen(previousScreen);
    }

    y += height;

    bOpt.colour = (Color){0, 150, 200, 255};
    char rainText[32];
    sprintf(rainText, "Rain (%s)", (gameConfig.raining) ? "On" : "Off");
    if (TextButton(bOpt, tOpt, rainText, 0, y)) {
        gameConfig.raining = !gameConfig.raining;
    }

    y += height;

    bOpt.colour = BROWN;
    char musicText[32];
    sprintf(musicText, "Music (%s)", (gameConfig.playingMusic) ? "On" : "Off");
    if (TextButton(bOpt, tOpt, musicText, 0, y)) {
        gameConfig.playingMusic = !gameConfig.playingMusic;
    }

    y += height;

    bOpt.colour = DARKPURPLE;
    char fullScreenText[32];
    sprintf(fullScreenText, "Fullscreen (%s)", (IsWindowFullscreen()) ? "Yes" : "No");
    if (TextButton(bOpt, tOpt, fullScreenText, 0, y)) {
        ToggleFullscreen();
        gameConfig.fullscreen = IsWindowFullscreen();
    }
}

void UpdateDrawDebugMenu(GameScreen previousScreen)
{
    DrawTitleText("Debug Options");

    TextOptions tOpt = {
        .font = smallFont,
        .size = 40,
        .shadowOffset = 2,
        .colour = WHITE
    };

    ButtonOptions bOpt = defaultButtonOptions;
    bOpt.shadowOffset = 4;
    bOpt.centered = true;
    bOpt.minWidth = 250;

    int y = virtualScreenHeight/2-80;
    int gap = 6;
    int height = GetButtonHeight(bOpt, tOpt) + gap;

    bOpt.colour = LIGHTGRAY;
    const int btnShadow = 4;
    if (TextButton(bOpt, tOpt, "<-- Back", 0, y)) {
        ChangeGameScreen(previousScreen);
    }

    y += height;

    bOpt.colour = (Color){0, 150, 200, 255};
    char zombieDebugText[64];
    sprintf(zombieDebugText, "Right-click zombie spawning (%s)", (debugZombieSpawning) ? "On" : "Off");
    if (TextButton(bOpt, tOpt, zombieDebugText, 0, y)) {
        debugZombieSpawning = !debugZombieSpawning;
    }

    y += height;

    bOpt.colour = YELLOW;
    if (TextButton(bOpt, tOpt, "Give suns", 0, y)) {
        sunsCollectedCount += 1000*SUN_VALUE;
    }

    y += height;
}

void UpdateDrawPauseMenu(void)
{
    DrawTitleText("Paused");

    TextOptions tOpt = {
        .font = smallFont,
        .size = 40,
        .shadowOffset = 2,
        .colour = WHITE
    };

    ButtonOptions bOpt = defaultButtonOptions;
    bOpt.shadowOffset = 4;
    bOpt.centered = true;
    bOpt.minWidth = 250;

    int y = virtualScreenHeight/2-80;
    int gap = 6;
    int height = GetButtonHeight(bOpt, tOpt) + gap;

    bOpt.colour = GREEN;
    if (TextButton(bOpt, tOpt, "Resume", 0, y)) {
        ChangeGameScreen(GAME_SCREEN_PLAYING);
    }

    y += height;
    bOpt.colour = LIGHTGRAY;
    if (TextButton(bOpt, tOpt, "Game Config", 0, y)) {
        ChangeGameScreen(GAME_SCREEN_CONFIG_MENU);
    }

    y += height;
    bOpt.colour = LIGHTGRAY;
    if (TextButton(bOpt, tOpt, "Debug Options", 0, y)) {
        ChangeGameScreen(GAME_SCREEN_DEBUG_MENU);
    }

    y += height;
    bOpt.colour = RED;
    if (TextButton(bOpt, tOpt, "Give Up", 0, y)) {
        ChangeGameScreen(GAME_SCREEN_START);
    }
}

void UpdateDrawGameOver(void)
{
    DrawTextWithShadow(bigFont, "YOU DIED!", GetCenteredTextX(bigFont, 50, "YOU DIED!", 0, virtualScreenWidth), 32, 50, 4, WHITE);
    DrawTextWithShadow(bigFont, "GAME OVER", GetCenteredTextX(bigFont, 50, "GAME OVER", 0, virtualScreenWidth), 80, 50, 4, WHITE);

    char killCountText[32];
    sprintf(killCountText, "You killed %i zombies!", zombiesKilledCount);
    DrawTextWithShadow(smallFont, killCountText, GetCenteredTextX(smallFont, 40, killCountText, 0, virtualScreenWidth), virtualScreenHeight/2-80, 40, 2, WHITE);

    TextOptions options = {
        .font = smallFont,
        .size = 40,
        .shadowOffset = 2,
        .colour = WHITE
    };

    ButtonOptions bOpt = defaultButtonOptions;
    bOpt.colour = LIGHTGRAY;
    bOpt.centered = true;

    if (TextButton(bOpt, options, "Return to Start", 0, virtualScreenHeight/2)) {
        ChangeGameScreen(GAME_SCREEN_START);
    }
}
