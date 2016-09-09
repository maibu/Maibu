/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  watchface_jobs.c
 *
 *    Description:  乔布斯表盘
 *    Corporation:
 * 
 *         Author:  gliu (), gliu@damaijiankang.com
 *        Created:  2015年03月27日 10时52分52秒
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


#ifdef LINUX
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "linux_screen_show.h"
#include "os_store_manage.h"
#include "os_time.h"
#include "os_comm.h"
#include "os_res_bitmap.h"
#include "os_sys_app_id.h"
#include "os_memory_manage.h"
#include "res_user_bitmap.h"
#include "os_sys_event.h"
#include "window_stack.h"
#include "window.h"
#include "screen_show.h"
#include "matrix.h"
#include "plug_status_bar.h"
#include "res_bitmap_base.h"
#else
#include "maibu_sdk.h"
#include "maibu_res.h"
#endif




/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_app_jobs_window_id = -1;

/*图层ID，通过该图层ID获取图层句柄*/
static int8_t g_app_jobs_layer_id = -1;


/*定义各个图层的位置*/

/*背景图层*/
#define JOBS_BG_ORIGIN_X		0
#define JOBS_BG_ORIGIN_Y		0
#define JOBS_BG_SIZE_H		128
#define JOBS_BG_SIZE_W		128


/*时间图层*/
#define JOBS_HM_ORIGIN_X		76
#define JOBS_HM_ORIGIN_Y		0
#define JOBS_HM_SIZE_H		20
#define JOBS_HM_SIZE_W		50


static void app_jobs_watch_time_update(int8_t hour, int8_t min, int8_t flag)
{

	/*根据窗口ID获取窗口句柄*/
	P_Window p_window = app_window_stack_get_window_by_id(g_app_jobs_window_id);	
	if (NULL == p_window)
	{
		return;	
	}

	P_Layer p_layer = app_window_get_layer_by_id(p_window, g_app_jobs_layer_id);
	if (NULL != p_layer)
	{
		char str[10] = "";
		sprintf(str, "%d:%02d", hour, min);	
		app_layer_set_text_text(p_layer, str);
		app_window_update(p_window);
	}
	
	return;

}


static void app_jobs_watch_time_change(enum SysEventType type, void *context)
{
	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		struct date_time t;
		app_service_get_datetime(&t);
		app_jobs_watch_time_update(t.hour, t.min, 1);
	}
}



static P_Window init_jobs_window()
{
	P_Window p_window = NULL;
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}


	/*创建背景图层*/
	GRect frame_bg = {{JOBS_BG_ORIGIN_X, JOBS_BG_ORIGIN_Y}, {JOBS_BG_SIZE_H, JOBS_BG_SIZE_W}};
	GBitmap bmp;
	res_get_user_bitmap(RES_BITMAP_WATCHFACE_JOBS_BG, &bmp);
	LayerBitmap layer_bitmap = {bmp, frame_bg, GAlignCenter};
	P_Layer bg_layer = app_layer_create_bitmap(&layer_bitmap);
	app_window_add_layer(p_window, bg_layer);


	GRect frame_hm_bg = {{JOBS_HM_ORIGIN_X, JOBS_HM_ORIGIN_Y}, {JOBS_HM_SIZE_H, JOBS_HM_SIZE_W}};
	struct date_time t;
	app_service_get_datetime(&t);
	char buf[10] = "";
	sprintf(buf, "%d:%02d", t.hour, t.min);
	LayerText text = {buf, frame_hm_bg, GAlignCenter, U_ASCII_ARIAL_20};
	P_Layer layer = app_layer_create_text(&text);
	g_app_jobs_layer_id = app_window_add_layer(p_window, layer);


	/*注册一个事件通知回调，当有时间改变是，立即更新时间*/
	maibu_service_sys_event_subscribe(app_jobs_watch_time_change);

	return p_window;

}




int main()
{

#ifdef LINUX	
	/*非APP编写*/	
	screen_init(SCREEN_ROW_NUMS,SCREEN_COL_NUMS);
	os_store_manage_init();	
	window_stack_init();
	set_current_app(0x7dd01);
	SHOW;
#endif


	/*创建显示汉字表盘窗口*/
	P_Window p_window = init_jobs_window(); 
	if(p_window != NULL)
	{
		/*放入窗口栈显示*/
		g_app_jobs_window_id = app_window_stack_push(p_window);
	}


#ifdef LINUX
	SHOW;
	/*非APP编写*/	
	while (1)
	{
		char input;	
	
		/*输入操作*/
		scanf("%c", &input);
		if (input == 'q')
		{
			break;	
		}
		else if (input == 'c')
		{
	
		}
		else if (input == 'g')
		{
			break;
		}
	
	}	

	app_window_stack_pop(p_window);
	window_stack_destory();
	screen_destory();
	os_store_manage_destory();

	SHOW;

#endif

	return 0;

}


