//#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <peekpoke.h>
//#include <c64.h>
//#include <cbm.h>

#define SPRITE_SIZE				64U

#define PLAYER_SPRITE           0U /*1*/
#define SHOT_SPRITE		        1U /*2*/
#define DEATH_SPRITE            2U /*4*/
#define ENEMY1_SPRITE           3U /*8*/
#define ENEMY2_SPRITE           4U /*16*/
#define ENEMY3_SPRITE           5U /*32*/
#define ENEMY4_SPRITE           6U /*64*/
//#define ENEMY5_SPRITE           7 /*128*/

#define MAX_DEATH_FRAMES        18U

#define PLAYER_ANIM_NORM        0U
#define SHOT_ANIM_LASER1		1U
#define DEATH_ANIM_EXPLODE      2U
#define ENEMY_ANIM_SPINNER      3U
//#define ENEMY_ANIM_POTATO     4U
#define ENEMY_ANIM_EGG          4U
#define ENEMY_ANIM_VIRUS        5U
#define DEATH_ANIM_PLAYER       6U

#define NO_ANIM					99U

#define CLR_BLACK				0U
#define CLR_WHITE             	1U
#define CLR_RED               	2U
#define CLR_CYAN              	3U
#define CLR_VIOLET            	4U
#define CLR_GREEN             	5U
#define CLR_BLUE              	6U
#define CLR_YELLOW            	7U
#define CLR_ORANGE            	8U
#define CLR_BROWN             	9U
#define CLR_PINK              	10U
#define CLR_DARK_GRAY         	11U
#define CLR_GREY              	12U
#define CLR_LIGHT_GREEN       	13U
#define CLR_LIGHT_BLUE        	14U
#define CLR_LIGHT_GRAY        	15U

/*
#define SPRITE_BANK_PLAYER_NORM1       	    240U
#define SPRITE_BANK_PLAYER_NORM2       	    241U
#define SPRITE_BANK_PLAYER_NORM3       	    242U
#define SPRITE_BANK_PLAYER_NORM4      	    243U
*/
#define SPRITE_BANK_PLAYER_NORM       	    243U
#define SPRITE_BANK_SHOT         	        244U
#define SPRITE_BANK_ENEMY1					245U
#define SPRITE_BANK_ENEMY2					246U
#define SPRITE_BANK_ENEMY3					247U
#define SPRITE_BANK_ENEMY4					248U
#define SPRITE_BANK_ENEMY5					249U
#define SPRITE_BANK_ENEMY6					250U
#define SPRITE_BANK_ENEMY7					251U
#define SPRITE_BANK_ENEMY8					252U
#define SPRITE_BANK_DEATH1                  253U
#define SPRITE_BANK_DEATH2                  254U
#define SPRITE_BANK_DEATH3                  255U

#define MIN_PLAYER_X                        24
#define MAX_PLAYER_X                        320
#define MIN_PLAYER_Y                        45U
#define MAX_PLAYER_Y                        229U

#define MoveSprite(sprite_number, x, y) \
	if (x > 255) \
	{ \
		/* if (!((PEEK(0xD010) >> sprite_number) & 1U)) */ \
			POKE(0xD010, (PEEK(0xD010) & ~(1U << sprite_number)) | (1U << sprite_number)); \
		POKE(0xD000 + sprite_number * 2, x - 256); \
    } \
	else \
	{ \
		/*if (!((PEEK(0xD010) >> sprite_number) & 1U)) */ \
			POKE(0xD010, (PEEK(0xD010) & ~(1U << sprite_number)) | (0U << sprite_number)); \
		POKE(0xD000 + sprite_number * 2, x); \
	} \
	POKE(0xD000 + sprite_number * 2 + 1, y); \

#define MoveSpriteToOrigin(sprite_number) \
	POKE(0xD000 + sprite_number * 2, 0U); \
	POKE(0xD000 + sprite_number * 2 + 1, 0U); \
	
#define ShowTitleScreen() \
	ClearAllText(CLR_YELLOW); \
	Generate_Stars(); \
    gotoxy(13U,10U); \
    cprintf("LASER INVADERS"); \
	do { \
		SetTextColor(RandomBrightColor(), 13U, 10U, 14U); \
		waitvsync(); \
	} while (!kbhit() && (PEEK(0xDC00) & 16U)); \

#define AnimateStar(Star, Stardead, Starcol)  		if (Stardead == 0) \
													{ \
														POKE(Star,32U); \
														if (Starcol > 39) \
														{ \
															Star = 0x0400 + (rand_range(2U,22U) * 40U) + 40U; \
															Stardead = rand_range(1U,10U); \
															Starcol = 39; \
														} \
														else \
														{ \
															Star -= 1; \
															Starcol -=1; \
															POKE(Star,99U); \
														} \
													} \
													else \
														Stardead -= 1; \

	
#define rand_range(a, b)				   				(rand() % (b + 1U - a) + a)
#define SetFlag(address, flag_number, flag_value)		POKE(address, (PEEK(address) & ~(1UL << flag_number)) | (flag_value << flag_number))
#define GetFlag(address, flag_number);					(PEEK(address) >> flag_number) & 1U
#define EnableSprite(sprite_number)						SetFlag(0xD015, sprite_number, 1U)
#define DisableSprite(sprite_number)					SetFlag(0xD015, sprite_number, 0U)
#define EnableMultiColorSprite(sprite_number) 			SetFlag(0xD01C, sprite_number, 1U)
#define SpriteCollision(sprite_number)					(PEEK(0xD01E) & (1U << sprite_number))
#define ClearAllSpriteCollisions()						POKE(0xD01E, 0U)
#define ShowClearScore() 								gotoxy(2U, 0U); 	score = 0U; cprintf("Score 0        "); memset((void *)0xD800, CLR_CYAN, 20UL)
#define ShowClearLives() 								gotoxy(30U, 0U); 	lives = 3U; cprintf("Lives 3  "); memset((void *)0xD814, CLR_WHITE, 20UL)
#define ShowScore(Score) 								gotoxy(2U, 0U); 	cprintf("Score %i00",Score); memset((void *)0xD800, CLR_CYAN, 20UL)
#define ShowLives(Lives) 								gotoxy(30U, 0U); 	cprintf("Lives %i",Lives); memset((void *)0xD814, CLR_WHITE, 20UL)
#define GetSpriteFrame(sprite_number)					PEEK(0x07F8+sprite_number)
#define SetSpriteFrame(sprite_number, bank_number)		POKE(0x07F8+sprite_number, bank_number)
#define ClearSpriteCollision(sprite_number)				POKE(0xD01E, PEEK(0xD01E) | (0U << sprite_number))		
#define GetSpriteColor(sprite_number)                   PEEK(0xD027+sprite_number)
#define SetSpriteColor(sprite_number, Color)			POKE(0xD027+sprite_number, Color)
#define SetSpriteMultiColor1(Color)						POKE(0xD025, Color)
#define SetSpriteMultiColor2(Color)						POKE(0xD026, Color)
#define ShowStar(pos, c)    							POKE(0x0400+pos,46U); POKE(0xD800+pos,c)
#define CONST_POKE(a,b)                                 __asm__ ("lda #%b", b);  __asm__ ("sta %w", a)

typedef struct  
{
	unsigned char Current_Animation;
	int Animate_Direction;
	unsigned char Frame_Mod;
	int x;
	int y;
	int xvel;
	int yvel;
	unsigned char alive;
	unsigned char lifetime;
} Entity;

static Entity player 	= { /*Current_Animation*/NO_ANIM,  /*Animate_Direction*/1, /*Frame_Mod*/4, /*x*/0, /*y*/0, /*xvel*/0, /*yvel*/0, /*live*/1, /*ifetime*/0};
static Entity shot   	= { /*Current_Animation*/NO_ANIM,  /*Animate_Direction*/1, /*Frame_Mod*/0, /*x*/0, /*y*/0, /*xvel*/0, /*yvel*/0, /*live*/0, /*ifetime*/0};
static Entity death  	= { /*Current_Animation*/NO_ANIM,  /*Animate_Direction*/1, /*Frame_Mod*/0, /*x*/0, /*y*/0, /*xvel*/0, /*yvel*/0, /*live*/0, /*ifetime*/0};
static Entity enemy1 	= { /*Current_Animation*/NO_ANIM,  /*Animate_Direction*/1, /*Frame_Mod*/7, /*x*/0, /*y*/0, /*xvel*/0, /*yvel*/0, /*live*/0, /*ifetime*/0};
static Entity enemy2    = { /*Current_Animation*/NO_ANIM,  /*Animate_Direction*/1, /*Frame_Mod*/7, /*x*/0, /*y*/0, /*xvel*/0, /*yvel*/0, /*live*/0, /*ifetime*/0};
static Entity enemy3 	= { /*Current_Animation*/NO_ANIM,  /*Animate_Direction*/1, /*Frame_Mod*/7, /*x*/0, /*y*/0, /*xvel*/0, /*yvel*/0, /*live*/0, /*ifetime*/0};
static Entity enemy4 	= { /*Current_Animation*/NO_ANIM,  /*Animate_Direction*/1, /*Frame_Mod*/7, /*x*/0, /*y*/0, /*xvel*/0, /*yvel*/0, /*live*/0, /*ifetime*/0};
static Entity* enemy;

