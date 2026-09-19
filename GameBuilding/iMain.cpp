#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

#ifdef UNICODE
#undef UNICODE
#endif
#ifdef _UNICODE
#undef _UNICODE
#endif

#include "iGraphics.h"
#include "GameUtility.h"
#include "Audio.h"
#include "Player.h"
#include "Enemy.h"
#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Level4.h"

bool escKeyWasDown = false;
int bgImage;
int bgImageLv1;
// NOTE: bgImageLv2 is NOT declared here. It lives in Level2.h, which also
// loads it inside loadLevel2Images(). Declaring it again in this file is what
// caused: error C2371: 'bgImageLv2' : redefinition; different basic types

void handleEscPress()
{
	// While two characters are talking the layer is already frozen, so ESC
	// skips the rest of what they have to say instead of opening the pause
	// menu. That is what makes a long conversation bearable on a retry.
	if (dialogueActive())
	{
		dialogueSkipAll();
		return;
	}

	if (gameState == STATE_LEVEL1 || gameState == STATE_LEVEL2 ||
		gameState == STATE_LEVEL3 || gameState == STATE_LEVEL4)
	{
		stateBeforePause = gameState;
		gameState = STATE_PAUSED;
	}
	else if (gameState == STATE_PAUSED)
	{
		gameState = stateBeforePause;
	}
	else if (gameState == STATE_INSTRUCTIONS)
	{
		gameState = STATE_MENU;
	}
	else if (gameState == STATE_STORY)
	{
		gameState = STATE_MENU;  // ESC backs out of the story, SKIP goes in
	}
}

void allImageLoad()
{
	// Ensure the 'Images' folder exists in your project directory
	// and contains a valid 'menu.png' file.
	static bool alreadyLoaded = false;
	if (alreadyLoaded) return;
	alreadyLoaded = true;

	bgImage = iLoadImage("Images/menu.png");
	// TO CHANGE THE LEVEL 1 BACKGROUND: swap this file name.
	bgImageLv1 = iLoadImageIfExists("Images/level1bg.png");
	if (bgImageLv1 == 0) bgImageLv1 = iLoadImage("Images/gameplay.png");

	// Images/CharacterImages/Player.png
	loadPlayerImages();

	// Images/Enime/Enime1.png  +  Images/Enime/Enimy2.png (the level 2 boss)
	loadEnemyImages();

	// Images/bglevel2.png  (the level 2 background)
	loadLevel2Images();

	// Images/level3BG.png  (the level 3 background)
	loadLevel3Images();

	// Images/Enime/Level 4/level4BG.png  (the level 4 background)
	loadLevel4Images();

	// the ornate health bars, the four result screens, and the alphabet
	loadHudImages();
	loadScreenArt();
	loadGameFont();
	loadButtonArt();        // Images/stausBar.png, if it is there
	loadStoryImages();      // Images/GameStory/1.png, 2.png, ... in order
	loadTalkBoxes();        // the three speech boxes the conversations use

	// Music/MenuMusic, Music/Level12Music, Music/WardenMusic, Music/Level3Music
	// plus the hit sounds Music/playerHit.wav and Music/enemyHit.wav
	musicInit();
}


// ---------------------------------------------------------------------------
//  WHICH TRACK SHOULD BE PLAYING RIGHT NOW
//
//  Menu, instructions and the end screens use the menu theme. Level 1 and the
//  opening of level 2 use the level theme. The moment the Warden is announced
//  the Warden theme takes over and stays until you leave the level.
// ---------------------------------------------------------------------------
int desiredMusicTrack()
{
	GameState st = (gameState == STATE_PAUSED) ? stateBeforePause : gameState;

	if (st == STATE_LEVEL2)
	{
		bool wardenIsHere = (level2Phase == L2_PHASE_BOSS_INTRO)
		                 || (level2Phase == L2_PHASE_BOSS_TALK)
		                 || (level2Phase == L2_PHASE_BOSS)
		                 || (level2Phase == L2_PHASE_DONE);
		return wardenIsHere ? MUSIC_WARDEN : MUSIC_LEVEL;
	}
	if (st == STATE_LEVEL1) return MUSIC_LEVEL;

	// Layer 3 has its own theme (Music/Level3Music.wav). If that file is
	// missing it falls back to the layer 1 track so the level is never silent.
	if (st == STATE_LEVEL3)
		return musicHasTrack(MUSIC_LEVEL3) ? MUSIC_LEVEL3 : MUSIC_LEVEL;

	// Layer 4 has its own theme (Music/Level4Music.wav).
	if (st == STATE_LEVEL4)
		return musicHasTrack(MUSIC_LEVEL4) ? MUSIC_LEVEL4 : MUSIC_WARDEN;

	// LAYER 1 CLEARED keeps the level theme playing. Layers 1 and 2 share one
	// track (Music/Level12Music.wav), and musicPlay does nothing when asked
	// for the track that is already playing - so holding it here means layer 2
	// picks the music up exactly where layer 1 left off, instead of cutting to
	// the menu theme and then starting the level theme again from the top.
	//
	// It has to be done this way round. The music is played with PlaySound,
	// which cannot say where it is or start from a given point, so the only
	// way not to restart a track is not to stop it.
	if (st == STATE_LEVEL_COMPLETE) return MUSIC_LEVEL;

	return MUSIC_MENU;      // menu, instructions, game over, every end screen
}

