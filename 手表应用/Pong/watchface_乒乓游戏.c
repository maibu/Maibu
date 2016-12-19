#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "maibu_sdk.h"
#include "maibu_res.h"

#define WATCHFACE_PING_BG_ORIGIN_X 	0
#define WATCHFACE_PING_BG_ORIGIN_Y 	0
#define WATCHFACE_PING_BG_SIZE_H		128
#define WATCHFACE_PING_BG_SIZE_W 	128

#define WATCHFACE_PING_TIME_HOUR1_ORIGIN_X 	23
#define WATCHFACE_PING_TIME_HOUR1_ORIGIN_Y 	5
#define WATCHFACE_PING_TIME_HOUR1_SIZE_H	12
#define WATCHFACE_PING_TIME_HOUR1_SIZE_W 	10

#define WATCHFACE_PING_TIME_HOUR2_ORIGIN_X 	33
#define WATCHFACE_PING_TIME_HOUR2_ORIGIN_Y 	5
#define WATCHFACE_PING_TIME_HOUR2_SIZE_H	12
#define WATCHFACE_PING_TIME_HOUR2_SIZE_W 	10

#define WATCHFACE_PING_TIME_MIN1_ORIGIN_X 	87
#define WATCHFACE_PING_TIME_MIN1_ORIGIN_Y 	5
#define WATCHFACE_PING_TIME_MIN1_SIZE_H		12
#define WATCHFACE_PING_TIME_MIN1_SIZE_W 	10

#define WATCHFACE_PING_TIME_MIN2_ORIGIN_X 	97
#define WATCHFACE_PING_TIME_MIN2_ORIGIN_Y 	5
#define WATCHFACE_PING_TIME_MIN2_SIZE_H		12
#define WATCHFACE_PING_TIME_MIN2_SIZE_W 	10

#define WATCHFACE_PING_LEFT_BOARD_START_ORIGIN_X 	4
#define WATCHFACE_PING_LEFT_BOARD_START_ORIGIN_Y 	50
#define WATCHFACE_PING_LEFT_BOARD_START_SIZE_H		28
#define WATCHFACE_PING_LEFT_BOARD_START_SIZE_W 		5

#define WATCHFACE_PING_RIGHT_BOARD_START_ORIGIN_X 	119
#define WATCHFACE_PING_RIGHT_BOARD_START_ORIGIN_Y 	50
#define WATCHFACE_PING_RIGHT_BOARD_START_SIZE_H		28
#define WATCHFACE_PING_RIGHT_BOARD_START_SIZE_W 	5

#define WATCHFACE_PING_BALL_START_ORIGIN_X 	111
#define WATCHFACE_PING_BALL_START_ORIGIN_Y 	61
#define WATCHFACE_PING_BALL_START_SIZE_H	6
#define WATCHFACE_PING_BALL_START_SIZE_W 	6

#define GAME_STATUS_BEGOAL (5)
#define GAME_STATUS_INITGAME (4)
#define GAME_STATUS_GOAL (3)
#define GAME_STATUS_PLAYING (2)
#define GAME_STATUS_PAUSED (1)
#define GAME_STATUS_BEREADY (0)

#define L_BOARD 0
#define R_BOARD 1
#define BALL 2

#define TIMER_SEC 16

//单次移动距离
#define START_MOVE_ONCE 4

static uint8_t g_ball_move = START_MOVE_ONCE;


static const GRect bmp_origin_size_bg = {
	{WATCHFACE_PING_BG_ORIGIN_X,WATCHFACE_PING_BG_ORIGIN_Y},
	{WATCHFACE_PING_BG_SIZE_H,WATCHFACE_PING_BG_SIZE_W}
};


