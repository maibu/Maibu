/*
 * =====================================================================================
 * 
 *    Corporation:  Shenzhen Maibu Technology Co., Ltd. All Rights Reserved.
 *       Filename:  app_stopwatch.c
 *         Author:  gliu , gliu@maibu.cc
 *        Created:  2015年09月16日 09时58分07秒
 * 
 *    Description:  秒表
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


/*时间图层位置*/
#define STOPWATCH_TIME_ORIGIN_X			0	
#define STOPWATCH_TIME_ORIGIN_Y			0
#define STOPWATCH_TIME_SIZE_H			16	
#define STOPWATCH_TIME_SIZE_W			104	


/*上面分秒图层位置*/
#define STOPWATCH_TOP_MIN_SEC_ORIGIN_X			1	
#define STOPWATCH_TOP_MIN_SEC_ORIGIN_Y			16
#define STOPWATCH_TOP_MIN_SEC_SIZE_H			30	
#define STOPWATCH_TOP_MIN_SEC_SIZE_W			82	


/*上面毫秒图层位置*/
#define STOPWATCH_TOP_MICRO_ORIGIN_X		81	
#define STOPWATCH_TOP_MICRO_ORIGIN_Y		18
#define STOPWATCH_TOP_MICRO_SIZE_H			16	
#define STOPWATCH_TOP_MICRO_SIZE_W			23	


/*侧边栏图层位置*/
#define STOPWATCH_SIDE_BG_ORIGIN_X		104	
#define STOPWATCH_SIDE_BG_ORIGIN_Y		0
#define STOPWATCH_SIDE_BG_SIZE_H			128	
#define STOPWATCH_SIDE_BG_SIZE_W			24	

/*开始/暂停图层位置*/
#define STOPWATCH_SIDE_PLAY_ORIGIN_X		108	
#define STOPWATCH_SIDE_PLAY_ORIGIN_Y		56
#define STOPWATCH_SIDE_PLAY_SIZE_H			16
#define STOPWATCH_SIDE_PLAY_SIZE_W			16	


/*第一个记录图层位置*/
#define STOPWATCH_RECORD1_ORIGIN_X		3	
#define STOPWATCH_RECORD1_ORIGIN_Y		46
#define STOPWATCH_RECORD1_SIZE_H			16	
#define STOPWATCH_RECORD1_SIZE_W			101	


/*第二个记录图层位置*/
#define STOPWATCH_RECORD2_ORIGIN_X		3	
#define STOPWATCH_RECORD2_ORIGIN_Y		62
#define STOPWATCH_RECORD2_SIZE_H			16	
#define STOPWATCH_RECORD2_SIZE_W			101	

/*第三个记录图层位置*/
#define STOPWATCH_RECORD3_ORIGIN_X		3	
#define STOPWATCH_RECORD3_ORIGIN_Y		78
#define STOPWATCH_RECORD3_SIZE_H			16	
#define STOPWATCH_RECORD3_SIZE_W			101	

/*第四个记录图层位置*/
#define STOPWATCH_RECORD4_ORIGIN_X		3	
#define STOPWATCH_RECORD4_ORIGIN_Y		94
#define STOPWATCH_RECORD4_SIZE_H			16	
#define STOPWATCH_RECORD4_SIZE_W			101	

/*第五个记录图层位置*/
#define STOPWATCH_RECORD5_ORIGIN_X		3	
#define STOPWATCH_RECORD5_ORIGIN_Y		110
#define STOPWATCH_RECORD5_SIZE_H			16	
#define STOPWATCH_RECORD5_SIZE_W			101	


typedef struct tag_StopWatch
{
	int8_t  state;		//开始暂停状态 0 暂停 1 开始
	int8_t  record_index;	//记录位置
	int8_t  flag;		//定时器启动标志
	uint32_t sys_micro;	//系统毫秒，精度1ms
	uint32_t cur_micro;	//精度为1ms
	uint32_t record[5];	//精度为1ms
	uint32_t record_pos[5];	//精度1ms
}StopWatch;

static StopWatch g_stopwatch;

