#pragma once

#ifndef LAWN_WIDESCREEN
#define LAWN_WIDESCREEN 1
#endif

#include "ConstEnums.h"
constexpr const double PI = 3.141592653589793;

// ============================================================
// * General Constants
// ============================================================
#if LAWN_WIDESCREEN
const int           BOARD_ADDITIONAL_WIDTH  = 240;   // Extra pixels to the right (1280-800-240=240)
const int           BOARD_WIDTH             = 1280;  // PC: 800 - Widescreen: 1280
const int           BOARD_HEIGHT            = 720;   // PC: 600 - Widescreen: 720
const int           WIDE_BOARD_WIDTH        = 800 + BOARD_ADDITIONAL_WIDTH;   // Logical width of the playing field
const int           BOARD_OFFSET            = 220;   // Unified alias for non-widescreen math
const int           BOARD_OFFSET_X          = 220;   // Horizontal offset of the board
const int           BOARD_OFFSET_Y          = 60;    // Vertical offset of the board
const int           BOARD_EDGE              = -100 + BOARD_ADDITIONAL_WIDTH;
const int           BOARD_IMAGE_WIDTH_OFFSET= 1180  + BOARD_ADDITIONAL_WIDTH;
const int           BOARD_ICE_START         = 800   + BOARD_ADDITIONAL_WIDTH;
const int           ROOF_POLE_START         = WIDE_BOARD_WIDTH + 70  - BOARD_ADDITIONAL_WIDTH;
const int           ROOF_POLE_END           = -BOARD_WIDTH;
const int           ROOF_TREE_START         = WIDE_BOARD_WIDTH + 130 - BOARD_ADDITIONAL_WIDTH;
const int           ROOF_TREE_END           = -670;
// Street zombie constants (widescreen)
const int           STREET_ZOMBIE_START_X       = 1000; // PC: 830 - Widescreen: 1000
const int           STREET_ZOMBIE_ROOF_START_X  = 900;
const int           STREET_ZOMBIE_START_Y       = 70;
const int           STREET_ZOMBIE_GRID_SIZE_X   = 30;   // PC: 56 - Widescreen: 30
const int           STREET_ZOMBIE_GRID_SIZE_Y   = 90;
const int           STREET_ZOMBIE_ROOF_OFFSET   = 30;   // PC: 30 - Widescreen: 15
#else
const int           BOARD_WIDTH             = 800;
const int           BOARD_HEIGHT            = 600;
const int           WIDE_BOARD_WIDTH        = 800;
const int           BOARD_OFFSET            = 220;   // Unified alias for non-widescreen
const int           BOARD_OFFSET_X          = 220;   // Same as BOARD_OFFSET
const int           BOARD_OFFSET_Y          = 0;     // No vertical offset in 4:3
const int           BOARD_ADDITIONAL_WIDTH  = 0;
const int           BOARD_EDGE              = -100;
const int           BOARD_IMAGE_WIDTH_OFFSET= 1180;
const int           BOARD_ICE_START         = 800;
const int           ROOF_POLE_START         = WIDE_BOARD_WIDTH + 70;
const int           ROOF_POLE_END           = -BOARD_WIDTH;
const int           ROOF_TREE_START         = WIDE_BOARD_WIDTH + 130;
const int           ROOF_TREE_END           = -670;
const int           STREET_ZOMBIE_START_X       = 830;
const int           STREET_ZOMBIE_ROOF_START_X  = 830;
const int           STREET_ZOMBIE_START_Y       = 70;
const int           STREET_ZOMBIE_GRID_SIZE_X   = 56;
const int           STREET_ZOMBIE_GRID_SIZE_Y   = 90;
const int           STREET_ZOMBIE_ROOF_OFFSET   = 30;
#endif

const int LAWN_XMIN = 40;
const int LAWN_YMIN = 80;
const int HIGH_GROUND_HEIGHT = 30;

