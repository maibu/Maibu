
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "maibu_sdk.h"
#include "maibu_res.h"

static int32_t g_window_id             = -1;


static uint32_t LITTLE_NOTE_PRESIST_DATA_KEY = 0x2002;
static char little_note_text_start[50] = {"星期二乘SpaceX去火星基地打电动"};
static char little_note_text[243] = {0};

enum GColor g_layer1_scroll = GColorWhite;
enum GColor g_layer1_others = GColorBlack;

static uint32_t g_start_seconds		   = 0;//标志记录开始的秒数
static uint32_t g_end_seconds		   = 0;//标志记录开始的秒数



static char wday[7][11]={"星期日","星期一","星期二","星期三","星期四","星期五","星期六"}; 

//时间图层坐标及高宽
#define LITTLE_NOTE_TIME_X 48
#define LITTLE_NOTE_TIME_Y 1
#define LITTLE_NOTE_TIME_H 30
#define LITTLE_NOTE_TIME_W 79

//月日图层坐标及高宽
#define LITTLE_NOTE_MON_DAY_X 4
#define LITTLE_NOTE_MON_DAY_Y 3
#define LITTLE_NOTE_MON_DAY_H 12
#define LITTLE_NOTE_MON_DAY_W 36

//星期图层坐标及高宽
#define LITTLE_NOTE_WEEK_X 4
#define LITTLE_NOTE_WEEK_Y 16
#define LITTLE_NOTE_WEEK_H 12
#define LITTLE_NOTE_WEEK_W 36

//记事框图层坐标及高宽
#define LITTLE_NOTE_LTEXT_X 4
#define LITTLE_NOTE_LTEXT_Y 35
#define LITTLE_NOTE_LTEXT_H 90
#define LITTLE_NOTE_LTEXT_W 120


P_Window init_window(void);

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


void watchapp_comm_callback(enum ESyncWatchApp type, uint8_t *context, uint16_t context_len)
{
	if(type == ESyncWatchAppUpdateParam)
	{	
		memcpy(little_note_text,0,243);
		if(context_len > 0)
		{
			memcpy(little_note_text,context,context_len);
			little_note_text[context_len] = '\0';
		}
		else if(context_len == 0)
		{
			memcpy(little_note_text,little_note_text_start,strlen(little_note_text_start));
		}


		app_persist_write_data_extend(LITTLE_NOTE_PRESIST_DATA_KEY,little_note_text,strlen(little_note_text));

		window_reloading();	
	}
}



void time_change(enum SysEventType type, void *context)
{
	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		window_reloading();
	}
}

/*创建并显示图片图层，需要坐标值，得到icon_key的数组，数组需要的参数值，P_Window*/
int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,enum GAlign how_to_align,enum GColor black_or_white,uint32_t bmp_knowed_key)
{	


	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_knowed_key, &bmp_point);
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
int32_t display_target_layerText(P_Window p_window,GRect  *temp_p_frame,enum GAlign how_to_align,enum GColor color,char * str,uint8_t font_type)
{
	LayerText temp_LayerText;
	memset(&temp_LayerText,0,sizeof(temp_LayerText));
	temp_LayerText.text = str;
	temp_LayerText.frame = *temp_p_frame;
	temp_LayerText.alignment = how_to_align;
	temp_LayerText.font_type = font_type;
	//temp_LayerText.bound_width = 0;
	
	P_Layer p_layer = app_layer_create_text(&temp_LayerText);
	
	if(p_layer != NULL)
	{
		app_layer_set_bg_color(p_layer, color);
		return app_window_add_layer(p_window, p_layer);
	}
	return 0;
}

