#include "menus.h"
#include "game.h"
#include "ui.h"
#include "zombie.h"
#include "sun.h"
#include <raylib.h>
#include <raymath.h>
#include "assets.h"
#include <stdio.h>

// Start menu
static TextButton startButton;
static TextButton configButton;
static TextButton quitButton;

// General
static TextButton backButton;

// Config menu
static TextButton rainButton;
static TextButton musicButton;
static TextButton fullscreenButton;

// Game over screen
static TextButton returnToStartButton;

// Pause menu
static TextButton resumeButton;
static TextButton gameConfigButton;
static TextButton debugOptionsButton;
static TextButton giveUpButton;

// Debug menu
static TextButton debugSpawningButton;
static TextButton giveSunsButton;

void CreateAllButtons(void)
{
    // Start menu
    startButton = CreateMenuTextButton("Start Game!", GREEN);
    configButton = CreateMenuTextButton("Game Config", LIGHTGRAY);
    quitButton = CreateMenuTextButton("Quit Game", RED);

    // Config menu
    rainButton = CreateMenuTextButton("", (Color){0, 150, 200, 255});
    rainButton.buttonOptions.minWidth = 250;

    musicButton = CreateMenuTextButton("", BROWN);
    musicButton.buttonOptions.minWidth = 250;

    fullscreenButton = CreateMenuTextButton("", DARKPURPLE);
    fullscreenButton.buttonOptions.minWidth = 250;
    
    // General
    backButton = CreateMenuTextButton("<-- Back", LIGHTGRAY);
    backButton.buttonOptions.minWidth = 250;

    // Game over screen
    returnToStartButton = CreateMenuTextButton("Return to Start", LIGHTGRAY);

    // Pause menu
    resumeButton = CreateMenuTextButton("Resume", GREEN);
    gameConfigButton = CreateMenuTextButton("Game Config", LIGHTGRAY);
    debugOptionsButton = CreateMenuTextButton("Debug Options", DARKGREEN);
    giveUpButton = CreateMenuTextButton("Give Up", RED);

    // Debug menu
    debugSpawningButton = CreateMenuTextButton("", (Color){0, 150, 200, 255});
    giveSunsButton = CreateMenuTextButton("Give suns", YELLOW);
}

void DrawTitleText(char *text)
{
    const int titleFontSize = 50;
    DrawCenteredTextWithShadow(bigFont, text, 64, titleFontSize, 4, WHITE);
}

void UpdateDrawStart(void)
{
    int tfSize = 75;
    DrawCenteredTextWithShadow(bigFont, "Zombies", 32, tfSize, 4, WHITE);
    DrawCenteredTextWithShadow(bigFont, "In Your", 32+tfSize, tfSize/2.0f, 4, WHITE);
    DrawCenteredTextWithShadow(bigFont, "Garden", 32+tfSize*1.5f, tfSize, 4, WHITE);

    int y = virtualScreenHeight/2;
    int gap = 6;
    int height = GetButtonHeight(startButton.buttonOptions, startButton.textOptions) + gap;

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
    DrawCenteredTextWithShadow(smallFont, creditText, virtualScreenHeight-32, 25, 2, WHITE);

    char *versionText = VERSION_STRING;
    DrawTextWithShadow(smallFont, versionText, 8, 5, 25, 2, WHITE);
}

void UpdateDrawConfigMenu(GameConfig *config, GameScreen previousScreen)
{
    DrawTitleText("Game Config");

    int y = virtualScreenHeight/2-80;
    int gap = 6;
    int height = GetButtonHeight(backButton.buttonOptions, backButton.textOptions) + gap;

    if (UpdateDrawTextButton(&backButton, 0, y)) {
        ChangeGameScreen(previousScreen);
    }

    y += height;

    char rainText[32];
    sprintf(rainText, "Rain (%s)", (gameConfig.raining) ? "On" : "Off");
    rainButton.text = rainText;
    if (UpdateDrawTextButton(&rainButton, 0, y)) {
        gameConfig.raining = !gameConfig.raining;
    }

    y += height;

    char musicText[32];
    sprintf(musicText, "Music (%s)", (gameConfig.playingMusic) ? "On" : "Off");
    musicButton.text = musicText;
    if (UpdateDrawTextButton(&musicButton, 0, y)) {
        gameConfig.playingMusic = !gameConfig.playingMusic;
    }

    y += height;

    char fullScreenText[32];
    sprintf(fullScreenText, "Fullscreen (%s)", (IsWindowFullscreen()) ? "Yes" : "No");
    fullscreenButton.text = fullScreenText;
    if (UpdateDrawTextButton(&fullscreenButton, 0, y)) {
        ToggleFullscreen();
        gameConfig.fullscreen = IsWindowFullscreen();
    }
}

void UpdateDrawDebugMenu(GameScreen previousScreen)
{
    DrawTitleText("Debug Options");

    int y = virtualScreenHeight/2-80;
    int gap = 6;
    int height = GetButtonHeight(backButton.buttonOptions, backButton.textOptions) + gap;


    if (UpdateDrawTextButton(&backButton, 0, y)) {
        ChangeGameScreen(previousScreen);
    }

    y += height;

    char zombieDebugText[64];
    sprintf(zombieDebugText, "Right-click zombie spawning (%s)", (debugZombieSpawning) ? "On" : "Off");
    debugSpawningButton.text = zombieDebugText;
    if (UpdateDrawTextButton(&debugSpawningButton, 0, y)) {
        debugZombieSpawning = !debugZombieSpawning;
    }

    y += height;

    if (UpdateDrawTextButton(&giveSunsButton, 0, y)) {
        sunsCollectedCount += 1000*SUN_VALUE;
    }

    y += height;
}

void UpdateDrawPauseMenu(void)
{
    DrawTitleText("Paused");

    int y = virtualScreenHeight/2-80;
    int gap = 6;
    int height = GetButtonHeight(resumeButton.buttonOptions, resumeButton.textOptions) + gap;


    if (UpdateDrawTextButton(&resumeButton, 0, y)) {
        ChangeGameScreen(GAME_SCREEN_PLAYING);
    }

    y += height;
    if (UpdateDrawTextButton(&gameConfigButton, 0, y)) {
        ChangeGameScreen(GAME_SCREEN_CONFIG_MENU);
    }

    y += height;
    if (UpdateDrawTextButton(&debugOptionsButton, 0, y)) {
        ChangeGameScreen(GAME_SCREEN_DEBUG_MENU);
    }

    y += height;
    if (UpdateDrawTextButton(&giveUpButton, 0, y)) {
        ChangeGameScreen(GAME_SCREEN_START);
    }
}

void UpdateDrawGameOver(void)
{
    DrawCenteredTextWithShadow(bigFont, "YOU DIED!", 32, 50, 4, WHITE);
    DrawCenteredTextWithShadow(bigFont, "GAME OVER", 80, 50, 4, WHITE);

    char killCountText[32];
    sprintf(killCountText, "You killed %i zombies!", zombiesKilledCount);
    DrawCenteredTextWithShadow(smallFont, killCountText, virtualScreenHeight/2-80, 40, 2, WHITE);

    if (UpdateDrawTextButton(&returnToStartButton, 0, virtualScreenHeight/2)) {
        ChangeGameScreen(GAME_SCREEN_START);
    }
}
