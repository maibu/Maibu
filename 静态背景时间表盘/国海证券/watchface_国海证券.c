#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "maibu_sdk.h"
#include "maibu_res.h"

#define WATCHFACE_GHZQ_BG_ORIGIN_X 	0
#define WATCHFACE_GHZQ_BG_ORIGIN_Y 	0
#define WATCHFACE_GHZQ_BG_WIZE_H		128
#define WATCHFACE_GHZQ_BG_SIZE_W 	128


#define WATCHFACE_GHZQ_TIME_HOUR1_ORIGIN_X 	6
#define WATCHFACE_GHZQ_TIME_HOUR1_ORIGIN_Y 	95
#define WATCHFACE_GHZQ_TIME_HOUR1_WIZE_H	25
#define WATCHFACE_GHZQ_TIME_HOUR1_SIZE_W 	26


#define WATCHFACE_GHZQ_TIME_HOUR2_ORIGIN_X 	32
#define WATCHFACE_GHZQ_TIME_HOUR2_ORIGIN_Y 	95
#define WATCHFACE_GHZQ_TIME_HOUR2_WIZE_H	25
#define WATCHFACE_GHZQ_TIME_HOUR2_SIZE_W 	26

#define WATCHFACE_GHZQ_TIME_MIN1_ORIGIN_X 	70
#define WATCHFACE_GHZQ_TIME_MIN1_ORIGIN_Y 	95
#define WATCHFACE_GHZQ_TIME_MIN1_WIZE_H		25
#define WATCHFACE_GHZQ_TIME_MIN1_SIZE_W 	26

#define WATCHFACE_GHZQ_TIME_MIN2_ORIGIN_X 	96
#define WATCHFACE_GHZQ_TIME_MIN2_ORIGIN_Y 	95
#define WATCHFACE_GHZQ_TIME_MIN2_WIZE_H		25
#define WATCHFACE_GHZQ_TIME_MIN2_SIZE_W 	26

#define WATCHFACE_GHZQ_WEEK_ORIGIN_X 	98
#define WATCHFACE_GHZQ_WEEK_ORIGIN_Y 	73
#define WATCHFACE_GHZQ_WEEK_WIZE_H		12
#define WATCHFACE_GHZQ_WEEK_SIZE_W 		30

#define WATCHFACE_GHZQ_DAY2_ORIGIN_X 	88
#define WATCHFACE_GHZQ_DAY2_ORIGIN_Y 	73
#define WATCHFACE_GHZQ_DAY2_WIZE_H		12
#define WATCHFACE_GHZQ_DAY2_SIZE_W 		9

#define WATCHFACE_GHZQ_STEP_ORIGIN_X  	113
#define WATCHFACE_GHZQ_STEP_ORIGIN_Y 	54
#define WATCHFACE_GHZQ_STEP_WIZE_H		12
#define WATCHFACE_GHZQ_STEP_SIZE_W 		9


const static char wday_str[7][8]={"周日","周一","周二","周三","周四","周五","周六"}; 


static GRect bmp_origin_size_bg = {
	{WATCHFACE_GHZQ_BG_ORIGIN_X,WATCHFACE_GHZQ_BG_ORIGIN_Y},
	{WATCHFACE_GHZQ_BG_WIZE_H,WATCHFACE_GHZQ_BG_SIZE_W}
};


static GRect bmp_origin_size[] = {

	{
		{WATCHFACE_GHZQ_TIME_HOUR1_ORIGIN_X,WATCHFACE_GHZQ_TIME_HOUR1_ORIGIN_Y},
		{WATCHFACE_GHZQ_TIME_HOUR1_WIZE_H,WATCHFACE_GHZQ_TIME_HOUR1_SIZE_W}
	},
	{
		{WATCHFACE_GHZQ_TIME_HOUR2_ORIGIN_X,WATCHFACE_GHZQ_TIME_HOUR2_ORIGIN_Y},
		{WATCHFACE_GHZQ_TIME_HOUR2_WIZE_H,WATCHFACE_GHZQ_TIME_HOUR2_SIZE_W}
	},
	{
		{WATCHFACE_GHZQ_TIME_MIN1_ORIGIN_X,WATCHFACE_GHZQ_TIME_MIN1_ORIGIN_Y},
		{WATCHFACE_GHZQ_TIME_MIN1_WIZE_H,WATCHFACE_GHZQ_TIME_MIN1_SIZE_W}
	},	

	{
		{WATCHFACE_GHZQ_TIME_MIN2_ORIGIN_X,WATCHFACE_GHZQ_TIME_MIN2_ORIGIN_Y},
		{WATCHFACE_GHZQ_TIME_MIN2_WIZE_H,WATCHFACE_GHZQ_TIME_MIN2_SIZE_W}
	},

	{
		{WATCHFACE_GHZQ_DAY2_ORIGIN_X,WATCHFACE_GHZQ_DAY2_ORIGIN_Y},
		{WATCHFACE_GHZQ_DAY2_WIZE_H,WATCHFACE_GHZQ_DAY2_SIZE_W}
	},

	{
		{WATCHFACE_GHZQ_WEEK_ORIGIN_X,WATCHFACE_GHZQ_WEEK_ORIGIN_Y},
		{WATCHFACE_GHZQ_WEEK_WIZE_H,WATCHFACE_GHZQ_WEEK_SIZE_W}
	},
	
	
	{
		{WATCHFACE_GHZQ_STEP_ORIGIN_X,WATCHFACE_GHZQ_STEP_ORIGIN_Y},
		{WATCHFACE_GHZQ_STEP_WIZE_H,WATCHFACE_GHZQ_STEP_SIZE_W}
	}
	
};


