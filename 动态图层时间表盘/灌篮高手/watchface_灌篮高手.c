#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "maibu_sdk.h"
#include "maibu_res.h"



#define WATCHFACE_STYLE_1_TIME_HOUR1_ORIGIN_X 	75
#define WATCHFACE_STYLE_1_TIME_HOUR1_ORIGIN_Y 	3
#define WATCHFACE_STYLE_1_TIME_HOUR1_WIZE_H		17
#define WATCHFACE_STYLE_1_TIME_HOUR1_SIZE_W 	10


#define WATCHFACE_STYLE_1_TIME_HOUR2_ORIGIN_X 	86
#define WATCHFACE_STYLE_1_TIME_HOUR2_ORIGIN_Y 	3
#define WATCHFACE_STYLE_1_TIME_HOUR2_WIZE_H		17
#define WATCHFACE_STYLE_1_TIME_HOUR2_SIZE_W 	10

#define WATCHFACE_STYLE_1_TIME_MIN1_ORIGIN_X 	104
#define WATCHFACE_STYLE_1_TIME_MIN1_ORIGIN_Y 	3
#define WATCHFACE_STYLE_1_TIME_MIN1_WIZE_H		17
#define WATCHFACE_STYLE_1_TIME_MIN1_SIZE_W 		10

#define WATCHFACE_STYLE_1_TIME_MIN2_ORIGIN_X 	115
#define WATCHFACE_STYLE_1_TIME_MIN2_ORIGIN_Y 	3
#define WATCHFACE_STYLE_1_TIME_MIN2_WIZE_H		17
#define WATCHFACE_STYLE_1_TIME_MIN2_SIZE_W 		10


#define WATCHFACE_STYLE_2_TIME_HOUR1_ORIGIN_X 	2
#define WATCHFACE_STYLE_2_TIME_HOUR1_ORIGIN_Y 	3
#define WATCHFACE_STYLE_2_TIME_HOUR1_WIZE_H		17
#define WATCHFACE_STYLE_2_TIME_HOUR1_SIZE_W 	10


#define WATCHFACE_STYLE_2_TIME_HOUR2_ORIGIN_X 	13
#define WATCHFACE_STYLE_2_TIME_HOUR2_ORIGIN_Y 	3
#define WATCHFACE_STYLE_2_TIME_HOUR2_WIZE_H		17
#define WATCHFACE_STYLE_2_TIME_HOUR2_SIZE_W 	10

#define WATCHFACE_STYLE_2_TIME_MIN1_ORIGIN_X 	31
#define WATCHFACE_STYLE_2_TIME_MIN1_ORIGIN_Y 	3
#define WATCHFACE_STYLE_2_TIME_MIN1_WIZE_H		17
#define WATCHFACE_STYLE_2_TIME_MIN1_SIZE_W 		10

#define WATCHFACE_STYLE_2_TIME_MIN2_ORIGIN_X 	42
#define WATCHFACE_STYLE_2_TIME_MIN2_ORIGIN_Y 	3
#define WATCHFACE_STYLE_2_TIME_MIN2_WIZE_H		17
#define WATCHFACE_STYLE_2_TIME_MIN2_SIZE_W 		10


static GRect bmp_origin_size_bg = {
	{0,0},
	{128,128}
};


