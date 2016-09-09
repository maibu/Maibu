/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  watchface_steel_seal.c
 *
 *    Description:  旋转钢印
 *    Corporation:
 * 
 *         Author:  gliu (), gliu@damaijiankang.com
 *        Created:  2015年03月30日 18时04分02秒
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
#include "os_comm.h"
#include "os_res_bitmap.h"
#include "os_sys_app_id.h"
#include "os_memory_manage.h"
#include "res_user_bitmap.h"
#include "os_sys_event.h"
#include "window_stack.h"
#include "window.h"
#include "screen_show.h"
#include "matrix.h"
#include "plug_status_bar.h"
#include "res_bitmap_base.h"

#else

#include "maibu_sdk.h"
#include "maibu_res.h"

#endif





/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_app_steel_window_id = -1;

/*图层ID，通过该图层ID获取图层句柄*/
static int8_t g_app_steel_block_layer_id = -1;


/*定义各个图层的位置*/

/*背景图层*/
#define STEEL_BG_ORIGIN_X		0
#define STEEL_BG_ORIGIN_Y		0
#define STEEL_BG_SIZE_H			128
#define STEEL_BG_SIZE_W			128

/*显示小时第一个文字文本图层*/
#define STEEL_H1_ORIGIN_X		1
#define STEEL_H1_ORIGIN_Y		0
#define STEEL_H1_SIZE_H			128		
#define STEEL_H1_SIZE_W			29

/*显示小时第二个文字文本图层*/
#define STEEL_H2_ORIGIN_X		30
#define STEEL_H2_ORIGIN_Y		0	
#define STEEL_H2_SIZE_H			128			
#define STEEL_H2_SIZE_W			29

/*显示分钟第一个文字文本图层*/
#define STEEL_M1_ORIGIN_X		70
#define STEEL_M1_ORIGIN_Y		0	
#define STEEL_M1_SIZE_H			128		
#define STEEL_M1_SIZE_W			29

/*显示分钟第二个文字文本图层*/
#define STEEL_M2_ORIGIN_X		99
#define STEEL_M2_ORIGIN_Y		0	
#define STEEL_M2_SIZE_H			128		
#define STEEL_M2_SIZE_W			29




static void app_steel_seal_watch_time_update(uint8_t flag)
{
	/*根据窗口ID获取窗口句柄*/
	P_Window p_window = app_window_stack_get_window_by_id(g_app_steel_window_id);	
	if (NULL == p_window)
	{
		return;	
	}

	P_Layer p_layer = app_window_get_layer_by_id(p_window, g_app_steel_block_layer_id);
	if (NULL != p_layer)
	{

		/*如果秒数为0，更新block中分钟元素*/
		int32_t bitmap_array[] = {RES_BITMAP_WATCHFACE_STEEL_0, RES_BITMAP_WATCHFACE_STEEL_1, RES_BITMAP_WATCHFACE_STEEL_2, RES_BITMAP_WATCHFACE_STEEL_3,
					RES_BITMAP_WATCHFACE_STEEL_4, RES_BITMAP_WATCHFACE_STEEL_5, RES_BITMAP_WATCHFACE_STEEL_6, RES_BITMAP_WATCHFACE_STEEL_7,
					RES_BITMAP_WATCHFACE_STEEL_8, RES_BITMAP_WATCHFACE_STEEL_9};

		struct date_time datetime;
		app_service_get_datetime(&datetime);

		GBitmap bitmap;
		res_get_user_bitmap(bitmap_array[datetime.min/10], &bitmap);
		app_layer_set_block_element_bitmap(p_layer, 2, &bitmap);
		res_get_user_bitmap(bitmap_array[datetime.min%10], &bitmap);
		app_layer_set_block_element_bitmap(p_layer, 3, &bitmap);

		if ((flag == 1) || (datetime.min == 0))	
		{
			res_get_user_bitmap(bitmap_array[datetime.hour/10], &bitmap);
			app_layer_set_block_element_bitmap(p_layer, 0, &bitmap);
			res_get_user_bitmap(bitmap_array[datetime.hour%10], &bitmap);
			app_layer_set_block_element_bitmap(p_layer, 1, &bitmap);
		}


		app_window_update(p_window);
	}
	

}



static void app_steel_seal_watch_time_change(enum SysEventType type, void *context)
{
	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		app_steel_seal_watch_time_update(1);
	}
}



