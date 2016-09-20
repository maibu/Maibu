/*
 * =====================================================================================
 * 
 *    Corporation:  Shenzhen Maibu Technology Co., Ltd. All Rights Reserved.
 *       Filename:  app_run_helper.c
 *         Author:  gliu , gliu@maibu.cc
 *        Created:  2015年10月13日 09时58分07秒
 * 
 *    Description:  跑步助手
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


/*首页图层位置*/
#define RUN_HELPER_INTRO_ORIGIN_X			0	
#define RUN_HELPER_INTRO_ORIGIN_Y			0
#define RUN_HELPER_INTRO_SIZE_H				128	
#define RUN_HELPER_INTRO_SIZE_W				128	


/*中间状态栏图层位置*/
#define RUN_HELPER_STATUS_BAR_ORIGIN_X			0
#define RUN_HELPER_STATUS_BAR_ORIGIN_Y			0
#define RUN_HELPER_STATUS_BAR_SIZE_H			12	
#define RUN_HELPER_STATUS_BAR_SIZE_W			104


/*标签图层位置*/
#define RUN_HELPER_LABEL_ORIGIN_X			2
#define RUN_HELPER_LABEL_ORIGIN_Y			29
#define RUN_HELPER_LABEL_SIZE_H				85	
#define RUN_HELPER_LABEL_SIZE_W				19

/*侧边栏背景图层位置*/
#define RUN_HELPER_SIDE_BG_ORIGIN_X			104
#define RUN_HELPER_SIDE_BG_ORIGIN_Y			0
#define RUN_HELPER_SIDE_BG_SIZE_H			128
#define RUN_HELPER_SIDE_BG_SIZE_W			24

/*侧边栏开始暂停图层位置*/
#define RUN_HELPER_SIDE_PLAY_PAUSE_ORIGIN_X			108
#define RUN_HELPER_SIDE_PLAY_PAUSE_ORIGIN_Y			12
#define RUN_HELPER_SIDE_PLAY_PAUSE_SIZE_H			16
#define RUN_HELPER_SIDE_PLAY_PAUSE_SIZE_W			16


/*距离图层*/
#define RUN_HELPER_DISTANCE_ORIGIN_X			21
#define RUN_HELPER_DISTANCE_ORIGIN_Y			19
#define RUN_HELPER_DISTANCE_SIZE_H			30
#define RUN_HELPER_DISTANCE_SIZE_W			82

/*计时图层*/
#define RUN_HELPER_TIME_ORIGIN_X			21
#define RUN_HELPER_TIME_ORIGIN_Y			56
#define RUN_HELPER_TIME_SIZE_H				30	
#define RUN_HELPER_TIME_SIZE_W				82

/*配速图层*/
#define RUN_HELPER_SPEED_ORIGIN_X			21
#define RUN_HELPER_SPEED_ORIGIN_Y			93
#define RUN_HELPER_SPEED_SIZE_H				30
#define RUN_HELPER_SPEED_SIZE_W				82

/*运动结束背景图层*/
#define RUN_HELPER_FINISH_ORIGIN_X			0
#define RUN_HELPER_FINISH_ORIGIN_Y			0
#define RUN_HELPER_FINISH_SIZE_H			128
#define RUN_HELPER_FINISH_SIZE_W			128

/*运动结束年月日图层*/
#define RUN_HELPER_STATUS_BAR_YMD_ORIGIN_X			1
#define RUN_HELPER_STATUS_BAR_YMD_ORIGIN_Y			2
#define RUN_HELPER_STATUS_BAR_YMD_SIZE_H			12
#define RUN_HELPER_STATUS_BAR_YMD_SIZE_W			78

/*运动结束时间图层*/
#define RUN_HELPER_STATUS_BAR_TIME_ORIGIN_X			91
#define RUN_HELPER_STATUS_BAR_TIME_ORIGIN_Y			2
#define RUN_HELPER_STATUS_BAR_TIME_SIZE_H			12
#define RUN_HELPER_STATUS_BAR_TIME_SIZE_W			36

/*运动结束总距离图层*/
#define RUN_HELPER_FINISH_DISTANCE_ORIGIN_X			2
#define RUN_HELPER_FINISH_DISTANCE_ORIGIN_Y			68
#define RUN_HELPER_FINISH_DISTANCE_SIZE_H			24
#define RUN_HELPER_FINISH_DISTANCE_SIZE_W			68


