/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  app_counter.c
 *
 *    Description:  计数器 
 *    Corporation:
 * 
 *         Author:  gliu (), gliu@damaijiankang.com
 *        Created:  2015年04月01日 09时34分08秒
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
#define COUNTER_BG_ORIGIN_X		0	
#define COUNTER_BG_ORIGIN_Y		0	
#define COUNTER_BG_SIZE_H		128
#define COUNTER_BG_SIZE_W		128

/*文本图层位置*/
#define COUNTER_TEXT_ORIGIN_X		0	
#define COUNTER_TEXT_ORIGIN_Y		41	
#define COUNTER_TEXT_SIZE_H		42
#define COUNTER_TEXT_SIZE_W		128

/*文本1图层位置*/
#define COUNTER_TEXT1_ORIGIN_X		0	
#define COUNTER_TEXT1_ORIGIN_Y		108	
#define COUNTER_TEXT1_SIZE_H		12
#define COUNTER_TEXT1_SIZE_W		128


/*文本框图层ID*/
static int8_t g_counter_text_layer_id = -1;

/*提示文本图层ID*/
static int8_t g_counter_hint_layer_id = -1;

/*当前窗口显示的计数*/
static int16_t g_count_index = 0;


/*计数最大值*/
#define COUNTER_MAX	9999

/*计数最小值*/
#define COUNTER_MIN	(-9999)

/*定义向上按键事件*/
static void counter_select_up(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		/*根据图层ID获取图层句柄*/
		P_Layer p_layer = app_window_get_layer_by_id(p_window, g_counter_text_layer_id);
		P_Layer p_hint_layer = app_window_get_layer_by_id(p_window, g_counter_hint_layer_id);
		if ((p_layer != NULL) && (p_hint_layer != NULL))
		{
			g_count_index++;
			if (g_count_index >=  COUNTER_MAX)
			{
				g_count_index = COUNTER_MAX;
			}
		
			/*设置文本图层新的文本内容并更新显示*/	
			char text[6] = "";
			sprintf(text, "%d", g_count_index);
			app_layer_set_text_text(p_layer, text);	

			
			/*更改提示*/	
			char hint[20] = "";
			if (g_count_index == 0)
			{
				sprintf(hint, "%s", "按上下键加减");
			}
			else
			{
				sprintf(hint, "%s", "按选择键清零");
			}
			app_layer_set_text_text(p_hint_layer, hint);	

			app_window_update(p_window);

			/*添加震动*/
			maibu_service_vibes_pulse(VibesPulseTypeShort, 0);
		}
	}
	
}


/*定义向下按键事件*/
static void counter_select_down(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		/*根据图层ID获取图层句柄*/
		P_Layer p_layer = app_window_get_layer_by_id(p_window, g_counter_text_layer_id);
		P_Layer p_hint_layer = app_window_get_layer_by_id(p_window, g_counter_hint_layer_id);
		if ((p_layer != NULL) && (p_hint_layer != NULL))
		{
			g_count_index--;
			if (g_count_index <=  COUNTER_MIN)
			{
				g_count_index = COUNTER_MIN;
			}
		
			/*设置文本图层新的文本内容并更新显示*/	
			char text[6] = "";
			sprintf(text, "%d", g_count_index);
			app_layer_set_text_text(p_layer, text);		

			/*更改提示*/	
			char hint[20] = "";
			if (g_count_index == 0)
			{
				sprintf(hint, "%s", "按上下键加减");
			}
			else
			{
				sprintf(hint, "%s", "按选择键清零");
			}
			app_layer_set_text_text(p_hint_layer, hint);	

			app_window_update(p_window);

			/*添加震动*/
			maibu_service_vibes_pulse(VibesPulseTypeShort, 0);
		}
	}
}


/*定义选择按键事件*/
static void counter_select_select(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		/*根据图层ID获取图层句柄*/
		P_Layer p_layer = app_window_get_layer_by_id(p_window, g_counter_text_layer_id);
		P_Layer p_hint_layer = app_window_get_layer_by_id(p_window, g_counter_hint_layer_id);
		if ((p_layer != NULL) && (p_hint_layer != NULL))
		{
			g_count_index = 0;
		
			/*设置文本图层新的文本内容并更新显示*/	
			char text[2] = "0";
			app_layer_set_text_text(p_layer, text);		

			/*更改提示*/	
			char hint[20] = "";
			sprintf(hint, "%s", "按上下键加减");
			app_layer_set_text_text(p_hint_layer, hint);	

			app_window_update(p_window);

			/*添加震动*/
			maibu_service_vibes_pulse(VibesPulseTypeShort, 0);
		}
	}
}


