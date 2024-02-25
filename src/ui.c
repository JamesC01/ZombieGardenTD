#include "ui.h"
#include "game.h"
#include "seed_packets.h"
#include <raylib.h>


ButtonOptions defaultButtonOptions = {
    LIGHTGRAY,
    BLACK,
    4,
    16,
    8,
    2,
    true
};

bool TextButton(ButtonOptions buttonOptions, TextOptions textOptions, char *text, int x, int y)
{
    // TODO: Consider making centering/minWidth code clearer
    Vector2 textSize = MeasureTextEx(textOptions.font, text, textOptions.size, 0);
    int width = textSize.x + buttonOptions.paddingX*2;

    if (width < buttonOptions.minWidth) {
        // Set padding so width == minWidth
        int diff = buttonOptions.minWidth - width;
        buttonOptions.paddingX = diff;
    }

    if (buttonOptions.centered) {
        int centeredTextX = GetCenteredTextX(textOptions.font, textOptions.size, text);
        x = centeredTextX - buttonOptions.paddingX/2;
    }

    Rectangle button = {
        x, y,
        textSize.x+buttonOptions.paddingX, textSize.y+buttonOptions.paddingY
    };

    bool btnClicked = false;

    bool cursorInside = CheckCollisionPointRec(GetMousePosVirtual(), button);
    if (cursorInside) {
        btnClicked = IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !draggingSeedPacket;

        bool btnHeldDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !draggingSeedPacket;

        float colourMult = (btnHeldDown) ? 0.5f : 0.75f;
        buttonOptions.colour.r *= colourMult;
        buttonOptions.colour.g *= colourMult;
        buttonOptions.colour.b *= colourMult;

        buttonOptions.shadowOffset /= 2;
    }

    Rectangle shadow = button;
    shadow.x += buttonOptions.shadowOffset;
    shadow.y += buttonOptions.shadowOffset;

    Rectangle outline = button;
    outline.x -= buttonOptions.outlineThickness;
    outline.y -= buttonOptions.outlineThickness;
    outline.width += buttonOptions.outlineThickness*2;
    outline.height += buttonOptions.outlineThickness*2;

    DrawRectangleRec(shadow, (Color){0, 0, 0, 100});
    DrawRectangleRec(outline, buttonOptions.outlineColour);
    DrawRectangleRec(button, buttonOptions.colour);

    DrawTextWithShadow(textOptions.font, text,
        x+buttonOptions.paddingX/2, y+buttonOptions.paddingY/2,
        textOptions.size, textOptions.shadowOffset, textOptions.colour
    );

    return btnClicked;
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
