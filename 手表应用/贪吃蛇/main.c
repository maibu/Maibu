/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  demo_text.c
 *
 *    Description:  
 *    Corporation:
 * 
 *         Author:  gliu (), gliu@maibu.cc
 *        Created:  2014年12月11日 17时06分32秒
 *
 * =====================================================================================
 *
 * =====================================================================================
 * 
 *   MODIFICATION HISTORY :
 *    
 *		     DATE :
 *		     DESC :
 * =====================================================================================
 */	
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "maibu_sdk.h"
#include "maibu_res.h"
#include "3dstypes.h"


void change_fresh_rate(u16 spd);
void start_spi(void);
void close_spi(void);
void write_multiple_lines(uint16_t line_number, unsigned char *matrix, uint16_t matrix_len, uint16_t line_size);

#define QUEUE_SIZE_MAX 30
#define SIZE_ROW (13)
#define SIZE_COL (16)

/* 描述一个游戏状态的结构体 */
typedef struct _GAME_CONTEXT{
	u8 q[QUEUE_SIZE_MAX];
	s8 qs, ql, d, dx, dy, tdx, tdy, fdx, fdy;
	//u8 map[SIZE_ROW][SIZE_COL];
	u8 status;
	u16 score;		
} GAME_CONTEXT,*PGAME_CONTEXT;

#define GAME_STATUS_GAMEOVER (3)
#define GAME_STATUS_PLAYING (2)
#define GAME_STATUS_PAUSED (1)
#define GAME_STATUS_INVALID (0)

#define MAP_FOOD (2)
#define MAP_SNAKE (1)
#define MAP_EMPTY (0)

static GAME_CONTEXT curr = {0};

#define GET_BIT(a,b)	((a) & (1<<(b)))
#define SET_BIT(a,b)	((a) |= (1<<(b)))
#define RESET_BIT(a,b)	((a) &= (~(1<<(b))))


static uint8_t g_timer_id = 0;
static uint8_t g_first_enter_handler_flag = 0;
static uint16_t g_score_plus = 4;

static uint16_t g_pre_score = 0;
static int32_t g_window_id = 0;

static int32_t g_score_pause_text_str_id;
static int32_t g_score_bmp_id_array[4];

static P_Window init_window(void);


//小号数字图片数组
static const uint32_t get_LIT_icon_key[] =
{
	RES_BITMAP_WATCHFACE_NUMBER_LIT_0,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_1,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_2,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_3,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_4,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_5,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_6,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_7,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_8,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_9
	
};

//大号数字图片数组
static const uint32_t get_BIG_icon_key[] =
{
	RES_BITMAP_WATCHFACE_NUMBER_BIG_0,  
	RES_BITMAP_WATCHFACE_NUMBER_BIG_1, 
	RES_BITMAP_WATCHFACE_NUMBER_BIG_2,  
	RES_BITMAP_WATCHFACE_NUMBER_BIG_3,  
	RES_BITMAP_WATCHFACE_NUMBER_BIG_4,  
	RES_BITMAP_WATCHFACE_NUMBER_BIG_5,  
	RES_BITMAP_WATCHFACE_NUMBER_BIG_6,  
	RES_BITMAP_WATCHFACE_NUMBER_BIG_7, 
	RES_BITMAP_WATCHFACE_NUMBER_BIG_8,  
	RES_BITMAP_WATCHFACE_NUMBER_BIG_9

};


/*创建并显示图片图层，需要坐标值，得到icon_key的数组，数组需要的参数值，P_Window*/
static int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,enum GAlign how_to_align,enum GColor black_or_white,uint32_t bmp_array_name)
{	


	GBitmap bmp_point = {0};
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_array_name, &bmp_point);
	LayerBitmap layer_bitmap_struct_l = {bmp_point, *temp_p_frame, how_to_align};
 	temp_P_Layer = app_layer_create_bitmap(&layer_bitmap_struct_l);
	
	if(temp_P_Layer != NULL)
	{
		app_layer_set_bg_color(temp_P_Layer, black_or_white);
		return app_window_add_layer(p_window, temp_P_Layer);
	}

	return 0;
}

