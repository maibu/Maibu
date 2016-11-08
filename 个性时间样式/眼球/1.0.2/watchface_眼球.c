#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "maibu_sdk.h"
#include "maibu_res.h"

#define WATCHFACE_BG_ORIGIN_X 	0
#define WATCHFACE_BG_ORIGIN_Y 	0
#define WATCHFACE_BG_WIZE_H		128
#define WATCHFACE_BG_SIZE_W 	128

#define WATCHFACE_EYE_TRACE_01_ORIGIN_X 	98
#define WATCHFACE_EYE_TRACE_01_ORIGIN_Y 	1
#define WATCHFACE_EYE_TRACE_01_WIZE_H		33
#define WATCHFACE_EYE_TRACE_01_SIZE_W 		29


#define WATCHFACE_EYE_TRACE_02_ORIGIN_X 	76
#define WATCHFACE_EYE_TRACE_02_ORIGIN_Y 	74
#define WATCHFACE_EYE_TRACE_02_WIZE_H		53
#define WATCHFACE_EYE_TRACE_02_SIZE_W 		51

#define WATCHFACE_EYE_TRACE_03_ORIGIN_X 	1
#define WATCHFACE_EYE_TRACE_03_ORIGIN_Y 	71
#define WATCHFACE_EYE_TRACE_03_WIZE_H		56
#define WATCHFACE_EYE_TRACE_03_SIZE_W 		35

#define WATCHFACE_EYE_TRACE_04_ORIGIN_X 	1
#define WATCHFACE_EYE_TRACE_04_ORIGIN_Y 	1
#define WATCHFACE_EYE_TRACE_04_WIZE_H		40
#define WATCHFACE_EYE_TRACE_04_SIZE_W 		36

#define WATCHFACE_EYE_MIN1_ORIGIN_X 	29
#define WATCHFACE_EYE_MIN1_ORIGIN_Y 	30
#define WATCHFACE_EYE_MIN1_SIZE_H		5
#define WATCHFACE_EYE_MIN1_SIZE_W 		4

#define WATCHFACE_EYE_MIN2_ORIGIN_X 	33
#define WATCHFACE_EYE_MIN2_ORIGIN_Y 	30
#define WATCHFACE_EYE_MIN2_SIZE_H		5
#define WATCHFACE_EYE_MIN2_SIZE_W 		4


const static GRect bmp_origin_size[] = {
	{
		{WATCHFACE_BG_ORIGIN_X,WATCHFACE_BG_ORIGIN_Y},
		{WATCHFACE_BG_WIZE_H,WATCHFACE_BG_SIZE_W}
	},
	{
		{WATCHFACE_EYE_TRACE_01_ORIGIN_X,WATCHFACE_EYE_TRACE_01_ORIGIN_Y},
		{WATCHFACE_EYE_TRACE_01_WIZE_H,WATCHFACE_EYE_TRACE_01_SIZE_W}
	},
	{
		{WATCHFACE_EYE_TRACE_02_ORIGIN_X,WATCHFACE_EYE_TRACE_02_ORIGIN_Y},
		{WATCHFACE_EYE_TRACE_02_WIZE_H,WATCHFACE_EYE_TRACE_02_SIZE_W}
	},
	{
		{WATCHFACE_EYE_TRACE_03_ORIGIN_X,WATCHFACE_EYE_TRACE_03_ORIGIN_Y},
		{WATCHFACE_EYE_TRACE_03_WIZE_H,WATCHFACE_EYE_TRACE_03_SIZE_W}
	},	

	{
		{WATCHFACE_EYE_TRACE_04_ORIGIN_X,WATCHFACE_EYE_TRACE_04_ORIGIN_Y},
		{WATCHFACE_EYE_TRACE_04_WIZE_H,WATCHFACE_EYE_TRACE_04_SIZE_W}
	}

};


