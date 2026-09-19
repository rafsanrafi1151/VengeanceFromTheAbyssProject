
#pragma once
#ifndef MENU_H
#define MENU_H

#include "GameUtility.h"

Button menuButtons[3] = {
    { (SCREEN_WIDTH - 300) / 2.0, 285, 300, 58, "PLAY", false },
    { (SCREEN_WIDTH - 300) / 2.0, 202, 300, 58, "INSTRUCTIONS", false },
    { (SCREEN_WIDTH - 300) / 2.0, 124, 300, 58, "EXIT", false }
};

Button instructionsButtons[1] = {
    { (SCREEN_WIDTH - 220) / 2.0, 60, 220, 50, "BACK", false }
};

int menuAmbientTick = 0;
extern int bgImage;

void updateMenuAmbient()
{
    menuAmbientTick++;
    uiTick++;              // drives the button glow
}


void drawMenuAmbience()
{
    iSetColor(14, 14, 14);
    for (int i = 0; i < 6; i++)
    {
        double t = menuAmbientTick * 0.01 + i * 1.3;
        double bx = 100 + i * 150 + 40 * sin(t);
        double by = 120 + 60 * cos(t * 0.7 + i);
        iFilledCircle(bx, by, 18 + 6 * sin(t * 1.7), 18);
    }
}

void drawMenu()
{
    iClear();

    iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImage);

    // a gentle darkening behind the buttons so the labels always read
    dimScreen(0.28);
    fillRectGradient(0, 0, SCREEN_WIDTH, 430, 6, 4, 10, 0.62, 6, 4, 10, 0.0);

    double cx = SCREEN_WIDTH / 2.0;

    // ---- title -----------------------------------------------------------
 /*   iSetColor(0, 0, 0);
    drawCenteredText(cx + 3, 517, (char*)"VENGEANCE FROM THE ABYSS",
                      GLUT_BITMAP_TIMES_ROMAN_24, 15);
    iSetColor(206, 42, 34);
    drawCenteredText(cx, 520, (char*)"VENGEANCE FROM THE ABYSS",
                      GLUT_BITMAP_TIMES_ROMAN_24, 15);

    fillRectAlpha(cx - 200, 508, 400, 2, 190, 50, 40, 0.8);

    iSetColor(150, 145, 150);
    drawCenteredText(cx, 484, (char*)"A blood oath sworn in the dark.",
                      GLUT_BITMAP_HELVETICA_18, 9); */

    for (int i = 0; i < 3; i++)
        drawButton(menuButtons[i]);

    iSetColor(105, 100, 105);
    drawCenteredText(cx, 40, (char*)"SPACE jump    K kick    ESC pause",
                      GLUT_BITMAP_8_BY_13, 7);
}

void drawInstructions()
{
    iClear();
    iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImage);
    dimScreen(0.72);
    iSetColor(190, 20, 20);
    drawCenteredText(SCREEN_WIDTH / 2.0, 540, (char*)"THE ABYSS GAUNTLET", GLUT_BITMAP_TIMES_ROMAN_24, 15);

    iSetColor(200, 200, 200);
    int y = 496;
    const char* lines[] = {
        "Falsely convicted, you are cast into the Abyss.",
        "Clear Layer 1, then face THE WARDEN in Layer 2.",
        "Layer 3 is the ROT and the GATE HOUND.",
        "Layer 4 is MEAFESTO alone - and he has two lives.",
        "Every layer starts you on full health.",
        "",
        "CONTROLS",
        "  A / D or Left / Right  -  Run (hold to build speed)",
        "  SPACE                  -  Jump  (tap = hop, hold = full jump)",
        "  SPACE in mid-air       -  Double jump",
        "  S  or  Down            -  Fast fall / slam back down",
        "  K                      -  Kick  (hits everyone in range)",
        "  ESC                    -  Pause",
        "",
        "Jump over a hostile and it cannot touch you.",
        "The Warden winds up before he swings - that is your window.",
    };
    for (int i = 0; i < 14; i++)
    {
        iText(SCREEN_WIDTH / 2.0 - 210, y, (char*)lines[i], GLUT_BITMAP_HELVETICA_18);
        y -= 26;
    }

    drawButton(instructionsButtons[0]);
}

