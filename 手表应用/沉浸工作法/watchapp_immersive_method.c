#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "maibu_sdk.h"
#include "maibu_res.h"


/*背景图片图层位置*/
#define IMMERSIVE_BG_ORIGIN_X		0	
#define IMMERSIVE_BG_ORIGIN_Y		0
#define IMMERSIVE_BG_SIZE_H		    128
#define IMMERSIVE_BG_SIZE_W			128


/*时间文本图层位置*/
#define IMMERSIVE_TIME_ORIGIN_X		0	
#define IMMERSIVE_TIME_ORIGIN_Y		1
#define IMMERSIVE_TIME_SIZE_H	    12
#define IMMERSIVE_TIME_SIZE_W		128

/*倒计时图片图层起始位置*/
#define IMMERSIVE_COUNTDOWN_ORIGIN_Y	40		
#define IMMERSIVE_COUNTDOWN_SIZE_H		34
#define IMMERSIVE_COUNTDOWN_SIZE_W		19

/*休息中/沉浸中图片图层位置*/
#define IMMERSIVE_TIPS_ORIGIN_X		48	
#define IMMERSIVE_TIPS_ORIGIN_Y		95	
#define IMMERSIVE_TIPS_SIZE_H		17
#define IMMERSIVE_TIPS_SIZE_W		31

#define COUNTDOWN_TOTAL_SECONDS  	(15*60)

#define COUNTDOWN_TOTAL_SECONDS_2  	(14*60+30)

/*初始状态*/
#define IMMERSIVE_STATE_INIT		3

/*沉浸中状态*/
#define IMMERSIVE_STATE_IMMERSIVE	1

/*休息中状态*/
#define IMMERSIVE_STATE_REST		2

/*沉浸结束选择状态*/
#define IMMERSIVE_STATE_IMMERSIVE_TIME_OUT	4

/*休息结束选择状态*/
#define IMMERSIVE_STATE_REST_TIME_OUT		5


//图片ID数组
static int32_t bmp_array_name[] = {
	RES_BITMAP_WATCHFACE_NUMBER_0,
	RES_BITMAP_WATCHFACE_NUMBER_1,
	RES_BITMAP_WATCHFACE_NUMBER_2,
	RES_BITMAP_WATCHFACE_NUMBER_3,
	RES_BITMAP_WATCHFACE_NUMBER_4,
	RES_BITMAP_WATCHFACE_NUMBER_5,
	RES_BITMAP_WATCHFACE_NUMBER_6,
	RES_BITMAP_WATCHFACE_NUMBER_7,
	RES_BITMAP_WATCHFACE_NUMBER_8,
	RES_BITMAP_WATCHFACE_NUMBER_9,
	RES_BITMAP_WATCHAPP_BG_01,
	RES_BITMAP_WATCHAPP_BG_02,
	RES_BITMAP_WATCHAPP_IMMERSING,
	RES_BITMAP_WATCHAPP_REST,
	RES_BITMAP_WATCHAPP_MAO_HAO,
	RES_BITMAP_WATCHAPP_IMMERSING_END,
	RES_BITMAP_WATCHAPP_REST_END
};

static int8_t g_countdown_array_param[5] = {1,5,14,0,0};


/*定时器ID*/
static uint8_t g_immersive_timer_id = -1;

/*窗口ID*/
static int32_t g_window_id = -1;

/*休息中/沉浸中图片图层ID*/
static int8_t g_layer_immersive_rest_id = -1;

/*时间文本图层ID*/
static int8_t g_layer_top_time_id = -1;

/*总秒数*/
static int32_t g_total_seconds = COUNTDOWN_TOTAL_SECONDS;

/*倒计时显示的图片数量*/
static int32_t g_counttime_show_pic_number = 5;

/*状态标识*/
static int32_t g_immersive_status = IMMERSIVE_STATE_INIT;

static int8_t g_layer_id_array[5] = {-1,-1,-1,-1,-1};

static bool	g_layer_immersive_rest_show_status = true;

static P_Window immersive_init_window(void);
static void immersive_timer_callback(date_time_t tick_time, uint32_t millis, void *context);

//重新载入并刷新窗口所有图层
static void window_reloading(P_Window p_old_window)
{
	P_Window p_new_window = immersive_init_window();
	if (NULL != p_new_window)
	{
			g_window_id = app_window_stack_replace_window(p_old_window, p_new_window);
	}	
}

/*创建并显示图片图层，需要坐标值，得到icon_key的数组，数组需要的参数值，P_Window*/
static int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,enum GAlign how_to_align,int32_t bmp_array_name[],int bmp_id_number)
{	
	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_array_name[bmp_id_number], &bmp_point);
	LayerBitmap layer_bitmap_struct_l = {bmp_point, *temp_p_frame, how_to_align};
 	temp_P_Layer = app_layer_create_bitmap(&layer_bitmap_struct_l);
	
	if(temp_P_Layer != NULL)
	{
		return app_window_add_layer(p_window, temp_P_Layer);
	}
	return 0;
}