/*运动结束热量图层*/
#define RUN_HELPER_FINISH_CALORIE_ORIGIN_X			70
#define RUN_HELPER_FINISH_CALORIE_ORIGIN_Y			68
#define RUN_HELPER_FINISH_CALORIE_SIZE_H			24
#define RUN_HELPER_FINISH_CALORIE_SIZE_W			56


/*运动结束平均配速图层*/
#define RUN_HELPER_FINISH_AVERAGE_SPEED_ORIGIN_X			2
#define RUN_HELPER_FINISH_AVERAGE_SPEED_ORIGIN_Y			110
#define RUN_HELPER_FINISH_AVERAGE_SPEED_SIZE_H			16
#define RUN_HELPER_FINISH_AVERAGE_SPEED_SIZE_W			68


/*运动结束最高配速图层*/
#define RUN_HELPER_FINISH_MAX_SPEED_ORIGIN_X			70
#define RUN_HELPER_FINISH_MAX_SPEED_ORIGIN_Y			110
#define RUN_HELPER_FINISH_MAX_SPEED_SIZE_H			16
#define RUN_HELPER_FINISH_MAX_SPEED_SIZE_W			56


/*状态*/
//#define RUN_HELPER_STATE_INTRO		0
#define RUN_HELPER_STATE_PLAY		1
#define RUN_HELPER_STATE_PAUSE		0
#define RUN_HELPER_STATE_FINISH		3
//界面刷新时间
#define RUN_HELPER_REFRESH_TIME     5000

typedef struct tag_Run
{
	int8_t   state;		//状态，引导状态，计数状态、暂停状态、完成状态
	int8_t 	 flag;		//开启定时器标志
	uint32_t sys_micro;	//现在毫秒数
	int32_t  time;		//跑步计数,单位毫秒
	int32_t  total_distance;	//总距离，单位cm
	int16_t  curr_speed;	//实时速度，cm/s
	int16_t  max_speed;	//最高速度，cm/s
	uint32_t total_calorie;	//卡路里
	uint32_t pre_calorie;	//前一秒的卡路里
}Run;

static Run g_run;


/*窗口ID*/
static int32_t g_run_window_id = -1;

/*状态栏时间图层ID*/
static int8_t g_run_status_bar_layer_id = -1;

/*距离图层ID*/
static int8_t g_run_distance_layer_id = -1;

/*计时图层ID*/
static int8_t g_run_time_layer_id = -1;

/*配速图层ID*/
static int8_t g_run_speed_layer_id = -1;

/*开始暂停图层*/
static int8_t g_run_play_pause_layer_id = -1;


/*定时器ID*/
static int8_t g_run_timer_id = -1;
//RES_BITMAP_WATCHAPP_RUN_HELPER_LABEL_SIDE_BG_BMP
//RES_BITMAP_WATCHAPP_RUN_HELPER_LABEL_BMP
//图片ID数组
int32_t bmp_array_name[] = {
	RES_BITMAP_WATCHAPP_RUN_HELPER_INTRO_BMP,
	RES_BITMAP_WATCHAPP_RUN_HELPER_FINISH_BMP,
	RES_BITMAP_WATCHAPP_RUN_HELPER_LABEL_SIDE_BG_BMP,
	RES_BITMAP_WATCHAPP_RUN_HELPER_SIDE_BG_BMP,
	RES_BITMAP_WATCHAPP_RUN_HELPER_SIDE_PAUSE_BMP,
	RES_BITMAP_WATCHAPP_RUN_HELPER_SIDE_PLAY_BMP
};


extern P_Window run_helper_create_init_window();
extern P_Window run_helper_create_finish_window();
extern P_Window run_helper_create_record_window();
//extern P_Window run_helper_create_intro_window();
extern void run_helper_timer_callback(date_time_t tick_time, uint32_t millis, void *context);


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
	static LayerText temp_LayerText = {0};
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

/*定义后退按键事件*/
static void run_helper_finish_select_back(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		memset(&g_run, 0, sizeof(Run));
		app_window_stack_pop(p_window);
	}
}