void updateMenuHover(int mx, int my)
{
    for (int i = 0; i < 3; i++)
        menuButtons[i].hover = pointInButton(menuButtons[i], mx, my);
}

void updateInstructionsHover(int mx, int my)
{
    instructionsButtons[0].hover = pointInButton(instructionsButtons[0], mx, my);
}

// ===========================================================================
//
//                    * * *  T H E   S T O R Y  * * *
//
//  PLAY no longer drops you straight into layer 1. It shows the story first,
//  one page at a time, and then the game begins.
//
//  TO ADD A PAGE: drop another picture in Images/GameStory/ named with the
//  next number - 1.png, then 2.png, then 3.png and so on. They are shown in
//  that order. The loader stops at the first number that is missing, so there
//  is nothing to update here; STORY_MAX_PAGES is only an upper limit.
//
//  Any picture shape works. Each page is fitted inside the screen with its
//  proportions kept, so a wide comic page is never squashed to fit.
//
// ===========================================================================
const int STORY_MAX_PAGES = 12;

Sprite storyPages[STORY_MAX_PAGES];
int    storyPageCount = 0;    // how many were actually found
int    storyPage      = 0;    // which one is on screen
int    storyTick      = 0;    // free running, drives the prompt pulse
int    storyFade      = 0;    // counts down at the start of each page

const int STORY_FADE_TICKS = 18;   // the short fade as a page comes up

// The bottom 52 pixels are kept clear of the artwork for the page counter,
// the prompt and the SKIP button, so nothing is ever written across a panel.
const double STORY_UI_STRIP = 52.0;
const double STORY_SIDE_PAD = 12.0;

Button storyButtons[1] = {
    { SCREEN_WIDTH - 146.0, 6, 128, 40, "SKIP", false }
};

void loadStoryImages()
{
    storyPageCount = 0;
    for (int i = 0; i < STORY_MAX_PAGES; i++)
        storyPages[i] = emptySprite();

    for (int i = 0; i < STORY_MAX_PAGES; i++)
    {
        char path[96];
        sprintf(path, "Images/GameStory/%d.png", i + 1);
        Sprite s = loadSprite(path);

        if (s.tex == 0)
        {
            // try the same number as a .jpg before giving up on it
            sprintf(path, "Images/GameStory/%d.jpg", i + 1);
            s = loadSprite(path);
        }
        if (s.tex == 0) break;       // that number is missing: the story ends here

        storyPages[storyPageCount] = s;
        storyPageCount++;
    }
}

// PLAY calls this. With no pictures in Images/GameStory/ it simply starts the
// game, so a missing folder can never leave you stuck on a blank screen.
void startStory()
{
    if (storyPageCount <= 0)
    {
        startNewGameFromMenu();
        return;
    }

    storyPage = 0;
    storyTick = 0;
    storyFade = STORY_FADE_TICKS;
    gameState = STATE_STORY;
}

void storySkip()
{
    startNewGameFromMenu();
}

// Click, ENTER, SPACE or the right arrow: next page, or into the game.
void storyAdvance()
{
    if (storyPage + 1 < storyPageCount)
    {
        storyPage++;
        storyFade = STORY_FADE_TICKS;
    }
    else
    {
        startNewGameFromMenu();
    }
}

void storyBack()
{
    if (storyPage > 0)
    {
        storyPage--;
        storyFade = STORY_FADE_TICKS;
    }
}

void updateStory()
{
    storyTick++;
    uiTick++;                          // keeps the SKIP button breathing
    if (storyFade > 0) storyFade--;
}

