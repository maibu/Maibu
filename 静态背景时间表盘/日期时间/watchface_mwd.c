/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  app_mwd_watch.c
 *
 *    Description:  显示月周日表盘 
 *    Corporation:
 * 
 *         Author:  gliu (), gliu@damaijiankang.com
 *        Created:  2015年03月27日 11时10分11秒
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
#include "os_res_bitmap.h"
#include "os_sys_app_id.h"
#include "os_memory_manage.h"
#include "res_user_bitmap.h"
#include "os_sys_event.h"
#include "os_app_manage.h"
#include "window_stack.h"
#include "window.h"
#include "screen_show.h"
#include "matrix.h"
#include "plug_status_bar.h"
#include "res_bitmap_base.h"
#include <kernel_header.h>
#include <os_res_bitmap.h>
#include <os_app.h>

#else


#include "maibu_res.h"
#include "maibu_sdk.h"

#endif





/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_app_mwd_window_id = -1;

/*小时分钟图层ID，通过该图层ID获取图层句柄*/
static int8_t g_app_mwd_hm_layer_id = -1;

/*月周日图层ID，通过该图层ID获取图层句柄*/
static int8_t g_app_mwd_wmd_layer_id = -1;


/*定义各个图层的位置*/

/*背景图层*/
#define MWD_BG_ORIGIN_X		0
#define MWD_BG_ORIGIN_Y		0
#define MWD_BG_SIZE_H		128
#define MWD_BG_SIZE_W		128

/*显示小时分钟文本图层*/
#define MWD_HM_ORIGIN_X		8
#define MWD_HM_ORIGIN_Y		88
#define MWD_HM_SIZE_H		32		
#define MWD_HM_SIZE_W		120

/*显示周月日文本图层*/
#define MWD_WMD_ORIGIN_X	9
#define MWD_WMD_ORIGIN_Y	68
#define MWD_WMD_SIZE_H		12
#define MWD_WMD_SIZE_W		119



/*年*/
int16_t g_official_year = -1;
/*月*/
int8_t g_official_month = -1;
/*日*/
int8_t g_official_day = -1;

void app_mwd_watch_update()
{ 

	/*根据窗口ID获取窗口句柄*/
	P_Window p_window = app_window_stack_get_window_by_id(g_app_mwd_window_id);	
	if (NULL == p_window)
	{
		return ;
	}

	struct date_time datetime;
	app_service_get_datetime(&datetime);
	char str[20] = "";
	P_Layer p_hm_layer = app_window_get_layer_by_id(p_window, g_app_mwd_hm_layer_id);
	if (p_hm_layer)
	{
		sprintf(str, "%02d:%02d", datetime.hour, datetime.min);
		app_layer_set_text_text(p_hm_layer, str);	
		app_window_update(p_window);
	}

	/*每天凌晨更新日期*/	
	//if ((datetime.hour == 0))
	if ( (datetime.mday != g_official_day) || (datetime.year != g_official_year) || (datetime.mon != g_official_month))
	{
		P_Layer p_wmd_layer = app_window_get_layer_by_id(p_window, g_app_mwd_wmd_layer_id);
		if (p_wmd_layer)
		{
			char wday[7][8]={"周日","周一","周二","周三","周四","周五","周六"}; 
			sprintf(str, "%s  %d/%d", wday[datetime.wday], datetime.mon, datetime.mday);
			app_layer_set_text_text(p_wmd_layer, str);	
			app_window_update(p_window);
		}

		g_official_year = datetime.year;
		g_official_month = datetime.mon;
		g_official_day = datetime.mday;
	}
}



void app_mwd_watch_time_change(enum SysEventType type, void *context)
{

	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		app_mwd_watch_update();
	}
}




P_Window init_mwd_window()
{
	P_Window p_window = NULL;
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	/*添加背景图片图层*/
	GRect frame_bg = {{MWD_BG_ORIGIN_X, MWD_BG_ORIGIN_Y}, {MWD_BG_SIZE_H, MWD_BG_SIZE_W}};
	GBitmap bitmap_bg;
	res_get_user_bitmap(RES_BITMAP_WATCHFACE_MWD_BG, &bitmap_bg);
	LayerBitmap lb_bg = {bitmap_bg, frame_bg, GAlignLeft};	
	P_Layer layer_bitmap_bg = app_layer_create_bitmap(&lb_bg);
	if(layer_bitmap_bg != NULL)
	{
		app_window_add_layer(p_window, layer_bitmap_bg);
	}


	/*添加小时分钟图层*/
	GRect frame_hm = {{MWD_HM_ORIGIN_X, MWD_HM_ORIGIN_Y}, {MWD_HM_SIZE_H, MWD_HM_SIZE_W}};
	struct date_time t;
	app_service_get_datetime(&t);
	char time_str[20] = "";
	sprintf(time_str, "%d:%02d", t.hour, t.min);
	LayerText lt_hm = {time_str, frame_hm, GAlignTopLeft, U_ASCII_ARIALBD_30, 0};
	P_Layer layer_text_hm = app_layer_create_text(&lt_hm);
	app_layer_set_bg_color(layer_text_hm, GColorBlack);
	if(layer_text_hm != NULL)
	{
		g_app_mwd_hm_layer_id = app_window_add_layer(p_window, layer_text_hm);
	}


	/*添加星期月图层*/
	GRect frame_wmd = {{MWD_WMD_ORIGIN_X, MWD_WMD_ORIGIN_Y}, {MWD_WMD_SIZE_H, MWD_WMD_SIZE_W}};
	memset(time_str, 0, sizeof(time_str));
	char wday[7][8]={"周日","周一","周二","周三","周四","周五","周六"}; 
	sprintf(time_str, "%s  %d/%d", wday[t.wday], t.mon, t.mday);
	LayerText lt_wmd = { time_str,frame_wmd, GAlignLeft,  U_ASCII_ARIAL_12, 0};
	P_Layer layer_text_wmd = app_layer_create_text(&lt_wmd);
	app_layer_set_bg_color(layer_text_wmd, GColorBlack);
	if(layer_text_wmd != NULL)
	{
		g_app_mwd_wmd_layer_id = app_window_add_layer(p_window, layer_text_wmd);
	}


	/*注册一个事件通知回调，当有时间改变是，立即更新时间*/
	maibu_service_sys_event_subscribe(app_mwd_watch_time_change);

	return p_window;

}




int main()
{

#ifdef LINUX	

	/*非APP编写*/	
	screen_init(SCREEN_ROW_NUMS,SCREEN_COL_NUMS);
	os_store_manage_init();	
	window_stack_init();
	SHOW;
#endif


	P_Window p_window = init_mwd_window(); 
	g_app_mwd_window_id = app_window_stack_push(p_window);



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

	window_stack_destory();
	screen_destory();
	os_store_manage_destory();

	SHOW;


#endif

	return 0;

}