static unsigned char *screen = (unsigned char *) 0x0400;
static unsigned char *screen_colors = (unsigned char *) 0xD800;

static unsigned char enemy_sprite_number;
static unsigned char frame_count;
static unsigned char revive;
static unsigned int score = 0;
static unsigned int high_score = 0;
static unsigned char lives = 3;
static unsigned char next_wave_delay = 0;
static unsigned char death_anim = DEATH_ANIM_EXPLODE;

static unsigned int star1;
static unsigned int star2;
static unsigned int star3;
static unsigned int star4;
static unsigned int star5;

static unsigned char starcol1 = 39;
static unsigned char starcol2 = 39;
static unsigned char starcol3 = 39;
static unsigned char starcol4 = 39;
static unsigned char starcol5 = 39;

static unsigned int stardead1 = 0;
static unsigned int stardead2 = 0;
static unsigned int stardead3 = 0;
static unsigned int stardead4 = 0;
static unsigned int stardead5 = 0;

static unsigned int laser_freq = 0;


const unsigned char player_norm[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0xB4,0x0A,0xD4,0xBD,0x0F,0xFF,0x03,0x40,0xC0,0x00,0xD0,0xC0,0x00,0x34,
	0xC0,0x00,0x7F,0x70,0x09,0xF5,0x5C,0x0B,0xD7,0xF4,0x09,0xF5,0x5C,0x00,0x7F,0x70,
	0x00,0x34,0xC0,0x00,0xD0,0xC0,0x03,0x40,0xC0,0xBD,0x0F,0xFF,0xB4,0x0A,0xD4,0x81};

const unsigned char player_shot[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x55,0x55,0x55,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x81};

const unsigned char death_explode1[] = {
	0x00,0x00,0x00,0x00,0x02,0x00,0x22,0x00,0x88,0x08,0x20,0x00,0x02,0x02,0x20,0x08,
	0xA8,0x00,0x02,0x82,0x80,0x8A,0xA8,0x82,0x22,0xAA,0xA0,0x02,0xAA,0x28,0x0A,0xAA,
	0x00,0x2A,0xAA,0x80,0x00,0xAA,0xA2,0x22,0xAA,0x20,0x02,0xAA,0x00,0x0A,0xAA,0x80,
	0x00,0x28,0x02,0x08,0x88,0xA0,0x20,0x20,0x00,0x88,0x80,0x00,0x00,0x00,0x80,0x81};
	
const unsigned char death_explode2[] = {
	0x00,0x00,0x02,0x88,0x22,0x00,0x00,0x00,0x00,0x02,0x28,0x08,0x00,0x02,0x00,0x20,
	0x00,0xA0,0x00,0x80,0x00,0x08,0x28,0x08,0x00,0x2A,0x20,0x28,0xA8,0x0A,0x00,0x2A,
	0x00,0x80,0xA8,0x80,0x00,0xAA,0x00,0x80,0x2A,0x00,0x00,0x88,0x08,0x00,0x20,0x08,
	0x20,0x00,0x02,0x00,0x08,0x00,0x88,0x20,0xA2,0x00,0x88,0x00,0x88,0x20,0x00,0x87};
	
const unsigned char death_explode3[] = {
	0x00,0x00,0x00,0x00,0x08,0x80,0x20,0x00,0x00,0x00,0x80,0x00,0x08,0x08,0x08,0x00,
	0x00,0x00,0x08,0x00,0x20,0x00,0x08,0x00,0x00,0x2A,0x00,0x20,0x88,0x08,0x00,0x2A,
	0x00,0x00,0xA8,0x00,0x20,0x22,0x20,0x00,0x08,0x00,0x00,0x00,0x08,0x02,0x00,0x00,
	0x00,0x02,0x20,0x00,0x00,0x00,0x20,0x20,0x00,0x00,0x02,0x08,0x00,0x00,0x00,0x87};

const unsigned char enemy_spinner1[] = {
	0x00,0xAA,0xA0,0x02,0x55,0x40,0x00,0xAA,0xA0,0x00,0x28,0x00,0x00,0x28,0x00,0x00,
	0xBE,0x00,0x02,0xFF,0xC0,0x0B,0xAA,0xA0,0x0A,0xBF,0x94,0x39,0x55,0x68,0xA5,0x55,
	0x55,0x39,0x55,0x68,0x0A,0xBF,0x94,0x0B,0xAA,0xA0,0x02,0xFF,0xC0,0x00,0xBE,0x00,
	0x00,0x28,0x00,0x00,0x28,0x00,0x00,0xAA,0xA0,0x02,0x55,0x40,0x00,0xAA,0xA0,0x86};

const unsigned char enemy_spinner2[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xAA,0xA0,0x02,0x55,0x40,0x00,0xAA,0xA0,0x02,0xAA,0xC0,0x2A,0xD7,0xA8,0xA5,0x55,
	0x55,0x39,0x55,0x68,0x0A,0xFF,0x80,0x00,0xAA,0xA0,0x02,0x55,0x40,0x00,0xAA,0xA0,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x86};

const unsigned char enemy_spinner3[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x15,0x00,0x00,0x7F,0x50,0x01,0xD5,0x64,0xBA,0xAA,
	0xAD,0x0A,0x95,0x44,0x00,0x2A,0xA0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x86};

const unsigned char enemy_spinner4[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x15,0x60,0x01,0xAA,0xA4,0xBE,0x55,
	0x7D,0x01,0xAA,0xA4,0x00,0x15,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x86};

const unsigned char enemy_spinner5[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2A,0xA0,0x0A,0x95,0x44,0xBA,0xAA,
	0xAD,0x01,0xD5,0x64,0x00,0x7F,0x50,0x00,0x15,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x86};

const unsigned char enemy_spinner6[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xAA,0xA0,0x02,0x55,0x40,0x00,0xAA,0xA0,0x0A,0xFF,0x80,0x39,0x55,0x68,0xA5,0x55,
	0x55,0x2A,0xD7,0xA8,0x02,0xAA,0xC0,0x00,0xAA,0xA0,0x02,0x55,0x40,0x00,0xAA,0xA0,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x86};

/*
const unsigned char enemy_potato1[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x02,
	0xAA,0x00,0x09,0xAA,0x80,0x0B,0xA6,0xA0,0x0A,0xAE,0xA0,0x0A,0x6A,0xA0,0x2A,0xE6,
	0x68,0x26,0xAE,0xE8,0x2E,0xAA,0xA8,0x0A,0x9A,0x98,0x0A,0xB9,0xB8,0x02,0xAB,0xA0,
	0x00,0x0A,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x89};
	
const unsigned char enemy_potato2[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x0A,0x98,0x00,0x0A,0xB8,0x00,0x0A,
	0xBA,0x00,0x0D,0xAA,0x00,0x0A,0x9A,0x80,0x0A,0xB9,0x80,0x0A,0xA9,0x80,0x0A,0xAA,
	0x80,0x02,0x66,0xA0,0x02,0xEA,0xA0,0x00,0xA9,0xA0,0x00,0x9B,0xA0,0x00,0x9A,0xA0,
	0x00,0x3A,0xA0,0x00,0x29,0xA0,0x00,0x0B,0x80,0x00,0x02,0x80,0x00,0x00,0x00,0x89};
	
const unsigned char enemy_potato3[] = {
	0x00,0x28,0x00,0x00,0x28,0x00,0x00,0x9A,0x00,0x00,0x9A,0x00,0x02,0xAB,0x80,0x02,
	0xAB,0x80,0x02,0xA6,0x80,0x02,0xA6,0x80,0x02,0x6A,0x80,0x02,0x6A,0x80,0x02,0xB9,
	0x80,0x02,0xB9,0x80,0x03,0xAA,0x80,0x03,0xAA,0x80,0x02,0xBA,0x00,0x02,0xBA,0x00,
	0x02,0x6A,0x00,0x02,0x6A,0x00,0x00,0xA0,0x00,0x00,0xA0,0x00,0x00,0x00,0x00,0x89};
	
const unsigned char enemy_potato4[] = {
	0x00,0x00,0x00,0x00,0x02,0x80,0x00,0x0A,0xA0,0x00,0x2B,0x60,0x00,0x2A,0xA0,0x00,
	0xAA,0xA0,0x02,0x9A,0xA0,0x03,0xAB,0xA0,0x09,0xA9,0xD0,0x09,0xBA,0xA0,0x0A,0x9A,
	0xA0,0x2E,0xAE,0x80,0x26,0x66,0x80,0x2A,0xAA,0x80,0x2A,0xAA,0x00,0x0A,0xA8,0x00,
	0x08,0xA0,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x89};
	
const unsigned char enemy_potato5[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xA0,0x00,0x0A,0xEA,0x80,0x2E,
	0x6E,0xA0,0x26,0xA6,0xA0,0x2A,0xAA,0xB8,0x2B,0xBA,0x98,0x29,0x9B,0xA8,0x0A,0xA9,
	0xA0,0x0A,0xBA,0xA0,0x0A,0x9A,0xE0,0x02,0xAA,0x60,0x00,0xAA,0x80,0x00,0x08,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x89};
	
const unsigned char enemy_potato6[] = {
	0x00,0xA0,0x00,0x02,0xB8,0x00,0x02,0xB8,0x00,0x02,0xAA,0x00,0x0A,0xA7,0x00,0x0A,
	0x6A,0x80,0x0A,0xAA,0x80,0x0A,0xAA,0xC0,0x02,0xAE,0xA0,0x02,0xA6,0xA0,0x02,0xBA,
	0xA0,0x02,0xAB,0xA8,0x00,0xAB,0xB8,0x00,0xAA,0xB0,0x00,0xAA,0x98,0x00,0x27,0xA8,
	0x00,0x0A,0xA0,0x00,0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x89};
	
const unsigned char enemy_potato7[] = {
	0x00,0x0A,0x80,0x00,0x0A,0x80,0x00,0x2A,0xC0,0x00,0x2A,0xC0,0x00,0xA6,0xA0,0x00,
	0xA6,0xA0,0x00,0xAA,0x60,0x00,0xAA,0x60,0x02,0xB6,0xA0,0x02,0xB6,0xA0,0x02,0xAA,
	0xC0,0x02,0xAA,0xC0,0x00,0xAE,0x80,0x00,0xAE,0x80,0x00,0x6A,0x80,0x00,0x6A,0x80,
	0x00,0xAB,0x80,0x00,0xAB,0x80,0x00,0x0A,0x00,0x00,0x0A,0x00,0x00,0x00,0x00,0x89};
	
const unsigned char enemy_potato8[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x80,0x00,0x2A,0xA8,0x00,
	0x2A,0xA8,0x00,0xAA,0x6A,0x00,0x9A,0xEE,0x02,0xAA,0xAE,0x02,0xAA,0xAA,0x09,0xA6,
	0xA8,0x0B,0xBA,0x70,0x0B,0xBA,0xA0,0x0A,0xAB,0x80,0x02,0xAB,0x80,0x02,0x6A,0x00,
	0x02,0xE8,0x00,0x02,0xE8,0x00,0x02,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x89};
*/

