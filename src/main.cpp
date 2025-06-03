// ****************************************************************************
//
//                                 Main code
//
// Try to make some space invaders, and don't quit on the process
// PFL 2023
// ****************************************************************************

#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sound.h"

    const uint led_pin   = 25;
    const uint sound_pin = 19;
	const uint joy_up_pin	= 16;
	const uint joy_down_pin	= 18;
	const uint joy_left_pin	= 20;
	const uint joy_right_pin = 21;
	const uint joy_fire1_pin = 17;
	const uint joy_fire2_pin = 22;
	
// copy of font
u8 Font_Copy[4096] __attribute__ ((aligned(4)));
// text screen (character + 2x4 bit attributes)
u8 Text[TEXTSIZE*2];
// Text
u8 TextForIntro[80*60*2];

const char Title[] = "INVASORES ESPACIALES";
const char* PageText = 	"           El Michu Software presenta un jueguito de Raspberry Pico\n"
						"                   (c) 2023 La casa del Franky Franky@Isla.pk";
						
// open text
const char IntroText[] = 
	"In 1977 NASA launched two Voyager spacecraft into deep space. Onboard both were\n"
	"gold discs with music, greetings and sounds from Earth. A message to aliens.\n"
	"On same year the Arecibo Telescope in Puerto Rico sent a message to the globular\n"
	"cluster Messier 13 in the hope an advance civilization will read us.\n"
	"\nNothing was heard from space, until now.\n"
	"Aliens reply with another message, this time, in a crop, from a farmland next to\n"
	"the Chilbolton radio telescope in Hampshire, UK, on 19 August 2001.\n\n"
	"It was a warning...\n";

// Text buffer
char TextBuffer[40];
// Image buffer
// Buffer for Image Segments, temporary
ALIGNED u8* Image_buff;

// clouds copy
ALIGNED u8 CloudsImg_Copy[sizeof(CloudsImg)];
// shooter spaceship copy images
ALIGNED u8 ShooterImg_Copy[sizeof(ShooterImg)];

u8 SpaceShipX0[SPACESHIPH]; // array of start of lines
u8 SpaceShipW0[SPACESHIPH]; // array of length of lines

// spaceship avatar for lives display
ALIGNED u8 ShooterSmImg_Copy[sizeof(ShooterSmImg)];
u8 SpaceShipSmX0[SPACESHIPSMH];
u8 SpaceShipSmW0[SPACESHIPSMH];

// shooter fire shot image
ALIGNED u8 FireImg_Copy[sizeof(Fire1Img)];
u8 SHOOTX0[SHOOTH]; // array of start of lines
u8 SHOOTW0[SHOOTH]; // array of length of lines

// invader fire shot image
ALIGNED u8 FireInv_Copy[sizeof(Fire2Img)];
u8 SHOOTINVX0[SHOOTINVH]; // array of start of lines
u8 SHOOTINVW0[SHOOTINVW]; // array of length of lines


// shooter layer sprites
sSprite HotairSprites[L0SPRITES_NUM+DEFENCE_NUM];
sSprite* HotairSpriteList[L0SPRITES_NUM+DEFENCE_NUM];
s8 HotairDX[L0SPRITES_NUM+DEFENCE_NUM];
s8 HotairDY[L0SPRITES_NUM+DEFENCE_NUM];

// INVADERS various copy images
ALIGNED u8 Invaders_Copy[sizeof(invader1)];
u8 BlueX0[INVADERH];
u8 BlueW0[INVADERW];

ALIGNED u8 GreenImg_Copy[sizeof(invader2)];
u8 GreenX0[INVADERH];
u8 GreenW0[INVADERW];

ALIGNED u8 RedImg_Copy[sizeof(invader3)];
u8 RedX0[INVADERH];
u8 RedW0[INVADERW];

ALIGNED u8 YellowImg_Copy[sizeof(invader4)];
u8 YellowX0[INVADERH];
u8 YellowW0[INVADERW];

ALIGNED u8 Invader5_Copy[sizeof(invader5)];
u8 Invader5X0[INVADERH];
u8 Invader5W0[INVADERW];


//ALIGNED u8 DeFenceImgWH[DEFENCEH][DEFENCEW];	// A 120 x 30 pixels image we are going to generate
ALIGNED u8 DeFenceImg_Copy1[120*30];	// A 120 x 30 pixels image we are going to generate
ALIGNED u8 DeFenceImg_Copy2[120*30];	// A 120 x 30 pixels image we are going to generate
ALIGNED u8 DeFenceImg_Copy3[120*30];	// A 120 x 30 pixels image we are going to generate
ALIGNED u8 DeFenceImg_Copy4[120*30];	// A 120 x 30 pixels image we are going to generate


u8 DefenceX0[DEFENCEH];
u8 DefenceW0[DEFENCEW];

// Explosion invaders bomb
// ALIGNED u8 ExplodeImg[16*16];	// A 16 x 16 pixels image, explosion foot print, almost monocromatic


// Invaders sprites
sSprite InvaderSprites[INVADERS_NUM];
sSprite* BalloonSpriteList[INVADERS_NUM];
u16		InvaderXPOS[INVADERS_NUM];
s8		FleetDX;	// Slide increment or decrement
u16		FleetSX;	// Home base position

// Defense sprite
sSprite* DeFenceSprites[DEFENCE_NUM];

// Canvas
sCanvas TitleCanvas;
sCanvas Block3Canvas;
sCanvas GameOverCanvas;


struct {
		u8	Lives=3;
		u8	*ShipImage=ShooterImg_Copy;
		u16	sPosx;
		u16	sPosy;
		u8	sSpeed=SHOOTER_SPEED;
		u16	Score;
		u8	shootray;
		u8  fired=0;
		u8	is_hit=0;
		u8  hit_cycles=0;
		u8  level=1;
		sSprite	*hit_invader;
} Player1;

struct {
		u8	Lives;
		u8	sSpeed;	// shoot speed
		u8  ySpeed;	// vertical bugs speed
		u16	Score;
		u8	shootray;
		u8  fired;
		bool	is_hit;
		bool	respanwn;
		u8  hit_cycles;
		u8  ships_alive;
		u16 hit_targetx;
		u16 hit_targety;
		sSprite	*hit_target;
} Invaders;

// transformation matrix
cMat2Df Mat;


//Sound Tone system for Pico
PicoTone ptone;

// C11 does not have to_string, I provide
char* to_string( int x ) {
  return itoa(x, TextBuffer, 10);
}

// Calculate position of invader(i)
int	Invader_XPOS(u8 i) {
	// calculate X position based on the ROWS and COLUMNS of the fleet
	return (u16) ALIGN4((i % INVADERS_COLUMNS) * (INVADERW+20) + INVADERW);
}

void InvertImg(u8 *ThisImgPtr) {
	u16 k;
	for (k=0;k<sizeof(invader1);k++) 	{
		ThisImgPtr[k]^=0xFF;
	}	
}

// Display hits
void ShowHits(u16 hits) {
		PrintSetCol(PC_COLOR(PC_BLACK, PC_GREEN));
		PrintSetPos(60, 2);
		PrintText("HITS       ");
		PrintSetPos(66, 2);
		PrintText(to_string(hits));	
}

// Get one pixel at position x,y inside sprite
// sx, sy are screen coordinates
u8 GetPixel(u16 sx, u16 sy, sSprite* spr) {
	
	int16_t	x=sx-spr->x;
	int16_t	y=sy-spr->y;
	
	if ((x+1)>0 && (y+1)>0 && y<(spr->h) && x<(spr->w)) { 
		return spr->img[x + spr->w * y];
	}
	return 0;
}

// Set a pixel inside sprite
// x, y are sprite coordinates, points inside the sprite widht, heigh
// returns true if x, y are inside the sprite screen position and pixel was set
// returns false if x, y are outside the sprite and pixel was not set
bool SetPixel(u8 pixel, u16 x, u16 y, sSprite* spr) {
	
	if (x+1>0 && y+1>0 && y<(spr->h) && x<(spr->w)) { 
		spr->img[x + spr->w * y]=pixel;
		return true;
	}	
	return false;
}

