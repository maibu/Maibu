#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "maibu_sdk.h"
#include "maibu_res.h"

#define WATCHFACE_BG_ORIGIN_X 	0
#define WATCHFACE_BG_ORIGIN_Y 	0
#define WATCHFACE_BG_WIZE_H		128
#define WATCHFACE_BG_SIZE_W 	128


#define WATCHFACE_TIME_HOUR1_ORIGIN_X 	9
#define WATCHFACE_TIME_HOUR1_ORIGIN_Y 	84
#define WATCHFACE_TIME_HOUR1_WIZE_H		12
#define WATCHFACE_TIME_HOUR1_SIZE_W 	8


#define WATCHFACE_TIME_HOUR2_ORIGIN_X 	17
#define WATCHFACE_TIME_HOUR2_ORIGIN_Y 	84
#define WATCHFACE_TIME_HOUR2_WIZE_H		12
#define WATCHFACE_TIME_HOUR2_SIZE_W 	8

#define WATCHFACE_TIME_MIN1_ORIGIN_X 	28
#define WATCHFACE_TIME_MIN1_ORIGIN_Y 	84
#define WATCHFACE_TIME_MIN1_WIZE_H		12
#define WATCHFACE_TIME_MIN1_SIZE_W 		8

#define WATCHFACE_TIME_MIN2_ORIGIN_X 	36
#define WATCHFACE_TIME_MIN2_ORIGIN_Y 	84
#define WATCHFACE_TIME_MIN2_WIZE_H		12
#define WATCHFACE_TIME_MIN2_SIZE_W 		8


#define WATCHFACE_WEEK_ORIGIN_X 	96
#define WATCHFACE_WEEK_ORIGIN_Y 	85
#define WATCHFACE_WEEK_WIZE_H		9
#define WATCHFACE_WEEK_SIZE_W 		23

#define WATCHFACE_MON1_ORIGIN_X 	52
#define WATCHFACE_MON1_ORIGIN_Y 	84
#define WATCHFACE_MON1_WIZE_H		12
#define WATCHFACE_MON1_SIZE_W 		8

#define WATCHFACE_MON2_ORIGIN_X 	60
#define WATCHFACE_MON2_ORIGIN_Y 	84
#define WATCHFACE_MON2_WIZE_H		12
#define WATCHFACE_MON2_SIZE_W 		8

#define WATCHFACE_DAY1_ORIGIN_X 	73
#define WATCHFACE_DAY1_ORIGIN_Y 	84
#define WATCHFACE_DAY1_WIZE_H		12
#define WATCHFACE_DAY1_SIZE_W 		8

#define WATCHFACE_DAY2_ORIGIN_X 	81
#define WATCHFACE_DAY2_ORIGIN_Y 	84
#define WATCHFACE_DAY2_WIZE_H		12
#define WATCHFACE_DAY2_SIZE_W 		8

#define WATCHFACE_MOVE_ORIGIN_X  	4
#define WATCHFACE_MOVE_ORIGIN_Y 	41
#define WATCHFACE_MOVE_WIZE_H		38
#define WATCHFACE_MOVE_SIZE_W 		116


static GRect bmp_origin_size_bg = {
	{WATCHFACE_BG_ORIGIN_X,WATCHFACE_BG_ORIGIN_Y},
	{WATCHFACE_BG_WIZE_H,WATCHFACE_BG_SIZE_W}
};


