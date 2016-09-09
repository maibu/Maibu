/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  app_english_watch.c
 *
 *    Description:  英文表盘 
 *    Corporation:
 * 
 *         Author:  gliu (), gliu@damaijiankang.com
 *        Created:  2015年03月30日 19时48分47秒
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




/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_app_english_window_id = -1;

/*定义各个图层的位置*/

/*背景图层*/
#define ENGLISH_BG_ORIGIN_X		0
#define ENGLISH_BG_ORIGIN_Y		0
#define ENGLISH_BG_SIZE_H		128
#define ENGLISH_BG_SIZE_W		128

/*显示小时文本图层*/
#define ENGLISH_H_ORIGIN_X		8
#define ENGLISH_H_ORIGIN_Y		20
#define ENGLISH_H_SIZE_H		30		
#define ENGLISH_H_SIZE_W		120

/*显示分钟十位文本图层*/
#define ENGLISH_M1_ORIGIN_X		8
#define ENGLISH_M1_ORIGIN_Y		51
#define ENGLISH_M1_SIZE_H		24
#define ENGLISH_M1_SIZE_W		120

/*显示分钟个位文本图层*/
#define ENGLISH_M2_ORIGIN_X		8
#define ENGLISH_M2_ORIGIN_Y		76
#define ENGLISH_M2_SIZE_H		20
#define ENGLISH_M2_SIZE_W		120


const char english_hour_array[][10] = 
{
	"twelve",
	"one",
	"two",
	"three",
	"four",				
	"five",
	"six",
	"seven",
	"eight",
	"nine",
	"ten",
	"eleven",
	"twelve",
	"thirteen",	
	"fourteen",
	"fifteen",
	"sixteen",
	"seventeen",
	"eighteen",
	"nineteen",
	"twenty"
};

const char english_min_array[][8] = 
{
	"twenty",
	"thirty",
	"forty",
	"fifty"
};

P_Window init_english_window(void);

int32_t display_target_layerText(P_Window p_window,GRect  *temp_p_frame,enum GAlign how_to_align,enum GColor color,char * str,uint8_t font_type)
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
		return app_window_add_layer(p_window, p_layer);
	}
	return 0;
}

//重新载入并刷新窗口所有图层
void window_reloading(void)
{
	/*根据窗口ID获取窗口句柄*/
	P_Window p_old_window = app_window_stack_get_window_by_id(g_app_english_window_id); 
	if (NULL != p_old_window)
	{
		P_Window p_window = init_english_window();
		if (NULL != p_window)
		{
			g_app_english_window_id = app_window_stack_replace_window(p_old_window, p_window);
		}	
	}
	
}



void app_english_watch_time_change(enum SysEventType type, void *context)
{
	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		window_reloading();
	}
}


P_Window init_english_window()
{
	P_Window p_window = NULL;
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}


	/*背景图片*/
	GRect frame_bg = {{ENGLISH_BG_ORIGIN_X, ENGLISH_BG_ORIGIN_Y}, {ENGLISH_BG_SIZE_H, ENGLISH_BG_SIZE_W}};
	
	GBitmap bitmap_bg = {0};
	P_Layer temp_P_Layer = NULL;
	
	res_get_user_bitmap(RES_BITMAP_WATCHFACE_ENGLISH_BG, &bitmap_bg);
	LayerBitmap layer_bitmap_struct_l = {bitmap_bg,frame_bg,GAlignLeft};
 	temp_P_Layer = app_layer_create_bitmap(&layer_bitmap_struct_l);
	if(temp_P_Layer != NULL)
	{
		app_layer_set_bg_color(temp_P_Layer, GColorWhite);
		app_window_add_layer(p_window, temp_P_Layer);
	}

	
	struct date_time t;
	app_service_get_datetime(&t);
	int8_t str_hour[10] = {0};

	/*小时*/
	frame_bg.origin.x = ENGLISH_H_ORIGIN_X;
	frame_bg.origin.y = ENGLISH_H_ORIGIN_Y;
	frame_bg.size.h = ENGLISH_H_SIZE_H;	
	frame_bg.size.w = ENGLISH_H_SIZE_W;	
	
	sprintf(str_hour, "%s", english_hour_array[t.hour%12]);
	
	display_target_layerText(p_window,&frame_bg,GAlignLeft,GColorBlack,str_hour,U_ASCII_ARIALBD_30);

	/*分钟第一个数字*/
	
	frame_bg.origin.x = ENGLISH_M1_ORIGIN_X;
	frame_bg.origin.y = ENGLISH_M1_ORIGIN_Y;
	frame_bg.size.h = ENGLISH_M1_SIZE_H;	
	frame_bg.size.w = ENGLISH_M1_SIZE_W;	
	
	/*如果分钟为0，则小时显示x o'clock*/
	if (t.min == 0)
	{
		strcpy(str_hour, "o'clock");
	}
	else if (t.min < 21)
	{
		strcpy(str_hour, english_hour_array[t.min]);
	}
	else
	{
		strcpy(str_hour, english_min_array[t.min/10 - 2]);
	}

	display_target_layerText(p_window,&frame_bg,GAlignLeft,GColorBlack,str_hour,U_ASCII_ARIAL_24);


	/*添加分钟个位图层*/
	if ((t.min > 20)&&((t.min%10) != 0))
	{
		
		frame_bg.origin.x = ENGLISH_M2_ORIGIN_X;
		frame_bg.origin.y = ENGLISH_M2_ORIGIN_Y;
		frame_bg.size.h = ENGLISH_M2_SIZE_H;	
		frame_bg.size.w = ENGLISH_M2_SIZE_W;	
		
		strcpy(str_hour, english_hour_array[t.min%10]);
		
		display_target_layerText(p_window,&frame_bg,GAlignLeft,GColorBlack,str_hour,U_ASCII_ARIAL_20);
		
	}

	/*注册一个事件通知回调，当有时间改变是，立即更新时间*/
	maibu_service_sys_event_subscribe(app_english_watch_time_change);

	return p_window;

}

int main()
{

	/*创建显示英文窗口*/
	P_Window p_window = init_english_window(); 
	if (p_window != NULL)
	{
		/*放入窗口栈显示k*/
		g_app_english_window_id = app_window_stack_push(p_window);
	}

	return 0;

}