const static GPoint eye_origin[] = {
	{32,2},
	{36,2},
	{40,2},
	{44,2},
	{48,2},
	{52,2},
	{56,2},
	{60,2},
	{62,4},
	{62,8},
	{62,12},
	{62,16},
	{62,20},
	{62,24},
	{62,28},
	{62,32},
	{62,36},
	{62,40},
	{62,44},
	{62,48},
	{62,52},
	{62,56},
	{62,60},
	{60,62},
	{56,62},
	{52,62},
	{48,62},
	{44,62},
	{40,62},
	{36,62},
	{32,62},
	{28,62},
	{24,62},
	{20,62},
	{16,62},
	{12,62},
	{8,62},
	{4,62},
	{2,60},
	{2,56},
	{2,52},
	{2,48},
	{2,44},
	{2,40},
	{2,36},
	{2,32},
	{2,28},
	{2,24},
	{2,20},
	{2,16},
	{2,12},
	{2,8},
	{2,4},
	{4,2},
	{8,2},
	{12,2},
	{16,2},
	{20,2},
	{24,2},
	{28,2}
		
};

//小号数字图片数组
const static uint32_t get_number_icon_key[] =
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
const static uint32_t get_bmp_icon_key[] =
{
	RES_BITMAP_WATCHFACE_BG,
	RES_BITMAP_WATCHFACE_TRACE_01,
	RES_BITMAP_WATCHFACE_TRACE_02,
	RES_BITMAP_WATCHFACE_TRACE_03,
	RES_BITMAP_WATCHFACE_TRACE_04,
	RES_BITMAP_WATCHFACE_EYE
};


/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_window_id = -1;


static P_Window init_window(void);

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
int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,enum GAlign how_to_align,enum GColor black_or_white,int32_t bmp_array_name)
{	


	GBitmap bmp_point = {0};
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_array_name, &bmp_point);
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

	
	struct date_time datetime;
	app_service_get_datetime(&datetime);

	/*创建背景图层*/
	display_target_layer(p_window,&bmp_origin_size[0],GAlignCenter,GColorWhite,get_bmp_icon_key[0]);


	if((datetime.min%60 >= 16)||(datetime.min%60 == 0))
	{
		display_target_layer(p_window,&bmp_origin_size[1],GAlignCenter,GColorWhite,get_bmp_icon_key[1]);
	}
	if((datetime.min%60 >= 35)||(datetime.min%60 <= 9))
	{
		display_target_layer(p_window,&bmp_origin_size[2],GAlignCenter,GColorWhite,get_bmp_icon_key[2]);
	}

	if((datetime.min%60 >= 51)||(datetime.min%60 <= 29))
	{
		display_target_layer(p_window,&bmp_origin_size[3],GAlignCenter,GColorWhite,get_bmp_icon_key[3]);
	}
	if((datetime.min%60 <= 43)&&(datetime.min%60 != 0))
	{
		display_target_layer(p_window,&bmp_origin_size[4],GAlignCenter,GColorWhite,get_bmp_icon_key[4]);
	}


	
	GRect temp_origin_size = {0};
	temp_origin_size.origin = eye_origin[datetime.min%60]; 
	temp_origin_size.size.h = 64;
	temp_origin_size.size.w = 64;

	display_target_layer(p_window,&temp_origin_size,GAlignCenter,GColorWhite,get_bmp_icon_key[5]);

	temp_origin_size.origin.x = eye_origin[datetime.min%60].x + WATCHFACE_EYE_MIN1_ORIGIN_X; 
	temp_origin_size.origin.y = eye_origin[datetime.min%60].y + WATCHFACE_EYE_MIN1_ORIGIN_Y; 
	temp_origin_size.size.h = WATCHFACE_EYE_MIN1_SIZE_H;
	temp_origin_size.size.w = WATCHFACE_EYE_MIN1_SIZE_W;

	display_target_layer(p_window,&temp_origin_size,GAlignCenter,GColorWhite,get_number_icon_key[datetime.hour/10]);

	temp_origin_size.origin.x = eye_origin[datetime.min%60].x + WATCHFACE_EYE_MIN2_ORIGIN_X; 
	temp_origin_size.origin.y = eye_origin[datetime.min%60].y + WATCHFACE_EYE_MIN2_ORIGIN_Y; 
	temp_origin_size.size.h = WATCHFACE_EYE_MIN2_SIZE_H;
	temp_origin_size.size.w = WATCHFACE_EYE_MIN2_SIZE_W;

	display_target_layer(p_window,&temp_origin_size,GAlignCenter,GColorWhite,get_number_icon_key[datetime.hour%10]);



	
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
