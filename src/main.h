// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#ifndef _MAIN_H
#define _MAIN_H

// if you wish to print debug info and apply game pauses uncomment following line
// it will print some text info and make some pauses on the game
// #define _GAME_DEBUG_INFO

#include "picovga.h"

// screen resolution
#define WIDTH	640	// screen width
#define HEIGHT	480	// screen height
#define DRV	VideoVGA // driver

// Layer 0 IMG area
#define IMGWIDTH WIDTH	// image width
#define IMGHEIGHT 432	// image height
#define IMGWIDTHBYTE WIDTH // image width in bytes

// Text mode settings
#define TEXTWIDTH  WIDTH // text width (at pixels)
#define TEXTHEIGHT 8*5	 // text height (at pixels)


#define TEXTW	(TEXTWIDTH/8) // text width (at characters, 640/8=80)
#define TEXTH	(TEXTHEIGHT/8) // text height (at characters, 48/8=6)
#define TEXTSIZE (TEXTW*TEXTH) // text size (80x6 = 480 characters)
#define TEXTWB	(TEXTW*2)	// width of text in bytes

// Image for game ending
// 8 BIT FORMAT, indexed, white pixel 255 used as transparency
extern ALIGNED const u8 Block3Img[440*330];
// image width: 440 pixels
// image height: 330 lines
// image width in bytes: 330 bytes
#define BLOCK3W 440
#define BLOCK3H 330

// Image for game over title
// 8 BIT FORMAT, indexed
extern ALIGNED const u8 GameOverImg[220*34];
#define GAMEOVERW 220
#define GAMEOVERH 34


// format: 8-bit pixel graphics
// image width: 440 pixels
// image height: 330 lines
// image pitch: 440 bytes
extern ALIGNED const u8 CloudsImg[145200];
#define CLOUDSW	440	// clouds width
#define CLOUDSH	330	// clouds height

extern ALIGNED const u8 SkullImg[64*145];
#define SKULLW	64	// clouds width
#define SKULLH	145	// clouds height


// format: 8-bit pixel graphics
// image width: 30 pixels
// image height: 40 lines
// image pitch: 30 bytes
extern ALIGNED const u8 ShooterImg[1200];

// format: 8-bit pixel graphics
// image width: 14 pixels
// image height: 20 lines
// image pitch:  20 bytes
extern ALIGNED const u8 ShooterSmImg[16*20];
#define ShooterAvatar_Index 3	// Its the position of the first avatar sprite on the sprites list
// 2x8 pixels fire shoot
extern ALIGNED const u8 Fire1Img[16];
// 8x8 pixels fire shoot
extern ALIGNED const u8 Fire2Img[64];
// 16x16 pixels fire shoot
extern ALIGNED const u8 ExplodeImg[256];
// MajorTom sound
extern ALIGNED const u8 MajorTomSnd[1571584];
// Laser sound
extern ALIGNED const u8 Laser[6536];
// Explosion1 sound
extern ALIGNED const u8 Explosion1[15398];
// Explosion2 sound
extern ALIGNED const u8 Explosion2[66397];
// Move sound
extern ALIGNED const u8 MoveLeft[5035];
// Move sound
extern ALIGNED const u8 MoveRight[5035];
// Spaceshooter hit
extern ALIGNED const u8 Shooterkilled[9642];
// Invaders shoot
extern ALIGNED const u8 InvaderShoot[11996];


// Name the layers
#define SPACESHIP_LAYER 1	// hot-air layer
#define INVADER_LAYER 2	// balloon first layer

#define SPACESHIPW  30	// Shooter image width
#define SPACESHIPH	40	// Shooter image height
#define SPACESHIPSMW 16 // Mini Shooter image width 
#define SPACESHIPSMH 20 // Mini Shooter image height

#define L0SPRITES_NUM 6	  // number of sprites on shooter layer, 
						  // 0=Space Ship 
						  // 1=Invader shot, 
						  // 2=Player1 fire shot, 
						  // 3=Mini Ship, avatar for lives
						  // 4=Mini Ship, avatar for lives
						  // 5=Mini Ship, avatar for lives						  
#define HOTAIR_MINSPEED 4 // min. move speed
#define HOTAIR_MAXSPEED 8 // max. move speed
#define SHOOTER_SPEED   6 // initial speed of shooter fire
#define SHOOTW 2	// Shoot fire image width
#define SHOOTH 8	// Shoot fire image height
#define INVADERFIRE_SPEED   4 // initial speed of invader shooter fire
#define SHOOTINVW 8	// Shoot fire image width
#define SHOOTINVH 8	// Shoot fire image height
#define DEFENCE_NUM 4	// number of sprites of fences
#define DEFENCEW 50		// Defense Fence image width
#define DEFENCEH 30		// Defense Fence fire image height
#define EXPLODEW 16		// Explosion stamp image width
#define EXPLODEH 16		// Explosion stamp image heigh
#define EXP_DAMAGEX 24  // Explosion damage width
#define EXP_DAMAGEY 24	// Explosion damage heigh

// format: 8-bit pixel graphics
// image width: 32 pixels
// image height: 20 lines
// image pitch: 32 bytes
extern ALIGNED const u8 invader1[640];
extern ALIGNED const u8 invader2[640];
extern ALIGNED const u8 invader3[640];
extern ALIGNED const u8 invader4[640];
extern ALIGNED const u8 invader5[640];

#define Arecibo_Rows	74
#define Arecibo_Cols	24
extern const u8 Arecibo_Reply[Arecibo_Rows*Arecibo_Cols];

#define INVADERW 32	// balloon width
#define INVADERH 20	// balloon height

#define INVADERS_ROWS 5 	// how many rows of invaders?
#define INVADERS_COLUMNS 6	// how many columns of invaders?
#define INVADERS_NUM INVADERS_ROWS*INVADERS_COLUMNS	// number of invader bugs  ROWS*COLUMNS  (all images on same layer)

// keys
#define KEY_L	'J'	// left
#define KEY_U	'I'	// up
#define KEY_R	'L'	// right
#define KEY_D	'K'	// down
#define KEY_FIRE 'A'	// pause
#define KEY_OVER 'X'	// Toggle Game Over
#define KEY_YES 'Y'	// Answer No
#define KEY_NO  'N'	// Answer Yes


#endif // _MAIN_H