// find out if the point sx,sy hits the target sprite
// sx, sy are screen coordinates
bool	is_hit(u16 sx, u16 sy, sSprite* spr, u8 delta=1) {
	// put point inside sprite
	// check if it hits some non transparent pixels
	// return true if hit
	
	u16 k;
	
	if ((spr->x)<(sx+1) && (sx+1)<(spr->x + spr->w)) {
		for (k=0;k<(delta+1);k++) {
			// if we got any pixed different from 0 it is a hit 
			if (GetPixel(sx+k,sy+k,spr)) return true;
		}
	}
	return false;
}

// impose one sprite into another
// the source sprite must have pixel values of 0, or 255
// the pixel values will perform bit AND operation, result saved into target sprite
// the sx, sy values are screen values
// sx, sy define the center of the stamp.

void Stamp_sprite(u16  sx, u16 sy, sSprite* source, sSprite* target) {
	u16 x,y;
	
	// sx, sy represent the center of the impact, it is the center of the stamp
	sx=sx - source->w/2;	// offset from the center
	sy=sy - source->h/2;	// offset from the center

//	PrintSetPos(0,1);

//	PrintText(" SSX:");PrintText(to_string(sx));
//	PrintText(" SSY:");PrintText(to_string(sy));
//	PrintText(" Target X:");PrintText(to_string(sx-target->x));
//	PrintText(" Target Y:");PrintText(to_string(sy-target->y));
//	sleep_ms(500);	
//	for (x=sx; x<(sx+source->w); x++)
//		for (y=sy; y<(sy+source->h); y++)

	for (x=sx; x<(sx+EXP_DAMAGEX); x++)
		for (y=sy; y<(sy+EXP_DAMAGEY); y++)

		{
//			SetPixel(GetPixel(x,y,target) & GetPixel(x,y,source), x,y,target);	// pixel = sourcepixel & targetpixel
			SetPixel(0,x- target->x,y- target->y,target);	// pixel = sourcepixel & targetpixel
		}
//	PrintText(" T X:");PrintText(to_string(x-target->x));
//	PrintText(" T Y:");PrintText(to_string(y-target->y));

}


// Erase pixel from the explosion on the fence
void MeltFence(u16 sx, u16 sy, sSprite *spr) {

	sSprite boom;
	
	boom.img = (u8*) ExplodeImg; 
//	boom.w = EXPLODEW;
//	boom.h = EXPLODEH;
	boom.w = EXP_DAMAGEX;
	boom.h = EXP_DAMAGEY;
	boom.wb = EXP_DAMAGEX;
//	boom.y = sy - EXPLODEH/2;
//	boom.x = sx - EXPLODEW/2;
	boom.y = sy - EXP_DAMAGEY/2;
	boom.x = sx - EXP_DAMAGEX/2;
	boom.x0 = NULL;
	boom.w0 = NULL;
#ifdef _GAME_DEBUG_INFO							
	PrintSetPos(0,0);
	PrintText("StampSpr X:");PrintText(to_string(sx));
	PrintText(" ");
	PrintText("StampSrp Y:");PrintText(to_string(sy));
	PrintText(" ");
#endif

	Stamp_sprite(sx,sy,&boom,spr);

}

// Set bright pixels at the explosion on the fence
// sx, sy are screen coordinates of the explosion center
void CrashFence(u16 sx, u16 sy, sSprite *spr) {

	u16 x,y;
	u8	u8Pixel;

#ifdef _GAME_DEBUG_INFO							
	PrintSetPos(0,0);
	PrintText("CrashSpr X:");PrintText(to_string(sx));
	PrintText(" ");
	PrintText("CrashSrp Y:");PrintText(to_string(sy));
	PrintText(" ");
#endif
	
	// sx, sy represent the center of the impact, it is the center of the stamp
	sx=sx - EXP_DAMAGEX/2;	// offset from the center
	sy=sy - EXP_DAMAGEY/2;	// offset from the center


	for (x=sx; x<(sx+EXP_DAMAGEX); x++)
		for (y=sy; y<(sy+EXP_DAMAGEY); y++)

		{
			u8Pixel=GetPixel(x- spr->x,y- spr->y,spr) ^ 0xFF;	// Invert pixel values
			SetPixel(u8Pixel,x- spr->x,y- spr->y,spr);
		}

}

// get character from keyboard (0 = no key)
char GetChar()
{
	char c = getchar_timeout_us(0);
	if (c == (char)PICO_ERROR_TIMEOUT) c = 0;
	if ((c >= 'a') && (c <= 'z')) c -= 32;
	return c;
}


// setup rotation transformation matrix
void SetMat(float a)
{
	// prepare transformation matrix
	// SKULL Image of real dimensions will be enlarged to 2X.
	Mat.PrepDrawImg(SKULLW, SKULLH, 0, 0, SKULLW*a, SKULLH*a, 0, 0, 0, 0, 0);

	// export matrix to int array[6]
	// Mat.ExportInt(MatInt);
}

void	Display_Intro_Text() {
	
	//PrintClear();
	PrintText(IntroText);
/****	
	PrintSetPos(21, 13);
	for (int r=0;r<Arecibo_Rows-50;r++) {
		for (int c=0;c<Arecibo_Cols;c++) {
			if (Arecibo_Reply[c+r*Arecibo_Cols]>0)	{ // There is a non-black pixel
				PrintSetCol(PC_COLOR(PC_BLACK, (Arecibo_Reply[c+r*Arecibo_Cols] % 10) + 5));
				PrintChar(char(143));
				sleep_ms(30);
				sleep_ms(30);
				
			} else PrintChar(char(32));
		}
		PrintChar('\n');PrintAddPos(21,0);
	}
	PrintText("\n...EOT...");
	* 
****/
}

// Prepare Base Screen
void	Prepare_Layer0_Sprites() {
	// initialize base layer 0
	ScreenClear(pScreen);

	// Prepare Font
	// copy font to RAM buffer
	memcpy(Font_Copy, FontBold8x8, sizeof(FontBold8x8));

	// insert here the TEXT  segment to write stats and SCORE
	sStrip* t = ScreenAddStrip(pScreen, TEXTHEIGHT);
	sSegm*  g = ScreenAddSegm(t, WIDTH);
	ScreenSegmAText(g,  Text, Font_Copy, 8, DefPal16, TEXTWB);		

	// shooter background stars
	sStrip*  t2 = ScreenAddStrip(pScreen, HEIGHT-TEXTHEIGHT);
	sSegm*   g2 = ScreenAddSegm(t2, WIDTH);
	ScreenSegmGraph8(g2, CloudsImg_Copy, CLOUDSW);
	g2->wrapx = CLOUDSW;
	g2->wrapy = CLOUDSH;		
	
		// Use the default Canvas	
	Canvas.img = CloudsImg_Copy;
	Canvas.w = CLOUDSW;
	Canvas.h = CLOUDSH;
	Canvas.wb = CLOUDSW;
	Canvas.format = CANVAS_8;

}


// Prepare Sprites
void	Prepare_Layer0_Text() {
	// initialize base layer 0
	ScreenClear(pScreen);

	// insert here the TEXT  segment to write stats and SCORE
	sStrip* t = ScreenAddStrip(pScreen, 16*10);	// Give me 10 lines of text of 16 character height
	sSegm*  g = ScreenAddSegm(t, WIDTH);
	ScreenSegmAText(g,  TextForIntro, Font_Copy, 16, DefPal16, 80*2);

	// Background stars seamless graphic
	sStrip*  t2 = ScreenAddStrip(pScreen, HEIGHT-TEXTHEIGHT);
	sSegm*   g2 = ScreenAddSegm(t2, WIDTH);
	ScreenSegmGraph8(g2, CloudsImg_Copy, CLOUDSW);
	g2->wrapx = CLOUDSW;
	g2->wrapy = CLOUDSH;
	
	// Use the default Canvas	
	Canvas.img = CloudsImg_Copy;
	Canvas.w = CLOUDSW;
	Canvas.h = CLOUDSH;
	Canvas.wb = CLOUDSW;
	Canvas.format = CANVAS_8;
}