/*创建并显示文本图层*/
static int32_t display_target_layerText(P_Window p_window,GRect  *temp_p_frame,enum GAlign how_to_align,enum GColor color,char * str,uint8_t font_type)
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
//重复代码
static void show_or_hide(P_Window p_window)
{
	maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_layer_immersive_rest_id),g_layer_immersive_rest_show_status);
	maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_layer_top_time_id),g_layer_immersive_rest_show_status);
}

static void get_time_str(char *time_str_buffer_p)
{
	struct date_time dt;
	app_service_get_datetime(&dt);	
	sprintf(time_str_buffer_p, "%02d:%02d", dt.hour, dt.min);
}
static void param_init(void)
{
	app_service_timer_unsubscribe(g_immersive_timer_id);
	
	g_total_seconds = COUNTDOWN_TOTAL_SECONDS;
	g_counttime_show_pic_number = 5;
	
	g_countdown_array_param[0] = 1;
	g_countdown_array_param[1] = 5;
	g_countdown_array_param[2] = 14;
	g_countdown_array_param[3] = 0;
	g_countdown_array_param[4] = 0;

}

static void param_init_time_out(void)
{
	app_service_timer_unsubscribe(g_immersive_timer_id);
	g_immersive_status = g_immersive_status - 3;
	g_total_seconds = COUNTDOWN_TOTAL_SECONDS_2;
	
//	g_layer_immersive_rest_show_status = true;

	g_counttime_show_pic_number = 5;
	
	g_countdown_array_param[0] = 1;
	g_countdown_array_param[1] = 4;
	g_countdown_array_param[2] = 14;
	g_countdown_array_param[3] = 3;
	g_countdown_array_param[4] = 0;
	
	g_immersive_timer_id = app_service_timer_subscribe(1000, immersive_timer_callback, NULL);	
	window_reloading(app_window_stack_get_window_by_id(g_window_id));
}

static void immersive_timer_callback(date_time_t tick_time, uint32_t millis, void *context)
{
	P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
	if (NULL == p_window)
	{
		return;
	}
	g_total_seconds--;

	g_counttime_show_pic_number = 4 + g_total_seconds/600;

	g_countdown_array_param[0] = g_total_seconds / 60 / 10;
	g_countdown_array_param[1] = g_total_seconds / 60 % 10;
	g_countdown_array_param[2] = 14;
	g_countdown_array_param[3] = g_total_seconds % 60 / 10;
	g_countdown_array_param[4] = g_total_seconds % 60 % 10;

	if(g_total_seconds == 599)
	{
		window_reloading(p_window);
	}
	else if(g_total_seconds == 0)
	{
		app_service_timer_unsubscribe(g_immersive_timer_id);
		g_immersive_timer_id = app_service_timer_subscribe(30*1000, param_init_time_out, NULL); 
				
		g_immersive_status = g_immersive_status + 3;
		
		window_reloading(p_window);
		maibu_service_vibes_pulse(VibesPulseTypeLong, 3);
	}
	else
	{
		int j;
		GBitmap bmp_point_temp;
		P_Layer layer_point_temp = NULL;

		char time_str_buffer_t[6] = {0};
		get_time_str(time_str_buffer_t);
		
		layer_point_temp = app_window_get_layer_by_id(p_window,g_layer_top_time_id);
		app_layer_set_text_text(layer_point_temp,time_str_buffer_t);


		for(j = (5-g_counttime_show_pic_number);j < 5;j++)
		{
			layer_point_temp = app_window_get_layer_by_id(p_window,g_layer_id_array[j]);
			
			if (layer_point_temp != NULL)
			{
				res_get_user_bitmap(bmp_array_name[g_countdown_array_param[j]], &bmp_point_temp);
				app_layer_set_bitmap_bitmap(layer_point_temp,&bmp_point_temp);
			}
		}
			
		app_window_update(p_window);
	}	
	
}


/*定义向上按键事件*/
static void immersive_select_up(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL == p_window)
	{
		return;
	}
	if((g_immersive_status == IMMERSIVE_STATE_INIT)||(g_immersive_status == IMMERSIVE_STATE_IMMERSIVE_TIME_OUT)||(g_immersive_status == IMMERSIVE_STATE_REST_TIME_OUT))
	{
		param_init();
		g_immersive_status = IMMERSIVE_STATE_IMMERSIVE;
		/*添加定时器*/
		g_immersive_timer_id = app_service_timer_subscribe(1000, immersive_timer_callback, NULL);	
		window_reloading(p_window);
	}

}


/*定义向下按键事件*/
static void immersive_select_down(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL == p_window)
	{
		return;
	}
	if((g_immersive_status == IMMERSIVE_STATE_INIT)||(g_immersive_status == IMMERSIVE_STATE_IMMERSIVE_TIME_OUT)||(g_immersive_status == IMMERSIVE_STATE_REST_TIME_OUT))
	{
		param_init();
		g_immersive_status = IMMERSIVE_STATE_REST;
		/*添加定时器*/
		g_immersive_timer_id = app_service_timer_subscribe(1000, immersive_timer_callback, NULL);	
		window_reloading(p_window);
	}
	
}