/*定义后退按键事件*/
static void run_helper_select_back(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{	
		app_window_stack_pop(p_window);
	}
}


//省代码空间重复函数
static void run_helper_select_repeat(P_Window p_window)
{
	g_run.state = RUN_HELPER_STATE_PLAY;
	g_run.flag = 1;
			
	SportData data;
	maibu_get_sport_data(&data, 0);		
			
	g_run.pre_calorie = data.calorie;
	g_run_timer_id = app_service_timer_subscribe(RUN_HELPER_REFRESH_TIME, run_helper_timer_callback, (void *)p_window);

}


/*定义上按键事件*/
static void run_helper_select_up(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		GBitmap bmp;
		P_Layer p_layer = app_window_get_layer_by_id(p_window, g_run_play_pause_layer_id);
		if (g_run.state == RUN_HELPER_STATE_PLAY)
		{
			res_get_user_bitmap(RES_BITMAP_WATCHAPP_RUN_HELPER_SIDE_PLAY_BMP, &bmp);
			g_run.state = RUN_HELPER_STATE_PAUSE;
			app_service_timer_unsubscribe(g_run_timer_id);
			g_run_timer_id = -1;

		}
		else
		{
			res_get_user_bitmap(RES_BITMAP_WATCHAPP_RUN_HELPER_SIDE_PAUSE_BMP, &bmp);
			run_helper_select_repeat(p_window);

		}
		app_layer_set_bitmap_bitmap(p_layer, &bmp);
		app_window_update(p_window);	
	}
}


/*定义下按键事件*/
static void run_helper_select_down(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		if(g_run_timer_id != -1)	
		{
			app_service_timer_unsubscribe(g_run_timer_id);
			g_run_timer_id = -1;
		}
			
		g_run.state = RUN_HELPER_STATE_FINISH;
		P_Window p_new_window = run_helper_create_finish_window();
		g_run_window_id = app_window_stack_replace_window(p_window, p_new_window);	
	}
}


#if 0
static void run_helper_select_select(void *context)
{

	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		P_Window p_new_window = run_helper_create_record_window();

		run_helper_select_repeat(p_new_window);

		g_run_window_id = app_window_stack_replace_window(p_window, p_new_window);
		
	}
}
#endif

static void run_helper_finish_select_select(void *context)
{

	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		memset(&g_run, 0, sizeof(Run));
		P_Window p_new_window = run_helper_create_record_window();
		g_run_window_id = app_window_stack_replace_window(p_window, p_new_window);	
	}
}


/*
 *--------------------------------------------------------------------------------------
 *     function:  run_helper_create_record_window
 *    parameter: 
 *       return:
 *  description:  创建记录窗口
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Window run_helper_create_record_window()
{

	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	GRect create_temp_frame;

#if 0

	/*添加标签*/
	create_temp_frame.origin.x = RUN_HELPER_LABEL_ORIGIN_X;
	create_temp_frame.origin.y = RUN_HELPER_LABEL_ORIGIN_Y; 
	create_temp_frame.size.h = RUN_HELPER_LABEL_SIZE_H; 
	create_temp_frame.size.w = RUN_HELPER_LABEL_SIZE_W;

	display_target_layer(p_window,&create_temp_frame,GAlignCenter,GColorWhite,bmp_array_name,2);



	/*添加侧边栏图层*/
	create_temp_frame.origin.x = RUN_HELPER_SIDE_BG_ORIGIN_X;
	create_temp_frame.origin.y = RUN_HELPER_SIDE_BG_ORIGIN_Y; 
	create_temp_frame.size.h = RUN_HELPER_SIDE_BG_SIZE_H; 
	create_temp_frame.size.w = RUN_HELPER_SIDE_BG_SIZE_W;
	
	display_target_layer(p_window,&create_temp_frame,GAlignCenter,GColorWhite,bmp_array_name,3);
#else
	
	/*添加标签侧边栏*/
	create_temp_frame.origin.x = 0;
	create_temp_frame.origin.y = 0; 
	create_temp_frame.size.h = 128; 
	create_temp_frame.size.w = 128;

	display_target_layer(p_window,&create_temp_frame,GAlignCenter,GColorWhite,bmp_array_name,2);
	