const int SEEDBANK_MAX = 10;
#if LAWN_WIDESCREEN
const int           SEED_BANK_OFFSET_X = 0 + BOARD_ADDITIONAL_WIDTH;
const int           SEED_BANK_OFFSET_X_END = 10 + BOARD_ADDITIONAL_WIDTH;
const int           SEED_BANK_OFFSET = BOARD_OFFSET_Y;
#else
const int           SEED_BANK_OFFSET_X = 0;
const int           SEED_BANK_OFFSET_X_END = 10;
const int           SEED_BANK_OFFSET = 0;
#endif
#if LAWN_WIDESCREEN
const int SEED_CHOOSER_OFFSET_Y = 720; // PC: 516 - Widescreen: 720
#else
const int SEED_CHOOSER_OFFSET_Y = 516;
#endif
const int SEED_PACKET_WIDTH = 50;
const int SEED_PACKET_HEIGHT = 70;
const int IMITATER_DIALOG_WIDTH = 500;
const int IMITATER_DIALOG_HEIGHT = 600;

// ============================================================
// * Level Related
// ============================================================
const int ADVENTURE_AREAS = 5;
const int LEVELS_PER_AREA = 10;
const int NUM_LEVELS = ADVENTURE_AREAS * LEVELS_PER_AREA;
const int FINAL_LEVEL = NUM_LEVELS;
const int FLAG_RAISE_TIME = 100;
const int LAST_STAND_FLAGS = 5;
const int ZOMBIE_COUNTDOWN_FIRST_WAVE = 1800;
const int ZOMBIE_COUNTDOWN = 2500;
const int ZOMBIE_COUNTDOWN_RANGE = 600;
const int ZOMBIE_COUNTDOWN_BEFORE_FLAG = 4500;
const int ZOMBIE_COUNTDOWN_BEFORE_REPICK = 5499;
const int ZOMBIE_COUNTDOWN_MIN = 400;
const int FOG_BLOW_RETURN_TIME = 2000;
const int SUN_COUNTDOWN = 425;
const int SUN_COUNTDOWN_RANGE = 275;
const int SUN_COUNTDOWN_MAX = 950;
const int SURVIVAL_NORMAL_FLAGS = 5;
const int SURVIVAL_HARD_FLAGS = 10;

// ============================================================
// * Store Related
// ============================================================
#if LAWN_WIDESCREEN
const int STORESCREEN_ITEMOFFSET_1_X = 422 + BOARD_OFFSET_X;
const int STORESCREEN_ITEMOFFSET_1_Y = 210 + BOARD_OFFSET_Y;
const int STORESCREEN_ITEMOFFSET_2_X = 382 + BOARD_OFFSET_X;
const int STORESCREEN_ITEMOFFSET_2_Y = 331 + BOARD_OFFSET_Y;
const int STORESCREEN_ITEMSIZE       = 86;
const int STORESCREEN_COINBANK_X     = 650 + BOARD_ADDITIONAL_WIDTH + BOARD_OFFSET_X;
const int STORESCREEN_COINBANK_Y     = BOARD_HEIGHT - 41;
const int STORESCREEN_PAGESTRING_X   = 470 + BOARD_OFFSET_X;
const int STORESCREEN_PAGESTRING_Y   = 500 + (BOARD_OFFSET_Y * 2);
#else
const int STORESCREEN_ITEMOFFSET_1_X = 422;
const int STORESCREEN_ITEMOFFSET_1_Y = 206;
const int STORESCREEN_ITEMOFFSET_2_X = 372;
const int STORESCREEN_ITEMOFFSET_2_Y = 310;
const int STORESCREEN_ITEMSIZE       = 74;
const int STORESCREEN_COINBANK_X     = 650;
const int STORESCREEN_COINBANK_Y     = 559;
const int STORESCREEN_PAGESTRING_X   = 470;
const int STORESCREEN_PAGESTRING_Y   = 500;
#endif
