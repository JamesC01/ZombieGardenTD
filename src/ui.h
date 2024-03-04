#ifndef UI_H
#define UI_H
#include <raylib.h>

typedef struct {
    Font font;
    int size;
    int shadowOffset;
    Color colour;
} TextOptions;

typedef struct {
    Color colour;
    Color outlineColour;
    int shadowOffset;
    int paddingX;
    int paddingY;
    int outlineThickness;
    bool centered;
    int minWidth;
} ButtonOptions;

extern ButtonOptions defaultButtonOptions;

bool TextButton(ButtonOptions buttonOptions, TextOptions textOptions, char *text, int x, int y);

void DrawTextWithShadow(Font font, const char *text, int x, int y, float fontSize, float shadowOffset, Color tint);
int GetCenteredTextX(Font font, int size, char *text, int startX, int endX);

#endif