//小号数字图片数组
static uint32_t get_LIT_icon_key[] =
{
	RES_BITMAP_WATCHFACE_NUMBER_LIT_0,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_1,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_2,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_3,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_4,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_5,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_6,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_7,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_8,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_9,
	RES_BITMAP_WATCHFACE_XIE_XIAN,
	RES_BITMAP_WATCHFACE_SHOES
};

//大号数字图片数组
static uint32_t get_BIG_icon_key[] =
{
	RES_BITMAP_WATCHFACE_NUMBER_BIG_0,  
	RES_BITMAP_WATCHFACE_NUMBER_BIG_1, 
	RES_BITMAP_WATCHFACE_NUMBER_BIG_2,  
	RES_BITMAP_WATCHFACE_NUMBER_BIG_3,  
	RES_BITMAP_WATCHFACE_NUMBER_BIG_4,  
	RES_BITMAP_WATCHFACE_NUMBER_BIG_5,  
	RES_BITMAP_WATCHFACE_NUMBER_BIG_6,  
	RES_BITMAP_WATCHFACE_NUMBER_BIG_7, 
	RES_BITMAP_WATCHFACE_NUMBER_BIG_8,  
	RES_BITMAP_WATCHFACE_NUMBER_BIG_9

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
 /*创建并显示文本图层*/
int32_t display_target_layerText(P_Window p_window,const GRect  *temp_p_frame,enum GAlign how_to_align,enum GColor color,char * str,uint8_t font_type)
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

/*创建并显示图片图层，需要坐标值，得到icon_key的数组，数组需要的参数值，P_Window*/
int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,enum GAlign how_to_align,enum GColor black_or_white,uint32_t bmp_key)
{	


	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_key, &bmp_point);
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
	display_target_layer(p_window,&bmp_origin_size_bg,GAlignLeft,GColorWhite,RES_BITMAP_WATCHFACE_WEATHER_BG);

	struct date_time datetime;
	app_service_get_datetime(&datetime);
	

	uint8_t i = 0;
	/*创建时间图层*/
	uint32_t time_key_array[] = {datetime.hour/10,datetime.hour%10,datetime.min/10,datetime.min%10};

	for(i = 0;i<=3;i++)
	{
		display_target_layer(p_window,&bmp_origin_size[i],GAlignLeft,GColorWhite,get_BIG_icon_key[time_key_array[i]]);
	}

	/*创建月日图层*/
	uint32_t md_key_array[] = {datetime.mday%10,datetime.mday/10,10,datetime.mon%10,datetime.mon/10};
	GRect temp_origin_size = {0};
	temp_origin_size = bmp_origin_size[4];
	
	for(i = 0;i<=4;i++)
	{
		display_target_layer(p_window,&temp_origin_size,GAlignLeft,GColorWhite,get_LIT_icon_key[md_key_array[i]]);
		temp_origin_size.origin.x = temp_origin_size.origin.x - temp_origin_size.size.w;
	}
		
	/*创建星期图层*/
	
	int8_t temp_str[8] = {0};
	sprintf(temp_str, "%s",&wday_str[datetime.wday]);
	
	display_target_layerText(p_window,&bmp_origin_size[5],GAlignLeft,GColorBlack,temp_str,U_ASCII_ARIAL_12);
	
	/*创建步数图片图层*/
	//步数
	SportData sport_datas;
    maibu_get_sport_data(&sport_datas, 0);   

	temp_origin_size = bmp_origin_size[6];
	
	char temp_steps_str[8] = {0};
	
	sprintf(temp_steps_str,"%d",sport_datas.step);
	
	for(i = 1;i <= strlen(temp_steps_str);i++)
	{
		
		display_target_layer(p_window,&temp_origin_size,GAlignLeft,GColorWhite,get_LIT_icon_key[temp_steps_str[strlen(temp_steps_str)-i]-'0']);
		temp_origin_size.origin.x = temp_origin_size.origin.x - temp_origin_size.size.w; 
	}

	temp_origin_size.origin.x = temp_origin_size.origin.x - 7;
	temp_origin_size.size.w = 16;
	
	display_target_layer(p_window,&temp_origin_size,GAlignLeft,GColorWhite,RES_BITMAP_WATCHFACE_SHOES);

	/*定义一个窗口定时器，用于秒表显示*/
	app_window_timer_subscribe(p_window, 1000, window_reloading, NULL);
	
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