static GRect bmp_origin_size[] = {

	{
		{WATCHFACE_PING_LEFT_BOARD_START_ORIGIN_X,WATCHFACE_PING_LEFT_BOARD_START_ORIGIN_Y},
		{WATCHFACE_PING_LEFT_BOARD_START_SIZE_H,WATCHFACE_PING_LEFT_BOARD_START_SIZE_W}
	},

	{
		{WATCHFACE_PING_RIGHT_BOARD_START_ORIGIN_X,WATCHFACE_PING_RIGHT_BOARD_START_ORIGIN_Y},
		{WATCHFACE_PING_RIGHT_BOARD_START_SIZE_H,WATCHFACE_PING_RIGHT_BOARD_START_SIZE_W}
	},

	{
		{WATCHFACE_PING_BALL_START_ORIGIN_X,WATCHFACE_PING_BALL_START_ORIGIN_Y},
		{WATCHFACE_PING_BALL_START_SIZE_H,WATCHFACE_PING_BALL_START_SIZE_W}
	},
	
	{
		{WATCHFACE_PING_TIME_HOUR1_ORIGIN_X,WATCHFACE_PING_TIME_HOUR1_ORIGIN_Y},
		{WATCHFACE_PING_TIME_HOUR1_SIZE_H,WATCHFACE_PING_TIME_HOUR1_SIZE_W}
	},
	{
		{WATCHFACE_PING_TIME_HOUR2_ORIGIN_X,WATCHFACE_PING_TIME_HOUR2_ORIGIN_Y},
		{WATCHFACE_PING_TIME_HOUR2_SIZE_H,WATCHFACE_PING_TIME_HOUR2_SIZE_W}
	},
	{
		{WATCHFACE_PING_TIME_MIN1_ORIGIN_X,WATCHFACE_PING_TIME_MIN1_ORIGIN_Y},
		{WATCHFACE_PING_TIME_MIN1_SIZE_H,WATCHFACE_PING_TIME_MIN1_SIZE_W}
	},	

	{
		{WATCHFACE_PING_TIME_MIN2_ORIGIN_X,WATCHFACE_PING_TIME_MIN2_ORIGIN_Y},
		{WATCHFACE_PING_TIME_MIN2_SIZE_H,WATCHFACE_PING_TIME_MIN2_SIZE_W}
	}

};


//图片ID数组
static const int32_t bmp_array_name[] = {
	RES_BITMAP_WATCHFACE_NUMBER_0,
	RES_BITMAP_WATCHFACE_NUMBER_1,
	RES_BITMAP_WATCHFACE_NUMBER_2,
	RES_BITMAP_WATCHFACE_NUMBER_3,
	RES_BITMAP_WATCHFACE_NUMBER_4,
	RES_BITMAP_WATCHFACE_NUMBER_5,
	RES_BITMAP_WATCHFACE_NUMBER_6,
	RES_BITMAP_WATCHFACE_NUMBER_7,
	RES_BITMAP_WATCHFACE_NUMBER_8,
	RES_BITMAP_WATCHFACE_NUMBER_9
};


//比分
static uint16_t g_right_score = 0;
static uint16_t g_left_score = 0;

//球被击打数
//static uint16_t g_ball_hints = 0;
//球被击上下顶击打
static uint8_t g_board_top_bottom_hint_flag = 0;


//球位置增量
static int16_t  g_ball_delta_x = 0;
static int16_t  g_ball_delta_y = 0;


static uint8_t curr_status = 0;

//闪烁
static uint8_t g_flicker_count = 0;
static uint8_t g_flicker_time_sec = 0;

//左板
static uint8_t g_left_board_move_timer_count = 0;
//static uint8_t g_left_board_move_timer_count_min = 0;


/*图层ID*/
static int8_t g_ping_layer_id[3] = {-1,-1,-1};
static int8_t g_game_pause_layer_id = -1;

/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_window_id = -1;

static int8_t g_show_UI = 0;


static P_Window init_window(void);
static void element_move(int8_t element,int16_t distance);


//重新载入并刷新窗口所有图层
void window_reloading(void)
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


uint32_t get_random_number(uint8_t min,uint8_t max)
{
	int16_t x, y, z;
	maibu_get_accel_data(&x, &y, &z);
	struct date_time datetime;
	app_service_get_datetime(&datetime);

	uint32_t ret = max - ((x+y+z+(datetime.sec*datetime.min))%(max - min+1));
	

	return ret;
} 