/*100小时*/
#define STOPWATCH_MAX_MICRO	360000000

#define STOPWATCH_MAX_RECORD	5


/*窗口ID*/
static int32_t g_stopwatch_window_id = -1;

/*状态栏时间图层ID*/
static int8_t g_stopwatch_time_layer_id = -1;

/*秒表分钟秒钟图层ID*/
static int8_t g_stopwatch_hour_min_sec_layer_id = -1;

/*秒表毫秒图层ID*/
static int8_t g_stopwatch_sec_micro_layer_id = -1;

/*开始暂停图层ID*/
static int8_t g_stopwatch_side_play_layer_id = -1;


/*记录文本图层ID集合,注意初始化为-1*/
static int8_t g_stopwatch_record_layer_id_set[STOPWATCH_MAX_RECORD] = {-1, -1, -1, -1, -1};


/*定时器ID*/
static int8_t g_stopwatch_timer_id = -1;


int32_t bmp_array_name [] = 
{
	RES_BITMAP_WATCHAPP_STOPWATCH_SIDE_BG_BMP,
	RES_BITMAP_WATCHAPP_STOPWATCH_SIDE_PAUSE_BMP
};


extern P_Window stopwatch_create_init_window();
extern void stopwatch_timer_callback(date_time_t tick_time, uint32_t millis, void *context);



int8_t stopwatch_get_random_number(int32_t a)
{
	return(((a>>24)+(a>>16)+(a>>8)+a) & 0x0f); 
}


/*创建并显示图片图层，需要坐标值，得到icon_key的数组，数组需要的参数值，P_Window*/
P_Layer display_target_layer(GRect *temp_p_frame,enum GAlign how_to_align,int32_t bmp_array_name[],int bmp_id_number)
{	
	GBitmap bmp_point;
	
	res_get_user_bitmap(bmp_array_name[bmp_id_number], &bmp_point);
	LayerBitmap layer_bitmap_struct_l = {bmp_point, *temp_p_frame, how_to_align};
 	P_Layer temp_P_Layer = app_layer_create_bitmap(&layer_bitmap_struct_l);
	return temp_P_Layer;
	
}

/*创建并显示文本图层*/
P_Layer display_target_layerText(GRect  *temp_p_frame,enum GAlign how_to_align,char * str,uint8_t font_type)
{
	LayerText temp_LayerText = {0};
	temp_LayerText.text = str;
	temp_LayerText.frame = *temp_p_frame;
	temp_LayerText.alignment = how_to_align;
	temp_LayerText.font_type = font_type;
	
	P_Layer p_layer = app_layer_create_text(&temp_LayerText);
	
	return p_layer;
}

/*
 *--------------------------------------------------------------------------------------
 *     function:  stopwatch_create_sec_micro_layer
 *    parameter: 
 *       return:
 *  description:  创建秒毫秒图层
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Layer stopwatch_create_sec_micro_layer()
{
	char micro_buf[12] = "";
	int32_t micro = (g_stopwatch.cur_micro % 1000) / 100;

	if (g_stopwatch.cur_micro >= STOPWATCH_MAX_MICRO)
	{
		strcpy(micro_buf, "59");
	}
	else if(g_stopwatch.cur_micro >= 3600000)
	{
		sprintf(micro_buf, "%02d", (g_stopwatch.cur_micro/1000)%60);
	}
	else
	{
		sprintf(micro_buf, "%01d%01d", micro,  stopwatch_get_random_number(g_stopwatch.cur_micro));	
	}

	/*添加秒表毫秒图层*/
	GRect micro_frame = {{STOPWATCH_TOP_MICRO_ORIGIN_X, STOPWATCH_TOP_MICRO_ORIGIN_Y},
						{STOPWATCH_TOP_MICRO_SIZE_H,STOPWATCH_TOP_MICRO_SIZE_W}};

	P_Layer micro_layer = display_target_layerText(&micro_frame,GAlignCenter,micro_buf,U_ASCII_ARIAL_16);
	
	return micro_layer;
}


