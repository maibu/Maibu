/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  watchface_GShock.c
 *
 *    Description:  G-Shock表盘
 *    Corporation:
 * 
 *         Author:  claire 
 *        Created:  2016年03月18日
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
#define LAYER_NUM 12 //需定时更新的图层个数

/*图片坐标：第一项为背景图片，后面依次为步数，卡路里，公里数图层的最后一位*/
static uint8_t ORIGIN_X[LAYER_NUM+BACKGROUND_NUM] = {0, 59, 70, 86, 97, 21, 22,  22, 37, 56, 71,86, 97};
static uint8_t ORIGIN_Y[LAYER_NUM+BACKGROUND_NUM] = {0, 41, 41, 41, 41, 41, 60, 66, 66, 66, 66, 77, 77};
static uint8_t ORIGIN_W[LAYER_NUM+BACKGROUND_NUM] = {128, 9, 9, 9, 9, 33, 11,12, 12, 12, 12,  9, 9};
static uint8_t ORIGIN_H[LAYER_NUM+BACKGROUND_NUM] = {128, 12, 12, 12, 12, 12, 4, 23, 23, 23, 23, 12, 12};


/*图片名数组：分别为大数字0~9图片和小数字0~9图片*/
uint16_t bmp_array[30] = {PIC_0_1, PIC_1_1, PIC_2_1, PIC_3_1, PIC_4_1, PIC_5_1, PIC_6_1, PIC_7_1, PIC_8_1, PIC_9_1,
PIC_0_2, PIC_1_2, PIC_2_2, PIC_3_2, PIC_4_2, PIC_5_2, PIC_6_2, PIC_7_2, PIC_8_2, PIC_9_2, 
PIC_0_3, PIC_1_3, PIC_2_3, PIC_3_3, PIC_4_3, PIC_5_3, PIC_6_3, PIC_AM, PIC_PM,};


//表盘的记录的当天日期，用于比较日期是否变为第二天
static uint8_t g_today_num = -1;

/*小时分钟月日图层ID，通过该图层ID获取图层句柄*/
static int8_t g_layer_time[LAYER_NUM] ={ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_window = -1;

/*表盘应显示数据*/
static uint8_t watch_data[LAYER_NUM] = {0,0,0,0,0,0,0,0,0,0,0,0};

#define LAYER_SEC 6

/*
 *--------------------------------------------------------------------------------------
 *     function:  app_watch_data
 *    parameter:  0为watch_data所有元素值，11仅为watch_data中的秒
 *       return:
 *  description:  将数组参数赋值为当前表盘应显示值
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
static void app_watch_data(uint8_t layernum)
{
	struct date_time datetime;
	app_service_get_datetime(&datetime);

	watch_data[6] = datetime.hour%12/10+10;
	watch_data[7] = datetime.hour%12%10+10;
	watch_data[8] = datetime.min/10+10;
	watch_data[9] = datetime.min%10+10;

	if ((watch_data[6] == 10) && (watch_data[7] == 10))
	{
		watch_data[6] = 11;
		watch_data[7] = 12;
	}
	watch_data[10] = datetime.sec/10;
	watch_data[11] = datetime.sec%10;

	
	if (layernum == LAYER_SEC)
	{
		return;
	};

	watch_data[0] = datetime.mon/10;
	watch_data[1] = datetime.mon%10;
	watch_data[2] = datetime.mday/10;
	watch_data[3] = datetime.mday%10;

	if (watch_data[4] == 10)
		watch_data[4] = 29;

	if (datetime.hour < 12)
	{
		watch_data[5] = 27;
	}else
	{
		watch_data[5] = 28;
	}

	watch_data[4] = datetime.wday+20;
}



/*
 *--------------------------------------------------------------------------------------
 *     function:  app_watch_update
 *    parameter: 
 *       return:
 *  description:  更新图层数据
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
static void app_watch_update(uint8_t layermin, uint8_t layermax)
{
	P_Window p_window = NULL;
	P_Layer p_layer = NULL;
	GBitmap bitmap;
	uint8_t pos;

	/*根据窗口ID获取窗口句柄*/
	p_window = app_window_stack_get_window_by_id(g_window);
	if (p_window == NULL)
	{
		return;
	}

	app_watch_data(layermin);

	for (pos=layermin; pos<layermax; pos++)
	{
		/*获取数据图层句柄*/
		p_layer = app_window_get_layer_by_id(p_window, g_layer_time[pos]);	
		if (p_layer != NULL)
		{
			/*更新数据图层图片*/
			res_get_user_bitmap(bmp_array[watch_data[pos]], &bitmap);
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
		
		struct date_time datetime_now_change;
		app_service_get_datetime(&datetime_now_change);

		if(g_today_num != datetime_now_change.mday%10)
		{
			app_watch_update(0, 6);	
			g_today_num = datetime_now_change.mday%10;
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
    app_watch_update(6, 12);
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
		res_get_user_bitmap(bmp_array[watch_data[pos-BACKGROUND_NUM]], &bitmap);
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

	app_watch_data(0);

	/*创建所有图层并添加到窗口*/
	for (pos=BACKGROUND_NUM; pos<BACKGROUND_NUM+LAYER_NUM; pos++)
	{
		layer = get_layer(pos);
		g_layer_time[pos-BACKGROUND_NUM] = app_window_add_layer(p_window, layer);
	}

	g_today_num = watch_data[3];
		
	/*定义一个窗口定时器，用于秒表显示*/
	app_window_timer_subscribe(p_window, 1000, sec_callback, NULL);

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