const unsigned char enemy_egg1[] = {
	0x00,0x14,0x00,0x00,0x55,0x00,0x01,0x55,0x40,0x05,0x55,0x50,0x05,0x55,0x50,0x15,
	0x55,0x54,0x15,0x69,0x54,0x55,0xAA,0x55,0x56,0xAA,0x95,0x56,0xAA,0x95,0x56,0xAA,
	0x95,0x56,0xAA,0x95,0x56,0xAA,0x95,0x55,0xAA,0x55,0x15,0x69,0x54,0x15,0x55,0x54,
	0x05,0x55,0x50,0x05,0x55,0x50,0x01,0x55,0x40,0x00,0x55,0x00,0x00,0x14,0x00,0x82};
	
const unsigned char enemy_egg2[] = {
	0x00,0x1C,0x00,0x00,0x14,0x00,0x00,0x57,0x00,0x01,0x55,0x40,0x01,0x55,0xC0,0x01,
	0x55,0x40,0x05,0x69,0x50,0x05,0xAA,0x70,0x06,0xAA,0x50,0x06,0xAA,0x70,0x06,0xAA,
	0x50,0x06,0xAA,0x70,0x06,0xAA,0x50,0x05,0xAA,0x70,0x05,0x69,0x50,0x01,0x55,0x40,
	0x01,0x55,0xC0,0x01,0x55,0x40,0x00,0x57,0x00,0x00,0x14,0x00,0x00,0x1C,0x00,0x82};
	
const unsigned char enemy_egg3[] = {
	0x00,0x1C,0x00,0x00,0x14,0x00,0x00,0x57,0x00,0x00,0x57,0x00,0x00,0x55,0x00,0x01,
	0x55,0x40,0x01,0x65,0xC0,0x01,0xA9,0x40,0x02,0xA9,0xC0,0x02,0xA9,0x40,0x02,0xA9,
	0xC0,0x02,0xA9,0x40,0x02,0xA9,0xC0,0x01,0xA9,0x40,0x01,0x65,0xC0,0x01,0x55,0x40,
	0x00,0x55,0x00,0x00,0x57,0x00,0x00,0x57,0x00,0x00,0x14,0x00,0x00,0x1C,0x00,0x82};
	
const unsigned char enemy_egg4[] = {
	0x00,0x3C,0x00,0x00,0x1C,0x00,0x00,0x34,0x00,0x00,0x1C,0x00,0x00,0x34,0x00,0x00,
	0x1C,0x00,0x00,0x67,0x00,0x00,0xA7,0x00,0x02,0xA5,0x80,0x02,0xA7,0x80,0x02,0xA5,
	0x80,0x02,0xA7,0x80,0x02,0xA5,0x80,0x00,0xA7,0x00,0x00,0x67,0x00,0x00,0x1C,0x00,
	0x00,0x14,0x00,0x00,0x1C,0x00,0x00,0x14,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,0x82};
	
const unsigned char enemy_egg5[] = {
	0x00,0x3C,0x00,0x00,0x1C,0x00,0x00,0x34,0x00,0x00,0x1C,0x00,0x00,0x34,0x00,0x00,
	0x1C,0x00,0x00,0xB6,0x00,0x02,0x9E,0x80,0x0A,0xB6,0xA0,0x0A,0x9E,0xA0,0x0A,0xB6,
	0xA0,0x0A,0x9E,0xA0,0x0A,0xB6,0xA0,0x02,0x9E,0x80,0x00,0xB6,0x00,0x00,0x1C,0x00,
	0x00,0x34,0x00,0x00,0x1C,0x00,0x00,0x34,0x00,0x00,0x1C,0x00,0x00,0x34,0x00,0x82};
	
const unsigned char enemy_egg6[] = {
	0x00,0x3C,0x00,0x00,0x34,0x00,0x00,0x1C,0x00,0x00,0x34,0x00,0x00,0x1C,0x00,0x00,
	0x34,0x00,0x00,0xD9,0x00,0x00,0xDA,0x00,0x02,0x5A,0x80,0x02,0xDA,0x80,0x02,0x5A,
	0x80,0x02,0xDA,0x80,0x02,0x5A,0x80,0x00,0xDA,0x00,0x00,0xD9,0x00,0x00,0x34,0x00,
	0x00,0x14,0x00,0x00,0x34,0x00,0x00,0x14,0x00,0x00,0x34,0x00,0x00,0x34,0x00,0x82};
	
const unsigned char enemy_egg7[] = {
	0x00,0x34,0x00,0x00,0x14,0x00,0x00,0xD5,0x00,0x00,0xD5,0x00,0x00,0x55,0x00,0x01,
	0x55,0x40,0x03,0x59,0x40,0x01,0x6A,0x40,0x03,0x6A,0x80,0x01,0x6A,0x80,0x03,0x6A,
	0x80,0x01,0x6A,0x80,0x03,0x6A,0x80,0x01,0x6A,0x40,0x03,0x59,0x40,0x01,0x55,0x40,
	0x00,0x55,0x00,0x00,0xD5,0x00,0x00,0xD5,0x00,0x00,0x14,0x00,0x00,0x34,0x00,0x82};
	
const unsigned char enemy_egg8[] = {
	0x00,0x34,0x00,0x00,0x14,0x00,0x00,0xD5,0x00,0x01,0x55,0x40,0x03,0x55,0x40,0x01,
	0x55,0x40,0x05,0x69,0x50,0x0D,0xAA,0x50,0x05,0xAA,0x90,0x0D,0xAA,0x90,0x05,0xAA,
	0x90,0x0D,0xAA,0x90,0x05,0xAA,0x90,0x0D,0xAA,0x50,0x05,0x69,0x50,0x01,0x55,0x40,
	0x03,0x55,0x40,0x01,0x55,0x40,0x00,0xD5,0x00,0x00,0x14,0x00,0x00,0x34,0x00,0x82};

const unsigned char enemy_virus1[] = {
	0x00,0x20,0x10,0x00,0x20,0x10,0x02,0x22,0x20,0x22,0x12,0x90,0x38,0xA2,0x40,0x0B,
	0x82,0x82,0x01,0x29,0x26,0x00,0xAA,0xA8,0x13,0x6A,0x60,0x15,0xAA,0x90,0x0D,0xAA,
	0x40,0x6A,0xAA,0xA8,0x00,0xA6,0x64,0x01,0xA6,0x99,0x0A,0x6A,0x04,0x0C,0xA1,0x03,
	0x00,0x60,0x80,0x02,0x20,0x80,0x02,0x10,0x80,0x02,0x20,0xC0,0x00,0x10,0x00,0x82};
	
const unsigned char enemy_virus2[] = {
	0x00,0x00,0x00,0x00,0x23,0x10,0x02,0x22,0x20,0x02,0x12,0x60,0x30,0xA3,0x40,0x0B,
	0x83,0x82,0x01,0x29,0x24,0x00,0xAA,0xA8,0x03,0x6A,0x60,0x15,0xAA,0x90,0x0D,0xAA,
	0x40,0x1A,0xAA,0xA8,0x00,0xAE,0x64,0x01,0xAE,0x99,0x0A,0x6A,0x04,0x00,0xA1,0x04,
	0x00,0xA0,0x82,0x02,0x20,0x80,0x02,0x10,0x80,0x08,0x20,0xC0,0x00,0x10,0x00,0x82};
	