#endif	

	/*添加状态栏图层*/
	create_temp_frame.origin.x = RUN_HELPER_STATUS_BAR_ORIGIN_X;
	create_temp_frame.origin.y = RUN_HELPER_STATUS_BAR_ORIGIN_Y; 
	create_temp_frame.size.h = RUN_HELPER_STATUS_BAR_SIZE_H; 
	create_temp_frame.size.w = RUN_HELPER_STATUS_BAR_SIZE_W;
	
	struct date_time datetime;
	app_service_get_datetime(&datetime);
	char buf[6] = "";
	sprintf(buf, "%02d:%02d", datetime.hour, datetime.min);

	g_run_status_bar_layer_id = display_target_layerText(p_window,&create_temp_frame,GAlignCenter,GColorWhite,buf,U_ASCII_ARIAL_12);


	/*添加开始或暂停图层*/
	create_temp_frame.origin.x = RUN_HELPER_SIDE_PLAY_PAUSE_ORIGIN_X;
	create_temp_frame.origin.y = RUN_HELPER_SIDE_PLAY_PAUSE_ORIGIN_Y; 
	create_temp_frame.size.h = RUN_HELPER_SIDE_PLAY_PAUSE_SIZE_H; 
	create_temp_frame.size.w = RUN_HELPER_SIDE_PLAY_PAUSE_SIZE_W;
	
	if ((g_run.time == 0) || (g_run.state == RUN_HELPER_STATE_PAUSE))
	{
		g_run_play_pause_layer_id = display_target_layer(p_window,&create_temp_frame,GAlignCenter,GColorWhite,bmp_array_name,5);
	}
	else 
	{
		g_run_play_pause_layer_id = display_target_layer(p_window,&create_temp_frame,GAlignCenter,GColorWhite,bmp_array_name,4);
	}
	

	/*添加距离文本图层*/
	create_temp_frame.origin.x = RUN_HELPER_DISTANCE_ORIGIN_X;
	create_temp_frame.origin.y = RUN_HELPER_DISTANCE_ORIGIN_Y; 
	create_temp_frame.size.h = RUN_HELPER_DISTANCE_SIZE_H; 
	create_temp_frame.size.w = RUN_HELPER_DISTANCE_SIZE_W;
	
	sprintf(buf, "%01d.%02d", g_run.total_distance/100000, (g_run.total_distance/1000)%100);

	g_run_distance_layer_id = display_target_layerText(p_window,&create_temp_frame,GAlignRight,GColorWhite,buf,U_ASCII_ARIALBD_30);


	/*添加计时文本图层*/

	create_temp_frame.origin.x = RUN_HELPER_TIME_ORIGIN_X;
	create_temp_frame.origin.y = RUN_HELPER_TIME_ORIGIN_Y; 
	create_temp_frame.size.h = RUN_HELPER_TIME_SIZE_H; 
	create_temp_frame.size.w = RUN_HELPER_TIME_SIZE_W;
	
	if ((g_run.time/1000) >= 3600)
	{
		sprintf(buf, "%01d:%02d",  g_run.time/3600000, (g_run.time/60000)%60);
	}
	else
	{
		sprintf(buf, "%01d:%02d",  g_run.time/60000, (g_run.time/1000)%60);
	}

	g_run_time_layer_id = display_target_layerText(p_window,&create_temp_frame,GAlignRight,GColorWhite,buf,U_ASCII_ARIALBD_30);

	
	/*添加配速文本图层*/
	create_temp_frame.origin.x = RUN_HELPER_SPEED_ORIGIN_X;
	create_temp_frame.origin.y = RUN_HELPER_SPEED_ORIGIN_Y; 
	create_temp_frame.size.h = RUN_HELPER_SPEED_SIZE_H; 
	create_temp_frame.size.w = RUN_HELPER_SPEED_SIZE_W;
	
	SpeedInfo realt1_speed;
	maibu_get_speed_info(&realt1_speed);

	int32_t tmp = g_run.curr_speed == 0 ? 1 : g_run.curr_speed;
	int32_t hour_speed = (100000/tmp);
	if ((hour_speed >= 6000) || (realt1_speed.interval > 5))
	{
		strcpy(buf, "0:00");
	}
	else
	{
		sprintf(buf, "%01d:%02d", hour_speed/60, hour_speed%60);
	}

	g_run_speed_layer_id = display_target_layerText(p_window,&create_temp_frame,GAlignRight,GColorWhite,buf,U_ASCII_ARIALBD_30);


	/*这里先注册按键回调, 在回调中再具体处理回调怎么相应*/
	app_window_click_subscribe(p_window, ButtonIdBack, run_helper_select_back);
	app_window_click_subscribe(p_window, ButtonIdUp, run_helper_select_up);
	app_window_click_subscribe(p_window, ButtonIdDown, run_helper_select_down);

	return p_window;
}




