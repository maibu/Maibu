/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  app_tomato_clock.c
 *
 *    Description:  番茄钟 
 *    Corporation:
 * 
 *         Author:  gliu (), gliu@damaijiankang.com
 *        Created:  2015年04月03日 10时32分05秒
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

/*背景图层位置*/
#define TOMATO_BG_ORIGIN_X		0	
#define TOMATO_BG_ORIGIN_Y		0
#define TOMATO_BG_SIZE_H		128
#define TOMATO_BG_SIZE_W		128


/*工作倒计时文本图层位置*/
#define TOMATO_TEXT1_ORIGIN_X		0	
#define TOMATO_TEXT1_ORIGIN_Y		32	
#define TOMATO_TEXT1_SIZE_H		42
#define TOMATO_TEXT1_SIZE_W		128


/*按任意键开始文本图层位置*/
#define TOMATO_TEXT2_ORIGIN_X		0	
#define TOMATO_TEXT2_ORIGIN_Y		108	
#define TOMATO_TEXT2_SIZE_H		12
#define TOMATO_TEXT2_SIZE_W		128


/*打起精神开始工作文本图层位置*/
#define TOMATO_TEXT3_ORIGIN_X		0	
#define TOMATO_TEXT3_ORIGIN_Y		78	
#define TOMATO_TEXT3_SIZE_H		12
#define TOMATO_TEXT3_SIZE_W		128


/*休息倒计时文本图层位置*/
#define TOMATO_TEXT4_ORIGIN_X		0	
#define TOMATO_TEXT4_ORIGIN_Y		68	
#define TOMATO_TEXT4_SIZE_H		24
#define TOMATO_TEXT4_SIZE_W		128


/*大番茄图片图层*/
#define TOMATO_BIGT_BMP_ORIGIN_X		28
#define TOMATO_BIGT_BMP_ORIGIN_Y		27	
#define TOMATO_BIGT_BMP_SIZE_H			71
#define TOMATO_BIGT_BMP_SIZE_W			72


/*番茄图片图层位置*/
#define TOMATO_SMALLT_BMP_ORIGIN_X		23
#define TOMATO_SMALLT_BMP_ORIGIN_Y		106	
#define TOMATO_SMALLT_BMP_SIZE_H		12
#define TOMATO_SMALLT_BMP_SIZE_W		82


/*休息时间到图片图层位置*/
#define TOMATO_BREST_BMP_ORIGIN_X		35	
#define TOMATO_BREST_BMP_ORIGIN_Y		34	
#define TOMATO_BREST_BMP_SIZE_H			64
#define TOMATO_BREST_BMP_SIZE_W			59

/*休息倒计时图片图层位置*/
#define TOMATO_RESTING_BMP_ORIGIN_X		44	
#define TOMATO_RESTING_BMP_ORIGIN_Y		34	
#define TOMATO_RESTING_BMP_SIZE_H		28
#define TOMATO_RESTING_BMP_SIZE_W		40

/*结束图片图层位置*/
#define TOMATO_END_BMP_ORIGIN_X		11	
#define TOMATO_END_BMP_ORIGIN_Y		26	
#define TOMATO_END_BMP_SIZE_H		70
#define TOMATO_END_BMP_SIZE_W		106


/*初始状态*/
#define TOMATO_STATE_INIT	0

/*工作开始状态*/
#define TOMATO_STATE_BWORK	1

/*工作中状态*/
#define TOMATO_STATE_WORKING	2

/*休息开始状态*/
#define TOMATO_STATE_BREST	3

/*休息中状态*/
#define TOMATO_STATE_RESTING	4

/*结束状态*/
#define TOMATO_STATE_END	5

/*最大秒数*/
#define TOMATO_MAX_SECONDS	6901