// Prepare Sprites for defence, spaceship, spaceship avatar, fire, firebomb
void	Prepare_Layer1_Sprites() {
	
	sSprite* spr;
		
	// prepare array of space ships sprite lines
	SpritePrepLines(ShooterImg_Copy, SpaceShipX0, SpaceShipW0, SPACESHIPW, SPACESHIPH, SPACESHIPW, COL_BLACK, False);
	// prepare array of mini spaceship sprite lines
	SpritePrepLines(ShooterSmImg_Copy, SpaceShipSmX0, SpaceShipSmW0, SPACESHIPSMW, SPACESHIPSMH, SPACESHIPSMW, COL_BLACK, False);
	
	// prepare array of Player1 SHOT sprite lines
	SpritePrepLines(FireImg_Copy, SHOOTX0, SHOOTW0, SHOOTW, SHOOTH, SHOOTW, COL_BLACK, False);
	// prepare array of Invader SHOT sprite lines
	SpritePrepLines(FireInv_Copy, SHOOTINVX0, SHOOTINVW0, SHOOTINVW, SHOOTINVH, SHOOTINVW, COL_BLACK, False);
	// prepare array of DEFENSE sprite lines
	SpritePrepLines(DeFenceImg_Copy1, DefenceX0, DefenceW0, DEFENCEW, DEFENCEH, DEFENCEW, COL_BLACK, False);
	// prepare array of DEFENSE sprite lines
	SpritePrepLines(DeFenceImg_Copy2, DefenceX0, DefenceW0, DEFENCEW, DEFENCEH, DEFENCEW, COL_BLACK, False);
	// prepare array of DEFENSE sprite lines
	SpritePrepLines(DeFenceImg_Copy3, DefenceX0, DefenceW0, DEFENCEW, DEFENCEH, DEFENCEW, COL_BLACK, False);
	// prepare array of DEFENSE sprite lines
	SpritePrepLines(DeFenceImg_Copy4, DefenceX0, DefenceW0, DEFENCEW, DEFENCEH, DEFENCEW, COL_BLACK, False);

	// prepare space ship and defece sprites on layer 1
	for (int i = 0; i < L0SPRITES_NUM; i++)
	{
		spr = &HotairSprites[i];
		HotairSpriteList[i] = spr;
		switch  (i) {
			case 0:
			spr->img = ShooterImg_Copy; 
			spr->w = SPACESHIPW;
			spr->h = SPACESHIPH;
			spr->wb = SPACESHIPW;
			spr->y = HEIGHT-TEXTHEIGHT-SPACESHIPH;
			spr->x = WIDTH/2;
			spr->x0 = SpaceShipX0;
			spr->w0 = SpaceShipW0;
			HotairDX[i] = HOTAIR_MINSPEED;	// Animate horizontally value
			break;

			case 1:	// This is the invader shot sprite
			spr->img = FireInv_Copy;
			spr->w  = SHOOTINVW;
			spr->h  = SHOOTINVH;
			spr->wb = SHOOTINVW;
			spr->y = 200;	// it depens on who is firing
			spr->x = 500;
 			spr->x0 = SHOOTINVX0;
			spr->w0 = SHOOTINVW0;
			//HotairDY[i] = Player1.sSpeed;
			break;
		
			case 2:	// This is the Player1 shot sprite
			spr->img = FireImg_Copy;
			spr->w = SHOOTW;
			spr->h = SHOOTH;
			spr->wb = SHOOTW;
			spr->y = HEIGHT-TEXTHEIGHT-SPACESHIPH-SHOOTH + 4;
			spr->x = WIDTH/2 + SPACESHIPW/2; 
			spr->x0 = SHOOTX0;
			spr->w0 = SHOOTW0;
			HotairDY[i] = Player1.sSpeed;
			break;

			
			case 3:	// This is the first mini Player1 spaceship avatar
			spr->img = ShooterSmImg_Copy;
			spr->w = SPACESHIPSMW;
			spr->h = SPACESHIPSMH;
			spr->wb = SPACESHIPSMW;
			spr->y = HEIGHT-SPACESHIPSMH;
			spr->x = 0;
			spr->x0 = SpaceShipSmX0;
			spr->w0 = SpaceShipSmW0;
			HotairDY[i] = 0;
			HotairDX[i] = 0;
			break;
			case 4:	// This is the second mini Player1 spaceship avatar
			spr->img = ShooterSmImg_Copy;
			spr->w = SPACESHIPSMW;
			spr->h = SPACESHIPSMH;
			spr->wb = SPACESHIPSMW;
			spr->y = HEIGHT-SPACESHIPSMH;
			spr->x = (SPACESHIPSMW+4);
			spr->x0 = SpaceShipSmX0;
			spr->w0 = SpaceShipSmW0;
			HotairDY[i] = 0;
			HotairDX[i] = 0;
			break;
			case 5:	// This is third the mini Player1 spaceship avatar
			spr->img = ShooterSmImg_Copy;
			spr->w = SPACESHIPSMW;
			spr->h = SPACESHIPSMH;
			spr->wb = SPACESHIPSMW;
			spr->y = HEIGHT-SPACESHIPSMH;
			spr->x = (SPACESHIPSMW+4)*2;
			spr->x0 = SpaceShipSmX0;
			spr->w0 = SpaceShipSmW0;
			HotairDY[i] = 0;
			HotairDX[i] = 0;
			break;
			
		}
		
		spr->keycol = COL_BLACK;

	}
	
	// prepare fence sprites, add to the sprite list for shooter layer
	for (int i = L0SPRITES_NUM; i < L0SPRITES_NUM+DEFENCE_NUM; i++)
	{
		spr = &HotairSprites[i];
		HotairSpriteList[i] = spr;
		switch (i-L0SPRITES_NUM) {
			case 0: spr->img = DeFenceImg_Copy1; break;
			case 1: spr->img = DeFenceImg_Copy2; break;
			case 2: spr->img = DeFenceImg_Copy3; break;
			case 3: spr->img = DeFenceImg_Copy4; break;
		}
		
		
		spr->w = DEFENCEW;
		spr->h = DEFENCEH;
		spr->wb = DEFENCEW;
		spr->y = HEIGHT-TEXTHEIGHT-SPACESHIPH-DEFENCEH-20;
		spr->x0 = DefenceX0;
		spr->w0 = DefenceW0;
		spr->x = 130 + (DEFENCEW+60)*(i-L0SPRITES_NUM);
	}
	

	// setup shooter spaceship layer + fence sprites on shooter spaceship layer
	LayerSpriteSetup(SPACESHIP_LAYER, HotairSpriteList, L0SPRITES_NUM+DEFENCE_NUM, &Vmode, 0, 0, WIDTH, HEIGHT, COL_BLACK);
	LayerOn(SPACESHIP_LAYER);

}