const unsigned char enemy_virus3[] = {
	0x00,0x00,0x00,0x04,0x02,0x00,0x02,0x12,0x20,0x02,0x22,0x90,0x0C,0x93,0x80,0x0D,
	0x83,0xC2,0x02,0x29,0x24,0x00,0xAA,0xA8,0x43,0x6A,0x60,0x15,0xAA,0x94,0x09,0xAA,
	0x40,0x20,0xAA,0xA8,0x10,0xAE,0x64,0x02,0xAE,0x91,0x00,0x6A,0x04,0x00,0xA2,0x08,
	0x00,0xA0,0x81,0x02,0x10,0x80,0x02,0x20,0xC0,0x08,0x10,0x00,0x08,0x00,0x00,0x82};
	
const unsigned char enemy_virus4[] = {
	0x08,0x02,0x00,0x08,0x02,0x00,0x02,0x22,0x00,0x02,0x12,0x90,0x30,0xA2,0x40,0x0B,
	0x83,0xC3,0x01,0x29,0x24,0x00,0xAA,0xA8,0x03,0x6A,0x60,0x15,0xAA,0xA4,0x02,0xAA,
	0x40,0x28,0xAA,0xA8,0x10,0xAE,0x68,0x02,0xAE,0x98,0x08,0x6A,0x04,0x00,0x92,0x00,
	0x00,0x90,0x82,0x02,0x20,0xC0,0x02,0x20,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x82};
	
const unsigned char enemy_virus5[] = {
	0x00,0x00,0x00,0x04,0x02,0x00,0x02,0x02,0x20,0x22,0x22,0x90,0x38,0xA3,0x80,0x0B,
	0x83,0x80,0x01,0x29,0x24,0x00,0xAA,0xA8,0x13,0x6A,0x62,0x15,0xAA,0xB4,0x0A,0xAA,
	0x42,0x04,0xAA,0xA8,0x00,0xAE,0x64,0x01,0xAE,0x99,0x2A,0x6A,0x04,0x0C,0xA2,0x01,
	0x00,0x60,0x80,0x02,0x10,0x80,0x02,0x20,0xC0,0x02,0x00,0x00,0x00,0x00,0x00,0x82};
	
const unsigned char enemy_virus6[] = {
	0x00,0x00,0x00,0x00,0x03,0x00,0x02,0x22,0x24,0x22,0x12,0x90,0x38,0xA3,0x40,0x0B,
	0x83,0x80,0x01,0x29,0x26,0x00,0xAA,0xA8,0x03,0x6A,0x60,0x17,0xAA,0xA4,0x02,0xAA,
	0x40,0x28,0xAA,0xA8,0x10,0xAE,0x64,0x01,0xAE,0x99,0x02,0x6A,0x04,0x04,0x91,0x00,
	0x00,0x50,0x80,0x02,0x20,0x80,0x02,0x00,0x80,0x00,0x00,0xC0,0x00,0x00,0x00,0x82};
	
const unsigned char enemy_virus7[] = {
	0x00,0x00,0x00,0x00,0x00,0x10,0x02,0x20,0x20,0x02,0x12,0x90,0x38,0xA3,0x40,0x0B,
	0x83,0x83,0x01,0x29,0x26,0x00,0xAA,0xA8,0x03,0x6A,0x60,0x05,0xAA,0x94,0x01,0xAA,
	0x40,0x2A,0xAA,0xA8,0x10,0xAE,0x64,0x01,0xAE,0x99,0x02,0x6A,0x08,0x08,0xA1,0x00,
	0x08,0x50,0x80,0x01,0x20,0x80,0x02,0x10,0x80,0x00,0x00,0x80,0x00,0x00,0xC0,0x82};
	
const unsigned char enemy_virus8[] = {
	0x00,0x00,0x00,0x00,0x20,0x14,0x00,0x22,0x20,0x22,0x12,0x90,0x38,0xA3,0x40,0x0B,
	0x83,0x42,0x01,0x29,0x26,0x00,0xAA,0xA8,0x03,0x6A,0x60,0x35,0xAA,0x90,0x0D,0xAA,
	0x40,0x1A,0xAA,0xA8,0x00,0xAE,0x64,0x01,0xAE,0x99,0x0A,0x6A,0x04,0x04,0xA1,0x00,
	0x00,0x50,0x80,0x02,0x20,0x80,0x02,0x10,0x80,0x00,0x20,0xC0,0x00,0x00,0x00,0x82};
	
//unsigned int v = 0xD000;	 // START OF DISPLAY CHIP

static void LoadSpriteFrame(const unsigned char *sprite_array, unsigned char bank_number)
{
	memcpy((void*) (SPRITE_SIZE * bank_number), sprite_array, SPRITE_SIZE);
}

static unsigned char RandomBrightColor()
{
	switch (rand_range(0,7))
	{
		case 0 : return 1;  //WHITE
		case 1 : return 3;  //CYAN
		case 2 : return 4;  //PURPLE/VIOLET
		case 3 : return 7;  //YELLOW
		case 4 : return 8;  //ORANGE
		case 5 : return 10; //PINK
		case 6 : return 13; //LIGHT GREEN
		case 7 : return 7;  //YELLOW AGAIN
		default : return 1;
	}
	return 1;
}

/*
static unsigned char RandomStarColor()
{
	switch (rand_range(0,7))
	{
		case 0 : return CLR_WHITE;
				 break;
		case 1 : return CLR_LIGHT_BLUE;
				 break;
		case 2 : return CLR_WHITE;
				 break;
		case 3 : return CLR_YELLOW;
				 break;
		case 4 : return CLR_ORANGE;
				 break;
		case 5 : return CLR_PINK;
				 break;
		case 6 : return CLR_WHITE;
				 break;
		case 7 : return CLR_RED;
				 break;
		default : return CLR_WHITE;
	}
	return CLR_WHITE;
}
*/

//static void AnimatePlayer()//(unsigned char Animation)
//{
	/*
	player.Current_Animation = Animation; //SET CURRENT ANIMATION 

	if (Animation != player.Current_Animation)
	{
		player.Current_Animation = Animation; //SET CURRENT ANIMATION 
		SetSpriteFrame(PLAYER_SPRITE,SPRITE_BANK_PLAYER_NORM1);
	}
	*/

	/*
	if (GetSpriteColor(PLAYER_SPRITE) == CLR_BLACK)
	{
		SetSpriteFrame(PLAYER_SPRITE, SPRITE_BANK_PLAYER_NORM);
		player.Current_Animation = Animation; //SET CURRENT ANIMATION 
		SetSpriteColor(PLAYER_SPRITE, CLR_WHITE);
	}
	*/
	
	
	/*
	if (frame_count % player.Frame_Mod == 0U)
		switch (GetSpriteFrame(PLAYER_SPRITE))
		{
			case SPRITE_BANK_PLAYER_NORM1 : SetSpriteColor(PLAYER_SPRITE, CLR_PINK); SetSpriteFrame(PLAYER_SPRITE, SPRITE_BANK_PLAYER_NORM2);
				break;
			case SPRITE_BANK_PLAYER_NORM2 : SetSpriteColor(PLAYER_SPRITE, CLR_YELLOW); SetSpriteFrame(PLAYER_SPRITE, SPRITE_BANK_PLAYER_NORM3);
				break;
			case SPRITE_BANK_PLAYER_NORM3 : SetSpriteColor(PLAYER_SPRITE, CLR_WHITE); SetSpriteFrame(PLAYER_SPRITE, SPRITE_BANK_PLAYER_NORM4);
				break;
			case SPRITE_BANK_PLAYER_NORM4 : SetSpriteColor(PLAYER_SPRITE, CLR_RED); SetSpriteFrame(PLAYER_SPRITE, SPRITE_BANK_PLAYER_NORM1);
				break;
			default : SetSpriteFrame(PLAYER_SPRITE,SPRITE_BANK_PLAYER_NORM);
		}	
	*/	
	//if (frame_count % player.Frame_Mod == 0U)
	//if (frame_count % 3 == 0)
	//	SetSpriteColor(PLAYER_SPRITE, RandomBrightColor());
	
	//SetSpriteColor(PLAYER_SPRITE, RandomBrightColor());
	
	/*
	switch (rand_range(1U,4U))
		{
			case 1 : SetSpriteColor(PLAYER_SPRITE, CLR_YELLOW);
				break;
			case 2 : SetSpriteColor(PLAYER_SPRITE, CLR_PINK); 
				break;
			case 3 : SetSpriteColor(PLAYER_SPRITE, CLR_CYAN); 
				break;
			case 4 : SetSpriteColor(PLAYER_SPRITE, CLR_WHITE); 
				break;
			default : SetSpriteColor(PLAYER_SPRITE, CLR_WHITE); 
		}	
	*/
	
	/*
	//Bounce
	if (frame_count % 5 == 0)
		switch (GetSpriteFrame(PLAYER_SPRITE))
		{
			case SPRITE_BANK_PLAYER_NORM1 : player.Animate_Direction = 1; SetSpriteFrame(PLAYER_SPRITE, SPRITE_BANK_PLAYER_NORM2);
				break;
			case SPRITE_BANK_PLAYER_NORM2 : SetSpriteFrame(PLAYER_SPRITE,SPRITE_BANK_PLAYER_NORM2 + player.Animate_Direction);
				break;
			case SPRITE_BANK_PLAYER_NORM3 : SetSpriteFrame(PLAYER_SPRITE,SPRITE_BANK_PLAYER_NORM3 + player.Animate_Direction);
				break;
			case SPRITE_BANK_PLAYER_NORM4 : player.Animate_Direction = -1; SetSpriteFrame(PLAYER_SPRITE,SPRITE_BANK_PLAYER_NORM3);
				break;
			default : SetSpriteFrame(PLAYER_SPRITE,SPRITE_BANK_PLAYER_NORM1);
		}	
	*/
		