/*
 *--------------------------------------------------------------------------------------
 *     function:  run_helper_create_intro_window
 *    parameter: 
 *       return:
 *  description:  创建引导窗口
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
#if 0
P_Window run_helper_create_intro_window()
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}
	
		
	/*添加引导图层*/
	GRect intro_frame = {{RUN_HELPER_INTRO_ORIGIN_X, RUN_HELPER_INTRO_ORIGIN_Y}, 
				{RUN_HELPER_INTRO_SIZE_H, RUN_HELPER_INTRO_SIZE_W}};
	
	display_target_layer(p_window,&intro_frame,GAlignCenter,GColorWhite,bmp_array_name,0);
	
	app_window_click_subscribe(p_window, ButtonIdSelect, run_helper_select_select);
	app_window_click_subscribe(p_window, ButtonIdBack, run_helper_select_back);
	return p_window;
}
#endif


/*
 *--------------------------------------------------------------------------------------
 *     function:  run_helper_create_finish_window
 *    parameter: 
 *       return:
 *  description:  创建完成窗口
 * 	  other:
 *--------------------------------------------------------------------------------------
 */

//省代码空间重复函数
static void run_helper_create_finish_repeat(int32_t speed,char *buf)
{
	if (speed >= 6000)
	{
		strcpy(buf, "0:00");	
	}
	else
	{
		sprintf(buf, "%01d:%02d", speed/60, speed%60);
	}

}



