#include "ui.h"
#include "game.h"
#include "seed_packets.h"


ButtonOptions defaultButtonOptions = {
    LIGHTGRAY,
    BLACK,
    4,
    16,
    8,
    2,
    true
};

// TODO: Maybe make it so you can't click outside the button, then move into the button and let go to click
// TODO: clean this up
bool TextButton(ButtonOptions buttonOptions, TextOptions textOptions, char *text, int x, int y)
{
    Vector2 textSize = MeasureTextEx(textOptions.font, text, textOptions.size, 0);
    int width = textSize.x + buttonOptions.paddingX*2;

    if (width < buttonOptions.minWidth) {
        int diff = buttonOptions.minWidth - width;
        buttonOptions.paddingX = diff;
    }

    if (buttonOptions.centered) {
        int textCenterX = GetCenteredTextX(textOptions.font, textOptions.size, text);
        x = textCenterX - buttonOptions.paddingX/2;
    }

    Rectangle button = {
        x, y,
        textSize.x+buttonOptions.paddingX, textSize.y+buttonOptions.paddingY
    };



    Rectangle shadow = button;
    Rectangle outline = button;

    outline.x -= buttonOptions.outlineThickness;
    outline.y -= buttonOptions.outlineThickness;
    outline.width += buttonOptions.outlineThickness*2;
    outline.height += buttonOptions.outlineThickness*2;

    bool pressed = false;

    if (CheckCollisionPointRec(GetMousePosVirtual(), button)) {
        // TODO: clean this up
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !draggingSeedPacket) {
            buttonOptions.colour.r *= 0.5f;
            buttonOptions.colour.g *= 0.5f;
            buttonOptions.colour.b *= 0.5f;
        } else {
            buttonOptions.colour.r *= 0.75f;
            buttonOptions.colour.g *= 0.75f;
            buttonOptions.colour.b *= 0.75f;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !draggingSeedPacket) {
            pressed = true;
        }

        shadow.x += (float)buttonOptions.shadowOffset/2;
        shadow.y += (float)buttonOptions.shadowOffset/2;
    } else {
        shadow.x += buttonOptions.shadowOffset;
        shadow.y += buttonOptions.shadowOffset;
    }

    DrawRectangleRec(shadow, (Color){0, 0, 0, 100});
    DrawRectangleRec(outline, BLACK);
    DrawRectangleRec(button, buttonOptions.colour);

    DrawTextWithShadow(textOptions.font, text,
        x+buttonOptions.paddingX/2, y+buttonOptions.paddingY/2,
        textOptions.size, textOptions.shadowOffset, textOptions.colour
    );

    return pressed;
}

int GetCenteredTextX(Font font, int size, char *text)
{
    int textHalfWidth = MeasureTextEx(font, text, size, 0).x/2;

    return virtualScreenWidth/2-textHalfWidth;
}

void DrawTextWithShadow(Font font, const char *text, int x, int y, float fontSize, float shadowOffset, Color tint)
{
    const Color shadowColour = {0, 0, 0, 150};
    DrawTextEx(font, text, (Vector2){x+shadowOffset, y+shadowOffset}, fontSize, 0, shadowColour);
    DrawTextEx(font, text, (Vector2){x, y}, fontSize, 0, tint);
}
