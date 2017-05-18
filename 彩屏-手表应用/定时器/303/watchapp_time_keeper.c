/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  watchapp_timekeeper.c
 *
 *    Description:  计时器 
 *    Corporation:
 * 
 *         Author:  gliu (), gliu@damaijiankang.com
 *        Created:  2015年04月17日 10时03分03秒
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


/*设置顶部文字图层位置*/
#define TIMER_SET_TOP_TIME_ORIGIN_X		0	
#define TIMER_SET_TOP_TIME_ORIGIN_Y		4	
#define TIMER_SET_TOP_TIME_SIZE_H		16
#define TIMER_SET_TOP_TIME_SIZE_W		143

/*底层文字1提示图层位置*/
#define TIMER_HINT_TEXT1_ORIGIN_X	0	
#define TIMER_HINT_TEXT1_ORIGIN_Y	110
#define TIMER_HINT_TEXT1_SIZE_H		20
#define TIMER_HINT_TEXT1_SIZE_W		143

/*底层文字2提示图层位置*/
#define TIMER_HINT_TEXT2_ORIGIN_X	0	
#define TIMER_HINT_TEXT2_ORIGIN_Y	150
#define TIMER_HINT_TEXT2_SIZE_H		16
#define TIMER_HINT_TEXT2_SIZE_W		143


/*底层文字3提示图层位置*/
#define TIMER_HINT_TEXT3_ORIGIN_X	0	
#define TIMER_HINT_TEXT3_ORIGIN_Y	150
#define TIMER_HINT_TEXT3_SIZE_H		16
#define TIMER_HINT_TEXT3_SIZE_W		143



/*分钟设置最大值*/
#define TIMER_SET_MAX	61
#define TIMER_SET_MIN	0


/*当前倒计时时间, 秒*/
static int16_t g_timer_sec = -1;

/*当前设置的计时时间，分*/
static int16_t g_timer_pre_set = 5;

/*设置顶部时间ID*/
static int8_t g_timer_set_top_time_layer_id = -1;

/*设置数字图片ID*/
static int8_t g_timer_set_bmp_layer1_id = -1;
static int8_t g_timer_set_bmp_layer2_id = -1;

/*倒计时文本ID*/
static int8_t g_timer_count_bmp_layer1_id = -1;
static int8_t g_timer_count_bmp_layer2_id = -1;
static int8_t g_timer_count_bmp_layer3_id = -1;
static int8_t g_timer_count_bmp_layer4_id = -1;



/*窗口ID*/
static int32_t g_timer_window_id = -1;

/*定时器*/
static uint8_t g_timer_timer_id = -1;


P_Window timer_init_window();


//图片ID数组
int32_t bmp_array_name[] = {
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



/*创建并显示图片图层，需要坐标值，得到icon_key的数组，数组需要的参数值，P_Window*/
int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,enum GAlign how_to_align,enum GColor black_or_white,int32_t bmp_array_name_key)
{	
	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_array_name_key, &bmp_point);
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
int32_t display_target_layerText(P_Window p_window,GRect  *temp_p_frame,enum GAlign how_to_align,enum GColor color,char * str,uint8_t font_type,enum GColor fcolor)
{
	LayerText temp_LayerText = {0};
	temp_LayerText.text = str;
	temp_LayerText.frame = *temp_p_frame;
	temp_LayerText.alignment = how_to_align;
	temp_LayerText.font_type = font_type;
	temp_LayerText.foregroundColor = fcolor;
	
	P_Layer p_layer = app_layer_create_text(&temp_LayerText);
	
	if(p_layer != NULL)
	{
		app_layer_set_bg_color(p_layer, color);
		return app_window_add_layer(p_window, p_layer);
	}
	return 0;
}


static void layer_bmp_updata_handler(uint32_t bmp_key,P_Window p_window,int8_t layer_g_id)
{
	Layer * p_old_layer = app_window_get_layer_by_id(p_window, layer_g_id);
	if(NULL != p_old_layer)
	{
		GBitmap bitmap;
		res_get_user_bitmap(bmp_key, &bitmap);
		app_layer_set_bitmap_bitmap(p_old_layer, &bitmap);					
	}

}


