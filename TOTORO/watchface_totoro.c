/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  watchface_totoro.c
 *
 *    Description:  龙猫表盘
 *    Corporation:
 * 
 *         Author:  claire 
 *        Created:  2016年02月02日
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

#define BACKGROUND_NUM 1 //背景图片个数
#define TIME_NUM 4 //时间位数

/*图片坐标：第一项为背景图片，后面依次为时间的1~4位*/
static uint8_t ORIGIN_X[5] = {0, 24, 35, 52, 63};
static uint8_t ORIGIN_Y[5] = {0, 97, 97, 97, 97};
static uint8_t ORIGIN_H[5] = {128, 30, 30, 30, 30};
static uint8_t ORIGIN_W[5] = {128, 11, 11, 11, 11};


/*图片名数组：第一项为背景图片，后面依次为数字0~9图片*/
uint16_t bmp_array[11] = {RES_BITMAP_WATCHFACE_BG, RES_BITMAP_WATCHFACE_TIME_0, RES_BITMAP_WATCHFACE_TIME_1, RES_BITMAP_WATCHFACE_TIME_2, RES_BITMAP_WATCHFACE_TIME_3, RES_BITMAP_WATCHFACE_TIME_4, RES_BITMAP_WATCHFACE_TIME_5, RES_BITMAP_WATCHFACE_TIME_6, RES_BITMAP_WATCHFACE_TIME_7, RES_BITMAP_WATCHFACE_TIME_8, RES_BITMAP_WATCHFACE_TIME_9};


/*小时分钟月日图层ID，通过该图层ID获取图层句柄*/
static int8_t g_layer_time[TIME_NUM] ={ -1, -1, -1, -1};

/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_window = -1;


/*
 *--------------------------------------------------------------------------------------
 *     function:  app_watch_update
 *    parameter: 
 *       return:
 *  description:  更新时间图层
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
static void app_watch_update()
{
	P_Window p_window = NULL;
	P_Layer p_layer = NULL;
	GBitmap bitmap;
	struct date_time datetime;
	app_service_get_datetime(&datetime);
	uint8_t new_time[TIME_NUM] = {datetime.hour/10, datetime.hour%10, datetime.min/10, datetime.min%10};
	uint8_t pos;
	
	
	/*根据窗口ID获取窗口句柄*/
	p_window = app_window_stack_get_window_by_id(g_window);
	if (p_window == NULL)
	{
		return;
	}

	for (pos=0; pos<TIME_NUM; pos++)
	{
			/*获取时间图层句柄*/
			p_layer = app_window_get_layer_by_id(p_window, g_layer_time[pos]);	
			if (p_layer != NULL)
			{
				/*更新时间图层图片*/
				res_get_user_bitmap(bmp_array[new_time[pos]+BACKGROUND_NUM], &bitmap);
				app_layer_set_bitmap_bitmap(p_layer, &bitmap);
			}
	}

	/*窗口显示*/	
	app_window_update(p_window);
}


/*
 *--------------------------------------------------------------------------------------
 *     function:  app_watch_time_change 
 *    parameter: 
 *       return:
 *  description:  系统时间有变化时，更新时间图层
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
static void app_watch_time_change(enum SysEventType type, void *context)
{

	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		app_watch_update();	
	}
}



/*
 *--------------------------------------------------------------------------------------
 *     function:  get_layer
 *    parameter: 
 *       return:
 *  description:  生成表盘窗口的各图层
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
static P_Layer get_layer(uint8_t pos, uint8_t time[])
{
	GRect frame = {{ORIGIN_X[pos], ORIGIN_Y[pos]}, {ORIGIN_H[pos], ORIGIN_W[pos]}};
	GBitmap bitmap;
		
	/*获取用户图片*/
	if (pos < BACKGROUND_NUM)
	{
		res_get_user_bitmap(bmp_array[pos], &bitmap);
	}else
	{
		res_get_user_bitmap(bmp_array[time[pos-BACKGROUND_NUM]+BACKGROUND_NUM], &bitmap);
	}
	LayerBitmap layerbmp = { bitmap, frame, GAlignLeft};	

	/*生成图层*/
	P_Layer	 layer_back = NULL;
	layer_back = app_layer_create_bitmap(&layerbmp);

	return (layer_back);
}


/*
 *--------------------------------------------------------------------------------------
 *     function:  init_watch
 *    parameter: 
 *       return:
 *  description:  生成表盘窗口
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
static P_Window init_watch(void)
{
	P_Window p_window = NULL;
	P_Layer layer_background = NULL, layer_time = NULL;
	uint8_t pos;
	struct date_time datetime;
	app_service_get_datetime(&datetime);
	uint8_t current_time[TIME_NUM] = {datetime.hour/10, datetime.hour%10, datetime.min/10, datetime.min%10};

	/*创建一个窗口*/
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	/*创建背景图层*/
	layer_background = get_layer(0, current_time);

	/*添加背景图层到窗口*/
	app_window_add_layer(p_window, layer_background);

	/*创建时间图层并添加到窗口*/
	for (pos=BACKGROUND_NUM; pos<BACKGROUND_NUM+TIME_NUM; pos++)
	{
		layer_time = get_layer(pos, current_time);
		g_layer_time[pos-BACKGROUND_NUM] = app_window_add_layer(p_window, layer_time);
	}
	

	/*注册一个事件通知回调，当有时间改变时，立即更新时间*/
	maibu_service_sys_event_subscribe(app_watch_time_change);

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
	/*模拟器模拟显示时打开，打包时屏蔽*/
	//simulator_init();

	/*创建显示表盘窗口*/
	P_Window p_window = init_watch();
	if (p_window != NULL)
	{
		/*放入窗口栈显示*/
		g_window = app_window_stack_push(p_window);
	}
	
	/*模拟器模拟显示时打开，打包时屏蔽*/
	//simulator_wait();

	return 0;
}