void iDraw()
{
	// Guarantee image loading happens only once, strictly after 
	// the iGraphics/OpenGL context is fully initialized.
	static bool imagesLoaded = false;
	if (!imagesLoaded)
	{
		allImageLoad();
		imagesLoaded = true;
	}

	// Stretch the fixed 1000x600 world onto whatever size the window is,
	// keeping its shape. Everything below draws in world coordinates.
	applyLetterbox();

	switch (gameState)
	{
	case STATE_MENU:           drawMenu();               break;
	case STATE_INSTRUCTIONS:   drawInstructions();       break;
	case STATE_LEVEL1:         drawLevel1();             break;
	case STATE_LEVEL2:         drawLevel2();             break;
	case STATE_LEVEL3:         drawLevel3();             break;
	case STATE_LEVEL4:         drawLevel4();             break;
	case STATE_PAUSED:
		if      (stateBeforePause == STATE_LEVEL4) drawLevel4();
		else if (stateBeforePause == STATE_LEVEL3) drawLevel3();
		else if (stateBeforePause == STATE_LEVEL2) drawLevel2();
		else                                       drawLevel1();
		drawPauseOverlay();
		break;
	case STATE_GAMEOVER:        drawGameOverScreen();        break;
	case STATE_LEVEL_COMPLETE:  drawLevelCompleteScreen();   break;
	case STATE_GAME_COMPLETE:   drawGameCompleteScreen();    break;
	case STATE_LEVEL3_COMPLETE: drawLevel3CompleteScreen();  break;
	case STATE_LEVEL4_COMPLETE: drawLevel4CompleteScreen();  break;
	case STATE_STORY:           drawStory();                 break;
	}
}

void fixedUpdate()
{
	bool escNow = isKeyPressed(27) != 0; // 27 = ESC
	if (escNow && !escKeyWasDown) handleEscPress();
	escKeyWasDown = escNow;

	musicPlay(desiredMusicTrack());   // does nothing if it is already playing
	musicTick();                      // restarts a track that ran to the end

	if (hitPauseTimer > 0)
	{
		hitPauseTimer--;
		return;
	}

	// count the time spent actually fighting, for the result screens
	if (gameState == STATE_LEVEL1 || gameState == STATE_LEVEL2 ||
		gameState == STATE_LEVEL3 || gameState == STATE_LEVEL4)
		runTicks++;

	switch (gameState)
	{
	case STATE_MENU:   updateMenuAmbient(); break;
	case STATE_STORY:  updateStory(); storyKeys(); break;
	case STATE_LEVEL1: updateLevel1();      break;
	case STATE_LEVEL2: updateLevel2();      break;
	case STATE_LEVEL3: updateLevel3();      break;
	case STATE_LEVEL4: updateLevel4();      break;
	default:            break;
	}
}

void updateHoverForState(int mx, int my)
{
	switch (gameState)
	{
	case STATE_MENU:           updateMenuHover(mx, my);          break;
	case STATE_INSTRUCTIONS:   updateInstructionsHover(mx, my);  break;
	case STATE_PAUSED:         updatePauseHover(mx, my);         break;
	case STATE_GAMEOVER:       updateGameOverHover(mx, my);      break;
	case STATE_LEVEL_COMPLETE:  updateLevelCompleteHover(mx, my);  break;
	case STATE_GAME_COMPLETE:   updateGameCompleteHover(mx, my);   break;
	case STATE_LEVEL3_COMPLETE: updateLevel3CompleteHover(mx, my); break;
	case STATE_LEVEL4_COMPLETE: updateLevel4CompleteHover(mx, my); break;
	case STATE_STORY:          updateStoryHover(mx, my);       break;
	default: break;
	}
}

// Every mouse position arrives in REAL screen pixels and has to be brought
// back into the 1000x600 world first, or the buttons sit somewhere other than
// where the cursor is.
void iMouseMove(int mx, int my)
{
	int wx, wy;
	mouseToWorld(mx, my, wx, wy);
	updateHoverForState(wx, wy);
}

void iPassiveMouseMove(int mx, int my)
{
	int wx, wy;
	mouseToWorld(mx, my, wx, wy);
	updateHoverForState(wx, wy);
}

void iMouse(int button, int state, int inX, int inY)
{
	int mx, my;
	mouseToWorld(inX, inY, mx, my);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		switch (gameState)
		{
		case STATE_MENU:           handleMenuClick(mx, my);          break;
		case STATE_INSTRUCTIONS:   handleInstructionsClick(mx, my);  break;
		case STATE_PAUSED:         handlePauseClick(mx, my);         break;
		case STATE_GAMEOVER:       handleGameOverClick(mx, my);      break;
		case STATE_LEVEL_COMPLETE:  handleLevelCompleteClick(mx, my);  break;
		case STATE_GAME_COMPLETE:   handleGameCompleteClick(mx, my);   break;
		case STATE_LEVEL3_COMPLETE: handleLevel3CompleteClick(mx, my); break;
		case STATE_LEVEL4_COMPLETE: handleLevel4CompleteClick(mx, my); break;
		case STATE_STORY:          handleStoryClick(mx, my);       break;
		case STATE_LEVEL1:
		case STATE_LEVEL2:
		case STATE_LEVEL3:
		case STATE_LEVEL4:
			// clicking during a conversation moves it along; a click at any
			// other time in a layer does nothing, exactly as before
			if (dialogueActive()) dialogueAdvance();
			break;
		default: break;
		}
	}
}

int main()
{
	srand((unsigned int)time(0));

	// iInitialize contains the main loop and will block subsequent code.
	// allImageLoad() and iStart() have been removed from here.
	iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Vengeance from the Abyss");

	// The game is played FULLSCREEN. The picture keeps its 1000x600 shape and
	// is centred, so nothing is stretched out of proportion; applyLetterbox in
	// iDraw and mouseToWorld in the mouse handlers keep drawing and clicking
	// in agreement. Alt+F4 closes it.
	glutFullScreen();

	allImageLoad();

	iStart();

	return 0;
}