// Prepare Sprites for invaders
void	Prepare_Layer2_Sprites() {	// Setup invaders layer
	u16	i=0, j,k;
	sSprite* spr;

	//prepare array of invaders fleet sprite lines
	SpritePrepLines(Invaders_Copy, BlueX0, BlueW0, INVADERW, INVADERH, INVADERW, COL_BLACK, True);
	SpritePrepLines(GreenImg_Copy, GreenX0, GreenW0, INVADERW, INVADERH, INVADERW, COL_BLACK, True);
	SpritePrepLines(RedImg_Copy, RedX0, RedW0, INVADERW, INVADERH, INVADERW, COL_BLACK, True);
	SpritePrepLines(YellowImg_Copy, YellowX0, YellowW0, INVADERW, INVADERH, INVADERW, COL_BLACK, True);
	SpritePrepLines(Invader5_Copy, Invader5X0, Invader5W0, INVADERW, INVADERH, INVADERW, COL_BLACK, True);

	// prepare invaders sprites

	{
		for (j=0;j<INVADERS_ROWS;j++)
		for (k=0;k<INVADERS_COLUMNS;k++,i++)
		{
			
			spr = &InvaderSprites[i];
			BalloonSpriteList[i] = spr;

			switch (j)
			{
			case 0:
				spr->img = Invaders_Copy;
				spr->x0 = BlueX0;
				spr->w0 = BlueW0;
				break;

			case 1:
				spr->img = GreenImg_Copy;
				spr->x0 = GreenX0;
				spr->w0 = GreenW0;
				break;

			case 2:
				spr->img = RedImg_Copy;
				spr->x0 = RedX0;
				spr->w0 = RedW0;
				break;
			case 3:
				spr->img = YellowImg_Copy;
				spr->x0 = YellowX0;
				spr->w0 = YellowW0;
				break;
			case 4:
				spr->img = Invader5_Copy;
				spr->x0 = Invader5X0;
				spr->w0 = Invader5W0;
				break;
			}


			spr->keycol = COL_BLACK;
			spr->w = INVADERW;
			spr->h = INVADERH;
			spr->wb = INVADERW;

			// generate invader fleet position for each sprite
			spr->x = Invader_XPOS(i);
			InvaderXPOS[i]=spr->x;
			spr->y = j*(INVADERH+10) + INVADERH*2;

		}
	}

	// sort fast sprite list by X coordinate
	SortSprite(BalloonSpriteList, INVADERS_NUM);


	// setup invaders layer
	LayerSpriteSetup(INVADER_LAYER, BalloonSpriteList, INVADERS_NUM, &Vmode, 0, 0, WIDTH, HEIGHT, COL_BLACK);
	LayerOn(INVADER_LAYER);

}


// initialize videomode
void VideoInit()
{

	// setup videomode
	VgaCfgDef(&Cfg); // get default configuration
	Cfg.video = &DRV; // video timings
	Cfg.width = WIDTH; // screen width
	Cfg.height = HEIGHT; // screen height
	Cfg.freq = 180000; // system clock frequency
	
	Cfg.mode[SPACESHIP_LAYER] = LAYERMODE_SPRITEWHITE; // shooter spaceship
	Cfg.mode[INVADER_LAYER] = LAYERMODE_FASTSPRITEWHITE; // invaders fleet
	//Cfg.mode[INVADER_LAYER+1] = LAYERMODE_FASTSPRITEWHITE; // invaders fleet 2
	VgaCfg(&Cfg, &Vmode); // calculate videomode setup

	//Prepare_Layer0_Sprites();
	Prepare_Layer0_Text();
	Prepare_Layer1_Sprites();
	Prepare_Layer2_Sprites();

	// initialize system clock
	set_sys_clock_pll(Vmode.vco*1000, Vmode.pd1, Vmode.pd2);

	// initialize videomode
	VgaInitReq(&Vmode);
}

// Display Game Title
void DispGameTitle() {
// GAME TITLE
	PrintSetCol(PC_COLOR(PC_BLACK, PC_LTRED));
	PrintSetPos((TEXTW - sizeof(Title)+1)/2, 2);
	PrintText(Title);
}
// display help
void DispHelp()
{
	printf("\n");
	printf("%c ... left\n", KEY_L);
	printf("%c ... up\n", KEY_U);
	printf("%c ... right\n", KEY_R);
	printf("%c ... down\n", KEY_D);
	printf("%c ... Fire\n", KEY_FIRE);
}

// display main screen
// Working for Layer0 with split segments on TextMode + GRAPH8 mode
void DispMainScreen() {
	u8 waitt=0;

	
	LayerOff(SPACESHIP_LAYER);
	LayerOff(INVADER_LAYER);
	
	TitleCanvas.img = (u8*)Arecibo_Reply;
	TitleCanvas.w = Arecibo_Cols;
	TitleCanvas.h = Arecibo_Rows;
	TitleCanvas.wb = Arecibo_Cols;
	TitleCanvas.format = CANVAS_8;
	
	Block3Canvas.img = (u8*)Block3Img;
	Block3Canvas.w = BLOCK3W;
	Block3Canvas.h = BLOCK3H;
	Block3Canvas.wb =BLOCK3W;
	Block3Canvas.format = CANVAS_8;
	
	GameOverCanvas.img = (u8*)GameOverImg;
	GameOverCanvas.w = GAMEOVERW;
	GameOverCanvas.h = GAMEOVERH;
	GameOverCanvas.wb= GAMEOVERW;
	GameOverCanvas.format = CANVAS_8;

	
	PrintSetup(TextForIntro, 80, 60, 80*2);
	PrintSetCol(PC_COLOR(PC_BLACK, PC_WHITE));
	PrintClear();
	printf("\n");
	printf("Space Invaders for Raspberry PICO.\n");
	printf("An education game for earth things. M0\n");
	printf("Press any key to continue.\n");
	sleep_ms(2000);
	Display_Intro_Text();
		// display title
	//
	sleep_ms(2000);
	//CopyWhiteImg(Image_buff, Arecibo_Reply, sizeof(Arecibo_Reply));
	//DrawRect(&Canvas, 0, 0, 4, 4, COL_GREEN);
	//DrawImg(&Canvas, &TitleCanvas, 300, 0, 0, 0, Arecibo_Cols, Arecibo_Rows);

	/// Draw Arecibo Message
	for (int r=0;r<Arecibo_Rows;r++) {
		for (int c=0;c<Arecibo_Cols;c++) {
			if (Arecibo_Reply[c+r*Arecibo_Cols]>0)	{ // There is a non-black pixel
				//PrintSetCol(PC_COLOR(PC_BLACK, (Arecibo_Reply[c+r*Arecibo_Cols] % 10) + 5));
				//PrintChar(char(143));
				ptone.tone(1000,30, Pulse);
				DrawRect(&Canvas, c*4+80, r*4, 4, 4, (Arecibo_Reply[c+r*Arecibo_Cols]));
				sleep_ms(30);
				
				
			} else sleep_ms(10);
		if (GetChar()!=0 || !gpio_get(joy_right_pin) || !gpio_get(joy_left_pin) || !gpio_get(joy_up_pin) || !gpio_get(joy_down_pin) || !gpio_get(joy_fire1_pin) || !gpio_get(joy_fire2_pin)) {
			waitt=100;
			break;	// Skip is a key is pressed.
			}
			
		}
	if (waitt>0) break;
		
	}
	//PrintText("\n...EOT...");
	
	
	printf("\n");
	printf("Space Invaders for Raspberry PICO.\n");
	printf("An education game for earth things. M2\n");
	printf("Press any key to continue.\n");

	while ((GetChar()==0) & (waitt++ <100)) {sleep_ms(50);}
	// Clear screen
	
	ScreenClear(pScreen);
	// Give Up memory
	delete Image_buff;	// Give back the memory

	printf("Deleted Strip 0 Base Layer.\n");
	
	// Prepare Base Screen
	Prepare_Layer0_Sprites();
	

	printf("Layer 1&2 On.\n");

	LayerOn(SPACESHIP_LAYER);
	LayerOn(INVADER_LAYER);
	
}

void DisplayStats() {
	// Print the dashboard with the statistics of the game
	
	// Print Level
	PrintSetCol(PC_COLOR(PC_BLACK, PC_LTGRAY));
	PrintSetPos(36, 0);
	PrintText("LEVEL       ");
	PrintSetPos(42, 0);
	PrintText(to_string(Player1.level));
	// Print Player1 Hits
	ShowHits(Player1.Score);
	// Print Invader Hits
	PrintSetCol(PC_COLOR(PC_BLACK, PC_GREEN));
	PrintSetPos(0, 2);
	PrintText("INVADERS HITS       ");
	PrintSetPos(14, 2);
	PrintSetCol(PC_COLOR(PC_BLACK, PC_RED));
	PrintText(to_string(Invaders.Score));
	

	// Print random quotes
	
}

