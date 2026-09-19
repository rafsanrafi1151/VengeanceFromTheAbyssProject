#pragma once
#ifndef LEVELSETTINGS_H
#define LEVELSETTINGS_H

// ===========================================================================
//
//                    * * *  L E V E L   S E T T I N G S  * * *
//
//   THIS IS THE FILE TO EDIT WHEN YOU WANT TO CHANGE THE WAVES.
//   You do not have to touch any other file. Change a number here, press F5,
//   and the game plays differently.
//
//   Everything below is read once when a layer starts.
//
// ===========================================================================


// ---------------------------------------------------------------------------
//  WHAT ONE WAVE IS
//
//  A wave is just "how many of each enemy walk in together".
//    smallEnemies = the little enemy   (normalEnemeylevel1.png)
//    bigEnemies   = the big enemy      (level1Boss.png)
//    warspanEnemies  = WARSPAN, the layer 3 crawler  (Level 3/level3EnemyWarspan.png)
//  They all walk in from off screen at the same time.
// ---------------------------------------------------------------------------
struct Wave
{
    int smallEnemies;
    int bigEnemies;
    int warspanEnemies;
};
// NOTE: these are NOT called "small" and "big". Windows' own headers contain
// #define small char, so "int small;" turned into "int char;" and the whole
// file failed to compile. The names below just avoid that clash - the
// { 2, 1, 0 } lines just fill the three fields in order. Leaving a number off
// the end of a line is allowed and counts as a zero, so { 2, 1 } still means
// "2 small, 1 big, no warspan".


// ---------------------------------------------------------------------------
//  LEVEL 1 WAVES
//
//  ONE LINE = ONE WAVE. They run top to bottom.
//  When the last wave is cleared, layer 1 is finished.
//
//  TO ADD A WAVE:     add another  { small, big, warspan },  line.
//  TO REMOVE A WAVE:  delete a line.
//  TO MAKE IT HARDER: raise the numbers.
//
//  You do NOT have to update any count anywhere - the number of waves is
//  worked out from how many lines you wrote.
// ---------------------------------------------------------------------------
const Wave LEVEL1_WAVES[] =
{
    { 1, 1, 0 },   // wave 1  ->  1 small enemy + 1 big enemy
   // { 1, 1, 0 },  
	//{ 1, 1, 0 },
	//{ 1, 1, 0 },
	//{ 1, 1, 0 },// wave 2  ->  1 small enemy + 1 big enemy
};


// ---------------------------------------------------------------------------
//  LEVEL 2 WAVES  (the enemies BEFORE the Warden)
//
//  Same rules. When these waves are cleared, THE WARDEN arrives.
// ---------------------------------------------------------------------------
const Wave LEVEL2_WAVES[] =
{
    { 2, 1, 0 }, 
	//{ 2, 1, 0 },
	//{ 2, 1, 0 },
	//{ 2, 1, 0 },// wave 1  ->  2 small enemies + 1 big enemy
};


// ---------------------------------------------------------------------------
//  LEVEL 3 WAVES  (the enemies BEFORE the swamp thing)
//
//  TWO WAVES OF WARSPAN, the new layer 3 crawler. Both lines below ask for warspan
//  only, which is why the first two numbers are 0.
//
//  Same rules as every other table: add a line for another wave, change the
//  third number to send in more or fewer warspan. When these waves are cleared
//  the SWAMP THING rises, and after that the GATE HOUND.
// ---------------------------------------------------------------------------
const Wave LEVEL3_WAVES[] =
{
    { 0, 0, 2 },
//	{ 0, 0, 2 }, // wave 1  ->  2 warspan
  //  { 0, 0, 3 },   // wave 2  ->  3 warspan
};


// ---------------------------------------------------------------------------
//  WHICH LAYER THE "START" BUTTON BEGINS ON
//
//  Normally 1. Set it to 2, 3 or 4 while you are working on a later layer so
//  you do not have to play through the earlier ones every time you press F5.
//  SET IT BACK TO 1 BEFORE YOU HAND THE GAME IN.
// ---------------------------------------------------------------------------
const int START_LEVEL = 1;      // 1, 2, 3 or 4


// ---------------------------------------------------------------------------
//  HOW LONG THE GAME PAUSES BETWEEN WAVES  (62 ticks = about 1 second)
// ---------------------------------------------------------------------------
const int WAVE_CLEAR_DELAY_TICKS = 200;


// ---------------------------------------------------------------------------
//  THE HERO'S SPECIAL MOVE  --  the H key
//
//  It is a wide, heavy blow that hits EVERYTHING standing near you, on both
//  sides at once, and it is unlocked from this layer onwards.
// ---------------------------------------------------------------------------
const int SPECIAL_MOVE_FROM_LEVEL = 2;   // 2 = layer 2 up to the final layer


// ---------------------------------------------------------------------------
//  HOW MANY PICTURES ARE IN EACH ANIMATION SHEET
//
//  An animation sheet is one PNG holding several poses side by side in a
//  single row. The game slices it into this many equal pieces, so if a
//  character's animation looks squashed or shows two bodies at once, the
//  number below is wrong - count the poses in the PNG and put that number in.
//
//  The three newest sheets, with the counts they were actually cut to:
// ---------------------------------------------------------------------------
const int WARSPAN_ATTACK_FRAMES    = 5;   // Level 3/level3NormallEnemyAnimation.png
const int L4BOSS2_ANIM_FRAMES   = 5;   // Level 4/level4BosssAnimation.png
const int L4BOSS2_WALK_FRAMES   = 4;   // ... the walk cycle off the same sheet
const int PLAYER_SPECIAL_FRAMES = 5;   // CharacterImages/SpecialMovePlayer.png


// ===========================================================================
//  Below here is plumbing. You do not need to change any of it.
// ===========================================================================

// How many waves each table holds, counted automatically from the lines above.
const int LEVEL1_WAVE_COUNT = (int)(sizeof(LEVEL1_WAVES) / sizeof(LEVEL1_WAVES[0]));
const int LEVEL2_WAVE_COUNT = (int)(sizeof(LEVEL2_WAVES) / sizeof(LEVEL2_WAVES[0]));
const int LEVEL3_WAVE_COUNT = (int)(sizeof(LEVEL3_WAVES) / sizeof(LEVEL3_WAVES[0]));

// Reads one row out of a table, clamped so a bad index can never crash.
inline Wave waveAt(const Wave* table, int count, int index)
{
    Wave empty;
    empty.smallEnemies = 0;
    empty.bigEnemies   = 0;
    empty.warspanEnemies  = 0;
    if (table == 0 || count <= 0) return empty;
    if (index < 0)      index = 0;
    if (index >= count) index = count - 1;
    return table[index];
}

#endif