/*定义选择按键事件*/
static void immersive_select_select(void *context)
{
	
	P_Window p_window = (P_Window)context;
	if (NULL == p_window)
	{
		return;
	}
	if((g_immersive_status == IMMERSIVE_STATE_IMMERSIVE)||(g_immersive_status == IMMERSIVE_STATE_REST))
	{
		g_layer_immersive_rest_show_status = !g_layer_immersive_rest_show_status;
		show_or_hide(p_window);
		app_window_update(p_window);
	}

}


/*定义后退按键事件*/
static void immersive_select_back(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		if(g_immersive_status == IMMERSIVE_STATE_INIT)
		{
			app_window_stack_pop(p_window);
		}
		else
		{
			g_immersive_status = IMMERSIVE_STATE_INIT;
			g_layer_immersive_rest_show_status = true;

			param_init();
			window_reloading(p_window);
		}
	}
	
}


static P_Window immersive_init_window()
{
	P_Window p_window = NULL;

	/*创建窗口*/
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

/*--------添加图片图层------------*/
	/*添加黑色背景图片*/
	/*图片显示位置*/
	GRect frame_bmp = {{IMMERSIVE_BG_ORIGIN_X, IMMERSIVE_BG_ORIGIN_Y}, {IMMERSIVE_BG_SIZE_H, IMMERSIVE_BG_SIZE_W}};
	if(g_immersive_status == IMMERSIVE_STATE_INIT)
	{
		display_target_layer(p_window,&frame_bmp,GAlignLeft,bmp_array_name,10);
	}
	else if(g_immersive_status == IMMERSIVE_STATE_IMMERSIVE_TIME_OUT)
	{
		display_target_layer(p_window,&frame_bmp,GAlignLeft,bmp_array_name,15);
	}
	else if(g_immersive_status == IMMERSIVE_STATE_REST_TIME_OUT)
	{
		display_target_layer(p_window,&frame_bmp,GAlignLeft,bmp_array_name,16);
	}
	else
	{
		display_target_layer(p_window,&frame_bmp,GAlignLeft,bmp_array_name,11);

		/*--------显示倒计时-------*/
		
		frame_bmp.origin.x = 0;
		frame_bmp.origin.y = IMMERSIVE_COUNTDOWN_ORIGIN_Y;
		frame_bmp.size.h = IMMERSIVE_COUNTDOWN_SIZE_H;
		frame_bmp.size.w = IMMERSIVE_COUNTDOWN_SIZE_W;
		
		int i;
		for(i = (5-g_counttime_show_pic_number);i < 5;i++)
		{
			frame_bmp.origin.x = (128-g_counttime_show_pic_number*19)/2 + IMMERSIVE_COUNTDOWN_SIZE_W*(i-(5-g_counttime_show_pic_number));
			g_layer_id_array[i] = display_target_layer(p_window,&frame_bmp,GAlignLeft,bmp_array_name,g_countdown_array_param[i]);
		}
	
		
		/*--------显示时间-------*/
			
		char time_str_buffer[6] = {0};
		get_time_str(time_str_buffer);

		frame_bmp.origin.x = IMMERSIVE_TIME_ORIGIN_X;
		frame_bmp.origin.y = IMMERSIVE_TIME_ORIGIN_Y;
		frame_bmp.size.h = IMMERSIVE_TIME_SIZE_H;
		frame_bmp.size.w = IMMERSIVE_TIME_SIZE_W;

			
		g_layer_top_time_id = display_target_layerText(p_window,&frame_bmp,GAlignCenter,GColorBlack,time_str_buffer,U_ASCII_ARIAL_12);
			
		/*--------显示休息中、沉浸中-------*/
			
		frame_bmp.origin.x = IMMERSIVE_TIPS_ORIGIN_X;
		frame_bmp.origin.y = IMMERSIVE_TIPS_ORIGIN_Y;
		frame_bmp.size.h = IMMERSIVE_TIPS_SIZE_H;
		frame_bmp.size.w = IMMERSIVE_TIPS_SIZE_W;

		g_layer_immersive_rest_id = display_target_layer(p_window,&frame_bmp,GAlignCenter,bmp_array_name,(11 + g_immersive_status));

		show_or_hide(p_window);
	}
	
	/*添加窗口按键事件*/
	app_window_click_subscribe(p_window, ButtonIdDown, immersive_select_down);
	app_window_click_subscribe(p_window, ButtonIdUp, immersive_select_up);
	app_window_click_subscribe(p_window, ButtonIdSelect, immersive_select_select);
	app_window_click_subscribe(p_window, ButtonIdBack, immersive_select_back);


	return (p_window);

}

static int main(void)
{

	/*创建日期时间设置窗口*/
	P_Window p_window = immersive_init_window(); 

	/*放入窗口栈显示*/
	g_window_id = app_window_stack_push(p_window);


	return 0;

}