/*番茄时间状态点*/
static const int16_t tomato_state_array[15][2] = 
{
	/*第一个番茄钟*/
	{TOMATO_STATE_INIT, 0},
	{TOMATO_STATE_WORKING, 	1500},
	{TOMATO_STATE_BREST, 	1503},
	{TOMATO_STATE_RESTING, 	1800},

	/*第二个番茄钟*/
	{TOMATO_STATE_BWORK, 1803},
	{TOMATO_STATE_WORKING, 3300},
	{TOMATO_STATE_BREST, 3303},
	{TOMATO_STATE_RESTING, 3600},

	/*第三个番茄钟*/
	{TOMATO_STATE_BWORK, 3603},
	{TOMATO_STATE_WORKING, 5100},
	{TOMATO_STATE_BREST, 5103},
	{TOMATO_STATE_RESTING, 5400},

	/*第四个番茄钟*/
	{TOMATO_STATE_BWORK, 5403},
	{TOMATO_STATE_WORKING, 6900},
	{TOMATO_STATE_END, 6901}
};


/*小番茄图片key*/
static int16_t array[] = {
	RES_BITMAP_WATCHAPP_TOMATO_SMALLT1, 
	RES_BITMAP_WATCHAPP_TOMATO_SMALLT2, 
	RES_BITMAP_WATCHAPP_TOMATO_SMALLT3,
	RES_BITMAP_WATCHAPP_TOMATO_SMALLT4
	};


/*开启标志*/
static int8_t g_tomato_start_flg = 0;

/*定时器ID*/
static int8_t g_tomato_timer_id = -1;

/*总计时, 初始化为-1不能随便修改*/
static int16_t g_tomato_seconds = -1;

/*前一个状态, 不能随便修改*/
static int8_t g_tomato_pre_state = -1;


/*窗口ID*/
static int32_t g_tomato_window_id = -1;

/*工作文本框图层ID*/
static int8_t g_tomato_working_text_layer_id = -1;


/*休息文本框图层ID*/
static int8_t g_tomato_resting_text_layer_id = -1;


/*根据当前番茄钟已运行时间获取状态*/
static int8_t tomato_get_state(int16_t *sec)
{
	int i = 0;

	if (g_tomato_seconds == -1)
	{
		*sec = 0;
		return tomato_state_array[0][0];
	}

	if (g_tomato_seconds == 0)
	{
		*sec = 0;
		return tomato_state_array[1][0];
	}

	for (i = 1; i < (sizeof(tomato_state_array)) / (2 * sizeof(int16_t)); i++)
	{
		if ( (g_tomato_seconds >= (tomato_state_array[i - 1][1])) &&  (g_tomato_seconds < tomato_state_array[i][1]))
		{
			*sec = g_tomato_seconds - tomato_state_array[i - 1][1];
			return tomato_state_array[i][0];
		}
	}
	*sec = 0;
	return TOMATO_STATE_END; 
}



/*定义任意按键事件*/
static void tomato_select_any(void *context)
{
	/*如果是初始状态及完成状态，按键开始*/	
	int16_t sec = 0;
	int8_t state = tomato_get_state(&sec);
	if ((state == TOMATO_STATE_INIT) || (state == TOMATO_STATE_END))
	{
		g_tomato_seconds = -1;
		g_tomato_pre_state = TOMATO_STATE_INIT;
		g_tomato_start_flg = 1;
	}

}


/*定义后退按键事件*/
static void tomato_select_back(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		app_window_stack_pop(p_window);
		g_tomato_seconds = -1;
		g_tomato_pre_state = TOMATO_STATE_INIT;
		g_tomato_start_flg = 0;
		app_service_timer_unsubscribe(g_tomato_timer_id);
	}
}

/*创建图片图层*/
static P_Layer tomato_create_bmp_layer(P_Window p_window,GRect *frame_p,int16_t bmp_key)
{
	
	GBitmap bitmap;
	
	/*获取用户图片*/
	res_get_user_bitmap(bmp_key, &bitmap);
	LayerBitmap lb1 = { bitmap, *frame_p, GAlignLeft};	

	/*图层1*/
	P_Layer layer1 = app_layer_create_bitmap(&lb1);

	return (layer1);
}


/*创建文本图层*/
static P_Layer tomato_create_text_layer(P_Window p_window,char *buf,GRect *frame,uint8_t font_type_m)
{

	/*生成文本结构体*/
	LayerText text = {buf, *frame, GAlignCenter, font_type_m};

	/*创建文本图层*/
	P_Layer layer = app_layer_create_text(&text);
	app_layer_set_bg_color(layer, GColorBlack);

	return layer;

}