static void recovery_pram()
{
	
	g_ball_move = START_MOVE_ONCE;
	
	bmp_origin_size[L_BOARD].origin.x = WATCHFACE_PING_LEFT_BOARD_START_ORIGIN_X;
	bmp_origin_size[L_BOARD].origin.y = WATCHFACE_PING_LEFT_BOARD_START_ORIGIN_Y;
	bmp_origin_size[R_BOARD].origin.x = WATCHFACE_PING_RIGHT_BOARD_START_ORIGIN_X;
	bmp_origin_size[R_BOARD].origin.y = WATCHFACE_PING_RIGHT_BOARD_START_ORIGIN_Y;
	bmp_origin_size[BALL].origin.x = WATCHFACE_PING_BALL_START_ORIGIN_X;
	bmp_origin_size[BALL].origin.y = WATCHFACE_PING_BALL_START_ORIGIN_Y;
}


static void exitGame() {

	
	g_right_score = 0;
	g_left_score = 0;
	g_show_UI = 0;

	recovery_pram();
	
	curr_status = GAME_STATUS_BEREADY;
}

static void initGame() {

	exitGame();
}

static void readyGame() {
	if (curr_status == GAME_STATUS_GOAL) {
		
		curr_status = GAME_STATUS_BEREADY;
	}	
}


static void goalGame(int8_t goal_side) {
	if (curr_status == GAME_STATUS_PLAYING) {
		if(goal_side == R_BOARD)
		{
			if(g_right_score<99)
			{
				g_right_score++;
			}
		}
		else if(goal_side == L_BOARD)
		{
			if(g_right_score<99)
			{
				g_left_score++;
			}
		}
		curr_status = GAME_STATUS_GOAL;
		window_reloading();
	}	
}

static void pauseGame() {
	if (curr_status == GAME_STATUS_PLAYING) {
		curr_status = GAME_STATUS_PAUSED;
	}	
}

static void resumeGame() {
	if (curr_status == GAME_STATUS_PAUSED) {
		curr_status = GAME_STATUS_PLAYING;
	}
	
}

/*
 *--------------------------------------------------------------------------------------
 *     function:  
 *    parameter: 
 *       return:
 *  description:  生成表盘窗口的各图层
 * 	      other:
 *--------------------------------------------------------------------------------------
 */

/*创建并显示图片图层，需要坐标值，得到icon_key的数组，数组需要的参数值，P_Window*/
static int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,enum GAlign how_to_align,enum GColor black_or_white,uint16_t bmp_key)
{	

	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_key, &bmp_point);
	LayerBitmap layer_bitmap_struct_l = {bmp_point, *temp_p_frame, how_to_align};
 	temp_P_Layer = app_layer_create_bitmap(&layer_bitmap_struct_l);
	
	if(temp_P_Layer != NULL)
	{
		app_layer_set_bg_color(temp_P_Layer, black_or_white);
		return app_window_add_layer(p_window, temp_P_Layer);
	}

	return 0;
}

static int32_t update_target_layer(P_Window p_window,int8_t old_id,GRect *temp_p_frame,uint16_t bmp_key)
{
	P_Layer old_layer = app_window_get_layer_by_id(p_window,old_id);
	
	GBitmap bmp_point;
	
	res_get_user_bitmap(bmp_key, &bmp_point);
	LayerBitmap layer_bitmap_struct_l = {bmp_point, *temp_p_frame, GAlignCenter};
 	P_Layer new_layer = app_layer_create_bitmap(&layer_bitmap_struct_l);
	
	if(new_layer == NULL)
	{
		return old_id;
	}	
	
	return app_window_replace_layer(p_window,old_layer,new_layer);
	
}