//}


/*
void AnimateShot(unsigned char Animation)
{
	if (Animation != shot.Current_Animation)
	{
		shot.Current_Animation = Animation; //SET CURRENT ANIMATION 
		SetSpriteFrame(SHOT_SPRITE, SPRITE_BANK_SHOT);
	}
}
*/

void LoadEnemySpriteBanks(unsigned char Animation)
{
	
	if (Animation == ENEMY_ANIM_SPINNER)
	{
		LoadSpriteFrame(enemy_spinner1, SPRITE_BANK_ENEMY1);
		LoadSpriteFrame(enemy_spinner2, SPRITE_BANK_ENEMY2);
		LoadSpriteFrame(enemy_spinner3, SPRITE_BANK_ENEMY3);
		LoadSpriteFrame(enemy_spinner4, SPRITE_BANK_ENEMY4);
		LoadSpriteFrame(enemy_spinner5, SPRITE_BANK_ENEMY5);
		LoadSpriteFrame(enemy_spinner6, SPRITE_BANK_ENEMY6);
		LoadSpriteFrame(enemy_spinner1, SPRITE_BANK_ENEMY7);
		LoadSpriteFrame(enemy_spinner1, SPRITE_BANK_ENEMY8);
	}
	/*
	else if (Animation == ENEMY_ANIM_POTATO)
	{
		LoadSpriteFrame(enemy_potato1, SPRITE_BANK_ENEMY1);
		LoadSpriteFrame(enemy_potato2, SPRITE_BANK_ENEMY2);
		LoadSpriteFrame(enemy_potato3, SPRITE_BANK_ENEMY3);
		LoadSpriteFrame(enemy_potato4, SPRITE_BANK_ENEMY4);
		LoadSpriteFrame(enemy_potato5, SPRITE_BANK_ENEMY5);
		LoadSpriteFrame(enemy_potato6, SPRITE_BANK_ENEMY6);
		LoadSpriteFrame(enemy_potato7, SPRITE_BANK_ENEMY7);
		LoadSpriteFrame(enemy_potato8, SPRITE_BANK_ENEMY8);
	}
	*/
	else if (Animation == ENEMY_ANIM_EGG)
	{
		LoadSpriteFrame(enemy_egg1, SPRITE_BANK_ENEMY1);
		LoadSpriteFrame(enemy_egg2, SPRITE_BANK_ENEMY2);
		LoadSpriteFrame(enemy_egg3, SPRITE_BANK_ENEMY3);
		LoadSpriteFrame(enemy_egg4, SPRITE_BANK_ENEMY4);
		LoadSpriteFrame(enemy_egg5, SPRITE_BANK_ENEMY5);
		LoadSpriteFrame(enemy_egg6, SPRITE_BANK_ENEMY6);
		LoadSpriteFrame(enemy_egg7, SPRITE_BANK_ENEMY7);
		LoadSpriteFrame(enemy_egg8, SPRITE_BANK_ENEMY8);
	}
	else if (Animation == ENEMY_ANIM_VIRUS)
	{
		LoadSpriteFrame(enemy_virus1, SPRITE_BANK_ENEMY1);
		LoadSpriteFrame(enemy_virus2, SPRITE_BANK_ENEMY2);
		LoadSpriteFrame(enemy_virus3, SPRITE_BANK_ENEMY3);
		LoadSpriteFrame(enemy_virus4, SPRITE_BANK_ENEMY4);
		LoadSpriteFrame(enemy_virus5, SPRITE_BANK_ENEMY5);
		LoadSpriteFrame(enemy_virus6, SPRITE_BANK_ENEMY6);
		LoadSpriteFrame(enemy_virus7, SPRITE_BANK_ENEMY7);
		LoadSpriteFrame(enemy_virus8, SPRITE_BANK_ENEMY8);
	}
}

void AnimateEnemy()
{
	if (frame_count % enemy->Frame_Mod == 0)
		switch (GetSpriteFrame(enemy_sprite_number))
		{
			case SPRITE_BANK_ENEMY1 : if (enemy->Animate_Direction < 0) SetSpriteFrame(enemy_sprite_number,SPRITE_BANK_ENEMY8); else SetSpriteFrame(enemy_sprite_number,SPRITE_BANK_ENEMY2);
				break;
			case SPRITE_BANK_ENEMY2 : SetSpriteFrame(enemy_sprite_number,SPRITE_BANK_ENEMY2 + enemy->Animate_Direction);
				break;
			case SPRITE_BANK_ENEMY3 : SetSpriteFrame(enemy_sprite_number,SPRITE_BANK_ENEMY3 + enemy->Animate_Direction);
				break;
			case SPRITE_BANK_ENEMY4 : SetSpriteFrame(enemy_sprite_number,SPRITE_BANK_ENEMY4 + enemy->Animate_Direction);
				break;
			case SPRITE_BANK_ENEMY5 : SetSpriteFrame(enemy_sprite_number,SPRITE_BANK_ENEMY5 + enemy->Animate_Direction);
				break;
			case SPRITE_BANK_ENEMY6 : SetSpriteFrame(enemy_sprite_number,SPRITE_BANK_ENEMY6 + enemy->Animate_Direction);
				break;
			case SPRITE_BANK_ENEMY7 : SetSpriteFrame(enemy_sprite_number,SPRITE_BANK_ENEMY7 + enemy->Animate_Direction);
				break;
			case SPRITE_BANK_ENEMY8 : if (enemy->Animate_Direction > 0) SetSpriteFrame(enemy_sprite_number,SPRITE_BANK_ENEMY1); else SetSpriteFrame(enemy_sprite_number,SPRITE_BANK_ENEMY7);
				break;
			default : SetSpriteFrame(enemy_sprite_number,SPRITE_BANK_ENEMY1);
		}	
}

void AnimateDeath(unsigned char Animation)
{
	if (death.alive == 1)
	{
		//CHANGE ANIMATIONS IF IT HAS CHANGED	
		if (death.Current_Animation != Animation)
		{
			death.Current_Animation = Animation; //SET CURRENT ANIMATION 
			SetSpriteFrame(DEATH_SPRITE,SPRITE_BANK_DEATH1);
		}

		// SWAP ANIMATION FRAME
		if (Animation == DEATH_ANIM_EXPLODE)
		{
			if (death.lifetime <= 6)
			{
				if (frame_count % 2 == 0)
					SetSpriteColor(DEATH_SPRITE, CLR_WHITE); //death
				else 
					SetSpriteColor(DEATH_SPRITE, CLR_YELLOW); //death
				SetSpriteFrame(DEATH_SPRITE, SPRITE_BANK_DEATH1);
			}
			else if (death.lifetime > 6 && death.lifetime < 12)
			{
				if (frame_count % 2 == 0)
					SetSpriteColor(DEATH_SPRITE, CLR_YELLOW); //death
				else
					SetSpriteColor(DEATH_SPRITE, CLR_PINK); //death
				SetSpriteFrame(DEATH_SPRITE, SPRITE_BANK_DEATH2);
			}
			else if (death.lifetime > 12)
			{
				if (frame_count % 2 == 0)
					SetSpriteColor(DEATH_SPRITE, CLR_RED); //death
				else
					SetSpriteColor(DEATH_SPRITE, CLR_PINK); //death
					
				SetSpriteFrame(DEATH_SPRITE, SPRITE_BANK_DEATH3);
			}
		}
		// SWAP ANIMATION FRAME
		else //if (Animation == DEATH_ANIM_PLAYER)
		{
			if (frame_count % 2 == 0)
				SetSpriteColor(DEATH_SPRITE, RandomBrightColor()); //death
			if (death.lifetime <= 15)
			{
				SetSpriteFrame(DEATH_SPRITE, SPRITE_BANK_DEATH1);
			}
			else if (death.lifetime > 15 && death.lifetime < 30)
			{
				SetSpriteColor(DEATH_SPRITE, CLR_PINK); //death
			}
			else if (death.lifetime > 30)
			{
				SetSpriteFrame(DEATH_SPRITE, SPRITE_BANK_DEATH3);
			}
		}



	}
}