/*创建工作中计时图层*/
static P_Layer tomato_create_working_layer(P_Window p_window)
{

	GRect frame = {{TOMATO_TEXT1_ORIGIN_X, TOMATO_TEXT1_ORIGIN_Y}, { TOMATO_TEXT1_SIZE_H, TOMATO_TEXT1_SIZE_W }};
	char buf[8] = "";
	int16_t sec = 0;
	int8_t inter = 0;
	int8_t state = tomato_get_state(&sec);
	
	/*这里特殊处理，当第二个番茄开始，需要减少3秒*/
	if((g_tomato_seconds >= 1500) && (state == TOMATO_STATE_WORKING))
	{
		inter = 3;
	}

	int res_sec = (1500 - sec - inter);
	sprintf(buf, "%d:%02d", res_sec/60, res_sec%60);

	return tomato_create_text_layer(p_window,buf,&frame,U_ASCII_ARIAL_42);
}

/*创建休息中计时图层*/
static P_Layer tomato_create_resting_text_layer(P_Window p_window)
{

	GRect frame = {{TOMATO_TEXT4_ORIGIN_X, TOMATO_TEXT4_ORIGIN_Y}, { TOMATO_TEXT4_SIZE_H, TOMATO_TEXT4_SIZE_W }};
	char buf[8] = "";
	int16_t sec = 0;
	tomato_get_state(&sec);
	int res_sec = (297 - sec);
	sprintf(buf, "%d:%02d", res_sec/60, res_sec%60);

	return tomato_create_text_layer(p_window,buf,&frame,U_ASCII_ARIAL_24);
}


/*
 *--------------------------------------------------------------------------------------
 *     function:  tomato_init_window
 *    parameter: 
 *       return:
 *  description:  初始化窗口
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
static P_Window tomato_init_window()
{
	P_Window p_window = NULL;

	/*创建窗口*/
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

/*--------添加图片图层------------*/
	/*添加黑色背景图片*/
	/*图片显示位置*/
	GRect frame_bmp = {{TOMATO_BG_ORIGIN_X, TOMATO_BG_ORIGIN_Y}, {TOMATO_BG_SIZE_H, TOMATO_BG_SIZE_W}};
	
	/*创建图片图层*/
	P_Layer layer_bmp = tomato_create_bmp_layer(p_window,&frame_bmp,RES_BITMAP_WATCHAPP_TOMATO_BG);

	/*添加图层到窗口*/
	app_window_add_layer(p_window, layer_bmp);
	