static void calculate_next_position(GPoint *next,int16_t distance)
{
	int16_t next_x;
	int16_t next_y;

	double mul = distance/START_MOVE_ONCE;
	if((int)mul < 1)
	{
		mul = 1.0;
	}
	next_x = bmp_origin_size[BALL].origin.x + (int16_t)(g_ball_delta_x*mul);
	next_y = bmp_origin_size[BALL].origin.y + (int16_t)(g_ball_delta_y*mul);

	if(g_board_top_bottom_hint_flag == 0)
	{
		//球击打左板
		if((next_x <= bmp_origin_size[L_BOARD].origin.x + WATCHFACE_PING_LEFT_BOARD_START_SIZE_W)
			&&(next_y > bmp_origin_size[L_BOARD].origin.y - WATCHFACE_PING_BALL_START_SIZE_H)
			&&(next_y < bmp_origin_size[L_BOARD].origin.y + WATCHFACE_PING_LEFT_BOARD_START_SIZE_H))
		{
			next_x = (bmp_origin_size[L_BOARD].origin.x + WATCHFACE_PING_LEFT_BOARD_START_SIZE_W)*2 - next_x;
			g_ball_delta_x = -g_ball_delta_x;
		}	
		//球击打右板
		else if((next_x >= bmp_origin_size[R_BOARD].origin.x - WATCHFACE_PING_BALL_START_SIZE_W)
			&&(next_y > bmp_origin_size[R_BOARD].origin.y - WATCHFACE_PING_BALL_START_SIZE_H)
			&&(next_y < bmp_origin_size[R_BOARD].origin.y + WATCHFACE_PING_LEFT_BOARD_START_SIZE_H))
		{
			next_x = (bmp_origin_size[R_BOARD].origin.x - WATCHFACE_PING_BALL_START_SIZE_W)*2 - next_x;
			
			g_ball_delta_x = -g_ball_delta_x;
		}
	}
	g_board_top_bottom_hint_flag = 0;

	if(next_y <= 2)
	{
		next_y = 2 - next_y;
		g_ball_delta_y = 0-g_ball_delta_y;
	}
	else if(next_y >= 126-6)
	{
		next_y = 126-6-(next_y%(126-6));
		g_ball_delta_y = 0-g_ball_delta_y;
	}


	next->x = next_x;
	next->y = next_y;
}

static void element_move(int8_t element,int16_t distance)
{
	P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
	if(p_window != NULL)
	{	
		if((element == R_BOARD)||(element == L_BOARD))
		{
			bmp_origin_size[element].origin.y = bmp_origin_size[element].origin.y - distance;

			//y是uint8_t型的，没有负数
			if(bmp_origin_size[element].origin.y > 180)
			{
				bmp_origin_size[element].origin.y = 0;
			}
			else if(bmp_origin_size[element].origin.y > 125-WATCHFACE_PING_RIGHT_BOARD_START_SIZE_H)
			{
				bmp_origin_size[element].origin.y = 125-WATCHFACE_PING_RIGHT_BOARD_START_SIZE_H;
			}

			
			if(((element == L_BOARD)&&(bmp_origin_size[BALL].origin.x < WATCHFACE_PING_LEFT_BOARD_START_ORIGIN_X+WATCHFACE_PING_LEFT_BOARD_START_SIZE_W))
				||((element == R_BOARD)&&(bmp_origin_size[BALL].origin.x > WATCHFACE_PING_RIGHT_BOARD_START_ORIGIN_X)))
			{
				//上顶击打球
				if(((bmp_origin_size[element].origin.y >= bmp_origin_size[BALL].origin.y + WATCHFACE_PING_BALL_START_SIZE_H)
					&&(bmp_origin_size[element].origin.y <= bmp_origin_size[BALL].origin.y))
				//下顶击打球
				||((bmp_origin_size[element].origin.y + WATCHFACE_PING_LEFT_BOARD_START_SIZE_H >= bmp_origin_size[BALL].origin.y)
					&&(bmp_origin_size[element].origin.y + WATCHFACE_PING_LEFT_BOARD_START_SIZE_H <= bmp_origin_size[BALL].origin.y+WATCHFACE_PING_BALL_START_SIZE_H)))
				{
					g_ball_delta_y = 0-g_ball_delta_y;
					g_board_top_bottom_hint_flag = 1;
				}
			}
						
			g_ping_layer_id[element] = update_target_layer(p_window,g_ping_layer_id[element],&bmp_origin_size[element],RES_BITMAP_WATCHFACE_BOARD);
			app_window_update(p_window);
		}
		else if(element == BALL)
		{
						
			GPoint next_position;
			calculate_next_position(&next_position,distance);
						
			bmp_origin_size[BALL].origin = next_position;
		
			g_ping_layer_id[BALL] = update_target_layer(p_window,g_ping_layer_id[BALL],&bmp_origin_size[BALL],RES_BITMAP_WATCHFACE_BALL);
			app_window_update(p_window);

		}
	}
	
}