const static GRect bmp_origin_size[] = {

	{
		{WATCHFACE_STYLE_1_TIME_HOUR1_ORIGIN_X,WATCHFACE_STYLE_1_TIME_HOUR1_ORIGIN_Y},
		{WATCHFACE_STYLE_1_TIME_HOUR1_WIZE_H,WATCHFACE_STYLE_1_TIME_HOUR1_SIZE_W}
	},
	{
		{WATCHFACE_STYLE_1_TIME_HOUR2_ORIGIN_X,WATCHFACE_STYLE_1_TIME_HOUR2_ORIGIN_Y},
		{WATCHFACE_STYLE_1_TIME_HOUR2_WIZE_H,WATCHFACE_STYLE_1_TIME_HOUR2_SIZE_W}
	},
	{
		{WATCHFACE_STYLE_1_TIME_MIN1_ORIGIN_X,WATCHFACE_STYLE_1_TIME_MIN1_ORIGIN_Y},
		{WATCHFACE_STYLE_1_TIME_MIN1_WIZE_H,WATCHFACE_STYLE_1_TIME_MIN1_SIZE_W}
	},	

	{
		{WATCHFACE_STYLE_1_TIME_MIN2_ORIGIN_X,WATCHFACE_STYLE_1_TIME_MIN2_ORIGIN_Y},
		{WATCHFACE_STYLE_1_TIME_MIN2_WIZE_H,WATCHFACE_STYLE_1_TIME_MIN2_SIZE_W}
	},
	{
		{WATCHFACE_STYLE_2_TIME_HOUR1_ORIGIN_X,WATCHFACE_STYLE_2_TIME_HOUR1_ORIGIN_Y},
		{WATCHFACE_STYLE_2_TIME_HOUR1_WIZE_H,WATCHFACE_STYLE_2_TIME_HOUR1_SIZE_W}
	},
	{
		{WATCHFACE_STYLE_2_TIME_HOUR2_ORIGIN_X,WATCHFACE_STYLE_2_TIME_HOUR2_ORIGIN_Y},
		{WATCHFACE_STYLE_2_TIME_HOUR2_WIZE_H,WATCHFACE_STYLE_2_TIME_HOUR2_SIZE_W}
	},
	{
		{WATCHFACE_STYLE_2_TIME_MIN1_ORIGIN_X,WATCHFACE_STYLE_2_TIME_MIN1_ORIGIN_Y},
		{WATCHFACE_STYLE_2_TIME_MIN1_WIZE_H,WATCHFACE_STYLE_2_TIME_MIN1_SIZE_W}
	},	

	{
		{WATCHFACE_STYLE_2_TIME_MIN2_ORIGIN_X,WATCHFACE_STYLE_2_TIME_MIN2_ORIGIN_Y},
		{WATCHFACE_STYLE_2_TIME_MIN2_WIZE_H,WATCHFACE_STYLE_2_TIME_MIN2_SIZE_W}
	}
};


//小号数字图片数组
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
//中号数字图片数组
static uint32_t get_bmp_icon_key[] =
{
	RES_BITMAP_WATCHFACE_FACE_1,
	RES_BITMAP_WATCHFACE_FACE_2,
	RES_BITMAP_WATCHFACE_FACE_3,
	RES_BITMAP_WATCHFACE_FACE_4,
	RES_BITMAP_WATCHFACE_FACE_5,
	RES_BITMAP_WATCHFACE_FACE_6	
};


/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_window_id = -1;

/*表盘应显示数据*/
static uint8_t watch_data[5] = {0};
static uint8_t style_1_or_2 = 0;



static P_Window init_window(void);

int8_t get_random_number(int8_t pram)
{
	int16_t x, y, z;
	maibu_get_accel_data(&x, &y, &z);
	return z%pram;
} 


/*
 *--------------------------------------------------------------------------------------
 *     function:  app_watch_data
 *    parameter:  0为watch_data所有元素值，1仅为watch_data中的秒
 *       return:
 *  description:  将数组参数赋值为当前表盘应显示值
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
static void app_watch_data(void)
{
	int8_t random = 0;
		
	struct date_time datetime;
	app_service_get_datetime(&datetime);

	
	watch_data[0] = datetime.hour/10;
	watch_data[1] = datetime.hour%10;
	watch_data[2] = datetime.min/10;
	watch_data[3] = datetime.min%10;

	random = get_random_number(6);
	watch_data[4] = (watch_data[4]+random)%6;//图片的序号
				
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
		app_watch_data();
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


/*创建并显示文本图层*/
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
	display_target_layer(p_window,&bmp_origin_size_bg,GAlignCenter,GColorWhite,get_bmp_icon_key,watch_data[4]);

	
	/*创建时分图层*/
	uint8_t i = 0;
	uint8_t j = 0;
	
	if(watch_data[4]<=2)
	{
		j = 0;
	}
	else
	{
		j = 4;
	}
	
	for(i = 0;i <= 3;i++)
	{
		display_target_layer(p_window,&bmp_origin_size[j + i],GAlignLeft,GColorWhite,get_number_icon_key,watch_data[i]);
	}
	
	
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
		
	app_watch_data();
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