/*
 *--------------------------------------------------------------------------------------
 *     function:  stopwatch_create_hour_min_sec_layer
 *    parameter: 
 *       return:
 *  description:  创建分钟秒钟图层
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Layer stopwatch_create_hour_min_sec_layer()
{

	char sec_buf[12] = "";
	
	if (g_stopwatch.cur_micro >= STOPWATCH_MAX_MICRO)
	{
		strcpy(sec_buf, "99:59:59");
	}
	/*如果大于一个小时，则显示时分,否则显示分秒*/
	else if(g_stopwatch.cur_micro >= 3600000)
	{
		sprintf(sec_buf, "%02d:%02d", (g_stopwatch.cur_micro/(3600000)), ((g_stopwatch.cur_micro/(60000))%60));
	}
	else
	{
		sprintf(sec_buf, "%02d:%02d", (g_stopwatch.cur_micro/(60000)), (g_stopwatch.cur_micro/1000)%60);
	}

	/*添加秒表时钟分钟秒钟图层*/
	GRect hour_min_sec_frame = {{STOPWATCH_TOP_MIN_SEC_ORIGIN_X,  STOPWATCH_TOP_MIN_SEC_ORIGIN_Y},
					{STOPWATCH_TOP_MIN_SEC_SIZE_H, STOPWATCH_TOP_MIN_SEC_SIZE_W}};
		
	P_Layer hour_min_sec_layer = display_target_layerText(&hour_min_sec_frame,GAlignCenter,sec_buf,U_ASCII_ARIAL_30);

	return (hour_min_sec_layer);
}

/*
 *--------------------------------------------------------------------------------------
 *     function:  stopwatch_create_record_text_layer
 *    parameter: 
 *       return:
 *  description:  创建记录文本图层
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Layer stopwatch_create_record_text_layer(int8_t index)
{
	char buf[20] = "";
		
	/*添加图层*/
	GRect record_frame = {{STOPWATCH_RECORD1_ORIGIN_X, STOPWATCH_RECORD1_ORIGIN_Y + index * 16}, 
				{STOPWATCH_RECORD1_SIZE_H, STOPWATCH_RECORD1_SIZE_W}};
	sprintf(buf, "%02d:%02d:%02d.%02d", (g_stopwatch.record[index]/(3600000)), (g_stopwatch.record[index]/60000)%60, (g_stopwatch.record[index]/1000)%60, 
			(g_stopwatch.record[index]%1000)/10);
	P_Layer record_layer = display_target_layerText(&record_frame,GAlignLeft,buf,U_ASCII_ARIAL_16);
	
	return record_layer;
}


/*定义后退按键事件*/
static void stopwatch_select_back(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		app_window_stack_pop(p_window);
	}

}