static void serve_ball(int16_t distance)
{
	double angle;

	if(distance > 0)
	{
		angle = get_random_number(15,49)*3.141592/180;
				
	}else if(distance < 0)
	{
		distance = -distance;
		angle = (-1.0)*get_random_number(15,49)*3.141592/180;
	}
	
	
	g_ball_delta_x = (int16_t)(distance*(-cos(angle)));
	g_ball_delta_y = (int16_t)(distance*(-sin(angle)));
	
	
}

static void Ping_onKeyDown() {
	if(g_show_UI == 0)
	{
		return;
	}
	
	if (curr_status == GAME_STATUS_PLAYING) {
		element_move(R_BOARD,-28);
	}
	else if(curr_status == GAME_STATUS_BEREADY)	
	{
		curr_status = GAME_STATUS_PLAYING;
		serve_ball(-START_MOVE_ONCE);
		element_move(BALL,-g_ball_move);
	}

}

static void Ping_onKeyUp() {
	if(g_show_UI == 0)
	{
		return;
	}
	
	if (curr_status == GAME_STATUS_PLAYING) {
		element_move(R_BOARD,28);
	}
	else if(curr_status == GAME_STATUS_BEREADY)	
	{
		curr_status = GAME_STATUS_PLAYING;
		serve_ball(START_MOVE_ONCE);
		element_move(BALL,g_ball_move);
	}
		
	
}

static void Ping_onKeyBack() {
	P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
	if(p_window != NULL)
	{
		exitGame();
		app_window_stack_pop(p_window);		
	}
	
}

static void Ping_onKeySelect() {
	P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
	if(p_window != NULL)
	{
		
		if(g_show_UI == 0)
		{
			g_show_UI = 1;
			window_reloading();
		}
		else if(g_show_UI == 1)
		{
			if (curr_status == GAME_STATUS_PAUSED) {
					
				resumeGame();
			} else if (curr_status == GAME_STATUS_PLAYING) {
					
				pauseGame();
			}
			
		}
	}
}


