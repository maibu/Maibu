/*
 * =====================================================================================
 * 
 *    Corporation:  Shenzhen Maibu Technology Co., Ltd. All Rights Reserved.
 *       Filename:  app_count_score.c
 *         Author:  gliu , gliu@maibu.cc
 *        Created:  2015年09月16日 09时58分07秒
 * 
 *    Description:  比赛计分员
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


#ifdef LINUX
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "linux_screen_show.h"
#include "os_store_manage.h"
#include "os_time.h"
#include "os_res_bitmap.h"
#include "os_comm.h"
#include "res_user_bitmap.h"
#include "os_memory_manage.h"
#include "os_motor.h"
#include "os_lights.h"
#include "window_stack.h"
#include "window.h"
#include "screen_show.h"
#include "matrix.h"
#include "plug_status_bar.h"
#include "res_bitmap_base.h"
#include "os_app_manage.h"
#include "os_user_app_id.h"
#else

#include "maibu_sdk.h"
#include "maibu_res.h"

#endif



/*背景图层位置*/
#define COUNT_SCORE_BG_ORIGIN_X			0	
#define COUNT_SCORE_BG_ORIGIN_Y			0
#define COUNT_SCORE_BG_SIZE_H			128		
#define COUNT_SCORE_BG_SIZE_W			128	


/*首页图层位置*/
#define COUNT_SCORE_INTRO_ORIGIN_X		22	
#define COUNT_SCORE_INTRO_ORIGIN_Y		37
#define COUNT_SCORE_INTRO_SIZE_H			70	
#define COUNT_SCORE_INTRO_SIZE_W			83	


/*A图层*/
#define COUNT_SCORE_A_ORIGIN_X			20
#define COUNT_SCORE_A_ORIGIN_Y			32
#define COUNT_SCORE_A_SIZE_H			24	
#define COUNT_SCORE_A_SIZE_W			88


/*B图层*/
#define COUNT_SCORE_B_ORIGIN_X			20	
#define COUNT_SCORE_B_ORIGIN_Y			87
#define COUNT_SCORE_B_SIZE_H			24	
#define COUNT_SCORE_B_SIZE_W			88	


/*清零图层*/
#define COUNT_SCORE_CLEAR_ORIGIN_X			0
#define COUNT_SCORE_CLEAR_ORIGIN_Y			0
#define COUNT_SCORE_CLEAR_SIZE_H			128	
#define COUNT_SCORE_CLEAR_SIZE_W			128	


/*文件KEY*/
#define COUNT_SCORE_KEY		1


typedef struct tag_Score
{
	int16_t a;
	int16_t b;	
}Score, *P_Score;


static Score g_score;


/*窗口ID*/
static int32_t g_count_score_window_id = -1;

/*A图层ID*/
static int8_t g_count_score_a_layer_id = -1;

/*B图层ID*/
static int8_t g_count_score_b_layer_id = -1;




extern P_Window count_score_create_start_window();
extern P_Window count_score_create_intro_window();
extern P_Window count_score_create_clear_window();


/*定义后退按键事件*/
static void count_score_select_back(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		app_window_stack_pop(p_window);
	}

	/*保存数据*/
	#ifdef LINUX	
	app_persist_write_data_extend( USER_APP_ID_COUNT_SCORE, COUNT_SCORE_KEY, (unsigned char *)&g_score, sizeof(Score));
	#else
	app_persist_write_data_extend( COUNT_SCORE_KEY, (unsigned char *)&g_score, sizeof(Score));
	#endif

}


/*定义进入计分界面按键事件*/
static void count_score_start(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		P_Window p_new_window = count_score_create_start_window();
		g_count_score_window_id = app_window_stack_replace_window(p_window, p_new_window);	
	}

}



/*定义上按键事件*/
static void count_score_a_plus(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		g_score.a++;
		if (g_score.a > 999)
		{
			g_score.a = 999;
		}
		P_Layer p_a_layer = app_window_get_layer_by_id(p_window, g_count_score_a_layer_id);
		if (NULL != p_a_layer)
		{
			char buf[5] = "";
			sprintf(buf, "%02d", g_score.a);
			app_layer_set_text_text(p_a_layer, buf);
			app_window_update(p_window);
		}	

		/*震动*/
		maibu_service_vibes_pulse(VibesPulseTypeShort, 0);
	}
}


/*定义下按键事件*/
static void count_score_b_plus(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		g_score.b++;
		if (g_score.b > 999)
		{
			g_score.b = 999;
		}
		P_Layer p_b_layer = app_window_get_layer_by_id(p_window, g_count_score_b_layer_id);
		if (NULL != p_b_layer)
		{
			char buf[5] = "";
			sprintf(buf, "%02d", g_score.b);
			app_layer_set_text_text(p_b_layer, buf);
			app_window_update(p_window);
		}

		/*震动*/
		maibu_service_vibes_pulse(VibesPulseTypeShort, 0);
	}
}


/*定义选择清除按键事件*/
static void count_score_select(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		P_Window p_new_window = count_score_create_clear_window();
		g_count_score_window_id = app_window_stack_replace_window(p_window, p_new_window);	
	}
}