void MovePlayer()
{
	if (player.alive == 1)
	{
		//AnimatePlayer();//(PLAYER_ANIM_NORM);
		SetSpriteColor(PLAYER_SPRITE, RandomBrightColor());
		
		//ShowInt(PEEK(56320), 5, 1);
		if (PEEK(0xDC00) != 127U)
		{
			if (!(PEEK(0xDC00) & 16U) && //Joystick fire
			    shot.alive == 0U)
			{
				laser_freq = 160;
				shot.alive = 1U;
				shot.x = player.x+14U;
				shot.y = player.y;
				MoveSprite(SHOT_SPRITE, shot.x, shot.y);
				EnableSprite(1U); 
			}

			if (!(PEEK(0xDC00) & 8U) && //Joystick Right
				player.x < MAX_PLAYER_X)
				player.x += 2U;

			if (!(PEEK(0xDC00) & 4U) && //Joystick Left
				player.x > MIN_PLAYER_X)
				player.x -= 2U;

			if (!(PEEK(0xDC00) & 1U) && //Joystick Up
				player.y > MIN_PLAYER_Y)
				player.y -= 2U;

			if (!(PEEK(0xDC00) & 2U) && //Joystick Down
				player.y < MAX_PLAYER_Y)
				player.y += 2U;

			MoveSprite(PLAYER_SPRITE, player.x, player.y);
		}
	}
}

void MoveShot()
{
	if (shot.alive == 1)
	{
		//AnimateShot(SHOT_ANIM_LASER1);
		shot.x += 10;
		
		if (shot.x > MAX_PLAYER_X+24)
		{
			DisableSprite(1);
			shot.alive = 0;
			shot.x = 0;
			shot.y = 320;
		}
		MoveSprite(SHOT_SPRITE, shot.x, shot.y);
	}
}

void MoveEnemy()
{
	int sign = 1;
	
	AnimateEnemy();

/*	
	if (frame_count % 20 == 0)
	{

		if (rand_range(0,1) == 1)
			sign = -1;
		else
			sign = 1;
		
		enemy->yvel += sign;
		
		if (enemy->yvel < -3)
			enemy->yvel = -3;
		else if (enemy->yvel > 3)
			enemy->yvel = 3;
	}

	if (frame_count % 15 == 0)
	{

		if (rand_range(0,2) == 1)
			sign = 1;
		else
			sign = -1;
		
		enemy->xvel += sign;
		
		if (enemy->xvel < -3)
			enemy->xvel = -3;
		else if (enemy->xvel > 1)
			enemy->xvel = 1;
	}
*/
	
	enemy->x += enemy->xvel; // x Speed
	enemy->y += enemy->yvel;
	
	if (enemy->y < MIN_PLAYER_Y || enemy->y > MAX_PLAYER_Y) //reflect off walls
		enemy->yvel = enemy->yvel * -1;
	
	if (enemy->x < MIN_PLAYER_X-24)
	{
		enemy->alive = 0;
		DisableSprite(enemy_sprite_number); 
	}
	else
		MoveSprite(enemy_sprite_number, enemy->x, enemy->y);
}

static void SpawnEnemy()
{
	if (next_wave_delay == 0)
	{
		if (frame_count % 20 == 0)
			if (frame_count % rand_range(1,50) == 0)
			{
				EnableSprite(enemy_sprite_number); //enemy
				enemy->y = rand_range(MIN_PLAYER_Y,MAX_PLAYER_Y);
				enemy->x = MAX_PLAYER_X + 24;
				enemy->alive = 1;
				enemy->xvel = rand_range(-3,-2);;
				enemy->yvel = rand_range(-2,2);
				if (rand_range(0,1) == 1)
					enemy->Animate_Direction = 1;
				else 
					enemy->Animate_Direction = -1;
				MoveSprite(enemy_sprite_number, enemy->x, enemy->y);
			}
	}
}

void Die()
{
	if (death.alive == 1)
	{
		AnimateDeath(death_anim);
		death.lifetime += 1;
		
		//kill death!
		if ((death.lifetime > MAX_DEATH_FRAMES && death_anim == DEATH_ANIM_EXPLODE) || 
		    (death.lifetime > 45 && death_anim == DEATH_ANIM_PLAYER))
		{
			DisableSprite(DEATH_SPRITE); //death
			death.alive = 0;
			death.lifetime = 0;
			death.y = 320;
			death.x = 240;
			MoveSpriteToOrigin(DEATH_SPRITE);
		}
	}
}

void KillEnemy()
{
	if (shot.alive == 1)
		if (shot.x < enemy->x + 24)
			if (shot.x + 48 > enemy->x)
				if (shot.y < enemy->y + 21)
					if (shot.y + 21 > enemy->y) 
						if (SpriteCollision(enemy_sprite_number) || SpriteCollision(SHOT_SPRITE))
						{

							//shot--------------------------------------------------------------------------
							DisableSprite(SHOT_SPRITE);
							shot.alive = 0;
							shot.x = 0;
							shot.y = 320;
							//MoveSprite(1, shot.x, shot.y);

							//enemy-------------------------------------------------------------------------
							DisableSprite(enemy_sprite_number); 
							enemy->alive = 0;
							death.y = enemy->y;
							death.x = enemy->x;
							enemy->x = 0;
							enemy->y = 0;
							MoveSprite(enemy_sprite_number, enemy->x, enemy->y);

							//death-------------------------------------------------------------------------
							//AnimateDeath(DEATH_ANIM_EXPLODE); derp
							death_anim = DEATH_ANIM_EXPLODE;
							EnableSprite(DEATH_SPRITE); 
							death.alive = 1;
							death.lifetime = 0;
							MoveSprite(DEATH_SPRITE, death.x, death.y);
							
							ClearSpriteCollision(enemy_sprite_number);
							
							//SOUND: Explosion
							memset((void *)(0xD400+7UL), 0, 7); // clear voice 1 data
							
							/*
							POKE(0xD401+7UL, 25U); //FREQ
							POKE(0xD405+7UL, 9U); //ATTACK/DECAY
							POKE(0xD406+7UL, 15U); //SUSTAIN/RELEASE
							POKE(0xD404+7UL, 129U); //WAVEFORM: Triangle=17 Sawtooth=33 Pulse=65 Noise=129 
							*/
							
							CONST_POKE(0xD401+7UL, 25U); //FREQ
							CONST_POKE(0xD405+7UL, 9U); //ATTACK/DECAY
							CONST_POKE(0xD406+7UL, 15U); //SUSTAIN/RELEASE
							CONST_POKE(0xD404+7UL, 129U); //WAVEFORM: Triangle=17 Sawtooth=33 Pulse=65 Noise=129 

							score += 1;
							ShowScore(score);
							if (score % 50 == 0)
							{
								lives += 1;
								ShowLives(lives);
							}
						}
}

static void KillPlayer()
{
	if (enemy->alive && player.alive)
		if (player.x + 24 > enemy->x)
			if (player.x < enemy->x + 24)
				if (player.y < enemy->y + 21)
					if (player.y + 21 > enemy->y) 
						if (SpriteCollision(PLAYER_SPRITE))
						{
							//player--------------------------------------------------------------------------
							DisableSprite(PLAYER_SPRITE); 
							player.alive = 0;
							death.y = player.y;
							death.x = player.x;
							player.x = 0;
							player.y = 320;
							MoveSprite(PLAYER_SPRITE, player.x, player.y);

							//death---------------------------------------------------------------------------
							//AnimateDeath(DEATH_ANIM_EXPLODE); --derp
							death_anim = DEATH_ANIM_PLAYER;
							EnableSprite(DEATH_SPRITE);
							death.alive = 1;
							death.lifetime = 0;
							MoveSprite(DEATH_SPRITE, death.x, death.y);
							
							//SOUND: Explosion
							memset((void *)(0xD400+7UL), 0, 7); // clear voice 1 data
							CONST_POKE(0xD401+7UL, 7U); //FREQ
							CONST_POKE(0xD405+7UL, 10U); //ATTACK/DECAY
							CONST_POKE(0xD406+7UL, 4U); //SUSTAIN/RELEASE
							CONST_POKE(0xD404+7UL, 129U); //WAVEFORM: Triangle=17 Sawtooth=33 Pulse=65 Noise=129 
							
							/*
							POKE(0xD401+7UL, 7U); //FREQ
							POKE(0xD405+7UL, 10U); //ATTACK/DECAY
							POKE(0xD406+7UL, 4U); //SUSTAIN/RELEASE
							POKE(0xD404+7UL, 129U); //WAVEFORM: Triangle=17 Sawtooth=33 Pulse=65 Noise=129 
							*/

							ClearAllSpriteCollisions();
							
							lives -= 1;
							ShowLives(lives);
						}
}

static void ClearAllEnemies()
{
	enemy_sprite_number = ENEMY1_SPRITE;
	enemy = &enemy1;
	enemy->x = 320;
	enemy->y = 0;
	enemy->alive = 0;
	DisableSprite(enemy_sprite_number);
	MoveSprite(enemy_sprite_number, enemy->x, enemy->y);
	
	enemy_sprite_number = ENEMY2_SPRITE;
	enemy = &enemy2;
	enemy->x = 320;
	enemy->y = 30;
	enemy->alive = 0;
	DisableSprite(enemy_sprite_number);
	MoveSprite(enemy_sprite_number, enemy->x, enemy->y);
	
	enemy_sprite_number = ENEMY3_SPRITE;
	enemy = &enemy3;
	enemy->x = 320;
	enemy->y = 60;
	enemy->alive = 0;
	DisableSprite(enemy_sprite_number);
	MoveSprite(enemy_sprite_number, enemy->x, enemy->y);

	enemy_sprite_number = ENEMY4_SPRITE;
	enemy = &enemy4;
	enemy->x = 320;
	enemy->y = 90;
	enemy->alive = 0;
	DisableSprite(enemy_sprite_number);
	MoveSprite(enemy_sprite_number, enemy->x, enemy->y);
}