P_Window run_helper_create_finish_window()
{

	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	/*添加背景图层*/
	GRect finish_temp_frame = {{RUN_HELPER_FINISH_ORIGIN_X, RUN_HELPER_FINISH_ORIGIN_Y}, 
				{RUN_HELPER_FINISH_SIZE_H, RUN_HELPER_FINISH_SIZE_W}};
	display_target_layer(p_window,&finish_temp_frame,GAlignCenter,GColorWhite,bmp_array_name,1);


	/*添加状态栏, 显示年月日*/
	finish_temp_frame.origin.x = RUN_HELPER_STATUS_BAR_YMD_ORIGIN_X;
	finish_temp_frame.origin.y = RUN_HELPER_STATUS_BAR_YMD_ORIGIN_Y;
	finish_temp_frame.size.h = RUN_HELPER_STATUS_BAR_YMD_SIZE_H;
	finish_temp_frame.size.w = RUN_HELPER_STATUS_BAR_YMD_SIZE_W;

	struct date_time datetime;
	app_service_get_datetime(&datetime);
	char buf[8] = "";
	
	sprintf(buf, "%d-%02d-%02d", datetime.year,datetime.mon,datetime.mday);

	display_target_layerText(p_window,&finish_temp_frame,GAlignLeft,GColorBlack,buf,U_ASCII_ARIAL_12);

	/*添加状态栏, 显示时间*/
	finish_temp_frame.origin.x = RUN_HELPER_STATUS_BAR_TIME_ORIGIN_X;
	finish_temp_frame.origin.y = RUN_HELPER_STATUS_BAR_TIME_ORIGIN_Y;
	finish_temp_frame.size.h = RUN_HELPER_STATUS_BAR_TIME_SIZE_H;
	finish_temp_frame.size.w = RUN_HELPER_STATUS_BAR_TIME_SIZE_W;
		
	sprintf(buf, "%02d:%02d", datetime.hour, datetime.min);

	display_target_layerText(p_window,&finish_temp_frame,GAlignRight,GColorBlack,buf,U_ASCII_ARIAL_12);



	/*添加总距离*/
	finish_temp_frame.origin.x = RUN_HELPER_FINISH_DISTANCE_ORIGIN_X;
	finish_temp_frame.origin.y = RUN_HELPER_FINISH_DISTANCE_ORIGIN_Y;
	finish_temp_frame.size.h = RUN_HELPER_FINISH_DISTANCE_SIZE_H;
	finish_temp_frame.size.w = RUN_HELPER_FINISH_DISTANCE_SIZE_W;
	
	
	sprintf(buf, "%01d.%02d", g_run.total_distance/100000, (g_run.total_distance/1000)%100);
	
	display_target_layerText(p_window,&finish_temp_frame,GAlignLeft,GColorWhite,buf,U_ASCII_ARIALBD_24);
	

	/*添加热量消耗*/
	
	finish_temp_frame.origin.x = RUN_HELPER_FINISH_CALORIE_ORIGIN_X;
	finish_temp_frame.origin.y = RUN_HELPER_FINISH_CALORIE_ORIGIN_Y;
	finish_temp_frame.size.h = RUN_HELPER_FINISH_CALORIE_SIZE_H;
	finish_temp_frame.size.w = RUN_HELPER_FINISH_CALORIE_SIZE_W;
	

	sprintf(buf, "%d", g_run.total_calorie);
	display_target_layerText(p_window,&finish_temp_frame,GAlignLeft,GColorWhite,buf,U_ASCII_ARIALBD_24);


	/*添加平均配速*/
	finish_temp_frame.origin.x = RUN_HELPER_FINISH_AVERAGE_SPEED_ORIGIN_X;
	finish_temp_frame.origin.y = RUN_HELPER_FINISH_AVERAGE_SPEED_ORIGIN_Y;
	finish_temp_frame.size.h = RUN_HELPER_FINISH_AVERAGE_SPEED_SIZE_H;
	finish_temp_frame.size.w = RUN_HELPER_FINISH_AVERAGE_SPEED_SIZE_W;
	

	int32_t tmp = g_run.time/1000;
	tmp = (tmp == 0) ? 1 : tmp;	
	int32_t speed = g_run.total_distance / tmp;
	speed = (speed == 0) ?  1 : speed;			

	run_helper_create_finish_repeat(100000/speed,buf);

	display_target_layerText(p_window,&finish_temp_frame,GAlignLeft,GColorWhite,buf,U_ASCII_ARIALBD_16);
	

	/*添加最高配速*/
	finish_temp_frame.origin.x = RUN_HELPER_FINISH_MAX_SPEED_ORIGIN_X;
	finish_temp_frame.origin.y = RUN_HELPER_FINISH_MAX_SPEED_ORIGIN_Y;
	finish_temp_frame.size.h = RUN_HELPER_FINISH_MAX_SPEED_SIZE_H;
	finish_temp_frame.size.w = RUN_HELPER_FINISH_MAX_SPEED_SIZE_W;
	

	tmp = g_run.max_speed == 0 ? 1 : g_run.max_speed;
	run_helper_create_finish_repeat(100000/tmp,buf);
	
	display_target_layerText(p_window,&finish_temp_frame,GAlignLeft,GColorWhite,buf,U_ASCII_ARIALBD_16);

	app_window_click_subscribe(p_window, ButtonIdBack, run_helper_finish_select_back);
	app_window_click_subscribe(p_window, ButtonIdSelect, run_helper_finish_select_select);

	return p_window;
}