/*定义后退按键事件*/
static void counter_select_back(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		app_window_stack_pop(p_window);
	}
}


/*
 *--------------------------------------------------------------------------------------
 *     function:  counter_init_window
 *    parameter: 
 *       return:
 *  description:  初始化窗口
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
static P_Window counter_init_window()
{
	P_Window p_window = NULL;

	/*创建窗口*/
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

/*--------添加状态栏显示时间-------*/
	/*获取当前时间信息*/
	struct date_time dt;
	app_service_get_datetime(&dt);	
	


/*--------添加图片图层------------*/
	/*添加黑色背景图片*/
	/*图片显示位置*/
	GRect frame_bmp = {{COUNTER_BG_ORIGIN_X, COUNTER_BG_ORIGIN_Y}, {COUNTER_BG_SIZE_H, COUNTER_BG_SIZE_W}};
	
	/*获取用户定义图片*/
	GBitmap bitmap;
	res_get_user_bitmap(RES_BITMAP_WATCHAPP_COUNTER_BG, &bitmap);

	/*定义图片显示相关结构*/
	LayerBitmap lb = { bitmap, frame_bmp, GAlignLeft};	

	/*创建图片图层*/
	P_Layer layer_bmp = app_layer_create_bitmap(&lb);

	/*添加图层到窗口*/
	app_window_add_layer(p_window, layer_bmp);
	
	
	
		/*添加状态栏, 显示时间*/
	app_plug_status_bar_create(p_window, NULL, NULL, NULL);
	app_plug_status_bar_add_time(p_window);
	
	

/*---------添加显示文本图层---------------*/

	/*显示文本位置*/
	GRect frame_text = {{COUNTER_TEXT_ORIGIN_X, COUNTER_TEXT_ORIGIN_Y}, {COUNTER_TEXT_SIZE_H, COUNTER_TEXT_SIZE_W}};

	/*定义文本显示结构*/
	char text[6] = "";
	sprintf(text, "%d", g_count_index);
	LayerText lt = {text, frame_text, GAlignTop, U_ASCII_ARIAL_42};

	/*创建文本图层*/
	P_Layer layer_text = app_layer_create_text(&lt);

	/*添加图层到窗口*/
	g_counter_text_layer_id = app_window_add_layer(p_window, layer_text);


/*---------添加显示文本图层---------------*/

	/*显示文本位置*/
	GRect frame1_text = {{COUNTER_TEXT1_ORIGIN_X, COUNTER_TEXT1_ORIGIN_Y}, {COUNTER_TEXT1_SIZE_H, COUNTER_TEXT1_SIZE_W}};

	/*定义文本显示结构*/
	char text1[20] = "";
	if (g_count_index == 0)
	{
		sprintf(text1, "%s", "按上下键加减");
	}
	else
	{
		sprintf(text1, "%s", "按选择键清零");
	}
	LayerText lt1 = {text1, frame1_text, GAlignTop, U_GBK_SIMSUN_12};

	/*创建文本图层*/
	P_Layer layer_text1 = app_layer_create_text(&lt1);
	app_layer_set_bg_color(layer_text1, GColorBlack);

	/*添加图层到窗口*/
	g_counter_hint_layer_id = app_window_add_layer(p_window, layer_text1);

	
	/*添加窗口按键事件*/
	app_window_click_subscribe(p_window, ButtonIdDown, counter_select_down);
	app_window_click_subscribe(p_window, ButtonIdUp, counter_select_up);
	app_window_click_subscribe(p_window, ButtonIdSelect, counter_select_select);
	app_window_click_subscribe(p_window, ButtonIdBack, counter_select_back);
	return (p_window);

}

int main()
{

	/*APP编写*/
	/*创建日期时间设置窗口*/
	P_Window p_window = counter_init_window(); 

	/*放入窗口栈显示*/
	app_window_stack_push(p_window);

	return 0;

}