P_Window init_window(void)
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}
	char buffer[20] = {0};
	
	/*创建背景图层*/
	GRect temporary_frame = {{0, 0}, {32, 128}};
	P_GRect temp_p_frame = &temporary_frame;

	display_target_layer(p_window,temp_p_frame,GAlignCenter,GColorWhite,RES_BITMAP_WATCHFACE_LITTLE_NOTE_BG01);

	struct date_time t;
	app_service_get_datetime(&t);

	/*添加小时分钟图层*/
	temporary_frame.origin.x = LITTLE_NOTE_TIME_X;
	temporary_frame.origin.y = LITTLE_NOTE_TIME_Y;
	temporary_frame.size.h = LITTLE_NOTE_TIME_H;
	temporary_frame.size.w = LITTLE_NOTE_TIME_W;
	
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "%02d:%02d", t.hour, t.min);
	
	display_target_layerText(p_window,temp_p_frame,GAlignRight,g_layer1_others,buffer,U_ASCII_ARIALBD_30);

	
	/*添加月日图层*/
	temporary_frame.origin.x = LITTLE_NOTE_MON_DAY_X;
	temporary_frame.origin.y = LITTLE_NOTE_MON_DAY_Y;
	temporary_frame.size.h = LITTLE_NOTE_MON_DAY_H;
	temporary_frame.size.w = LITTLE_NOTE_MON_DAY_W;

	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "%02d-%02d", t.mon, t.mday);
	
	display_target_layerText(p_window,temp_p_frame,GAlignLeft,g_layer1_others,buffer,U_ASCII_ARIAL_12);
	

	/*添加星期图层*/
	temporary_frame.origin.x = LITTLE_NOTE_WEEK_X;
	temporary_frame.origin.y = LITTLE_NOTE_WEEK_Y;
	temporary_frame.size.h = LITTLE_NOTE_WEEK_H;
	temporary_frame.size.w = LITTLE_NOTE_WEEK_W;

	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "%s", wday[t.wday]);
	
	display_target_layerText(p_window,temp_p_frame,GAlignLeft,g_layer1_others,buffer,U_ASCII_ARIAL_12);
	

	/*添加记事框图层*/
	temporary_frame.origin.x = LITTLE_NOTE_LTEXT_X;
	temporary_frame.origin.y = LITTLE_NOTE_LTEXT_Y;
	temporary_frame.size.h = LITTLE_NOTE_LTEXT_H;
	temporary_frame.size.w = LITTLE_NOTE_LTEXT_W;
	
	int32_t total_bytes1;
	total_bytes1 = app_persist_get_data_size(LITTLE_NOTE_PRESIST_DATA_KEY);
	
	if(total_bytes1 == 0)
	{
		int32_t rettt = -1;
		rettt = app_persist_write_data_extend(LITTLE_NOTE_PRESIST_DATA_KEY,little_note_text_start,strlen(little_note_text_start));
	}
	
	
	/*初始化滚动结构体, 依次为滚动图层显示框架, 内容文件KEY，需要显示的内容大小、字体字号*/
	LayerScroll layer1_scroll = {0};

	layer1_scroll.frame = temporary_frame;
	layer1_scroll.key = LITTLE_NOTE_PRESIST_DATA_KEY;
	layer1_scroll.total_bytes = total_bytes1;
	layer1_scroll.font_type = U_ASCII_ARIAL_14;
	layer1_scroll.gap = 1;

	P_Layer	layer1 = app_layer_create_scroll(&layer1_scroll);

	app_layer_set_bg_color(layer1, g_layer1_scroll);
	app_window_add_layer(p_window, layer1);
	
	return p_window;
}

int main(void)
{
	g_start_seconds = app_get_time_number();
	
	//创建储存空间
	app_persist_create(LITTLE_NOTE_PRESIST_DATA_KEY, sizeof(little_note_text));
	
	/*创建显示窗口*/
	P_Window p_window = init_window(); 
	/*放入窗口栈显示*/
	g_window_id = app_window_stack_push(p_window);
	
	//注册手机设置回调函数
	maibu_comm_register_watchapp_callback(watchapp_comm_callback);

	/*注册一个事件通知回调，当有时间改变时，立即更新时间*/
	maibu_service_sys_event_subscribe(time_change);
	return 0;
}