/*创建并显示文本图层*/
static int32_t display_target_layerText(P_Window p_window,GRect  *temp_p_frame,enum GAlign how_to_align,enum GColor color,char * str,uint8_t font_type)
{
	LayerText temp_LayerText = {0};
	temp_LayerText.text = str;
	temp_LayerText.frame = *temp_p_frame;
	temp_LayerText.alignment = how_to_align;
	temp_LayerText.font_type = font_type;
	
	P_Layer p_layer = app_layer_create_text(&temp_LayerText);

	
	if(p_layer != NULL)
	{
		app_layer_set_bg_color(p_layer, color);
		int8_t temp = -1;
		temp = app_window_add_layer(p_window, p_layer);
		
		return temp;
	}
	return 0;
}


//重新载入并刷新窗口所有图层
static void window_reloading(void)
{
	/*根据窗口ID获取窗口句柄*/
	P_Window p_old_window = app_window_stack_get_window_by_id(g_window_id);
	if (NULL != p_old_window)
	{
		P_Window p_window = init_window();
		if (NULL != p_window)
		{
			g_window_id = app_window_stack_replace_window(p_old_window, p_window);
		}	
	}
	
}
	
static void getQueueAt(int* px, int* py, int pos) {
	int p = curr.qs + pos ;
	if (p >= QUEUE_SIZE_MAX) {
		p -= QUEUE_SIZE_MAX;
	}
	u8 t = curr.q[p];
	*px = (t & 0xf0) >> 4;
	*py = (t & 0x0f);
}

static void putQueue(int x, int y) {
	curr.qs -= 1;
	if (curr.qs < 0) curr.qs = QUEUE_SIZE_MAX - 1;
	u8 t = (u8) ((x << 4) | y);
	curr.q[curr.qs] = t;

	curr.ql += 1;
}

static int checkSnakeOnPoint(int x, int y) {
	int i, tx, ty;
	for (i = 0; i < curr.ql; i++) {
		getQueueAt(&tx, &ty, i);
		if ((tx == x) && (ty == y)) {
			return 1;
		} 
	}
	return 0;
}

static void UpdateScreen()
{
	u8 row, col, i, j;			
	u8 buf[16] ;
	//u8 str[6] ;
	u8 charBuf[8];
	int x, y;
	
	
	memset(buf, 0xff, sizeof(buf));
	
	
	change_fresh_rate(0);
	start_spi();


	for (row=0; row<SIZE_ROW; row++) {
		memset(buf, 0x00, sizeof(buf));
		if (row == curr.fdx) {
			buf[curr.fdy] = 0xff;
		}
		for (i = 0; i < curr.ql; i++) {
			getQueueAt(&x, &y, i);
			if (row == x) {
				buf[y] = 0xff;
			}
		}
		for (i = 0; i < 8; i++) {
			write_multiple_lines(row * 8 + i, buf, 16, 1);
		}
		
	}
	
	close_spi();
	
	P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
	if(p_window == NULL)
	{					
		return;
	}

	
	
	if(g_first_enter_handler_flag == 0)
	{
		g_first_enter_handler_flag = 1;
		GRect frame;

		frame.origin.x = 0;
		frame.origin.y = 104;
		frame.size.h = 24;
		frame.size.w = 128;
		
		display_target_layer(p_window,&frame,GAlignCenter,GColorWhite,BMP_SNAKE_BLACK_BOTTOM);

	
		frame.origin.x = (128-4*9)/2-1;
		frame.origin.y = 109;
		frame.size.h = 14;
		frame.size.w = 9;

		uint8_t loop;
		uint16_t array_num = 1000;
			
		for(loop = 0;loop <= 3;loop++)
		{
			g_score_bmp_id_array[loop] = display_target_layer(p_window,&frame,GAlignCenter,GColorWhite,get_LIT_icon_key[curr.score % 10000/array_num%10]);
			frame.origin.x = frame.origin.x + frame.size.w;
			array_num = array_num/10;
		}

		
		frame.origin.x = 0;
		frame.origin.y = 109;
		frame.size.h = 14;
		frame.size.w = 128;

		g_score_pause_text_str_id = display_target_layerText(p_window,&frame,GAlignCenter,GColorBlack,"已暂停，按选择键继续",U_ASCII_ARIAL_12);
		maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_score_pause_text_str_id),false);

	}
	else if(g_first_enter_handler_flag == 1)
	{
		if(g_pre_score != curr.score)
		{
			g_pre_score = curr.score;						

			uint8_t loop;
			uint16_t array_num = 1000;
				
			for(loop = 0;loop <= 3;loop++)
			{
				P_Layer layer = app_window_get_layer_by_id(p_window,g_score_bmp_id_array[loop]);
				
				
				GBitmap bitmap;
				res_get_user_bitmap(get_LIT_icon_key[curr.score % 10000/array_num%10], &bitmap);
				array_num = array_num/10;
				app_layer_set_bitmap_bitmap(layer,&bitmap);
		
			}			
		}
	}

	app_window_update(p_window);
	
}