#if 0
/*
 *--------------------------------------------------------------------------------------
 *     function:  app_steel_timer_callback
 *    parameter: 
 *       return:
 *  description:  定时更新时间
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
static void app_steel_timer_callback(date_time_t tick_time, uint32_t millis, void *context)
{
	app_steel_seal_watch_time_update(0);
}
#endif


static P_Window init_steel_window()
{
	int32_t bitmap_array[] = {RES_BITMAP_WATCHFACE_STEEL_0, RES_BITMAP_WATCHFACE_STEEL_1, RES_BITMAP_WATCHFACE_STEEL_2, RES_BITMAP_WATCHFACE_STEEL_3,
				RES_BITMAP_WATCHFACE_STEEL_4, RES_BITMAP_WATCHFACE_STEEL_5, RES_BITMAP_WATCHFACE_STEEL_6, RES_BITMAP_WATCHFACE_STEEL_7,
				RES_BITMAP_WATCHFACE_STEEL_8, RES_BITMAP_WATCHFACE_STEEL_9};
	P_Window p_window = NULL;
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}


	/*添加背景图片图层*/
	GRect frame_bg = {{STEEL_BG_ORIGIN_X, STEEL_BG_ORIGIN_Y}, {STEEL_BG_SIZE_H, STEEL_BG_SIZE_W}};
	GBitmap bitmap_bg;
	res_get_user_bitmap(RES_BITMAP_WATCHFACE_STEEL_BG, &bitmap_bg);
	LayerBitmap lb_bg = {bitmap_bg, frame_bg, GAlignLeft};	
	P_Layer layer_bitmap_bg = app_layer_create_bitmap(&lb_bg);
	if(layer_bitmap_bg != NULL)
	{
		app_window_add_layer(p_window, layer_bitmap_bg);
	}


	/*添加块*/
	GRect block_frame_bg = {{STEEL_BG_ORIGIN_X, STEEL_BG_ORIGIN_Y}, {STEEL_BG_SIZE_H, STEEL_BG_SIZE_W}};
	LayerBlock struct_block= {block_frame_bg};
	BElement element;		

	struct date_time t;
	app_service_get_datetime(&t);
	GBitmap bitmap;

	/*小时第一个数字*/
	memset(&element, 0, sizeof(BElement));
	element.type = BELEMENT_TYPE_BITMAP;
	element.color = GColorBlack;
	element.frame.origin.x = STEEL_H1_ORIGIN_X;
	element.frame.origin.y = STEEL_H1_ORIGIN_Y;
	element.frame.size.h = STEEL_H1_SIZE_H;	
	element.frame.size.w = STEEL_H1_SIZE_W;	
	res_get_user_bitmap(bitmap_array[t.hour/10], &bitmap);
	element.context.bmp = bitmap;
	app_layer_add_block_element(&struct_block, &element);

	/*小时第二个数字*/
	memset(&element, 0, sizeof(BElement));
	element.type = BELEMENT_TYPE_BITMAP;
	element.color = GColorBlack;
	element.frame.origin.x = STEEL_H2_ORIGIN_X;
	element.frame.origin.y = STEEL_H2_ORIGIN_Y;
	element.frame.size.h = STEEL_H2_SIZE_H;	
	element.frame.size.w = STEEL_H2_SIZE_W;	
	res_get_user_bitmap(bitmap_array[t.hour%10], &bitmap);
	element.context.bmp = bitmap;
	app_layer_add_block_element(&struct_block, &element);


	/*分钟第一个数字*/
	memset(&element, 0, sizeof(BElement));
	element.type = BELEMENT_TYPE_BITMAP;
	element.color = GColorBlack;
	element.frame.origin.x = STEEL_M1_ORIGIN_X;
	element.frame.origin.y = STEEL_M1_ORIGIN_Y;
	element.frame.size.h = STEEL_M1_SIZE_H;	
	element.frame.size.w = STEEL_M1_SIZE_W;	
	res_get_user_bitmap(bitmap_array[t.min/10], &bitmap);
	element.context.bmp = bitmap;
	app_layer_add_block_element(&struct_block, &element);

	/*分钟第二个数字*/
	memset(&element, 0, sizeof(BElement));
	element.type = BELEMENT_TYPE_BITMAP;
	element.color = GColorBlack;
	element.frame.origin.x = STEEL_M2_ORIGIN_X;
	element.frame.origin.y = STEEL_M2_ORIGIN_Y;
	element.frame.size.h = STEEL_M2_SIZE_H;	
	element.frame.size.w = STEEL_M2_SIZE_W;	
	res_get_user_bitmap(bitmap_array[t.min%10], &bitmap);
	element.context.bmp = bitmap;
	app_layer_add_block_element(&struct_block, &element);

	/*创建块图层*/
	P_Layer layer = app_layer_create_block(&struct_block);
	if(layer != NULL)
	{
		g_app_steel_block_layer_id = app_window_add_layer(p_window, layer);
	}


	/*添加窗口定时器，定时更新*/
//	app_window_timer_subscribe(p_window, 60000, app_steel_timer_callback, NULL);

	/*注册一个事件通知回调，当有时间改变是，立即更新时间*/
	maibu_service_sys_event_subscribe(app_steel_seal_watch_time_change);

	return p_window;

}




int main()
{

#ifdef LINUX	
	/*非APP编写*/	
	screen_init(SCREEN_ROW_NUMS,SCREEN_COL_NUMS);
	os_store_manage_init();	
	window_stack_init();
	SHOW;
#endif


	/*创建显示汉字表盘窗口*/
	P_Window p_window = init_steel_window(); 
	if (NULL != p_window)
	{
		/*放入窗口栈显示k*/
		g_app_steel_window_id = app_window_stack_push(p_window);
	}


#ifdef LINUX
	SHOW;
	/*非APP编写*/	
	while (1)
	{
		char input;	
	
		/*输入操作*/
		scanf("%c", &input);
		if (input == 'q')
		{
			break;	
		}
		else if (input == 'c')
		{
	
		}
		else if (input == 'g')
		{
			break;
		}
	
	}	

	app_window_stack_pop(p_window);
	window_stack_destory();
	screen_destory();
	os_store_manage_destory();

	SHOW;

#endif

	return 0;

}