/*定义上按键事件*/
static void stopwatch_select_up(void *context)
{
	/*当计数开始并且小于最大值，才响应上按键*/
	if (!((g_stopwatch.state == 1) && (g_stopwatch.cur_micro < STOPWATCH_MAX_MICRO)))
	{
		return;
	}

	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		/*记录满, 移位*/
		if (g_stopwatch.record_index >= 5)
		{
	
			/*不能使用memmove*/
			int32_t tmp[5] = {0};
			memcpy((char *)(&tmp[0]), (char *)(&g_stopwatch.record[1]), sizeof(int32_t) * 4);
			memcpy((char *)(&g_stopwatch.record[0]), (char *)(&tmp[0]), sizeof(int32_t) * 4);
			memcpy((char *)(&tmp[0]), (char *)(&g_stopwatch.record_pos[1]), sizeof(int32_t) * 4);
			memcpy((char *)(&g_stopwatch.record_pos[0]), (char *)(&tmp[0]), sizeof(int32_t) * 4);
	
			g_stopwatch.record[4] = g_stopwatch.cur_micro - g_stopwatch.record_pos[3] + stopwatch_get_random_number(g_stopwatch.cur_micro);
			g_stopwatch.record_pos[4] = g_stopwatch.cur_micro;

			/*全部更新一遍*/
			int i = 0;
			char buff[20] = "";
			P_Layer p_record_layer = NULL;
			for (i = 0; i < STOPWATCH_MAX_RECORD; i++)
			{
				p_record_layer = app_window_get_layer_by_id(p_window, g_stopwatch_record_layer_id_set[i]);
				if (NULL != p_record_layer)
				{
					sprintf(buff, "%02d:%02d:%02d.%02d",g_stopwatch.record[i]/3600000, (g_stopwatch.record[i]/60000)%60, (g_stopwatch.record[i]/1000)%60, 
									(g_stopwatch.record[i]%1000)/10);
					app_layer_set_text_text(p_record_layer, buff);
				}
			}

		}
		else
		{
			g_stopwatch.record[g_stopwatch.record_index] = g_stopwatch.cur_micro + stopwatch_get_random_number(g_stopwatch.cur_micro);

			if(g_stopwatch.record_index != 0)
			{
				g_stopwatch.record[g_stopwatch.record_index] = g_stopwatch.record[g_stopwatch.record_index] - g_stopwatch.record_pos[g_stopwatch.record_index - 1];
			}
															
			g_stopwatch.record_pos[g_stopwatch.record_index] = g_stopwatch.cur_micro;
			
			/*有创建则更新*/
			char buff[20] = "";
			P_Layer p_record_layer = app_window_get_layer_by_id(p_window, g_stopwatch_record_layer_id_set[g_stopwatch.record_index]);
			sprintf(buff, "%02d:%02d:%02d.%02d", g_stopwatch.record[g_stopwatch.record_index]/3600000, (g_stopwatch.record[g_stopwatch.record_index]/60000)%60, 
					(g_stopwatch.record[g_stopwatch.record_index]/1000)%60, (g_stopwatch.record[g_stopwatch.record_index]%1000)/10);
			app_layer_set_text_text(p_record_layer, buff);
			maibu_layer_set_visible_status(p_record_layer, true);

			g_stopwatch.record_index++;
			
		}

		app_window_update(p_window);	
	}
}


/*定义下按键事件*/
static void stopwatch_select_down(void *context)
{

	/*当计数开始，才相应下按键*/
	if ((g_stopwatch.cur_micro != 0) || ((g_stopwatch.cur_micro == 0) && g_stopwatch.state == 1))
	{
		P_Window p_window = (P_Window)context;
		if (NULL != p_window)
		{
			memset(&g_stopwatch, 0, sizeof(StopWatch));
			if(g_stopwatch_timer_id != -1)	
			{
				app_service_timer_unsubscribe(g_stopwatch_timer_id);
				g_stopwatch_timer_id = -1;
			}

			P_Window p_new_window = stopwatch_create_init_window();
			g_stopwatch_window_id = app_window_stack_replace_window(p_window, p_new_window);	
		}
	}
}



static void stopwatch_select_select(void *context)
{

	/*如果计数满，则选择键不处理*/
	if (g_stopwatch.cur_micro >= STOPWATCH_MAX_MICRO)
	{
		return;
	}

	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		GBitmap bmp;
		res_get_user_bitmap(RES_BITMAP_WATCHAPP_STOPWATCH_SIDE_PAUSE_BMP, &bmp);
		P_Layer p_side_play_layer = app_window_get_layer_by_id(p_window, g_stopwatch_side_play_layer_id);

		if(0 == g_stopwatch.state)
		{
			/*开定时器*/
			g_stopwatch.flag = 1;
			g_stopwatch.state = 1;
			g_stopwatch_timer_id = app_service_timer_subscribe(100, stopwatch_timer_callback, (void *)p_window);
			
		
			/*暂停图标图层需要修改为可见属性*/	
			maibu_layer_set_visible_status(p_side_play_layer, true);
	
		}	
		else
		{
			//关闭定时器
			app_service_timer_unsubscribe(g_stopwatch_timer_id);
			g_stopwatch.state = 0;
			g_stopwatch_timer_id = -1;

			/*暂停图标图层需要修改为不可见属性*/	
			maibu_layer_set_visible_status(p_side_play_layer, false);

		}

		app_layer_set_bitmap_bitmap(p_side_play_layer, &bmp);
		app_window_update(p_window);	
	}
}