static u32 seed;

static int updateRandSeed() {
	u16 x,y, z;
	maibu_get_accel_data(&x, &y, &z);

	seed += x;
	seed += y;
	seed += z;
}

static int myRand() {
	seed *= 3;
	seed += 17355;
	return (seed % 10000);
}

static int getNext(int* x, int* y) {
	*x += curr.dx;
	*y += curr.dy;
	
    if (*x < 0) return 0;
    if (*y < 0) return 0;
    if (*x >= SIZE_ROW) return 0;
    if (*y >= SIZE_COL) return 0;
    return 1;
}

static void gameOver() {
	curr.status = GAME_STATUS_GAMEOVER;
	app_service_timer_unsubscribe(g_timer_id);
	window_reloading();	
}



static void makeFd() {
	int x, y;
	while(1) {
		x = myRand() % (SIZE_ROW - 2) + 1;
		y = myRand() % (SIZE_COL - 2) + 1;
		if (!checkSnakeOnPoint(x, y)) {
			curr.fdx = x;
			curr.fdy = y;
			return;
		}
	}
}

static void gameStep() {
	int headX, headY, tailX, tailY;

    if ((curr.tdy != 0) || (curr.tdx != 0)) {
        curr.dx = curr.tdx;
        curr.dy = curr.tdy;
        curr.tdx = 0;
        curr.tdy = 0;
    }


	getQueueAt(&headX, &headY, 0);


	if (!getNext(&headX, &headY)) {
		gameOver();
		
		return;
	}


	getQueueAt(&tailX, &tailY, curr.ql - 1);
	
	if (checkSnakeOnPoint(headX, headY)) {
		gameOver();
		
		return;
	}

	if ((headX == curr.fdx) && (headY == curr.fdy)) {
		g_score_plus++;
		curr.score += g_score_plus;
		putQueue(headX, headY);
		if (curr.ql >= QUEUE_SIZE_MAX - 3) {

			curr.ql -= 1;
		}
		makeFd();
		return;
	}
	curr.ql -= 1;
	putQueue(headX, headY);
}

static void GameTimerCallback(date_time_t tick_time, uint32_t millis, void* context) {
	if (curr.status == GAME_STATUS_PLAYING) {
		updateRandSeed();
		gameStep();
		if(curr.status == GAME_STATUS_PLAYING)
		{
			UpdateScreen();
		}
	}
	
}


static int initGame() {
	memset(&curr, 0, sizeof(curr));
	int i;

	for (i = 5; i >= 0; i--) {
		putQueue(8, 6+i);
	}

	curr.dx = 0;
	curr.dy = -1;
	makeFd();
	curr.status = GAME_STATUS_PLAYING;
	g_first_enter_handler_flag = 0;
	g_score_plus = 10;
}

static void exitGame() {
	curr.status = GAME_STATUS_INVALID;
}

static void pauseGame() {
	if (curr.status == GAME_STATUS_PLAYING) {
		curr.status = GAME_STATUS_PAUSED;
	}
	
	
}

static void resumeGame() {
	if (curr.status == GAME_STATUS_PAUSED) {
		curr.status = GAME_STATUS_PLAYING;
	}

	
}

static void turnLeftRight(int isRight) {
	curr.tdx = curr.dy;
	curr.tdy = curr.dx;
	if (curr.dy != 0) {
		curr.tdx = -curr.tdx;
		curr.tdy = -curr.tdy;
	}
	if (!isRight) {
		curr.tdx = -curr.tdx;
		curr.tdy = -curr.tdy;
	}
}