void timer_callback(date_time_t tick_time, uint32_t millis, void *context)
{
	
	if (g_timer_sec <= 0)
	{
		return;
	}
	

	g_timer_sec--;


	/*根据窗口ID获取窗口句柄*/
	P_Window p_window = app_window_stack_get_window_by_id(g_timer_window_id);
	/*如果完成目标，系统提醒*/	
	if (g_timer_sec == 0)
	{
		NotifyParam	param;
		memset(&param, 0, sizeof(NotifyParam));
		
		res_get_user_bitmap(RES_BITMAP_TIMER_SYS_BELL,  &param.bmp);	
		sprintf(param.main_title, "%d分钟", g_timer_pre_set);
		strcpy(param.sub_title, "计时结束");
		param.pulse_type = VibesPulseTypeMiddle;		
		param.pulse_time = 6;		
		maibu_service_sys_notify(&param);
	
		/*标识系统已经提醒了，再次进入应用不需要提醒*/
		g_timer_sec = -1;		

		/*注销定时器*/
    	app_service_timer_unsubscribe(g_timer_timer_id);	

		//添加震动
		maibu_service_vibes_pulse(VibesPulseTypeMiddle, 6);
			
		//添加背光	
		os_lights_open(0);
	}
	//倒计时
	else 
	{
		if(g_timer_sec == 10*60-1)
		{
			window_reloading(); 	
		}
		else
		{
			if(g_timer_sec > 10*60)
			{
				layer_bmp_updata_handler(bmp_array_name[g_timer_sec/60/10],p_window,g_timer_count_bmp_layer1_id);
			}
			
			layer_bmp_updata_handler(bmp_array_name[g_timer_sec/60%10],p_window,g_timer_count_bmp_layer2_id);
			layer_bmp_updata_handler(bmp_array_name[g_timer_sec%60/10],p_window,g_timer_count_bmp_layer3_id);
			layer_bmp_updata_handler(bmp_array_name[g_timer_sec%60%10],p_window,g_timer_count_bmp_layer4_id);
		}		
		
		app_window_update(p_window);
		
	}	

}




//重新载入并刷新窗口所有图层
void window_reloading(void)
{
	/*根据窗口ID获取窗口句柄*/
	P_Window p_old_window = app_window_stack_get_window_by_id(g_timer_window_id); 
	if (NULL != p_old_window)
	{
		P_Window p_window = timer_init_window();
		if (NULL != p_window)
		{
			g_timer_window_id = app_window_stack_replace_window(p_old_window, p_window);
		}	
	}
	
}


//timer_select重复代码
void timer_select_up_and_down(void *context,uint8_t TIMER_SET_MAX_OR_MIN,int16_t g_timer_pre_set_NUMBER,int16_t plus_or_subtract_1)
{
	P_Window p_window = app_window_stack_get_window_by_id(g_timer_window_id);
	if (NULL != p_window)
	{
		/*设置*/
		if (g_timer_sec == -1)
		{	
			int16_t temp_timer_pre_set = g_timer_pre_set;
			g_timer_pre_set = g_timer_pre_set + plus_or_subtract_1;
			if (g_timer_pre_set ==  TIMER_SET_MAX_OR_MIN)
			{
				g_timer_pre_set = g_timer_pre_set_NUMBER;
			}
			/*根据图层ID获取图层句柄*/
			if((temp_timer_pre_set < 9)&&(g_timer_pre_set < 9))
			{
				layer_bmp_updata_handler(bmp_array_name[g_timer_pre_set],p_window,g_timer_set_bmp_layer2_id);
			}
			else if((temp_timer_pre_set >10)&&(g_timer_pre_set > 10))
			{
				layer_bmp_updata_handler(bmp_array_name[g_timer_pre_set/10],p_window,g_timer_set_bmp_layer1_id);
				layer_bmp_updata_handler(bmp_array_name[g_timer_pre_set%10],p_window,g_timer_set_bmp_layer2_id);
			}
			else
			{
				window_reloading();		
			}

			app_window_update(p_window);
			
		}
	}

}

/*定义向上按键事件*/
void timer_select_up(void *context)
{
	timer_select_up_and_down(context,TIMER_SET_MAX,1,1);
}


/*定义向下按键事件*/
void timer_select_down(void *context)
{
	timer_select_up_and_down(context,TIMER_SET_MIN,60,-1);
}