void  time_change (enum SysEventType type, void *context)
{
	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		
		P_Window p_window = app_window_stack_get_window_by_id(g_stopwatch_window_id);
		if (NULL == p_window)
		{
			return ;
		}

		char str_time[6] = {0};
		struct date_time datetime;

		app_service_get_datetime(&datetime);

		P_Layer p_hm_layer = app_window_get_layer_by_id(p_window, g_stopwatch_time_layer_id);
		if (NULL == p_hm_layer)
		{
			return;
		}

		sprintf(str_time, "%02d:%02d", datetime.hour, datetime.min);
		app_layer_set_text_text(p_hm_layer, str_time);
		app_window_update(p_window);
			
	}
}




/*
 *--------------------------------------------------------------------------------------
 *     function:  stopwatch_create_init_window
 *    parameter: 
 *       return:
 *  description:  
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Window stopwatch_create_init_window()
{

	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}


	/*这里的图层变化很多，所以初始化的时候，一次性把所有图层都建立好，然后再根据状态去控制那些图层需要显示，那些图层隐藏*/

	/*添加时间图层*/
	GRect time_frame = {{STOPWATCH_TIME_ORIGIN_X, STOPWATCH_TIME_ORIGIN_Y}, 
				{STOPWATCH_TIME_SIZE_H, STOPWATCH_TIME_SIZE_W}};
	struct date_time datetime;
	app_service_get_datetime(&datetime);
	char buf[6] = "";
	sprintf(buf, "%02d:%02d", datetime.hour, datetime.min);
	P_Layer time_layer = display_target_layerText(&time_frame,GAlignCenter,buf,U_ASCII_ARIAL_12);
	
	g_stopwatch_time_layer_id = app_window_add_layer(p_window, time_layer);

	
	//时、分钟、秒、毫秒显示图层
	P_Layer hour_min_sec_layer = stopwatch_create_hour_min_sec_layer();
	g_stopwatch_hour_min_sec_layer_id = app_window_add_layer(p_window, hour_min_sec_layer);
	
	P_Layer sec_micro_layer = stopwatch_create_sec_micro_layer();
	g_stopwatch_sec_micro_layer_id = app_window_add_layer(p_window, sec_micro_layer);


	//多个记录文本图层
	P_Layer record_layer = NULL;
	uint8_t i = 0;
	for (i = 0; i < STOPWATCH_MAX_RECORD; i++)
	{
		record_layer = stopwatch_create_record_text_layer(i);
		if(g_stopwatch.record[i] == 0)
		{
			maibu_layer_set_visible_status(record_layer, false);
		}
		g_stopwatch_record_layer_id_set[i] = app_window_add_layer(p_window, record_layer);
	}


	/*添加侧边栏图层*/
	GRect side_bg_frame = {{STOPWATCH_SIDE_BG_ORIGIN_X, STOPWATCH_SIDE_BG_ORIGIN_Y}, 
			{ STOPWATCH_SIDE_BG_SIZE_H, STOPWATCH_SIDE_BG_SIZE_W}};
	P_Layer	 side_bg_layer = display_target_layer(&side_bg_frame,GAlignCenter,bmp_array_name,0);
	app_window_add_layer(p_window, side_bg_layer);

	/*添加开始暂停图层*/
	GRect side_play_frame = {{STOPWATCH_SIDE_PLAY_ORIGIN_X, STOPWATCH_SIDE_PLAY_ORIGIN_Y}, 
				{ STOPWATCH_SIDE_PLAY_SIZE_H, STOPWATCH_SIDE_PLAY_SIZE_W}};
	P_Layer side_play_pause_layer = display_target_layer(&side_play_frame,GAlignCenter,bmp_array_name,1);
	if ((g_stopwatch.state == 0))
	{
		maibu_layer_set_visible_status(side_play_pause_layer, false);
	}
	else
	{
		maibu_layer_set_visible_status(side_play_pause_layer, true);
	}
	
	/*如果计时满了，需要隐藏*/
	if (g_stopwatch.cur_micro >= STOPWATCH_MAX_MICRO)	
	{
		maibu_layer_set_visible_status(side_play_pause_layer, false);
	}
	g_stopwatch_side_play_layer_id = app_window_add_layer(p_window, side_play_pause_layer);


	/*注册一个事件通知回调，当有时间改变时，立即更新时间*/
    maibu_service_sys_event_subscribe(time_change);

	/*这里先注册按键回调, 在回调中再具体处理回调怎么相应*/
	app_window_click_subscribe(p_window, ButtonIdBack, stopwatch_select_back);
	app_window_click_subscribe(p_window, ButtonIdUp, stopwatch_select_up);
	app_window_click_subscribe(p_window, ButtonIdDown, stopwatch_select_down);
	app_window_click_subscribe(p_window, ButtonIdSelect, stopwatch_select_select);

	
	return (p_window);

}


