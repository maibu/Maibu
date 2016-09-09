/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  watchface_100km.c
 *
 *    Description:  深圳百公里
 *    Corporation:
 * 
 *         Author:  claire 
 *        Created:  2016年03月01日
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
#define LAYER_NUM 16 //需定时更新的图层个数

/*图片坐标：第一项为背景图片，后面依次为步数，卡路里，公里数图层的最后一位*/
static uint8_t ORIGIN_X[LAYER_NUM+BACKGROUND_NUM] = {0, 115, 104, 93, 82, 71, 60, 49, 62, 55, 48, 41, 34, 107, 97, 90, 83};
static uint8_t ORIGIN_Y[LAYER_NUM+BACKGROUND_NUM] = {0, 73, 73, 73, 73, 73, 73, 73, 108, 108, 108, 108, 108, 108, 108, 108, 108};
static uint8_t ORIGIN_W[LAYER_NUM+BACKGROUND_NUM] = {128, 9, 9, 9, 9, 9, 9, 9, 6, 6, 6, 6, 6, 6, 6, 6, 6};
static uint8_t ORIGIN_H[LAYER_NUM+BACKGROUND_NUM] = {128, 23, 23, 23, 23, 23, 23, 23, 16, 16, 16, 16, 16, 16, 16, 16, 16};


/*图片名数组：分别为大数字0~9图片和小数字0~9图片*/
uint16_t bmp_array[22] = {PIC_0_1, PIC_1_1, PIC_2_1, PIC_3_1, PIC_4_1, PIC_5_1, PIC_6_1, PIC_7_1, PIC_8_1, PIC_9_1, PIC_10_1, PIC_0_2, PIC_1_2, PIC_2_2, PIC_3_2, PIC_4_2,  PIC_5_2, PIC_6_2, PIC_7_2, PIC_8_2, PIC_9_2, PIC_10_2};


/*小时分钟月日图层ID，通过该图层ID获取图层句柄*/
static int8_t g_layer_time[LAYER_NUM] ={ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_window = -1;

/*由系统时间转换为当前表盘应显示时间*/
static uint8_t watch_current_data[LAYER_NUM] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};



/*
 *--------------------------------------------------------------------------------------
 *     function:  app_watch_data
 *    parameter: 
 *       return:
 *  description:  将数组参数赋值为当前表盘应显示值
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
static void app_watch_data(uint8_t watch_data[])
{
	SportData data;
	maibu_get_sport_data(&data, 0);
	uint8_t pos;
	
	for  (pos=0; pos<7; pos++)
	{
		if (data.step == 0 && pos != 0)
		{
			watch_data[pos] = 10;
		}else
		{
			watch_data[pos] = data.step % 10;
			data.step = data.step / 10;
		}
	}

	for  (pos=7; pos<12; pos++)
	{
		if (data.calorie == 0 && pos != 7)
		{
			watch_data[pos] = 10;
		}else
		{
			watch_data[pos] = data.calorie % 10;
			data.calorie = data.calorie / 10;
		}
	}

	data.distance = data.distance / 10000;
	for  (pos=12; pos<16; pos++)
	{
		if (data.distance == 0 && pos > 13)
		{
			watch_data[pos] = 10;
		}else
		{
			watch_data[pos] = data.distance % 10;
			data.distance = data.distance / 10;
		}
	}

}



/*
 *--------------------------------------------------------------------------------------
 *     function:  app_sport_timer_callback
 *    parameter: 
 *       return:
 *  description:  定时更新图层
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
static void app_sport_timer_callback()
{
	P_Window p_window = NULL;
	P_Layer p_layer = NULL;
	GBitmap bitmap;
	uint8_t pos;
	uint8_t watch_new_data[LAYER_NUM] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	/*根据窗口ID获取窗口句柄*/
	p_window = app_window_stack_get_window_by_id(g_window);
	if (p_window == NULL)
	{
		return;
	}

	app_watch_data(watch_new_data);

	for (pos=0; pos<LAYER_NUM; pos++)
	{
		if (watch_new_data[pos] != watch_current_data[pos])
		{
			/*获取时间图层句柄*/
			p_layer = app_window_get_layer_by_id(p_window, g_layer_time[pos]);	
			if (p_layer != NULL)
			{
				/*更新时间图层图片*/
				if (pos < 7)
					res_get_user_bitmap(bmp_array[watch_new_data[pos]], &bitmap);
				else 
					res_get_user_bitmap(bmp_array[watch_new_data[pos]+11], &bitmap);
				app_layer_set_bitmap_bitmap(p_layer, &bitmap);
			}
			watch_current_data[pos] = watch_new_data[pos];
		}
	}

	/*窗口显示*/	
	app_window_update(p_window);
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
		if (pos < 7)
			res_get_user_bitmap(bmp_array[watch_current_data[pos-BACKGROUND_NUM]], &bitmap);
		else 
			res_get_user_bitmap(bmp_array[watch_current_data[pos-BACKGROUND_NUM]+11], &bitmap);
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

	app_watch_data(watch_current_data);

	/*创建步数，卡路里，公里图层并添加到窗口*/
	for (pos=BACKGROUND_NUM; pos<BACKGROUND_NUM+LAYER_NUM; pos++)
	{
		layer = get_layer(pos);
		g_layer_time[pos-BACKGROUND_NUM] = app_window_add_layer(p_window, layer);
	}
	

	/*添加窗口定时器，定时更新*/
	app_window_timer_subscribe(p_window, 300, app_sport_timer_callback, NULL);

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