static GRect bmp_origin_size[] = {

	{
		{WATCHFACE_TIME_HOUR1_ORIGIN_X,WATCHFACE_TIME_HOUR1_ORIGIN_Y},
		{WATCHFACE_TIME_HOUR1_WIZE_H,WATCHFACE_TIME_HOUR1_SIZE_W}
	},
	{
		{WATCHFACE_TIME_HOUR2_ORIGIN_X,WATCHFACE_TIME_HOUR2_ORIGIN_Y},
		{WATCHFACE_TIME_HOUR2_WIZE_H,WATCHFACE_TIME_HOUR2_SIZE_W}
	},
	{
		{WATCHFACE_TIME_MIN1_ORIGIN_X,WATCHFACE_TIME_MIN1_ORIGIN_Y},
		{WATCHFACE_TIME_MIN1_WIZE_H,WATCHFACE_TIME_MIN1_SIZE_W}
	},	

	{
		{WATCHFACE_TIME_MIN2_ORIGIN_X,WATCHFACE_TIME_MIN2_ORIGIN_Y},
		{WATCHFACE_TIME_MIN2_WIZE_H,WATCHFACE_TIME_MIN2_SIZE_W}
	},

	{
		{WATCHFACE_MOVE_ORIGIN_X,WATCHFACE_MOVE_ORIGIN_Y},
		{WATCHFACE_MOVE_WIZE_H,WATCHFACE_MOVE_SIZE_W}
	},

	{
		{WATCHFACE_MOVE_ORIGIN_X,WATCHFACE_MOVE_ORIGIN_Y},
		{WATCHFACE_MOVE_WIZE_H,WATCHFACE_MOVE_SIZE_W}
	},

	{
		{WATCHFACE_MON1_ORIGIN_X,WATCHFACE_MON1_ORIGIN_Y},
		{WATCHFACE_MON1_WIZE_H,WATCHFACE_MON1_SIZE_W}
	},

	{
		{WATCHFACE_MON2_ORIGIN_X,WATCHFACE_MON2_ORIGIN_Y},
		{WATCHFACE_MON2_WIZE_H,WATCHFACE_MON2_SIZE_W}
	},

	{
		{WATCHFACE_DAY1_ORIGIN_X,WATCHFACE_DAY1_ORIGIN_Y},
		{WATCHFACE_DAY1_WIZE_H,WATCHFACE_DAY1_SIZE_W}
	},

	{
		{WATCHFACE_DAY2_ORIGIN_X,WATCHFACE_DAY2_ORIGIN_Y},
		{WATCHFACE_DAY2_WIZE_H,WATCHFACE_DAY2_SIZE_W}
	},

	{
		{WATCHFACE_WEEK_ORIGIN_X,WATCHFACE_WEEK_ORIGIN_Y},
		{WATCHFACE_WEEK_WIZE_H,WATCHFACE_WEEK_SIZE_W}
	},
	
	

};


//数字图片数组
static uint32_t get_number_icon_key[] =
{
	RES_BITMAP_WATCHFACE_NUMBER_0,  
	RES_BITMAP_WATCHFACE_NUMBER_1, 
	RES_BITMAP_WATCHFACE_NUMBER_2,  
	RES_BITMAP_WATCHFACE_NUMBER_3,  
	RES_BITMAP_WATCHFACE_NUMBER_4,  
	RES_BITMAP_WATCHFACE_NUMBER_5,  
	RES_BITMAP_WATCHFACE_NUMBER_6,  
	RES_BITMAP_WATCHFACE_NUMBER_7, 
	RES_BITMAP_WATCHFACE_NUMBER_8,  
	RES_BITMAP_WATCHFACE_NUMBER_9

};

//星期图片数组
static uint32_t get_WEEK_icon_key[] =
{
	RES_BITMAP_WATCHFACE_WEEK_SUN,
	RES_BITMAP_WATCHFACE_WEEK_MON,
	RES_BITMAP_WATCHFACE_WEEK_TUE,
	RES_BITMAP_WATCHFACE_WEEK_WED,
	RES_BITMAP_WATCHFACE_WEEK_THU,
	RES_BITMAP_WATCHFACE_WEEK_FRI,
	RES_BITMAP_WATCHFACE_WEEK_SAT,
	RES_BITMAP_WATCHFACE_BG
};

//动作图片数组
static uint32_t get_MOVE_icon_key[] =
{
	RES_BITMAP_WATCHFACE_MOVE_1,
	RES_BITMAP_WATCHFACE_MOVE_2,
	RES_BITMAP_WATCHFACE_MOVE_3
};


/*动作图层ID*/
static int8_t g_layer_move_id = -1;

/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_window_id = -1;

/*表盘应显示数据*/
static uint8_t watch_data[11] = {0};

static P_Window init_window(void);