void stopwatch_timer_callback(date_time_t tick_time, uint32_t millis, void *context)
{
	static int32_t pre_sec = -1;

	if (g_stopwatch.flag == 1)
	{
		g_stopwatch.sys_micro = millis - 100;
		g_stopwatch.flag = 0;
	}
	g_stopwatch.cur_micro += (millis - g_stopwatch.sys_micro);
	g_stopwatch.sys_micro = millis;

	/*如果超过限制*/
	if (g_stopwatch.cur_micro >= STOPWATCH_MAX_MICRO)
	{
		g_stopwatch.cur_micro = STOPWATCH_MAX_MICRO;
		app_service_timer_unsubscribe(g_stopwatch_timer_id);
	}


	/*根据窗口ID获取窗口句柄*/
	P_Window p_window = app_window_stack_get_window_by_id(g_stopwatch_window_id);
	if (NULL == p_window)
	{
		return ;
	}

		
	if (g_stopwatch.cur_micro >= STOPWATCH_MAX_MICRO)
	{
		P_Window p_new_window = stopwatch_create_init_window();
		g_stopwatch_window_id = app_window_stack_replace_window(p_window, p_new_window);	
	}
	else
	{
		char str[20] = "";
		int8_t flag = 0;

		/*更新秒表显示*/
		P_Layer p_hour_min_sec_layer = app_window_get_layer_by_id(p_window, g_stopwatch_hour_min_sec_layer_id);
		P_Layer p_micro_layer = app_window_get_layer_by_id(p_window, g_stopwatch_sec_micro_layer_id);
		uint32_t sec = (g_stopwatch.cur_micro/1000)%60;
		uint32_t min = (g_stopwatch.cur_micro/60000);

		if ((NULL != p_hour_min_sec_layer) && (NULL != p_micro_layer))
		{
			/*如果大于一个小时*/
			if (g_stopwatch.cur_micro >= 3600000)
			{	
				if (pre_sec != sec)
				{
					sprintf(str, "%02d", sec);
					app_layer_set_text_text(p_micro_layer, str);
					sprintf(str, "%02d:%02d", min/60, min%60);
					app_layer_set_text_text(p_hour_min_sec_layer, str);
					flag++;
					pre_sec = sec;		
				}
		
			}
			else
			{
				if (pre_sec != sec)
				{
					sprintf(str, "%02d:%02d", min, sec);
					app_layer_set_text_text(p_hour_min_sec_layer, str);
					pre_sec = sec;
				}

				int32_t micro = (g_stopwatch.cur_micro % 1000) / 100;
				sprintf(str, "%01d%01d", micro,   stopwatch_get_random_number(g_stopwatch.cur_micro));
				app_layer_set_text_text(p_micro_layer, str);
				flag++;
			}	
		}
		

		if (flag)
		{	
			app_window_update(p_window);
		}
	}
}


int main()
{

	P_Window p_window = stopwatch_create_init_window(); 

	/*放入窗口栈显示*/
	g_stopwatch_window_id = app_window_stack_push(p_window);

	
	return 0;

}