/*定义清除OK事件*/
static void count_score_clear_ok(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		g_score.a = 0;
		g_score.b = 0;

		P_Window p_new_window = count_score_create_start_window();
		g_count_score_window_id = app_window_stack_replace_window(p_window, p_new_window);	
	}
}


/*定义清除CANCEL事件*/
static void count_score_clear_cancel(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		P_Window p_new_window = count_score_create_start_window();
		g_count_score_window_id = app_window_stack_replace_window(p_window, p_new_window);	
	}
}



/*
 *--------------------------------------------------------------------------------------
 *     function:  count_score_create_clear_window
 *    parameter: 
 *       return:
 *  description:  创建开始清除窗口
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Window count_score_create_clear_window()
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	/*添加背景图片图层*/
	GRect frame_bg_bmp = {{COUNT_SCORE_CLEAR_ORIGIN_X, COUNT_SCORE_CLEAR_ORIGIN_Y}, 
			{ COUNT_SCORE_CLEAR_SIZE_H, COUNT_SCORE_CLEAR_SIZE_W}};
	GBitmap bg_bitmap;
	res_get_user_bitmap(RES_BITMAP_WATCHAPP_COUNT_SCORE_CLEAR_BMP, &bg_bitmap);
	LayerBitmap lb1 = {bg_bitmap, frame_bg_bmp, GAlignCenter};	
	P_Layer	 layer_bg_bmp = app_layer_create_bitmap(&lb1);
	app_window_add_layer(p_window, layer_bg_bmp);


	/*后退按键回调*/
	app_window_click_subscribe(p_window, ButtonIdBack, count_score_clear_cancel);
	app_window_click_subscribe(p_window, ButtonIdUp, count_score_clear_ok);
	app_window_click_subscribe(p_window, ButtonIdDown, count_score_clear_cancel);
	
	return (p_window);
}





/*
 *--------------------------------------------------------------------------------------
 *     function:  count_score_create_start_window
 *    parameter: 
 *       return:
 *  description:  创建开始计分窗口
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Window count_score_create_start_window()
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}




	/*添加背景图片图层*/
	GRect frame_bg_bmp = {{COUNT_SCORE_BG_ORIGIN_X, COUNT_SCORE_BG_ORIGIN_Y}, 
			{ COUNT_SCORE_BG_SIZE_H, COUNT_SCORE_BG_SIZE_W}};
	GBitmap bg_bitmap;
	res_get_user_bitmap(RES_BITMAP_WATCHAPP_COUNT_SCORE_BG_BMP, &bg_bitmap);
	LayerBitmap lb1 = {bg_bitmap, frame_bg_bmp, GAlignCenter};	
	P_Layer	 layer_bg_bmp = app_layer_create_bitmap(&lb1);
	app_window_add_layer(p_window, layer_bg_bmp);

	/*添加状态栏, 显示时间*/
	app_plug_status_bar_create(p_window, NULL, NULL, NULL);
	app_plug_status_bar_add_time(p_window);
	app_plug_status_bar_add_battery(p_window);
	
	/*添加a分数文本图层*/
	GRect a_frame = {{COUNT_SCORE_A_ORIGIN_X, COUNT_SCORE_A_ORIGIN_Y}, 
				{COUNT_SCORE_A_SIZE_H, COUNT_SCORE_A_SIZE_W}};
	char buf[5] = "";
	sprintf(buf, "%02d", g_score.a);
	LayerText a_text = {buf, a_frame, GAlignCenter, U_ASCII_ARIAL_24};
	P_Layer a_layer = app_layer_create_text(&a_text);
	app_layer_set_bg_color(a_layer, GColorBlack);
	g_count_score_a_layer_id = app_window_add_layer(p_window, a_layer);


	/*添加b分数文本图层*/
	GRect b_frame = {{COUNT_SCORE_B_ORIGIN_X, COUNT_SCORE_B_ORIGIN_Y}, 
				{COUNT_SCORE_B_SIZE_H, COUNT_SCORE_B_SIZE_W}};
	sprintf(buf, "%02d", g_score.b);
	LayerText b_text = {buf, b_frame, GAlignCenter, U_ASCII_ARIAL_24};
	P_Layer b_layer = app_layer_create_text(&b_text);
	app_layer_set_bg_color(b_layer, GColorBlack);
	g_count_score_b_layer_id = app_window_add_layer(p_window, b_layer);


	/*后退按键回调*/
	app_window_click_subscribe(p_window, ButtonIdBack, count_score_select_back);
	app_window_click_subscribe(p_window, ButtonIdUp, count_score_a_plus);
	app_window_click_subscribe(p_window, ButtonIdDown, count_score_b_plus);
	app_window_click_subscribe(p_window, ButtonIdSelect, count_score_select);
	
	return (p_window);
}