static void onKeyDown() {
	if (curr.status == GAME_STATUS_PLAYING) {
		turnLeftRight(0);
	}
		

}

static void onKeyUp() {
	if (curr.status == GAME_STATUS_PLAYING) {
		turnLeftRight(1);
	}
		
	
}

static void onKeyBack() {
	P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
	if(p_window != NULL)
	{
		exitGame();
		app_window_stack_pop(p_window);
		
	}
	
}

static void onKeySelect() {
	P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
	if (NULL != p_window)
	{
		if (curr.status == GAME_STATUS_INVALID) {
			curr.status = GAME_STATUS_PLAYING;
			initGame();
			
		} else if (curr.status == GAME_STATUS_PAUSED) {
			maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_score_pause_text_str_id),false);
			app_window_update(p_window);		
			resumeGame();
		} else if (curr.status == GAME_STATUS_PLAYING) {
			maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_score_pause_text_str_id),true);
			app_window_update(p_window);		
			pauseGame();
		} else if (curr.status == GAME_STATUS_GAMEOVER) {
			curr.status = GAME_STATUS_INVALID;
			window_reloading();
		}
	}	
}



static P_Window init_window(void)
{
	P_Window p_window  = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}
		
	/*创建文本图层框架范围*/
	GRect frame = {{25, 25}, {38, 78}};

	if(curr.status == GAME_STATUS_INVALID)
	{
		display_target_layer(p_window,&frame,GAlignCenter,GColorWhite,BMP_SNAKE_TITLE);

		frame.origin.x = 0;
		frame.origin.y = 104;
		frame.size.h = 24;
		frame.size.w = 128;
		
		display_target_layer(p_window,&frame,GAlignCenter,GColorWhite,BMP_SNAKE_BLACK_BOTTOM);
			
		frame.origin.x = 0;
		frame.origin.y = 85;
		frame.size.h = 12;
		
		display_target_layerText(p_window,&frame,GAlignCenter,GColorWhite,"上键-左   下键-右",U_ASCII_ARIAL_12);


		frame.origin.x = 0;
		frame.origin.y = 110;
			
		display_target_layerText(p_window,&frame,GAlignCenter,GColorBlack,"按选择键开始",U_ASCII_ARIAL_12);
		
	}
	else if(curr.status == GAME_STATUS_GAMEOVER)
	{
		
		frame.origin.x = 0;
		frame.origin.y = 0;
		frame.size.h = 81;
		frame.size.w = 128;
				
		display_target_layer(p_window,&frame,GAlignCenter,GColorWhite,BMP_SNAKE_GAME_OVER);

		frame.origin.x = (128-4*18)/2-1;
		frame.origin.y = 84;
		frame.size.h = 20;
		frame.size.w = 18;

		uint8_t loop;
		uint16_t array_num = 1000;
			
		for(loop = 0;loop <= 3;loop++)
		{
			display_target_layer(p_window,&frame,GAlignCenter,GColorWhite,get_BIG_icon_key[curr.score % 10000/array_num%10]);
			frame.origin.x = frame.origin.x + frame.size.w;
			array_num = array_num/10;
		}
		
		frame.origin.x = 0;
		frame.origin.y = 110;
		frame.size.h = 12;
		frame.size.w = 128;
		
		display_target_layerText(p_window,&frame,GAlignCenter,GColorWhite,"按选择键再来一次",U_ASCII_ARIAL_12);
		
	}

	app_window_click_subscribe(p_window, ButtonIdDown, onKeyDown);
	app_window_click_subscribe(p_window, ButtonIdUp, onKeyUp);
	app_window_click_subscribe(p_window, ButtonIdBack, onKeyBack);
	app_window_click_subscribe(p_window, ButtonIdSelect, onKeySelect);

	if(curr.status != GAME_STATUS_GAMEOVER)
	{
		g_timer_id = app_window_timer_subscribe(p_window, 400, GameTimerCallback, 0);
	}
	return p_window;

}

int main() {

	P_Window p_window = init_window();
	if (NULL == p_window)
	{
		exitGame();
		return 0;
	}	

	
	/*把窗口放入窗口栈中显示*/
	g_window_id = app_window_stack_push(p_window);
	
	return 0;
}