/*--------添加状态栏显示时间-------*/
	
	/*添加状态栏, 显示时间*/
	app_plug_status_bar_create(p_window, NULL, NULL, NULL);
	app_plug_status_bar_add_time(p_window);


	/*判断当前状态*/
	int16_t sec = 0;
	int8_t state = tomato_get_state(&sec);
	P_Layer p_layer_working =  NULL;
	P_Layer p_layer_anykey = NULL;
	P_Layer p_layer_bwork = NULL;
	P_Layer p_layer_small_tomato = NULL;
	P_Layer p_layer_brest = NULL;
	P_Layer p_layer_resting_bmp = NULL;
	P_Layer p_layer_resting_text = NULL;
	P_Layer p_layer_end_bmp = NULL;
	P_Layer p_layer_anykey_tomato = NULL;

	char buf[30] = "";	
	
	/*添加小番茄*/
	GRect frame = {{TOMATO_SMALLT_BMP_ORIGIN_X, TOMATO_SMALLT_BMP_ORIGIN_Y}, {TOMATO_SMALLT_BMP_SIZE_H, TOMATO_SMALLT_BMP_SIZE_W}};
	p_layer_small_tomato = tomato_create_bmp_layer(p_window,&frame,array[g_tomato_seconds / 1800]);
	app_window_add_layer(p_window, p_layer_small_tomato);

	if (state == TOMATO_STATE_INIT)
	{
			maibu_layer_set_visible_status(p_layer_small_tomato,false);

			frame.origin.x = TOMATO_BIGT_BMP_ORIGIN_X;
			frame.origin.y = TOMATO_BIGT_BMP_ORIGIN_Y;
			frame.size.h = TOMATO_BIGT_BMP_SIZE_H;
			frame.size.w = TOMATO_BIGT_BMP_SIZE_W;
			p_layer_working = tomato_create_bmp_layer(p_window,&frame,RES_BITMAP_WATCHAPP_TOMATO_TOMATO);

			frame.origin.x = TOMATO_TEXT2_ORIGIN_X;
			frame.origin.y = TOMATO_TEXT2_ORIGIN_Y;
			frame.size.h = TOMATO_TEXT2_SIZE_H;
			frame.size.w = TOMATO_TEXT2_SIZE_W;
			sprintf(buf, "%s", "按任意键开始");
			p_layer_anykey = tomato_create_text_layer(p_window,buf,&frame,U_GBK_SIMSUN_12);

			g_tomato_working_text_layer_id = app_window_add_layer(p_window, p_layer_working);	
			app_window_add_layer(p_window, p_layer_anykey);	
			
			
	}else if (state == TOMATO_STATE_BWORK)
	{

			p_layer_working = tomato_create_working_layer(p_window);

 			frame.origin.x = TOMATO_TEXT3_ORIGIN_X;
			frame.origin.y = TOMATO_TEXT3_ORIGIN_Y;
			frame.size.h = TOMATO_TEXT3_SIZE_H;
			frame.size.w = TOMATO_TEXT3_SIZE_W;
			sprintf(buf, "%s", "打起精神 开始工作");
			p_layer_bwork = tomato_create_text_layer(p_window,buf,&frame,U_GBK_SIMSUN_12);


			g_tomato_working_text_layer_id = app_window_add_layer(p_window, p_layer_working);	
			app_window_add_layer(p_window, p_layer_bwork);
			
			
	}else if (state == TOMATO_STATE_WORKING)
	{
		p_layer_working = tomato_create_working_layer(p_window);
			
		g_tomato_working_text_layer_id = app_window_add_layer(p_window, p_layer_working);	

	}else if (state == TOMATO_STATE_BREST)
	{
		frame.origin.x = TOMATO_BREST_BMP_ORIGIN_X;
		frame.origin.y = TOMATO_BREST_BMP_ORIGIN_Y;
		frame.size.h = TOMATO_BREST_BMP_SIZE_H;
		frame.size.w = TOMATO_BREST_BMP_SIZE_W;
		p_layer_brest = tomato_create_bmp_layer(p_window,&frame,RES_BITMAP_WATCHAPP_TOMATO_BREST);

		
		app_window_add_layer(p_window, p_layer_brest);	
	
	}else if (state == TOMATO_STATE_RESTING)
	{
		frame.origin.x = TOMATO_RESTING_BMP_ORIGIN_X;
		frame.origin.y = TOMATO_RESTING_BMP_ORIGIN_Y;
		frame.size.h = TOMATO_RESTING_BMP_SIZE_H;
		frame.size.w = TOMATO_RESTING_BMP_SIZE_W;
		p_layer_resting_bmp = tomato_create_bmp_layer(p_window,&frame,RES_BITMAP_WATCHAPP_TOMATO_RESTING);
	
		
		p_layer_resting_text = tomato_create_resting_text_layer(p_window);

			
		app_window_add_layer(p_window, p_layer_resting_bmp);	
		g_tomato_resting_text_layer_id = app_window_add_layer(p_window, p_layer_resting_text);	
	}else if (state == TOMATO_STATE_END)
	{
			maibu_layer_set_visible_status(p_layer_small_tomato,false);

			frame.origin.x = TOMATO_END_BMP_ORIGIN_X;
			frame.origin.y = TOMATO_END_BMP_ORIGIN_Y;
			frame.size.h = TOMATO_END_BMP_SIZE_H;
			frame.size.w = TOMATO_END_BMP_SIZE_W;
			p_layer_end_bmp = tomato_create_bmp_layer(p_window,&frame,RES_BITMAP_WATCHAPP_TOMATO_END);


			frame.origin.x = TOMATO_TEXT2_ORIGIN_X;
			frame.origin.y = TOMATO_TEXT2_ORIGIN_Y;
			frame.size.h = TOMATO_TEXT2_SIZE_H;
			frame.size.w = TOMATO_TEXT2_SIZE_W;
			sprintf(buf, "%s", "按任意键开始");
			p_layer_anykey_tomato = tomato_create_text_layer(p_window,buf,&frame,U_GBK_SIMSUN_12);

			app_window_add_layer(p_window, p_layer_end_bmp);	
			app_window_add_layer(p_window, p_layer_anykey_tomato);
			
	}

	/*添加窗口按键事件*/
	app_window_click_subscribe(p_window, ButtonIdDown, tomato_select_any);
	app_window_click_subscribe(p_window, ButtonIdUp, tomato_select_any);
	app_window_click_subscribe(p_window, ButtonIdSelect, tomato_select_any);
	app_window_click_subscribe(p_window, ButtonIdBack, tomato_select_back);


	return (p_window);

}