// Draw image with 2X scale (source and destination must have same format)
/***
 * Unused function, it is slow
 * 
void DrawImg2x(sCanvas* canvas, sCanvas* src, int xd, int yd, int xs, int ys, int w, int h)
{
	// must have same format
	if (canvas->format != src->format) return;

	// limit coordinates X
	if (xd < 0)
	{
		w += xd;
		xs -= xd;
		xd = 0;
	}

	if (xs < 0)
	{
		w += xs;
		xd -= xs;
		xs = 0;
	}

	if (xd + 2*w > canvas->w) w = canvas->w - xd;
	if (xs + w > src->w) w = src->w - xs;
	if (w <= 0) return;

	// limit coordinates Y
	if (yd < 0)
	{
		h += yd;
		ys -= yd;
		yd = 0;
	}

	if (ys < 0)
	{
		h += ys;
		yd -= ys;
		ys = 0;
	}

	if (yd + 2*h > canvas->h) h = canvas->h - yd;
	if (ys + h > src->h) h = src->h - ys;
	if (h <= 0) return;

	// check format
	switch(canvas->format)
	{
	// 8-bit pixels
	case CANVAS_8:
		{
			int wbd = canvas->wb;
			int wbs = src->wb;
			u8* d = canvas->img + xd + yd*wbd;
			u8* dp;
			u8* s = src->img + xs + ys*wbs;
			u8* ds;
			
			for (; h > 0; h--)
			{
				//memcpy(d, s, w);
				dp = d;
				ds = s;
				for (u16 iw=0;iw<w;iw++) {
					*(d++)= (*s);
					//printf("d=%u s=%u    ",d,s);					

					*(d++)= *(s++);
					//printf("d=%u s=%u h=%u ",d,s,h);					
					
					//if (!(iw % 2)) printf("\n");

				}
				printf("\n");
				d = dp;
				d += wbd;
				dp = d;
				
				s = ds;
				
				
				for (u16 iw=0;iw<w;iw++) {
					*(d++)= (*s);
					//printf("d=%u s=%u  ",d,s);					

					*(d++)= *(s++);
					printf("d=%u s=%u  h=%u   ",d - canvas->img,s - src->img,h);
					if (!(iw % 2)) printf("\n");


				}
					
				d = dp;
				d += wbd;
				s = ds;
				s += wbs;
				
			}
		}
		break;
	}
}
* 
* 
* Better use scaled image
****/


// Transition to game over. Hide ship, show sign
void GameOverStart() {
		pScreen->strip[1].seg[0].offx=100;
		pScreen->strip[1].seg[0].offy=0;
		// show alien eating pizza
		DrawImg(&Canvas, &Block3Canvas, 0, 0, 0,0, BLOCK3W, BLOCK3H);			
		DrawImg(&Canvas, &GameOverCanvas, (CLOUDSW-GAMEOVERW)/2, 30, 0,0, GAMEOVERW, GAMEOVERH);			

		HotairDX[0] = 0;	// stop moving ship
		Invaders.hit_target->x=WIDTH+120;	// Get it offscreen
		printf("\n***** GAME OVER *****\n");

}

void Rebuild_Defenses() {
	u16 i=0,x,y;
	
	// copy invader images
	CopyWhiteImg(Invaders_Copy, invader1, sizeof(invader1));
	CopyWhiteImg(GreenImg_Copy, invader2, sizeof(invader2));
	CopyWhiteImg(RedImg_Copy, invader3, sizeof(invader3));
	CopyWhiteImg(YellowImg_Copy, invader4, sizeof(invader4));
	CopyWhiteImg(Invader5_Copy, invader5, sizeof(invader5));
	
	// Generate Fence image
	for (u16 q=0;q<sizeof(DeFenceImg_Copy1);q++)
		DeFenceImg_Copy1[q]=0x0;	// Transparency pixel


	for (x=0;x<DEFENCEW;x++) {		// build a fence, and average new spike with the last 2 spikes
		u8	lastH1, lastH2, tColor; // to build a not too sharp fence.
		for (i=(RandU8MinMax(0,DEFENCEH*3/2) + lastH1/2 + lastH2/3) / 2,y=i;i<DEFENCEH;i++) {
			tColor=RandU8MinMax(65,250);
			DeFenceImg_Copy1[x+(i*DEFENCEW)]=COLRGB(tColor,RandU8MinMax(65,250),127);
		}
		lastH2=lastH1;
		lastH1=y;
	}

	for (x=0;x<DEFENCEW;x++) {	// build a fence, and average new spike with the last 2 spikes, to build a not too sharp fence.
		u8	lastH1, lastH2, tColor;
		for (i=(RandU8MinMax(0,DEFENCEH*2/3) + lastH1/2 + lastH2/3) / 2,y=i;i<DEFENCEH;i++) {
			tColor=RandU8MinMax(65,250);
			DeFenceImg_Copy2[x+(i*DEFENCEW)]=COLRGB(tColor,80,90);
		}
		lastH2=lastH1;
		lastH1=y;
	}

	for (x=0;x<DEFENCEW;x++) {	// build a fence, and average new spike with the last 2 spikes, to build a not too sharp fence.
		u8	lastH1, lastH2, tColor;
		for (i=(RandU8MinMax(0,DEFENCEH*2/3) + lastH1/2 + lastH2/3) / 2,y=i;i<DEFENCEH;i++) {
			tColor=RandU8MinMax(65,250);
			DeFenceImg_Copy3[x+(i*DEFENCEW)]=COLRGB(tColor,100,127);
		}
		lastH2=lastH1;
		lastH1=y;
	}

	for (x=0;x<DEFENCEW;x++) {	// build a fence, and average new spike with the last 2 spikes, to build a not too sharp fence.
		u8	lastH1, lastH2, tColor;
		for (i=(RandU8MinMax(0,DEFENCEH*1/2) + lastH1/2 + lastH2/3) / 2,y=i;i<DEFENCEH;i++) {
			tColor=RandU8MinMax(65,250);
			DeFenceImg_Copy4[x+(i*DEFENCEW)]=COLRGB(tColor,160,160);
		}
		lastH2=lastH1;
		lastH1=y;
	}

}
void Reset_Background() {
	//copy background
	memcpy(CloudsImg_Copy, CloudsImg, sizeof(CloudsImg));	
}

void Reset_ShipAvatars() {
	// Ship position reset
	sSprite* spr;

	spr = &HotairSprites[0];
	spr->y = HEIGHT-TEXTHEIGHT-SPACESHIPH;
	spr->x = WIDTH/2;

	// MiniShip avatar position reset
	spr = &HotairSprites[3];
	spr->x = 0;
	spr = &HotairSprites[4];
	spr->x = (SPACESHIPSMW+4);
	spr = &HotairSprites[5];
	spr->x = (SPACESHIPSMW+4)*2;	
}
void Reset_Game() {
	Reset_Background();
	
	// copy shooter spaceship
	CopyWhiteImg(ShooterImg_Copy, ShooterImg, sizeof(ShooterImg));
	// copy spaceship avatar
	CopyWhiteImg(ShooterSmImg_Copy, ShooterSmImg, sizeof(ShooterSmImg));
	// copy shoot fire1
	CopyWhiteImg(FireImg_Copy,Fire1Img, sizeof(Fire1Img));
	// copy shoot fire2
	CopyWhiteImg(FireInv_Copy,Fire2Img, sizeof(Fire2Img));


	Rebuild_Defenses();
	Reset_ShipAvatars();

	// Game Logic
	Player1.Lives=3;
	Player1.Score=0;
	Player1.ShipImage=ShooterImg_Copy;
	Player1.sPosx=(WIDTH/2)-SHOOTW/2;
	Player1.sPosy=HEIGHT-TEXTHEIGHT-SPACESHIPH;
	Player1.sSpeed=SHOOTER_SPEED;	
	Player1.fired=0;
	Player1.hit_cycles=0;
	Player1.level=1;

	Invaders.sSpeed=INVADERFIRE_SPEED;
	Invaders.ySpeed=1;
	Invaders.is_hit=false;
	Invaders.respanwn=false;
	Invaders.hit_cycles=0;
	Invaders.Lives=3;
	Invaders.Score=0;
	Invaders.ships_alive=INVADERS_NUM;
	
	// Fleet Horizontal move
	FleetDX=4;		// Initial shift of the fleet
	FleetSX=INVADERW;
	
	// Reset Invaders Fleet
	
	
}