/*
 *--------------------------------------------------------------------------------------
 *     function:  app_watch_data
 *    parameter:  0为watch_data所有元素值，1仅为watch_data中的秒
 *       return:
 *  description:  将数组参数赋值为当前表盘应显示值
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
static void app_watch_data(uint8_t sec_refresh)
{
	struct date_time datetime;
	app_service_get_datetime(&datetime);
	
	watch_data[4] = (watch_data[4]+1)%3;
	//watch_data[5] = datetime.sec%10;
	if(sec_refresh == 1)
	{
		return;
	}
	watch_data[0] = datetime.hour/10;
	watch_data[1] = datetime.hour%10;
	watch_data[2] = datetime.min/10;
	watch_data[3] = datetime.min%10;

	watch_data[6] = datetime.mon/10;
	watch_data[7] = datetime.mon%10;
	watch_data[8] = datetime.mday/10;
	watch_data[9] = datetime.mday%10;

	watch_data[10] = datetime.wday;

}

//重新载入并刷新窗口所有图层
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
		app_watch_data(0);
		window_reloading();
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
  	P_Window p_window = NULL;
	P_Layer p_layer = NULL;
	GBitmap bitmap = {0};
	uint8_t i;

	/*根据窗口ID获取窗口句柄*/
	p_window = app_window_stack_get_window_by_id(g_window_id);
	if (p_window == NULL)
	{
		return;
	}

	app_watch_data(1);

	//for (i = 0; i <= 1; i++)
	{
		/*获取数据图层句柄*/
		p_layer = app_window_get_layer_by_id(p_window, g_layer_move_id);	
		if (p_layer != NULL)
		{
			/*更新数据图层图片*/
			res_get_user_bitmap(get_MOVE_icon_key[watch_data[4]], &bitmap);
			app_layer_set_bitmap_bitmap(p_layer, &bitmap);
		}
	}

	/*窗口显示*/	
	app_window_update(p_window);
}
 


/*
 *--------------------------------------------------------------------------------------
 *     function:  
 *    parameter: 
 *       return:
 *  description:  生成表盘窗口的各图层
 * 	      other:
 *--------------------------------------------------------------------------------------
 */

/*创建并显示图片图层，需要坐标值，得到icon_key的数组，数组需要的参数值，P_Window*/
int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,enum GAlign how_to_align,enum GColor black_or_white,int32_t bmp_array_name[],int bmp_id_number)
{	


	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_array_name[bmp_id_number], &bmp_point);
	LayerBitmap layer_bitmap_struct_l = {bmp_point, *temp_p_frame, how_to_align};
 	temp_P_Layer = app_layer_create_bitmap(&layer_bitmap_struct_l);
	
	if(temp_P_Layer != NULL)
	{
		app_layer_set_bg_color(temp_P_Layer, black_or_white);
		return app_window_add_layer(p_window, temp_P_Layer);
	}

	return 0;
}


static P_Window init_window(void)
{
	P_Window p_window = NULL;

	/*创建一个窗口*/
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	/*创建背景图层*/
	display_target_layer(p_window,&bmp_origin_size_bg,GAlignLeft,GColorWhite,get_WEEK_icon_key,7);

	/*创建时分图层*/
	uint8_t i = 0;
	for(i = 0;i <= 3;i++)
	{
		display_target_layer(p_window,&bmp_origin_size[i],GAlignLeft,GColorWhite,get_number_icon_key,watch_data[i]);
	}
	
	/*创建秒图层*/
	//for(i = 4;i <= 5;i++)
	i=4;
	{
		g_layer_move_id = display_target_layer(p_window,&bmp_origin_size[i],GAlignLeft,GColorWhite,get_MOVE_icon_key,watch_data[i]);
	}

	/*创建月日图层*/
	for(i = 6;i <= 9;i++)
	{
		display_target_layer(p_window,&bmp_origin_size[i],GAlignLeft,GColorWhite,get_number_icon_key,watch_data[i]);
	}

	/*创建星期图层*/
	display_target_layer(p_window,&bmp_origin_size[10],GAlignLeft,GColorWhite,get_WEEK_icon_key,watch_data[10]);

	/*定义一个窗口定时器，用于秒表显示*/
	app_window_timer_subscribe(p_window, 200, sec_callback, NULL);
	
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
	
	app_watch_data(0);
	/*创建显示表盘窗口*/
	P_Window p_window = init_window();
	if (p_window != NULL)
	{
		/*放入窗口栈显示*/
		g_window_id = app_window_stack_push(p_window);

		/*注册一个事件通知回调，当有时间改变时，立即更新时间*/
		maibu_service_sys_event_subscribe(app_watch_time_change);
	}
		
	return 0;
}
