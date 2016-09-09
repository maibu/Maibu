/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  watchface_SlashDigital.c
 *
 *    Description:  斜线数字表盘
 *    Corporation:
 * 
 *         Author:  claire 
 *        Created:  2016年03月15日
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
static uint8_t ORIGIN_X[5] = {0, 18, 18, 79, 79};
static uint8_t ORIGIN_Y[5] = {0, 8, 67, 8, 67};
static uint8_t ORIGIN_H[5] = {128, 58, 58, 58, 58};
static uint8_t ORIGIN_W[5] = {128, 40, 40, 40, 40};


/*图片名数组：第一项为背景图片，后面依次为数字0~9图片*/
uint16_t bmp_array[10] = {PIC_0_1, PIC_1_1, PIC_2_1, PIC_3_1, PIC_4_1, PIC_5_1, PIC_6_1, PIC_7_1, PIC_8_1, PIC_9_1};


/*小时分钟月日图层ID，通过该图层ID获取图层句柄*/
static int8_t g_layer_time[TIME_NUM] ={ -1, -1, -1, -1};

/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_window = -1;

/*当前表盘应显示时间*/
static int8_t watch_time[TIME_NUM] = {0,0,0,0};


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
	uint8_t pos;
	struct date_time datetime;

	app_service_get_datetime(&datetime);

	int8_t current_time[TIME_NUM] = {datetime.hour/10, datetime.hour%10, datetime.min/10, datetime.min%10};

	/*根据窗口ID获取窗口句柄*/
	p_window = app_window_stack_get_window_by_id(g_window);
	if (p_window == NULL)
	{
		return;
	}

	for (pos=0; pos<TIME_NUM; pos++)
	{
		if (current_time[pos] != watch_time[pos])
		{
			watch_time[pos] = current_time[pos];
			/*获取时间图层句柄*/
			p_layer = app_window_get_layer_by_id(p_window, g_layer_time[pos]);	
			if (p_layer != NULL)
			{
				/*更新时间图层图片*/
				res_get_user_bitmap(bmp_array[watch_time[pos]], &bitmap);
				app_layer_set_bitmap_bitmap(p_layer, &bitmap);
			}
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
static P_Layer get_layer(uint8_t pos)
{
	GRect frame = {{ORIGIN_X[pos], ORIGIN_Y[pos]}, {ORIGIN_H[pos], ORIGIN_W[pos]}};
	GBitmap bitmap;

	/*获取用户图片*/
	if (pos < BACKGROUND_NUM)
	{
		res_get_user_bitmap(PIC_BG, &bitmap);
	}else
	{
		res_get_user_bitmap(bmp_array[watch_time[pos-BACKGROUND_NUM]], &bitmap);
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
	P_Layer layer = NULL;
	uint8_t pos;
	struct date_time datetime;

	app_service_get_datetime(&datetime);
	watch_time[0] = datetime.hour/10;
	watch_time[1] = datetime.hour%10;
	watch_time[2] = datetime.min/10;
	watch_time[3] = datetime.min%10;

	/*创建一个窗口*/
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	/*创建背景图层*/
	layer = get_layer(0);

	/*添加背景图层到窗口*/
	app_window_add_layer(p_window, layer);

	/*创建时间图层并添加到窗口*/
	for (pos=BACKGROUND_NUM; pos<BACKGROUND_NUM+TIME_NUM; pos++)
	{
		layer = get_layer(pos);
		g_layer_time[pos-BACKGROUND_NUM] = app_window_add_layer(p_window, layer);
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