static void SpawnPlayer()
{
	ClearAllSpriteCollisions();
	player.alive = 1;
	player.y = MAX_PLAYER_X / 2 - 11;
	player.x = MIN_PLAYER_Y + 20;
	player.Frame_Mod = 3;
	MoveSprite(PLAYER_SPRITE, player.x, player.y);
	SetSpriteColor(PLAYER_SPRITE, CLR_WHITE);
	SetSpriteFrame(PLAYER_SPRITE, SPRITE_BANK_PLAYER_NORM);
	EnableSprite(PLAYER_SPRITE);
	ClearAllEnemies();
}

/*
static void Generate_Platform(int Height)
{
	int Start;
	int Slot;
	int i;
	Slot = rand_range(2, 31);
	Start = Height * 40;
	for (i = 0; i < 40; i++)
	{
		if (i <= Slot || i > Slot + 7)
		{
			POKE(1024+Start+i,120);//230
			POKE(55296+Start+i,CLR_CYAN);
		}
	}
}
*/

static void Generate_Stars()
{
	unsigned char i;
	unsigned int pos;
	unsigned char c;
	for (i = 0UL; i < 50; ++i)
	{
		pos = rand_range(40UL, 999UL);
		c = RandomBrightColor();
		screen[pos] = '.';
		screen_colors[pos] = c;
		
		//stars[i] = pos;
		//star_colors[i] = c;
	}
}

static void ClearAllText(unsigned char color)
{
	unsigned char x;
	memset((void *)0x0400, 32U, 1000UL);
	
	memset((void *)0xD800, color, 40UL);
	for (x = 1; x < 25; ++x)
		memset((void *)(0xD800 + (x * 40)), RandomBrightColor(), 40UL);
	//memset((void *)0xD800, color, 1000UL);
}

static void ClearTopLine(unsigned char color)
{
	memset((void *)0x0400,32U,40U);
	memset((void *)0xD800,color,40U);
}

static void SetTextColor(unsigned char color, int x, int y, int len)
{
	memset((void *)(0xD800+(x + (y * 40UL))),color,len);
}

static void Init_Animated_Stars()
{
	star1 = rand_range(0x0428,0x07C0);
	star2 = rand_range(0x0428,0x07C0);
	star3 = rand_range(0x0428,0x07C0);
	star4 = rand_range(0x0428,0x07C0);
	star5 = rand_range(0x0428,0x07C0);
	POKE(star1, 46U);
	POKE(star2, 46U);
	POKE(star3, 46U);
	POKE(star4, 46U);
	POKE(star5, 46U);
}

static void GameOver()
{
	//unsigned char i;
	unsigned char x = 15U;
	unsigned char y = 10U;
	unsigned int Title_Len = 9UL;
	//unsigned char prev_color = CLR_BROWN;
	unsigned char current_color = CLR_RED;
	//unsigned char next_color = CLR_PINK;
	
	ClearAllEnemies();

	frame_count = 0U;
	
	ClearAllText(CLR_RED);

    gotoxy(x,y);
    cprintf("GAME OVER");

	if (score > high_score) 
	{
		gotoxy(3U, 7U); 	
		high_score = score;
		cprintf("WOO HOO! NEW HIGH SCORE %i00 !!!",high_score); 
	}
	
	do 
	{
		++frame_count;
		
		if (frame_count % 5U == 0U)
			switch (current_color)
			{
				case CLR_BROWN : current_color = CLR_RED;
					break;
				case CLR_RED : current_color = CLR_PINK;
					break;
				case CLR_PINK : current_color = CLR_BROWN;
					break;
				default : 
					current_color = CLR_BROWN;
			}
		
		if (frame_count % 4 == 0)
		{
			SetTextColor(current_color, x, y, Title_Len);
			SetTextColor(rand_range(0,1) == 0 ? CLR_GREEN : CLR_LIGHT_GREEN, 0U, 7U, 40);
		}

		if (frame_count > 240)
			frame_count = 0;

		waitvsync();
		
	} while (PEEK(197)==64U);
		
		
	ClearAllText(CLR_CYAN);
	
	Init_Animated_Stars();
	
	ClearTopLine(CLR_PINK);
	ShowClearScore();
	ShowClearLives();

	SetTextColor(CLR_CYAN, 0, 0, 20);
	SetTextColor(CLR_WHITE, 20, 0, 20);
	
	ClearAllSpriteCollisions();
	
	SpawnPlayer();
}

static void SetEnemyFrameMod(unsigned char Frame_Mod)
{
	enemy1.Frame_Mod = Frame_Mod;
	enemy2.Frame_Mod = Frame_Mod;
	enemy3.Frame_Mod = Frame_Mod;
	enemy4.Frame_Mod = Frame_Mod;
}

static void SetEnemyColors(unsigned char animation)
{
	unsigned int c = 0;
	if (enemy1.Current_Animation != animation)
	{
		enemy1.Current_Animation = animation;
		enemy2.Current_Animation = animation;
		enemy3.Current_Animation = animation;
		enemy4.Current_Animation = animation;
	}
		/*
		SetSpriteColor(ENEMY1_SPRITE, CLR_VIOLET);
		SetSpriteColor(ENEMY2_SPRITE, CLR_RED);
		SetSpriteColor(ENEMY3_SPRITE, CLR_BLUE);
		SetSpriteColor(ENEMY4_SPRITE, CLR_ORANGE);
		*/
		
	c = rand_range(2,4);
	SetSpriteColor(ENEMY1_SPRITE, c == 3 ? 2 : c);
	
	c = rand_range(6,8);
	SetSpriteColor(ENEMY2_SPRITE, c == 7 ? 8 : c);
	
	SetSpriteColor(ENEMY3_SPRITE, rand_range(9,10));

	c = rand_range(11,14);
	SetSpriteColor(ENEMY4_SPRITE, c == 12 || c == 13 ? 11U : c);

		/*
		switch (animation)
		{
			// case ENEMY_ANIM_POTATO : 
				// SetSpriteColor(ENEMY1_SPRITE, CLR_BROWN); //enemy
				// SetSpriteColor(ENEMY2_SPRITE, CLR_DARK_GRAY); //enemy
				// SetSpriteColor(ENEMY3_SPRITE, CLR_ORANGE); //enemy
				// SetSpriteColor(ENEMY4_SPRITE, CLR_RED); //enemy
				// break;
			case ENEMY_ANIM_VIRUS : 
				SetSpriteColor(ENEMY1_SPRITE, CLR_VIOLET); //enemy
				SetSpriteColor(ENEMY2_SPRITE, CLR_RED); //enemy
				SetSpriteColor(ENEMY3_SPRITE, CLR_BLUE); //enemy
				SetSpriteColor(ENEMY4_SPRITE, CLR_ORANGE); //enemy
				break;
			default : ; //ENEMY_ANIM_SPINNER, ENEMY_ANIM_EGG
				SetSpriteColor(ENEMY1_SPRITE, CLR_VIOLET); //enemy
				SetSpriteColor(ENEMY2_SPRITE, CLR_RED); //enemy
				SetSpriteColor(ENEMY3_SPRITE, CLR_BLUE); //enemy
				SetSpriteColor(ENEMY4_SPRITE, CLR_WHITE); //enemy
				break;
		}
		*/

}

/*
static unsigned char rand_left_pos()
{
	unsigned char rb = (unsigned char)rand();
	
	__asm__ ("lda #%b", rb); //put random byte into the accumulator 
	__asm__ ("and #%b", 0b11100000);  //use bit mask to limit max to 31
	__asm__ ("sta %w", 0xC100);       //put the value at 0xC100
	
	if (PEEK(0xC100) > 23U) //value at 0xC100 is greater than 23
	{
		__asm__ ("lda #%b", 0xC100);     //put value at 0xC100 into the accumulator
		__asm__ ("and #%b", 0b11110000); //use bit mask to limit max to 15
		__asm__ ("sta %w", 0xC100);      //put the value back at 0xC100
	}

	return PEEK(0xC100);
}
*/