/*
 *--------------------------------------------------------------------------------------
 *     function:  sec_callback 
 *    parameter: 
 *       return:
 *  description:  每秒更新一次秒表图层
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
static void sec_callback(date_time_t tick_time, uint32_t millis,void *context)
{
	/*根据窗口ID获取窗口句柄*/
	P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
	if (p_window == NULL)
	{
		return;
	}
	
	if(curr_status == GAME_STATUS_PLAYING)
	{
				
		if(g_right_score + g_left_score <= 5)
		{
			g_ball_move = START_MOVE_ONCE;			
		}
		else if(g_right_score + g_left_score <= 10)
		{
			g_ball_move = START_MOVE_ONCE*1.5;			
		}
		else 
		{
			g_ball_move = START_MOVE_ONCE*2;			
		}
		

		element_move(BALL,g_ball_move);
		
		//判断是否进入左右边界	

		//x是uint8_t型的，没有负数
		if((bmp_origin_size[BALL].origin.x > 180)||(bmp_origin_size[BALL].origin.x <= 2))
		{
			goalGame(R_BOARD);
			return;
		}
		else if(bmp_origin_size[BALL].origin.x >= 125-6)
		{
			goalGame(L_BOARD);
			return;
		}
		
		//左板自动移动规则
		
		if((bmp_origin_size[BALL].origin.x < 61)&&(g_ball_delta_x < 0))
		{
			g_left_board_move_timer_count++;
			
			if(g_left_board_move_timer_count >= 4)
			{
				g_left_board_move_timer_count = 0;
				
				if(bmp_origin_size[L_BOARD].origin.y - bmp_origin_size[BALL].origin.y >= 5)
				{
					element_move(L_BOARD,28);
				}
				else if(bmp_origin_size[L_BOARD].origin.y - bmp_origin_size[BALL].origin.y <= -10)
				{
					element_move(L_BOARD,-28);
				}
			}
		}		
	}
	else if(curr_status == GAME_STATUS_GOAL)
	{
		g_flicker_time_sec++;
		if(g_flicker_time_sec <= 50/TIMER_SEC)
		{
			maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_ping_layer_id[L_BOARD]),false);
			maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_ping_layer_id[R_BOARD]),false);
			
		}
		else if(g_flicker_time_sec <= 100/TIMER_SEC)
		{			
			maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_ping_layer_id[L_BOARD]),true);
			maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_ping_layer_id[R_BOARD]),true);
			
		}
		else
		{
			g_flicker_time_sec = 0;
			g_flicker_count++;
		}
		app_window_update(p_window);
		//退出GOAL状态
		if(g_flicker_count > 4)
		{
			g_flicker_count = 0;
			curr_status = GAME_STATUS_BEREADY;
			recovery_pram();
			window_reloading();
		}
	}

		
}

static P_Window init_window(void)
{
	P_Window p_window = NULL;

	/*创建一个窗口*/
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	/*创建背景图层*/
	if(g_show_UI == 0)
	{
		display_target_layer(p_window,&bmp_origin_size_bg,GAlignLeft,GColorWhite,RES_BITMAP_WATCHFACE_START);
	}
	else if(g_show_UI == 1)
	{
		display_target_layer(p_window,&bmp_origin_size_bg,GAlignLeft,GColorWhite,RES_BITMAP_WATCHFACE_BG);

		/*创建比分图层*/
		uint8_t i = 0;
		uint8_t temp_array[4] = {g_left_score/10,g_left_score%10,g_right_score/10,g_right_score%10};
		for(i = 0;i <= 3;i++)
		{
			display_target_layer(p_window,&bmp_origin_size[i+3],GAlignLeft,GColorWhite,bmp_array_name[temp_array[i]]);
		}

		g_ping_layer_id[L_BOARD] = display_target_layer(p_window,&bmp_origin_size[L_BOARD],GAlignLeft,GColorWhite,RES_BITMAP_WATCHFACE_BOARD);
		g_ping_layer_id[R_BOARD] = display_target_layer(p_window,&bmp_origin_size[R_BOARD],GAlignLeft,GColorWhite,RES_BITMAP_WATCHFACE_BOARD);
		g_ping_layer_id[BALL] = display_target_layer(p_window,&bmp_origin_size[BALL],GAlignLeft,GColorWhite,RES_BITMAP_WATCHFACE_BALL);

		/*定义一个窗口定时器，用于秒表显示*/
		app_window_timer_subscribe(p_window, TIMER_SEC, sec_callback, NULL);
	}	
	
	app_window_click_subscribe(p_window, ButtonIdDown, Ping_onKeyDown);
	app_window_click_subscribe(p_window, ButtonIdUp,  Ping_onKeyUp);
	app_window_click_subscribe(p_window, ButtonIdBack,  Ping_onKeyBack);
	app_window_click_subscribe(p_window, ButtonIdSelect,  Ping_onKeySelect);
	
	return p_window;
}



/*
 *--------------------------------------------------------------------------------------
 *     function:  main
 *    parameter: 
 *       return:
 *  description:  主程序
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
int main()
{
	
	//initGame();
	/*创建显示表盘窗口*/
	P_Window p_window = init_window();
	if (p_window != NULL)
	{
		/*放入窗口栈显示*/
		g_window_id = app_window_stack_push(p_window);
	}
		
	return 0;
}


