#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "maibu_sdk.h"
#include "maibu_res.h"

#define WATCHFACE_EVA_BG_ORIGIN_X 	0
#define WATCHFACE_EVA_BG_ORIGIN_Y 	0
#define WATCHFACE_EVA_BG_WIZE_H		128
#define WATCHFACE_EVA_BG_SIZE_W 	128


#define WATCHFACE_EVA_TIME_HOUR1_ORIGIN_X 	0
#define WATCHFACE_EVA_TIME_HOUR1_ORIGIN_Y 	6
#define WATCHFACE_EVA_TIME_HOUR1_WIZE_H		116
#define WATCHFACE_EVA_TIME_HOUR1_SIZE_W 	128


#define WATCHFACE_EVA_TIME_HOUR2_ORIGIN_X 	23
#define WATCHFACE_EVA_TIME_HOUR2_ORIGIN_Y 	26
#define WATCHFACE_EVA_TIME_HOUR2_WIZE_H		76
#define WATCHFACE_EVA_TIME_HOUR2_SIZE_W 	82

#define WATCHFACE_EVA_TIME_MIN1_ORIGIN_X 	38
#define WATCHFACE_EVA_TIME_MIN1_ORIGIN_Y 	41
#define WATCHFACE_EVA_TIME_MIN1_WIZE_H		46
#define WATCHFACE_EVA_TIME_MIN1_SIZE_W 		52

#define WATCHFACE_EVA_TIME_MIN2_ORIGIN_X 	48
#define WATCHFACE_EVA_TIME_MIN2_ORIGIN_Y 	49
#define WATCHFACE_EVA_TIME_MIN2_WIZE_H		30
#define WATCHFACE_EVA_TIME_MIN2_SIZE_W 		32

static GRect bmp_origin_size_bg = {
	{WATCHFACE_EVA_BG_ORIGIN_X,WATCHFACE_EVA_BG_ORIGIN_Y},
	{WATCHFACE_EVA_BG_WIZE_H,WATCHFACE_EVA_BG_SIZE_W}
};


static GRect bmp_origin_size[] = {

	{
		{WATCHFACE_EVA_TIME_HOUR1_ORIGIN_X,WATCHFACE_EVA_TIME_HOUR1_ORIGIN_Y},
		{WATCHFACE_EVA_TIME_HOUR1_WIZE_H,WATCHFACE_EVA_TIME_HOUR1_SIZE_W}
	},
	{
		{WATCHFACE_EVA_TIME_HOUR2_ORIGIN_X,WATCHFACE_EVA_TIME_HOUR2_ORIGIN_Y},
		{WATCHFACE_EVA_TIME_HOUR2_WIZE_H,WATCHFACE_EVA_TIME_HOUR2_SIZE_W}
	},
	{
		{WATCHFACE_EVA_TIME_MIN1_ORIGIN_X,WATCHFACE_EVA_TIME_MIN1_ORIGIN_Y},
		{WATCHFACE_EVA_TIME_MIN1_WIZE_H,WATCHFACE_EVA_TIME_MIN1_SIZE_W}
	},	

	{
		{WATCHFACE_EVA_TIME_MIN2_ORIGIN_X,WATCHFACE_EVA_TIME_MIN2_ORIGIN_Y},
		{WATCHFACE_EVA_TIME_MIN2_WIZE_H,WATCHFACE_EVA_TIME_MIN2_SIZE_W}
	}

};

//小小号数字图片数组
static uint32_t get_SMALL_icon_key[] =
{
	RES_BITMAP_WATCHFACE_NUMBER_SMALL_0,
	RES_BITMAP_WATCHFACE_NUMBER_SMALL_1,
	RES_BITMAP_WATCHFACE_NUMBER_SMALL_2,
	RES_BITMAP_WATCHFACE_NUMBER_SMALL_3,
	RES_BITMAP_WATCHFACE_NUMBER_SMALL_4,
	RES_BITMAP_WATCHFACE_NUMBER_SMALL_5,
	RES_BITMAP_WATCHFACE_NUMBER_SMALL_6,
	RES_BITMAP_WATCHFACE_NUMBER_SMALL_7,
	RES_BITMAP_WATCHFACE_NUMBER_SMALL_8,
	RES_BITMAP_WATCHFACE_NUMBER_SMALL_9
	
	
};

//小号数字图片数组
static uint32_t get_LIT_icon_key[] =
{
	RES_BITMAP_WATCHFACE_NUMBER_LIT_0,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_1,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_2,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_3,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_4,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_5
	
	
};
//中号数字图片数组
static uint32_t get_MID_icon_key[] =
{
	RES_BITMAP_WATCHFACE_NUMBER_MID_0,
	RES_BITMAP_WATCHFACE_NUMBER_MID_1,
	RES_BITMAP_WATCHFACE_NUMBER_MID_2,
	RES_BITMAP_WATCHFACE_NUMBER_MID_3,
	RES_BITMAP_WATCHFACE_NUMBER_MID_4,
	RES_BITMAP_WATCHFACE_NUMBER_MID_5,
	RES_BITMAP_WATCHFACE_NUMBER_MID_6,
	RES_BITMAP_WATCHFACE_NUMBER_MID_7,
	RES_BITMAP_WATCHFACE_NUMBER_MID_8,
	RES_BITMAP_WATCHFACE_NUMBER_MID_9
	
};

//大号数字图片数组
static uint32_t get_BIG_icon_key[] =
{
	RES_BITMAP_WATCHFACE_NUMBER_BIG_0,  
	RES_BITMAP_WATCHFACE_NUMBER_BIG_1, 
	RES_BITMAP_WATCHFACE_NUMBER_BIG_2,
	RES_BITMAP_WATCHFACE_BG

};

/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_window_id = -1;
P_Window init_window(void);

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
		window_reloading();
	}
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


P_Window init_window(void)
{
	P_Window p_window = NULL;

	/*创建一个窗口*/
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	struct date_time datetime;
	app_service_get_datetime(&datetime);
	
	/*创建背景图层*/
	display_target_layer(p_window,&bmp_origin_size_bg,GAlignLeft,GColorWhite,get_BIG_icon_key,3);

	/*创建时分图层*/
	display_target_layer(p_window,&bmp_origin_size[0],GAlignLeft,GColorWhite,get_BIG_icon_key,datetime.hour/10);
	display_target_layer(p_window,&bmp_origin_size[1],GAlignLeft,GColorWhite,get_MID_icon_key,datetime.hour%10);
	display_target_layer(p_window,&bmp_origin_size[2],GAlignLeft,GColorWhite,get_LIT_icon_key,datetime.min/10);
	display_target_layer(p_window,&bmp_origin_size[3],GAlignLeft,GColorWhite,get_SMALL_icon_key,datetime.min%10);

		
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