void drawStory()
{
    iClear();

    if (storyPage < 0) storyPage = 0;
    if (storyPage >= storyPageCount) storyPage = storyPageCount - 1;

    const Sprite& page = storyPages[storyPage];

    // ---- fit the page in the space above the strip, proportions kept ----
    // A page wider than the box loses height, a taller one loses width, and
    // either way it is centred in the box. Nothing is ever stretched, so a
    // wide comic page and a tall portrait page both come out true.
    double boxX = STORY_SIDE_PAD;
    double boxY = STORY_UI_STRIP;
    double boxW = SCREEN_WIDTH  - STORY_SIDE_PAD * 2.0;
    double boxH = SCREEN_HEIGHT - STORY_UI_STRIP - STORY_SIDE_PAD;

    double pw = boxW;
    double ph = pw / page.aspect;
    if (ph > boxH)
    {
        ph = boxH;
        pw = ph * page.aspect;
    }
    double px = boxX + (boxW - pw) / 2.0;
    double py = boxY + (boxH - ph) / 2.0;

    // the page rises into place over the first few ticks
    double t     = 1.0 - (double)storyFade / (double)STORY_FADE_TICKS;  // 0 -> 1
    double alpha = 255.0 * (0.25 + 0.75 * t);
    double lift  = 10.0 * (1.0 - t);

    drawSpriteUV(page.tex, px, py - lift, pw, ph, false,
                  page.u0, page.v0, page.u1, page.v1,
                  255, 255, 255, alpha);

    // ---- the strip along the bottom -------------------------------------
    fillRectGradient(0, 0, SCREEN_WIDTH, STORY_UI_STRIP + 10,
                      4, 3, 5, 0.92, 4, 3, 5, 0.0);

    // ---- the page counter, bottom left ----------------------------------
    char counter[32];
    sprintf(counter, "%d / %d", storyPage + 1, storyPageCount);
    if (gameFontTex != 0)
        drawGameText(20, 17, counter, 21, 168, 156, 144);
    else
    { iSetColor(170, 160, 150); iText(20, 17, counter, GLUT_BITMAP_8_BY_13); }

    // ---- the prompt, bottom centre --------------------------------------
    double pulse = 0.5 + 0.5 * sin(storyTick * 0.08);
    const char* prompt = (storyPage + 1 < storyPageCount)
                          ? "CLICK OR PRESS ENTER FOR THE NEXT PAGE"
                          : "CLICK OR PRESS ENTER TO BEGIN";
    // (ESC goes back to the menu, SKIP jumps straight into the game)
    if (gameFontTex != 0)
        drawStatusText(SCREEN_WIDTH / 2.0 - 40, 17, prompt, 20,
                        150 + 90 * pulse, 140 + 80 * pulse, 128 + 60 * pulse);
    else
    { iSetColor(150 + 90 * pulse, 140 + 80 * pulse, 130);
      drawCenteredText(SCREEN_WIDTH / 2.0 - 40, 22, const_cast<char*>(prompt),
                        GLUT_BITMAP_HELVETICA_18, 9); }

    drawButton(storyButtons[0]);
}

void updateStoryHover(int mx, int my)
{
    storyButtons[0].hover = pointInButton(storyButtons[0], mx, my);
}

void handleStoryClick(int mx, int my)
{
    if (pointInButton(storyButtons[0], mx, my)) { storySkip(); return; }
    storyAdvance();
}

// ENTER or the right arrow moves on, the left arrow goes back, ESC returns to
// the menu. Deliberately NOT the space bar: space is the jump button, and a
// space still held down as the last page turns would make the hero jump the
// moment layer 1 starts.
//
// Called once per key press from fixedUpdate, never every tick, so one press
// never flips through the whole story.
bool storyKeyWasDown = false;

void storyKeys()
{
    bool nextHeld = (isKeyPressed(13) != 0)
                     || (isSpecialKeyPressed(GLUT_KEY_RIGHT) != 0);
    bool backHeld = (isSpecialKeyPressed(GLUT_KEY_LEFT) != 0);

    bool held = nextHeld || backHeld;
    if (held && !storyKeyWasDown)
    {
        if (nextHeld) storyAdvance();
        else          storyBack();
    }
    storyKeyWasDown = held;
}


void handleMenuClick(int mx, int my)
{
    // PLAY shows the story pages first; startStory falls straight through to
    // the game if Images/GameStory/ has nothing in it.
    if (pointInButton(menuButtons[0], mx, my))       startStory();
    else if (pointInButton(menuButtons[1], mx, my))  gameState = STATE_INSTRUCTIONS;
    else if (pointInButton(menuButtons[2], mx, my))  exit(0);
}

void handleInstructionsClick(int mx, int my)
{
    if (pointInButton(instructionsButtons[0], mx, my))
        gameState = STATE_MENU;
}

#endif 
