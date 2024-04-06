#include "ui.h"
#include "assets.h"
#include "game.h"
#include "seed_packets.h"
#include <raylib.h>
#include <stdio.h>


ButtonOptions defaultButtonOptions = {
    LIGHTGRAY,
    BLACK,
    4,
    32,
    2,
    2,
    true
};

TextButton CreateMenuTextButton(char *text, Color colour)
{
    TextOptions tOpt = {
        .font = smallFont,
        .size = 40,
        .shadowOffset = 2,
        .colour = WHITE
    };

    ButtonOptions bOpt = defaultButtonOptions;
    bOpt.centered = true;
    bOpt.shadowOffset = 4;
    bOpt.colour = colour;

    return (TextButton){.text=text, .textOptions=tOpt, .buttonOptions=bOpt};
}

bool UpdateDrawTextButton(TextButton *button, int x, int y)
{
    // TODO: Consider making centering/minWidth code clearer
    Vector2 textSize = MeasureTextEx(button->textOptions.font, button->text, button->textOptions.size, 0);
    int width = textSize.x + button->buttonOptions.paddingX*2;

    if (width < button->buttonOptions.minWidth) {
        // Set padding so width == minWidth
        int diff = button->buttonOptions.minWidth - width;
        button->buttonOptions.paddingX = diff;
    }

    if (button->buttonOptions.centered) {
        int centeredTextX = GetCenteredTextX(button->textOptions.font, button->textOptions.size, button->text, 0, virtualScreenWidth);
        x = centeredTextX - button->buttonOptions.paddingX/2;
    }

    Rectangle buttonRect = {
        x, y,
        textSize.x+button->buttonOptions.paddingX, textSize.y+button->buttonOptions.paddingY
    };

    Color modifiedButtonColour = button->buttonOptions.colour;
    float colourMult = 1.0f;

    bool cursorInside = CheckCollisionPointRec(GetMousePosVirtual(), buttonRect);
    if (cursorInside) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !draggingSeedPacket) {
            button->down = true;
        }

        colourMult = 0.75f;

        button->buttonOptions.shadowOffset /= 2;
    }

    if (button->down) {
        colourMult = 0.5f;
    }

    modifiedButtonColour.r *= colourMult;
    modifiedButtonColour.g *= colourMult;
    modifiedButtonColour.b *= colourMult;

    bool btnTriggered = IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && button->down && !draggingSeedPacket;
    if (btnTriggered) {
        PlaySound(peaShootSound);
        button->down = false;
    }

    DrawBorderedRectangleWithShadow(
        buttonRect,
        button->buttonOptions.shadowOffset,
        button->buttonOptions.outlineThickness,
        modifiedButtonColour,
        button->buttonOptions.outlineColour
    );

    DrawTextWithShadow(button->textOptions.font, button->text,
        x+button->buttonOptions.paddingX/2, y+button->buttonOptions.paddingY/2,
        button->textOptions.size, button->textOptions.shadowOffset, button->textOptions.colour
    );

    return btnTriggered;
}

int GetCenteredTextX(Font font, int size, const char *text, int startX, int endX)
{
    int textHalfWidth = MeasureTextEx(font, text, size, 0).x/2;

    return startX+(endX-startX)/2-textHalfWidth;
}

void DrawTextWithShadow(Font font, const char *text, int x, int y, float fontSize, float shadowOffset, Color tint)
{
    const Color shadowColour = {0, 0, 0, 150};
    DrawTextEx(font, text, (Vector2){x+shadowOffset, y+shadowOffset}, fontSize, 0, shadowColour);
    DrawTextEx(font, text, (Vector2){x, y}, fontSize, 0, tint);
}

void DrawCenteredTextWithShadow(Font font, const char *text, int y, float fontSize, float shadowOffset, Color tint)
{
    DrawTextWithShadow(font, text, GetCenteredTextX(font, fontSize, text, 0, virtualScreenWidth), y, fontSize, shadowOffset, tint);
}

void DrawBorderedRectangle(Rectangle rect, int outlineThickness, Color colour, Color borderColour)
{
    Rectangle border = rect;
    border.x -= outlineThickness;
    border.y -= outlineThickness;
    border.width += outlineThickness*2;
    border.height += outlineThickness*2;

    DrawRectangleRec(border, borderColour);
    DrawRectangleRec(rect, colour);
}

void DrawBorderedRectangleWithShadow(Rectangle rect, int shadowOffset, int outlineThickness, Color colour, Color borderColour)
{
    Rectangle shadow = rect;
    shadow.x += shadowOffset;
    shadow.y += shadowOffset;

    DrawRectangleRec(shadow, (Color){0, 0, 0, 100});
    DrawBorderedRectangle(rect, outlineThickness, colour, borderColour);
}