/*
 *--------------------------------------------------------------------------------------
 *     function:  count_score_create_intro_window
 *    parameter: 
 *       return:
 *  description:  创建显示介绍页窗口
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Window count_score_create_intro_window()
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}




	/*添加背景图片图层*/
	GRect frame_bg_bmp = {{COUNT_SCORE_BG_ORIGIN_X, COUNT_SCORE_BG_ORIGIN_Y}, 
			{ COUNT_SCORE_BG_SIZE_H, COUNT_SCORE_BG_SIZE_W}};
	GBitmap bg_bitmap;
	res_get_user_bitmap(RES_BITMAP_WATCHAPP_COUNT_SCORE_BG_BMP, &bg_bitmap);
	LayerBitmap lb1 = {bg_bitmap, frame_bg_bmp, GAlignCenter};	
	P_Layer	 layer_bg_bmp = app_layer_create_bitmap(&lb1);
	app_window_add_layer(p_window, layer_bg_bmp);

	/*添加状态栏, 显示时间*/
	app_plug_status_bar_create(p_window, NULL, NULL, NULL);
	app_plug_status_bar_add_time(p_window);
	app_plug_status_bar_add_battery(p_window);
	
	/*添加说明图片图层*/
	GRect frame_intro_bmp = {{COUNT_SCORE_INTRO_ORIGIN_X, COUNT_SCORE_INTRO_ORIGIN_Y}, 
			{ COUNT_SCORE_INTRO_SIZE_H, COUNT_SCORE_INTRO_SIZE_W}};
	GBitmap intro_bitmap;
	res_get_user_bitmap(RES_BITMAP_WATCHAPP_COUNT_SCORE_INTRO_BMP, &intro_bitmap);
	LayerBitmap lb2 = {intro_bitmap, frame_intro_bmp, GAlignCenter};	
	P_Layer	 layer_intro_bmp = app_layer_create_bitmap(&lb2);
	app_window_add_layer(p_window, layer_intro_bmp);


	/*后退按键回调*/
	app_window_click_subscribe(p_window, ButtonIdBack, count_score_select_back);
	app_window_click_subscribe(p_window, ButtonIdUp, count_score_start);
	app_window_click_subscribe(p_window, ButtonIdDown, count_score_start);
	app_window_click_subscribe(p_window, ButtonIdSelect, count_score_start);
	
	return (p_window);
}







/*
 *--------------------------------------------------------------------------------------
 *     function:  count_score_create_init_window
 *    parameter: 
 *       return:
 *  description:  
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Window count_score_create_init_window()
{
	P_Window p_window = NULL;

	Score data;
	memset(&data, 0, sizeof(Score));

	/*创建一个可读可写的保存构体的文件key*/
	#ifdef LINUX
	app_persist_create(USER_APP_ID_COUNT_SCORE, COUNT_SCORE_KEY, sizeof(Score));
	#else
	app_persist_create(COUNT_SCORE_KEY, sizeof(Score));
	#endif


	/*读取结构信息*/
	#ifdef LINUX
	app_persist_read_data(USER_APP_ID_COUNT_SCORE, COUNT_SCORE_KEY, 0, (unsigned char *)&data, sizeof(Score));
	#else
	app_persist_read_data(COUNT_SCORE_KEY, 0, (unsigned char *)&data, sizeof(Score));
	#endif

	/*有一个大，则替换并保存*/
	if ((g_score.a > data.a) || (g_score.b > data.b))
	{
		/*保存数据*/
		#ifdef LINUX	
		app_persist_write_data_extend( USER_APP_ID_COUNT_SCORE, COUNT_SCORE_KEY, (unsigned char *)&g_score, sizeof(Score));
		#else
		app_persist_write_data_extend( COUNT_SCORE_KEY, (unsigned char *)&g_score, sizeof(Score));
		#endif
	}
	else
	{
		g_score.a = data.a;
		g_score.b = data.b;	
	}


	/*如果比分是0:0， 创建显示介绍页的窗口*/
	if ((g_score.a == 0) && (g_score.b == 0))
	{
		p_window = count_score_create_intro_window();		
	}
	else	/*创建显示比分的窗口*/
	{
		p_window = count_score_create_start_window();		
	}


	return p_window;
}


int main()
{

#ifdef LINUX	
	/*非APP编写*/	
	screen_init(SCREEN_ROW_NUMS,SCREEN_COL_NUMS);
	os_store_manage_init();	
	window_stack_init();
	os_comm_init();
	set_current_app(0x7d701);
#endif

	P_Window p_window = count_score_create_init_window(); 

	/*放入窗口栈显示*/
	g_count_score_window_id = app_window_stack_push(p_window);



#ifdef LINUX
	/*非APP编写*/	
	SHOW;
	while (1)
	{
		char input;	
	
		/*输入操作*/
		scanf("%c", &input);
		if (input == 'd')
		{
			window_stack_button(ButtonIdDown);	
		}
		else if (input == 'u')
		{
			window_stack_button(ButtonIdUp);	
		}	
		else if (input == 's')
		{
			window_stack_button(ButtonIdSelect);		
		}
		else if (input == 'b')
		{
			window_stack_button(ButtonIdBack);		
		}
		else if (input == 'q')
		{
			break;
		}
	
	}	
	SHOW;

	window_stack_destory();
	screen_destory();
	os_store_manage_destory();

	SHOW;
#endif
	return 0;

}