void ResetInvadersFleetPosition() {
	sSprite* spr;
	u8 ix=0;
	u16 j,k;
	
	FleetSX=INVADERW;
	for (j=0;j<INVADERS_ROWS;j++)
		for (k=0;k<INVADERS_COLUMNS;k++,ix++) {
		
			spr = &InvaderSprites[ix];
			// generate invader fleet position for each sprite
			spr->x = Invader_XPOS(ix);
			InvaderXPOS[ix]=spr->x;
			spr->y = j*(INVADERH+10) + INVADERH*2;
	}
}

// main function
int main()
{
	int i,x,y;
	u8	iMarchInvaders=0;
	u16 gametick=0;
	sSprite* spr;
	sSprite* invader, *target;
	char ch;
	bool	GameOver=false;


	// copy font to RAM buffer
	memcpy(Font_Copy, FontBold8x16, sizeof(FontBold8x16));
	
	// initialize random number generator (we use constant value to get "nice" variant)
	RandSetSeed(5);


	Reset_Game();
	

	// initialize stdio
    stdio_init_all();
    
    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
    // Initialize JOYSTICK buttons
    gpio_init(joy_up_pin);
    gpio_set_dir(joy_up_pin, GPIO_IN);
    gpio_pull_up(joy_up_pin);

    gpio_init(joy_down_pin);    
    gpio_set_dir(joy_down_pin, GPIO_IN);
    gpio_pull_up(joy_down_pin);

    gpio_init(joy_left_pin);
    gpio_set_dir(joy_left_pin, GPIO_IN);
    gpio_pull_up(joy_left_pin);

    gpio_init(joy_right_pin);
    gpio_set_dir(joy_right_pin, GPIO_IN);
    gpio_pull_up(joy_right_pin);
    
    
    gpio_init(joy_fire1_pin);
    gpio_set_dir(joy_fire1_pin, GPIO_IN);
    gpio_pull_up(joy_fire1_pin);
    
    
    gpio_init(joy_fire2_pin);
    gpio_set_dir(joy_fire2_pin, GPIO_IN);
    gpio_pull_up(joy_fire2_pin);
    
	// run VGA core
	StartVgaCore();

	// initialize videomode
	VideoInit();
	
	//Sound Tone system for Pico
    // Initialize sound pin
    gpio_init(sound_pin);
    gpio_set_dir(sound_pin, GPIO_OUT);

// Print some text

	PrintSetup(Text, TEXTW, TEXTH, TEXTWB);
	PrintSetCol(PC_COLOR(PC_BLACK, PC_WHITE));
	PrintClear();


// Main chooser screen
	DispMainScreen();
// restore background image
	Reset_Background();
//Make some sound to intro the invaders;

	ptone.tone(440,2000, Pulse, 10,15);
	sleep_ms(2000); // Wait before starting	

// Print Title
	PrintSetup(Text, TEXTW, TEXTH, TEXTWB);
	PrintSetCol(PC_COLOR(PC_BLACK, PC_WHITE));
	PrintText(PageText);

	DispGameTitle();
	printf("Press any key to continue.\n");
	

// Start playing our intro syth 80's sound
// output PWM sound (sound must be PCM 8-bit mono 22050Hz)
//  snd = pointer to sound
//  len = length of sound in number of samples
//  speed = relative speed (1=normal)
//  rep = True to repeat sample
// Init PWM Sound
	PWMSndInit();
	
//PlaySound(Laser, 6536, false);

	
	x=9;
	while (GetChar()==0 && gpio_get(joy_right_pin) && gpio_get(joy_left_pin) && gpio_get(joy_up_pin) && gpio_get(joy_down_pin) && gpio_get(joy_fire1_pin) && gpio_get(joy_fire2_pin) ) {
		if (x++ >15) x=9;
		PrintSetPos(30,4);
		sleep_ms(50);
		PrintSetCol(PC_COLOR(PC_BLACK,x));
		PrintText("Press any key to Play");
		sleep_ms(50);
		}

	PrintClear();
	DispGameTitle();
	
// Send instructions to STDIO
	DispHelp();		
// Play our song
	PlaySound(MajorTomSnd, 1571584, true);
	// main loop
	while (true)
	{
/***	
	UpdateVars();
	UpdateBackground();
	UpdateScreen();
***/
	
/**************
 * Update Background
***************/
		// animate sky
		if (!GameOver) {
			if (!(gametick%200)) pScreen->strip[1].seg[0].offx += 4;

			if (pScreen->strip[1].seg[0].offx > CLOUDSW) pScreen->strip[1].seg[0].offx -= CLOUDSW;
			pScreen->strip[1].seg[0].offy += 1;
			if (pScreen->strip[1].seg[0].offy > CLOUDSH) pScreen->strip[1].seg[0].offy -= CLOUDSH;
			
		}
		
		
		// move shooter ship
		// read keyboard or input
		ch = GetChar();
		i=0;
		
		if (ch==KEY_OVER )	// Toggle Game Over
		{			
			GameOver=!GameOver;
			printf(" [%c] Toggle Game Over ", ch);
		}

		if (((ch==KEY_YES) || !gpio_get(joy_fire1_pin) || !gpio_get(joy_fire2_pin)) & GameOver )	// Reset Game
		{			
			GameOver=!GameOver;
			printf(" [%c] Play Again ", ch);
			Reset_Game();
			//Reset_Background();
			ResetInvadersFleetPosition();
			PrintSetPos(30,4);
			PrintText("GOOD LUCK! or else...    ");

			
		}


		if (!GameOver) {
			spr = &HotairSprites[0];

			x = spr->x + HotairDX[0];
			if (x < 0)	// reached limit of screen
			{
				x = 0;
//				HotairDX[i] = RandU8MinMax(HOTAIR_MINSPEED, HOTAIR_MAXSPEED);
				HotairDX[0] = 0;
			}
			
			if (x > (WIDTH - SPACESHIPW))	// limit of screen
			{			
				x = WIDTH - SPACESHIPW;
				HotairDX[0] = 0;
			}


			if (ch==KEY_R || !gpio_get(joy_right_pin))	// Move to the right
			{
				HotairDX[0] = HOTAIR_MINSPEED;
				printf(" [%c] right ", KEY_R);
			}


			
			if (ch==KEY_L || !gpio_get(joy_left_pin))	// Move to the left
			{			
				HotairDX[0] = -(HOTAIR_MINSPEED);
				printf(" [%c] left ", KEY_L);
			}
			

			spr->x = x;
			
			// decrement Ship speed, so it stop if no key is being press
			if (!(gametick % 10)) {
				if (HotairDX[0]>0) HotairDX[0]--;
				if (HotairDX[0]<0) HotairDX[0]++;
			}
			
/*** Y does not move
			y = spr->y + HotairDY[i];
			if (y < 0)
			{
				y = 0;
				HotairDY[i] = RandU8MinMax(HOTAIR_MINSPEED, HOTAIR_MAXSPEED);
			}

			if (y > HEIGHT - SPACESHIPH)
			{			
				y = HEIGHT - SPACESHIPH;
				HotairDY[i] = -RandU8MinMax(HOTAIR_MINSPEED, HOTAIR_MAXSPEED);
			}
			spr->y = y;
***/
		



		spr = &HotairSprites[1];
		if (Invaders.fired) {
		
		// ANIMATE INVADERS BOMB
		
			// the bomb moves y only, stays same x
			y = spr->y + Invaders.sSpeed;			
			
//			if (y > (HEIGHT-TEXTHEIGHT-SPACESHIPH))	// reach the top of screen, reset shot
			if (y > (HEIGHT-TEXTHEIGHT))	// reach the top of screen, reset shot
				{
					//it missed or was put away from screen;
					Invaders.fired=0;
				}
			spr->y = y;
			
			// Insert sound for invaders bomb, from low to high
			// if (!Player1.fired) ptone.tone(100 + y*2,20);

		// Invaders bomb	
		// Colision detection
		// Check X,Y pos on fences or Player1
		
		
			for (i = L0SPRITES_NUM; i < L0SPRITES_NUM+DEFENCE_NUM; i++)
			{
				target=&HotairSprites[i];

				// compare hit box for defence
				// broad detection for fence
				if (((target->y < (spr->y+spr->h)) &&  spr->y < (target->y+DEFENCEH)) &&
					((target->x < spr->x+spr->w) &&  spr->x< (target->x+DEFENCEW)) && !Invaders.is_hit) {
				// it's possible hit, go to narrow hit detection
				
				// TO DO
				// Insert narrow hit detection here.
				
						u8	HitPixel;
				// Perform narrow hit detection
				// TO DO - check box around bomb
				// for /// 8x8 box
				
						for (u16 xp=spr->x;xp<spr->x+spr->w;xp++) {
							for (u16 yp=spr->y;yp<spr->y+spr->h;yp++) {
								HitPixel=GetPixel(xp,yp, target); 	// This is the position inside the sprite where we hit
													
								if (HitPixel != 0xFF && HitPixel !=0x00) {
								// Yes, we hit
								// Yes, we hit the redmonians earth things
								// just for fun, blink the onboard LED
									gpio_put(led_pin, true);
						
									ptone.tone(300,300,Noise);
									

									Invaders.is_hit=1;
									Invaders.fired=0;
									Invaders.hit_target=target;
									Invaders.hit_targetx=spr->x+spr->w/2;
									Invaders.hit_targety=spr->y+spr->h;
		#ifdef _GAME_DEBUG_INFO								
									SetPixel(250,Invaders.hit_targetx - target->x,Invaders.hit_targety - target->y, target);
									PrintSetPos(40,0);
									PrintText("HitPixel:");
									PrintText(to_string(HitPixel));
									PrintText(" Tx:");
									PrintText(to_string(Invaders.hit_targetx - target->x));
									PrintText(" ");
									PrintText(" Ty:");
									PrintText(to_string(Invaders.hit_targety - target->y));
									PrintText(" ");
		#endif					

									CrashFence(Invaders.hit_targetx,Invaders.hit_targety,Invaders.hit_target);
								//InvertImg(target->img);
								// Hide bomb
								spr->y=500;
		#ifdef _GAME_DEBUG_INFO						
								sleep_ms(2000);
		#endif
								gpio_put(led_pin, false);

								break;
							}
							if (Invaders.is_hit) break;	// get out of here
						}
					}								
			}
		}


/*** Invaders hit our ship
 ***/
		// Check if invaders hit our shooter ship
			target=&HotairSprites[0];
			// compare hit box for shooter ship
			// broad detection for shooter ship
			if (((target->y < (spr->y+spr->h)) &&  spr->y < (target->y+target->h)) &&
				((target->x < spr->x+spr->w) &&  spr->x< (target->x+target->w)) && !Invaders.is_hit) {
					//InvertImg(target->img);
					Invaders.Score++;
					PrintSetPos(30,4);
					PrintText("Invaders Got You! ");
					// Play sound for ship wreck
					PlaySFX(Explosion2,66397,1);
					if (Invaders.Lives>0) Invaders.Lives--;
					
					Invaders.hit_target=target;
					Invaders.hit_targetx=spr->x+spr->w/2;
					Invaders.hit_targety=spr->y+spr->h;
					
					InvertImg(target->img);
					Invaders.is_hit=1;
					Invaders.fired=0;


				// Remove ship avatar from screen
					spr = &HotairSprites[Invaders.Lives+ShooterAvatar_Index];
					spr->x=WIDTH+100;	// Get it offscreen

					if (!Invaders.Lives) {
						GameOver=true;
						GameOverStart();
					}
					
					gpio_put(led_pin, true);
					//ptone.tone(1000,600,Noise);
					
					sleep_ms(1000);
			}
			
			
		}
		
		// animate fire shot
		
		spr = &HotairSprites[2];
		if (!Player1.fired) {	
			
			// the shot follows the ship
				spr->x = HotairSprites[0].x + SPACESHIPW/2;
				spr->y = HEIGHT-TEXTHEIGHT-SPACESHIPH-SHOOTH + 4;
		} else 	{
			// the shot moves y only, stays same x
					y = spr->y - Player1.sSpeed;
			// Print some debug info
#ifdef _GAME_DEBUG_INFO			
					PrintSetCol(PC_COLOR(PC_BLACK, PC_LTRED));
					PrintSetPos(1, 2);PrintText("Fire!! ");PrintText("X:    ");
					PrintSetPos(10, 2);					
					PrintText(to_string(spr->x));
					
					PrintSetPos(14, 2); PrintText("Y:     ");
					PrintSetPos(16, 2);
					PrintText(to_string(y));
#endif
				// Insert sound for missile, from high to low
				//	ptone.tone(440 + y*2,0);


					
					if (y < 0)	// reach the top of screen, reset shot
					{
						y = HEIGHT-TEXTHEIGHT-SPACESHIPH-SHOOTH + 4;
						
						spr->x=HotairSprites[0].x + SPACESHIPW/2;
						Player1.fired=0;
						
						// End fire sound
						//ptone.noTone();
					}
					spr->y = y;
					
					// Colision detection for invaders fleet
					// Check X,Y pos on invaders fleet
					// traverse the invaders fleet list

						for (i = 0; i < INVADERS_NUM; i++)
						{
							invader=&InvaderSprites[i];

							// broad compare hit box
							if (((invader->y < spr->y) &&  spr->y < (invader->y+INVADERH)) &&
								((invader->x < spr->x) &&  spr->x < (invader->x+INVADERW)) && !Player1.is_hit) {
							// Broad hit detected
									u8	*ThisImg=invader->img;
									u16	AimHit;
							// Perform narrow hit detection for invaders bomb		
									AimHit=invader->w*(spr->y - invader->y) + (spr->x - invader->x); 	// This is the position inside the sprite where we hit
									if (*(ThisImg+AimHit) != 0xFF && *(ThisImg+AimHit) !=0x00) {
										// Yes, we hit the invader
										// just for fun, blink the onboard LED
										gpio_put(led_pin, true);
							
										Player1.is_hit=true;
										Player1.Score++;
										Player1.fired=0;
										// End fire sound
										//ptone.noTone();
										Player1.hit_invader=invader;
										// insert explosion sound here
										// ptone.tone(200,200,Noise);
										PlaySFX(Explosion1,15398,1);

#ifdef _GAME_DEBUG_INFO						
										PrintText(" Zx:");
										PrintText(to_string(spr->x - invader->x));
										PrintText(" Zy:");
										PrintText(to_string(spr->y - invader->y));
#endif
										InvertImg(ThisImg);
									break;
								}
								if (Player1.is_hit) break;
							}
							if (Player1.is_hit) break;
						}
						//if (Player1.is_hit) break;
						
				}
				
			if (Player1.is_hit) {				
				Player1.hit_cycles++;

				if (Player1.hit_cycles>10) {
					InvertImg(Player1.hit_invader->img);
					Player1.is_hit=0;
					Player1.hit_cycles=0;
					Player1.hit_invader->x+=WIDTH*2;	// Take it off screen, hide
					// turn off the on board LED
					gpio_put(led_pin, false);
					
					// Count how many invaders are left on screen
					// Check X,Y pos on invaders fleet
					// traverse the invaders fleet list
					Invaders.ships_alive=0;
						for (i = 0; i < INVADERS_NUM; i++)
						{
							invader=&InvaderSprites[i];
							if (invader->x < WIDTH) Invaders.ships_alive++;
						}
					
				}

			}


// Stop blink effect for invader's target			
			if (Invaders.is_hit) {
				Invaders.hit_cycles++;

				if (Invaders.hit_cycles>10) {
					Invaders.is_hit=0;
					Invaders.hit_cycles=0;
					// turn off the on board LED
					gpio_put(led_pin, false);
					MeltFence(Invaders.hit_targetx,Invaders.hit_targety,Invaders.hit_target);
				}

			}


			if (!GameOver && !(gametick % 240) && !Invaders.ships_alive) {		
				// appear a new fleet of invaders
				ResetInvadersFleetPosition();
				Invaders.ships_alive=INVADERS_NUM;
				Rebuild_Defenses();
				Player1.level++;
				// Make game harder
				Invaders.sSpeed+= (Player1.level%3) ? 0:1;
				if (Invaders.sSpeed>12) Invaders.sSpeed=12;
				
				// Give an extra ship every 4 levels
				Player1.Lives+=(Player1.level%4) ? 0:1;
				
				// After level 10 the invaders respawn, you have to kill all the batch
				// before they reach the end
				Invaders.respanwn=(Player1.level>=10) ? true:false;
				
				// Make invaders faster
				Invaders.ySpeed+=(Player1.level<16 && Player1.level>3 && (Player1.level%4)) ? 0:1;
				FleetDX+=(FleetDX>0 && Player1.level<16 && Player1.level>3 && (Player1.level%4)) ? 0:1;
			}
			
		// animate invaders sprites

			if (!(gametick % 20)) {		// slow move the invaders fleet vertically
				for (i = 0; i < INVADERS_NUM; i++)
				{
					spr = &InvaderSprites[i];
					y = spr->y + Invaders.ySpeed;
					//x = spr->x + FleetDX;
					if (y > HEIGHT-100) {		// invader row hit the bottom of screen			
						y=INVADERH*2;				
						//x=InvaderXPOS[i]+FleetSX;

						// Only restore position for sprites inside screen view
						//if (Invaders.respanwn && x<WIDTH) {
						//	spr->x = x;
						//	}
					}
					spr->y = y;

				}
			}
						
		if (!(gametick % 64) & !GameOver) {	// Slide the invaders fleet, move horizontally
			// FleetSX is the X Home position horizontally
			// FleetSX will move between INVADERW and INVADERW*8
			// FleetDX is the X delta position horizontally, 
			// FleetDX will be a positive or negative integer
			// a sprite will draw at
			// invader->x=FleetDX+Invader->x

			if (FleetSX > INVADERW*8 || FleetSX<INVADERW ) {	
				// Keep within screen limits
				FleetDX=-FleetDX;
			} 
			
			
			// move the fleet horizontally
			for (i = 0; i<INVADERS_NUM; i++)
				{
					invader=&InvaderSprites[i];
					invader->x+=FleetDX;
					
				}
			FleetSX+=FleetDX;
			// play sound for invaders fleet move	
			if (!Player1.fired && !Invaders.fired) {
				//ptone.tone(120,100); 
				switch (iMarchInvaders++) { 
					case 0:PlaySFX(MoveLeft, 5035, 1.2f);break;
					case 1:PlaySFX(MoveLeft, 5035, 1.1f);break;
					case 2:PlaySFX(MoveLeft, 5035, 1.0f);break;

				//ptone.tone(60,100);
					case 3:PlaySFX(MoveRight,5035, 0.8f);break;
					case 4:iMarchInvaders=0;break;
				}
			}

		}
		
		if (!(gametick % 20)) {
		// change palette color, blink effect			
		for (u16 wpx=0; wpx < sizeof(Fire2Img); wpx++) {
			if (FireInv_Copy[wpx]==0xFD) FireInv_Copy[wpx]=4;							 			
			if (FireInv_Copy[wpx]==0xE4) FireInv_Copy[wpx]=3;							 			
			}
		 }

	}

		if (GameOver) {	// Slide the invaders fleet game over
					
			FleetDX=4;
	
				// move the fleet horizontally
				for (i = 0; i < INVADERS_NUM; i++)
					{
						invader=&InvaderSprites[i];
						if (invader->x < WIDTH)
						invader->x+=FleetDX;
					}
					
		// animate BACKGROUND when game is over, just scroll horizontally
			if (!(gametick%50)) pScreen->strip[1].seg[0].offx += 4;
			if (pScreen->strip[1].seg[0].offx > CLOUDSW) pScreen->strip[1].seg[0].offx -= CLOUDSW;			

                          			
		}


		// delay
		sleep_ms(20);	// should be  40 for normal speed
		// Print sound delay var
		//PrintSetPos(56, 3);
		//PrintText(to_string(ptone.snddelay));PrintText(" ");


		if (GameOver & !(gametick+20 % 40)) {	// restore background, later blink the Game Over sign
			DrawImg(&Canvas, &Block3Canvas, 0, 0, 0,0, BLOCK3W, BLOCK3H);			
		}
		
		if (GameOver & !(gametick % 40)) { // blink the Game Over sign
			DrawImg(&Canvas, &GameOverCanvas, (CLOUDSW-GAMEOVERW)/2, 30, 0,0, GAMEOVERW, GAMEOVERH);			
		}
		
/*		
*/	
		
		
		if (!(gametick % 40)) {
			// change palette color, blink effect			
			for (u16 wpx=0; wpx < sizeof(Fire2Img); wpx++) {
				if (FireInv_Copy[wpx]==0x04) FireInv_Copy[wpx]=0xFD;
				if (FireInv_Copy[wpx]==0x03) FireInv_Copy[wpx]=0xE4;							 			
			}
		}

		
		// Try Fire
		if (!Player1.fired && (ch==KEY_FIRE || !gpio_get(joy_fire1_pin) || !gpio_get(joy_fire2_pin))) {
		// generate player1 fire
			Player1.fired=1;
			PlaySFX(Laser, 6536, 1.0f);
			printf(" [%c] Fire ", KEY_FIRE);
		}
		
		if ((!GameOver) & !((gametick) %  60) && !Invaders.fired) {
		// generate invaders fire
			if (RandU8Max(10)>3) {
				// Choose an invader to fire / take position
				for (i=INVADERS_NUM - 1;i+1>0;i--) {
					// if invader is alive, test it
					invader=&InvaderSprites[i];
					if (RandU8Max(16+i/3)>10 && invader->x < WIDTH) {
						spr = &HotairSprites[1];
						
						spr->x= invader->x + INVADERW/2 + SHOOTINVW/2;
						spr->y= invader->y;
						Invaders.fired=1;
						break;
					}
				}
				// Play sound for invaders bomb
				PlaySFX(InvaderShoot,11996,1);
			}
		}
		
		if (!(gametick % 200)) {
			// Send instructions to STDIO
			
			if (GameOver) {	
					PrintSetPos(30,4);
					PrintText("Press Fire to play again.");

				printf("Offx=%u  Offy=%u   ",pScreen->strip[1].seg[0].offx,pScreen->strip[1].seg[0].offy);
				printf("\n>>>> GAME OVER <<<< Do you want to play again (Y/N/X) ? ");
			} else DispHelp();
		}
		gametick++;
		DisplayStats();
	
	}
	printf("¿Por qué estamos aquí???\n");
	PrintText("Invaders Win! ");

}