/*定义选择按键事件*/
void timer_select_select(void *context)
{

	P_Window p_window = app_window_stack_get_window_by_id(g_timer_window_id);
	if (NULL != p_window)
	{
		/*设置*/
		if (g_timer_sec == -1)
		{
			
			g_timer_sec = g_timer_pre_set * 60;
	
			/*创建新的窗口*/	
			P_Window p_new_window = timer_init_window(); 
			
			/*创建窗口级别定时器*/
			g_timer_timer_id = app_service_timer_subscribe(1000, timer_callback, (void*)p_window);	
			
			/*替换窗口*/
			g_timer_window_id = app_window_stack_replace_window(p_window, p_new_window);
			

		}
		else if (g_timer_sec != -1)//计时倒计时
		{

			g_timer_sec = -1;
		
			/*注销定时器*/
			app_service_timer_unsubscribe(g_timer_timer_id);	
			
			/*创建新的窗口*/	
			P_Window p_new_window = timer_init_window(); 
			
			/*替换窗口*/
			g_timer_window_id = app_window_stack_replace_window(p_window, p_new_window);

			
		}

	}
}


/*定义后退按键事件*/
void timer_select_back(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		app_window_stack_pop(p_window);
		
		g_timer_window_id = -1;

		/*如果倒计时结束，则设置为-1，下次进入后，显示选择分钟界面*/
		if (g_timer_sec == 0)
		{
			g_timer_sec = -1;
		}
	}
}


/*
 *--------------------------------------------------------------------------------------
 *     function:  timer_init_window
 *    parameter: 
 *       return:
 *  description:  初始化窗口
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Window timer_init_window()
{
	P_Window p_window = NULL;

	/*创建窗口*/
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