static void tomato_timer_callback(date_time_t tick_time, uint32_t millis, void *context)
{

	if (g_tomato_start_flg != 1)
	{
		return;
	}

	/*如果是初始状态或者完成状态*/
	g_tomato_seconds++;
	if (g_tomato_seconds > TOMATO_MAX_SECONDS)
	{
		g_tomato_seconds = TOMATO_MAX_SECONDS;
		return;
	}

	/*如果开始番茄钟,需要震动*/
	if (g_tomato_seconds == 0)
	{
		maibu_service_vibes_pulse(VibesPulseTypeShort, 0);
	}

	int16_t sec = 0;
	int8_t state = tomato_get_state(&sec);


	/*根据窗口ID获取窗口句柄*/
	P_Window p_window = app_window_stack_get_window_by_id(g_tomato_window_id);	
	if (NULL == p_window)
	{
		return;
	}


	/*如果不是前面状态，则创建新的窗口*/	
	if (state != g_tomato_pre_state)
	{
		
		/*创建新窗口*/	
		P_Window p_new_window = tomato_init_window();

		/*替换原来的窗口*/
		g_tomato_window_id = app_window_stack_replace_window(p_window, p_new_window);	

		g_tomato_pre_state = state;

		/*如果是工作开始三秒后，不震动及背光*/
		if ((state != TOMATO_STATE_WORKING) && (state != TOMATO_STATE_RESTING))		
		{
			/*新的状态，开启背光3秒和震动1次*/
			maibu_service_vibes_pulse(VibesPulseTypeShort, 0);
		}
	
	}/*更新图层*/	
	else
	{
		
		int16_t res_sec = 0; 
		int8_t inter = 0;
		P_Layer p_old_layer = NULL;
		char text[8] = "";
		
		if (state == TOMATO_STATE_WORKING)
		{
				if (g_tomato_seconds >= 1500)
				{
					inter = 3;		
				}
				res_sec = (1500 - sec - inter);
				p_old_layer = app_window_get_layer_by_id(p_window, g_tomato_working_text_layer_id);
				if(NULL != p_old_layer)
				{
					sprintf(text, "%d:%02d", res_sec/60, res_sec%60);
					app_layer_set_text_text(p_old_layer, text);
					app_window_update(p_window);
				}
		}else if (state == TOMATO_STATE_RESTING)
		{
				res_sec = (297 - sec);
				p_old_layer = app_window_get_layer_by_id(p_window, g_tomato_resting_text_layer_id);
				if(NULL != p_old_layer)
				{
					sprintf(text, "%d:%02d", res_sec/60, res_sec%60);
					app_layer_set_text_text(p_old_layer, text);
					app_window_update(p_window);
				}
		}

	}
}

int main(void)
{

	/*APP编写*/
	/*创建日期时间设置窗口*/
	P_Window p_window = tomato_init_window(); 

	/*放入窗口栈显示*/
	g_tomato_window_id = app_window_stack_push(p_window);

	/*添加定时器*/
	g_tomato_timer_id = app_service_timer_subscribe(1000, tomato_timer_callback, (void *)p_window);	

	return 0;

}