//int main (void)
void main (void)
{
	unsigned char i;
	unsigned char kb_was_hit;
	int score = 0;
	unsigned char scroll = 0;
	unsigned char game_continue = 1;
	unsigned char seconds = 0;
	unsigned char next_enemy_anim = ENEMY_ANIM_EGG;
	
	memset((void *)0xD400, 0, 24); //clear SID
	CONST_POKE(0xD418, 15); //VOLUME - max is 15
	
	
	//unsigned char xScroll = 0x07;
	//unsigned char last_star;
	
	//unsigned char scroll_counter = 0;
	
	//POKE(0xDD00,0b00000011);
	/*
	lda $DD00
	and #%11111100
	ora #%000000xx ;<- your desired VIC bank value, see above
	sta $DD00
	*/

	revive = 0;

	enemy_sprite_number = ENEMY1_SPRITE;
	enemy = &enemy1;

	_randomize();

	CONST_POKE(0xD020,CLR_DARK_GRAY); //screen boarder color
	CONST_POKE(0xD021,CLR_BLACK); //screen color

	//POKE(56322,224); //disable keyboard
	frame_count = 1;

	kb_was_hit = 0;
	//SpawnEnemy(frame_count);

	for (i = 0; i < 8; i++)
		EnableMultiColorSprite(i);
	
	SetSpriteColor(PLAYER_SPRITE, CLR_WHITE); //player
	SetSpriteColor(SHOT_SPRITE, CLR_WHITE);  //player shot
	SetSpriteColor(DEATH_SPRITE, CLR_ORANGE); //death
	
	/*
	SetSpriteColor(ENEMY1_SPRITE, CLR_VIOLET); //enemy
	SetSpriteColor(ENEMY2_SPRITE, CLR_RED); //enemy
	SetSpriteColor(ENEMY3_SPRITE, CLR_BLUE); //enemy
	SetSpriteColor(ENEMY4_SPRITE, CLR_WHITE); //enemy
	*/
	SetEnemyColors(next_enemy_anim);
	
	SetSpriteMultiColor1(CLR_CYAN);
	SetSpriteMultiColor2(CLR_GREEN);
	
	LoadSpriteFrame(player_norm, SPRITE_BANK_PLAYER_NORM);
	//LoadSpriteFrame(player_norm1, SPRITE_BANK_PLAYER_NORM1);
	//LoadSpriteFrame(player_norm2, SPRITE_BANK_PLAYER_NORM2);
	//LoadSpriteFrame(player_norm3, SPRITE_BANK_PLAYER_NORM3);
	//LoadSpriteFrame(player_norm4, SPRITE_BANK_PLAYER_NORM4);
	LoadSpriteFrame(player_shot, SPRITE_BANK_SHOT);
	SetSpriteFrame(SHOT_SPRITE, SPRITE_BANK_SHOT); //no animate shot function

	LoadEnemySpriteBanks(next_enemy_anim);
	
	LoadSpriteFrame(death_explode1, SPRITE_BANK_DEATH1);
	LoadSpriteFrame(death_explode2, SPRITE_BANK_DEATH2);
	LoadSpriteFrame(death_explode3, SPRITE_BANK_DEATH3);
	
	ShowTitleScreen();

	ClearAllText(CLR_CYAN);
	
	//Generate_Stars();
	Init_Animated_Stars();
	
	ClearTopLine(CLR_PINK);
	ShowClearScore();
	ShowClearLives();

	SetTextColor(CLR_CYAN, 0, 0, 20);
	SetTextColor(CLR_WHITE, 20, 0, 20);
	
	for (i = 0; i < 8; i++)
		SpriteCollision(i); //clear collisions
	
	SpawnPlayer();
	SetFlag(0xD01D, SHOT_SPRITE, 1U); //stretch width

	do 
	{
		++frame_count;
		if (frame_count > 240)
			frame_count = 1;

		//Start ---- Handle next wave ---------------------------------------------------------------
		if (frame_count % 60 == 0)
		{
			++seconds;
			
			//Next wave wait cycle
			if (seconds >= 15)
			{
				next_enemy_anim = rand_range(ENEMY_ANIM_SPINNER, ENEMY_ANIM_VIRUS); 
				//next_enemy_anim = next_enemy_anim + 1 > ENEMY_ANIM_VIRUS ? ENEMY_ANIM_SPINNER : next_enemy_anim + 1;
				next_wave_delay = 180; //3 seconds delay to wait for enemies to leave screan
				seconds = 0;
			}
		}

		//Do next wave!!!
		if (next_wave_delay == 1)
		{
			if (next_enemy_anim == ENEMY_ANIM_VIRUS /*|| next_enemy_anim == ENEMY_ANIM_POTATO */)
				SetEnemyFrameMod(4);
			else
				SetEnemyFrameMod(7);
			ClearAllEnemies();
			SetEnemyColors(next_enemy_anim);
			LoadEnemySpriteBanks(next_enemy_anim);
		}
		
		if (next_wave_delay > 0)
			next_wave_delay -= 1;
		//End ---- Handle next wave -------------------------------------------------------------------

		
		// debug(20, SpriteCollision(2));
		
		//++frame_count;
		//frame_count = frame_count + 1;
		
		//ShowInt(frame_count, 5, 1);
		/*
		POKE(54270,scroll);
		if (scroll < 8)
			++scroll;
		else
			scroll = 0;
		*/
		
		//SpriteCollision(2); //clear collisions
		MovePlayer();
		MoveShot();
		
		//play laser sound
		if (laser_freq > 0)
		{
			//memset((void *)0xD400, 0, 24);
			memset((void *)0xD400, 0, 7); // clear voice 1 data
			POKE(0xD401, laser_freq); //FREQ
			CONST_POKE(0xD405, 4U); //ATTACK/DECAY
			CONST_POKE(0xD406, 4U); //SUSTAIN/RELEASE
			CONST_POKE(0xD404, 33U); //WAVEFORM: Triangle=17 Sawtooth=33 Pulse=65 Noise=129 
			laser_freq -= 16;
		}
		
		enemy_sprite_number = ENEMY1_SPRITE;
		enemy = &enemy1;
		if (enemy->alive == 0)
			SpawnEnemy();
		else
		{
			MoveEnemy();
			KillEnemy();
			KillPlayer();
		}
		
		enemy_sprite_number = ENEMY2_SPRITE; 
		enemy = &enemy2;
		if (enemy->alive == 0)
			SpawnEnemy();
		else
		{
			MoveEnemy();
			KillEnemy();
			KillPlayer();
		}

		enemy_sprite_number = ENEMY3_SPRITE; 
		enemy = &enemy3;
		if (enemy->alive == 0)
			SpawnEnemy();
		else
		{
			MoveEnemy();
			KillEnemy();
			KillPlayer();
		}
		
		enemy_sprite_number = ENEMY4_SPRITE; 
		enemy = &enemy4;
		if (enemy->alive == 0)
			SpawnEnemy();
		else
		{
			MoveEnemy();
			KillEnemy();
			KillPlayer();
		}

		Die();

		//revive player or game over
		if (!player.alive)
		{
			++revive;
			if (revive >= 120)
			{
				revive = 0;
				if (lives == 0)
					GameOver();
				else
					SpawnPlayer();
			}
		}

		AnimateStar(star1, stardead1, starcol1);
		AnimateStar(star2, stardead2, starcol2);
		AnimateStar(star3, stardead3, starcol3);
		AnimateStar(star4, stardead4, starcol4);
		AnimateStar(star5, stardead5, starcol5);


		/*
		//-----------------------------------------------------------------
		if (stardead1 == 0)
		{
			POKE(star1,32U);
			if (starcol1 > 39)
			{
				star1 = 0x0400 + (rand_range(1U,23U) * 40U) + 40U;
				stardead1 = rand_range(1U,10U);
				starcol1 = 39;
			}
			else
			{
				star1 -= 1;
				starcol1 -=1;
				POKE(star1,99U);
			}
		}
		else
			stardead1 -= 1;

		//-----------------------------------------------------------------
		if (stardead2 == 0)
		{
			POKE(star2,32U);
			if (starcol2 > 39)
			{
				star2 = 0x0400 + (rand_range(1U,23U) * 40U) + 40U;
				stardead2 = rand_range(1U,10U);
				starcol2 = 39;
			}
			else
			{
				star2 -= 1;
				starcol2 -=1;
				POKE(star2,99U);
			}
		}
		else
			stardead2 -= 1;


		//-----------------------------------------------------------------
		if (stardead3 == 0)
		{
			POKE(star3,32U);
			if (starcol3 > 39)
			{
				star3 = 0x0400 + (rand_range(1U,23U) * 40U) + 39U;
				stardead3 = rand_range(1U,10U);
				starcol3 = 39;
			}
			else
			{
				star3 -= 1;
				starcol3 -=1;
				POKE(star3,99U);
			}
		}
		else
			stardead3 -= 1;
		
		//-----------------------------------------------------------------
		if (stardead4 == 0)
		{
			POKE(star4,32U);
			if (starcol4 > 39)
			{
				star4 = 0x0400 + (rand_range(1U,23U) * 40U) + 39U;
				stardead4 = rand_range(1U,5U);
				starcol4 = 39;
			}
			else
			{
				star4 -= 1;
				starcol4 -=1;
				POKE(star4,99U);
			}
		}
		else
			stardead4 -= 1;

		//-----------------------------------------------------------------
		if (stardead5 == 0)
		{
			POKE(star5,32U);
			if (starcol5 > 39)
			{
				star5 = 0x0400 + (rand_range(1U,23U) * 40U) + 39U;
				stardead5 = rand_range(1U,5U);
				starcol5 = 39;
			}
			else
			{
				star5 -= 1;
				starcol5 -=1;
				POKE(star5,99U);
			}
		}
		else
			stardead5 -= 1;
		*/
			
		waitvsync();
		
	} while(1); //while (PEEK(197) != 63); //Run-stop pressed
	
	//free(stars);

	//__asm__ ("jmp %w", 0xFCE2); // soft reset the Commodore 64
}