/*--------添加状态栏显示时间-------*/
	char text[17] = "";
	GRect frame_bmp = {0};
	

	
	/*根据状态创建不同界面*/
	if (g_timer_sec == -1)
	{
		//设置界面
		frame_bmp.origin.x = 0;
		frame_bmp.origin.y = 0;
		frame_bmp.size.h= 176;
		frame_bmp.size.w= 176;
		display_target_layer(p_window,&frame_bmp,GAlignCenter,GColorBlack,RES_BITMAP_WATCHAPP_COUNTER_BG1);

		//设置分钟
		frame_bmp.origin.y = 59;
		frame_bmp.size.h = 39;
		frame_bmp.size.w = 29;
		if(g_timer_pre_set >= 10)
		{
			frame_bmp.origin.x = (143-29*2-1)/2;
			g_timer_set_bmp_layer1_id = display_target_layer(p_window,&frame_bmp,GAlignCenter,GColorBlack,bmp_array_name[g_timer_pre_set/10]);
			frame_bmp.origin.x = frame_bmp.origin.x + frame_bmp.size.w;
		}
		else
		{
			frame_bmp.origin.x = (143-29*1)/2;
		}	
		
		g_timer_set_bmp_layer2_id = display_target_layer(p_window,&frame_bmp,GAlignCenter,GColorBlack,bmp_array_name[g_timer_pre_set%10]);		
		
		
		//文本提示
		frame_bmp.origin.x = TIMER_HINT_TEXT1_ORIGIN_X;
		frame_bmp.origin.y = TIMER_HINT_TEXT1_ORIGIN_Y;
		frame_bmp.size.h = TIMER_HINT_TEXT1_SIZE_H;
		frame_bmp.size.w = TIMER_HINT_TEXT1_SIZE_W;

		display_target_layerText(p_window,&frame_bmp,GAlignCenter,GColorWhite,"分钟",U_ASCII_ARIAL_20,GColorBlack);
		
		frame_bmp.origin.x = TIMER_HINT_TEXT2_ORIGIN_X;
		frame_bmp.origin.y = TIMER_HINT_TEXT2_ORIGIN_Y;
		frame_bmp.size.h = TIMER_HINT_TEXT2_SIZE_H;
		frame_bmp.size.w = TIMER_HINT_TEXT2_SIZE_W;
		
		display_target_layerText(p_window,&frame_bmp,GAlignCenter,GColorWhite,"请选择计时时长",U_ASCII_ARIAL_16,GColorBlack);
		
		
	}
	else if (g_timer_sec > 0)
	{
		//倒计时界面
		frame_bmp.origin.x = 0;
		frame_bmp.origin.y = 0;
		frame_bmp.size.h = 176;
		frame_bmp.size.w = 176;
		display_target_layer(p_window,&frame_bmp,GAlignCenter,GColorBlack,RES_BITMAP_WATCHAPP_COUNTER_BG2);

		//倒计时分钟
		frame_bmp.origin.y = 59;
		frame_bmp.size.h = 39;
		frame_bmp.size.w = 29;

		if(g_timer_sec >= 10*60)
		{
			frame_bmp.origin.x = (143-29*4-12-1)/2;
			g_timer_count_bmp_layer1_id = display_target_layer(p_window,&frame_bmp,GAlignCenter,GColorBlack,bmp_array_name[g_timer_sec/60/10]);
			frame_bmp.origin.x = frame_bmp.origin.x + frame_bmp.size.w;
		}
		else
		{
			frame_bmp.origin.x = (143-29*3-12)/2;
		}
		
		g_timer_count_bmp_layer2_id = display_target_layer(p_window,&frame_bmp,GAlignCenter,GColorBlack,bmp_array_name[g_timer_sec/60%10]);
		frame_bmp.origin.x = frame_bmp.origin.x + frame_bmp.size.w;
		
		display_target_layer(p_window,&frame_bmp,GAlignLeft,GColorBlack,RES_BITMAP_WATCHFACE_MAOHAO);
		frame_bmp.origin.x = frame_bmp.origin.x + 12;

		g_timer_count_bmp_layer3_id = display_target_layer(p_window,&frame_bmp,GAlignCenter,GColorBlack,bmp_array_name[g_timer_sec%60/10]);
		frame_bmp.origin.x = frame_bmp.origin.x + frame_bmp.size.w;

		g_timer_count_bmp_layer4_id = display_target_layer(p_window,&frame_bmp,GAlignCenter,GColorBlack,bmp_array_name[g_timer_sec%60%10]);
		frame_bmp.origin.x = frame_bmp.origin.x + frame_bmp.size.w;

		//文本提示
		frame_bmp.origin.x = TIMER_HINT_TEXT3_ORIGIN_X;
		frame_bmp.origin.y = TIMER_HINT_TEXT3_ORIGIN_Y;
		frame_bmp.size.h = TIMER_HINT_TEXT3_SIZE_H;
		frame_bmp.size.w = TIMER_HINT_TEXT3_SIZE_W;

		memset(text, 0, sizeof(text));
		sprintf(text, "计时%d分钟", g_timer_pre_set);
		
		display_target_layerText(p_window,&frame_bmp,GAlignCenter,GColorWhite,text,U_ASCII_ARIAL_16,GColorBlack);

	}

	
	struct date_time dt;
	app_service_get_datetime(&dt);	
	
	
	
	/*添加状态栏, 显示时间*/
	frame_bmp.origin.x = TIMER_SET_TOP_TIME_ORIGIN_X;
	frame_bmp.origin.y = TIMER_SET_TOP_TIME_ORIGIN_Y;
	frame_bmp.size.h= TIMER_SET_TOP_TIME_SIZE_H;
	frame_bmp.size.w= TIMER_SET_TOP_TIME_SIZE_W;

	sprintf(text, "%02d:%02d",dt.hour,dt.min);
	g_timer_set_top_time_layer_id = display_target_layerText(p_window,&frame_bmp,GAlignCenter,GColorWhite,text,U_ASCII_ARIAL_16,GColorBlack);
	


	/*添加窗口按键事件*/
	app_window_click_subscribe(p_window, ButtonIdDown, timer_select_down);
	app_window_click_subscribe(p_window, ButtonIdUp, timer_select_up);
	app_window_click_subscribe(p_window, ButtonIdSelect, timer_select_select);
	app_window_click_subscribe(p_window, ButtonIdBack, timer_select_back);
	return (p_window);

}

void  time_change (enum SysEventType type, void *context)
{
	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
	
		/*根据窗口ID获取窗口句柄*/
		P_Window p_window = app_window_stack_get_window_by_id(g_timer_window_id);
		
		P_Layer p_layer = app_window_get_layer_by_id(p_window, g_timer_set_top_time_layer_id);
		if (p_layer != NULL)
		{
			struct date_time dt;
			app_service_get_datetime(&dt);	

			char text[8] = "";
			sprintf(text, "%02d:%02d",dt.hour,dt.min);

			/*设置文本图层新的文本内容并更新显示*/	
			app_layer_set_text_text(p_layer, text);		
			app_window_update(p_window);
		}
	}
	
}

int main()
{

	/*APP编写*/
	/*创建日期时间设置窗口*/
	P_Window p_window = timer_init_window(); 

	/*放入窗口栈显示*/
	g_timer_window_id = app_window_stack_push(p_window);
	/*注册一个事件通知回调，当有改变时，改变表盘显示数据*/
    maibu_service_sys_event_subscribe(time_change);

	return 0;

}

