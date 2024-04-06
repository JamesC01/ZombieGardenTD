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

typedef struct {
    ButtonOptions buttonOptions;
    TextOptions textOptions;
    char *text;
    bool down;
} TextButton;

extern ButtonOptions defaultButtonOptions;

bool UpdateDrawTextButton(TextButton *button, int x, int y);

void DrawTextWithShadow(Font font, const char *text, int x, int y, float fontSize, float shadowOffset, Color tint);
void DrawCenteredTextWithShadow(Font font, const char *text, int y, float fontSize, float shadowOffset, Color tint);
void DrawBorderedRectangleWithShadow(Rectangle rect, int shadowOffset, int outlineThickness, Color colour, Color borderColour);
void DrawBorderedRectangle(Rectangle rect, int outlineThickness, Color colour, Color borderColour);
int GetCenteredTextX(Font font, int size, const char *text, int startX, int endX);
TextButton CreateMenuTextButton(char *text, Color colour);

#endif