void run_helper_timer_callback(date_time_t tick_time, uint32_t millis, void *context)
{
	static int8_t pre_min = 0;


	if (g_run.flag == 1)
	{
		g_run.sys_micro = millis - 1000;
		g_run.flag = 0;
	}
	g_run.time += (millis - g_run.sys_micro);
	g_run.sys_micro = millis;


	/*更新距离、实时配速、卡路里*/
	SportData data;
	maibu_get_sport_data(&data, 0);

	SpeedInfo realt_speed;
	maibu_get_speed_info(&realt_speed);
	
	/*由于每天凌晨，系统会保存前一天的运动数据，并清零。所有这里需要特殊处理*/
	int16_t cur_calorie = data.calorie - g_run.pre_calorie;
	cur_calorie = cur_calorie < 0 ? 0 : cur_calorie;
	g_run.pre_calorie = data.calorie;
	
	//获取实时速度cm/s
	if(realt_speed.flag == 1)
	{
		g_run.curr_speed = realt_speed.real_time_speed;
	}
	else
	{
		g_run.curr_speed = 0;
	}
	g_run.curr_speed  = g_run.curr_speed  < 0 ? 0 : g_run.curr_speed;
	
	//更新最快速度cm/s
	g_run.max_speed = g_run.curr_speed > g_run.max_speed ? g_run.curr_speed : g_run.max_speed;
	//更新总距离，总卡路里
	if(realt_speed.flag == 1)
	{
		g_run.total_distance += g_run.curr_speed;
	}
	g_run.total_calorie += cur_calorie;

	 /*根据窗口ID获取窗口句柄*/
	P_Window p_window = app_window_stack_get_window_by_id(g_run_window_id);
	if (NULL == p_window)
	{
		return ;
	}

		
	/*更新时间*/
	char str[20] = "";
	char tmp[10] = "";
	struct date_time datetime;
	app_service_get_datetime(&datetime);
	if (pre_min != datetime.min)
	{
		pre_min = datetime.min;
		P_Layer p_hm_layer = app_window_get_layer_by_id(p_window, g_run_status_bar_layer_id);
		if (NULL == p_hm_layer)
		{
			return;
		}

		sprintf(str, "%02d:%02d", datetime.hour, datetime.min);
		app_layer_set_text_text(p_hm_layer, str);
	}

	/*更新距离显示*/
	P_Layer p_distance_layer = app_window_get_layer_by_id(p_window, g_run_distance_layer_id);
	if (NULL != p_distance_layer)
	{
		memset(tmp, 0, sizeof(tmp));
		maibu_layer_get_text_text(p_distance_layer, tmp, sizeof(tmp));
		sprintf(str, "%01d.%02d", g_run.total_distance/100000, ((g_run.total_distance/1000)%100));
		//sprintf(str, "%d", cur_speed);;	
		if (0  != strcmp(tmp, str))
		{
			app_layer_set_text_text(p_distance_layer, str);
		}
	}

	/*更新计时*/
	P_Layer p_time_layer = app_window_get_layer_by_id(p_window, g_run_time_layer_id);
	if (NULL != p_time_layer)
	{
		memset(tmp, 0, sizeof(tmp));
		maibu_layer_get_text_text(p_time_layer, tmp, sizeof(tmp));
		if ((g_run.time/1000) >= 3600)
		{
			sprintf(str, "%01d:%02d",  g_run.time/3600000, (g_run.time/60000)%60);
		}
		else
		{
			sprintf(str, "%01d:%02d",  g_run.time/60000, (g_run.time/1000)%60);
		}
		if (0  != strcmp(tmp, str))
		{
			app_layer_set_text_text(p_time_layer, str);
		}
	}
	
	/*更新配速显示*/
	P_Layer p_speed_layer = app_window_get_layer_by_id(p_window, g_run_speed_layer_id);
	if (NULL != p_speed_layer)
	{
		memset(tmp, 0, sizeof(tmp));
		maibu_layer_get_text_text(p_speed_layer, tmp, sizeof(tmp));
	
		int32_t hour_speed = g_run.curr_speed == 0 ? 1 : g_run.curr_speed;	
		hour_speed = (100000/hour_speed);
		
		if ((hour_speed >= 6000) || (realt_speed.interval > 10000))
		{
			strcpy(str, "0:00");
		}
		else
		{
			sprintf(str, "%01d:%02d", hour_speed/60, hour_speed%60);
		}
		if (0  != strcmp(tmp, str))
		{
			app_layer_set_text_text(p_speed_layer, str);
		}
	}
	
	
	app_window_update(p_window);

}


/*
 *--------------------------------------------------------------------------------------
 *     function:  run_helper_create_init_window
 *    parameter: 
 *       return:
 *  description:  创建初始化窗口
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Window run_helper_create_init_window()
{
	P_Window p_window = NULL;

	/*根据不同状态显示不同窗口*/
	if(g_run.state == RUN_HELPER_STATE_FINISH)
	{
		p_window = run_helper_create_finish_window();	
	}
	else
	{
		p_window = run_helper_create_record_window();

	}

	return p_window;
}


int main()
{
	P_Window p_window = run_helper_create_init_window(); 

	/*放入窗口栈显示*/
	g_run_window_id = app_window_stack_push(p_window);

	return 0;

}

