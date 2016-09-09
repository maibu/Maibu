/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  watchface_weather.c
 *
 *    Description:  天气表盘APP 
 *    Corporation:
 * 
 *         Author:  gliu (), gliu@damaijiankang.com
 *        Created:  2014年12月15日 10时35分34秒
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
#include "os_user_app_id.h"
#include "window_stack.h"
#include "window.h"
#include "screen_show.h"
#include "matrix.h"
#include "plug_status_bar.h"
//#include "res_bitmap_base.h"
#else

#include "maibu_sdk.h"
#include "maibu_res.h"

#endif

/*GPS及海拔数据结构*/
static SGpsAltitude g_gps_altitude;

/*Web通讯ID*/
static uint32_t g_comm_id_web = 0;

/*Phone通讯ID*/
static uint32_t g_comm_id_gps = 0;



/*每TIME_INTERVAL没有更新天气，增加1次*/
static int8_t g_update_time = 0;

/*更新时间间隔,单位分钟*/
#define TIME_INTERVAL		15 

/*多久没有更新，则显示默认图标及温度, 单位15分钟*/
#define TIME_LIMIT	8	


/*存放当前天气图标KEY*/
#define WEATHER_ICON_KEY	1

/*存放当前天气温度图标*/
#define WEATHER_TEMP_KEY	2


/*如果没有获取到天气图标，设置特殊天气图标*/
#define WEATHER_ICON_SPEC	0

/*如果没有获取到温度，设置特殊温度标志*/
#define WEATHER_TEMP_SPEC	125



/*获取天气数据云服务器地址*/
#define WEATHER_WEB     "http://api.openweathermap.org/data/2.5/weather?appid=这里替换成你申请的APPID&cnt=1&"



/*图片ID*/

/*no data*/
#define WEATHER_ICON_NO_DATA		0

/*clear sky*/
#define WEATHER_ICON_CLEAR_SKY		1

/*few clouds*/
#define WEATHER_ICON_FEW_CLOUDS		2

/*scattered clouds*/
#define WEATHER_ICON_SCATTERED_CLOUDS	3

/*broken clouds*/
#define WEATHER_ICON_BROKEN_CLOUDS	4

/*shower rain*/
#define WEATHER_ICON_SHOWER_RAIN	9

/*rain*/ 
#define WEATHER_ICON_RAIN		10

/*thunderstorm*/			
#define WEATHER_ICON_THUNDERSTORM	11	

/*snow*/
#define WEATHER_ICON_SNOW		13

/*mist*/
#define WEATHER_ICON_MIST		50




/*窗口ID*/
static int32_t g_window_id = -1;

/*图层ID*/

/*小时分钟图层*/
static int8_t g_layer_id_hm = -1;

/*周月日图层*/
static int8_t g_layer_id_wmd = -1;

/*温度图层*/
static int8_t g_layer_id_temp = -1;

/*温度图标图层*/
static int8_t g_layer_id_icon = -1;



/*定义各个图层的位置*/

/*背景图层*/
#define LAYER_BG_ORIGIN_X	0
#define LAYER_BG_ORIGIN_Y	0
#define LAYER_BG_SIZE_H		128
#define LAYER_BG_SIZE_W		128

/*显示小时分钟文本图层*/
#define LAYER_HM_ORIGIN_X	0
#define LAYER_HM_ORIGIN_Y	11
#define LAYER_HM_SIZE_H		42
#define LAYER_HM_SIZE_W		128

/*显示周月日文本图层*/
#define LAYER_WMD_ORIGIN_X	0
#define LAYER_WMD_ORIGIN_Y	51
#define LAYER_WMD_SIZE_H	14
#define LAYER_WMD_SIZE_W	128

/*显示天气图片图层*/
#define LAYER_WICON_ORIGIN_X	14
#define LAYER_WICON_ORIGIN_Y	79
#define LAYER_WICON_SIZE_H	40
#define LAYER_WICON_SIZE_W	40

/*显示温度文本图层*/
#define LAYER_TEMP_ORIGIN_X	0
#define LAYER_TEMP_ORIGIN_Y	84
#define LAYER_TEMP_SIZE_H	30
#define LAYER_TEMP_SIZE_W	104




void weather_init_store()
{

	/*创建保存天气图标文件*/
#ifdef LINUX
	app_persist_create( USER_APP_ID_WEATHER_WATCH, WEATHER_ICON_KEY, 1);
#else
	app_persist_create( WEATHER_ICON_KEY, 1);
#endif


	/*先读取天气图标, 如果是第一次，设置默认值*/	
	uint8_t icon_type = 0;
	int8_t icon_read_size = 0;

#ifdef LINUX
	icon_read_size = app_persist_read_data( USER_APP_ID_WEATHER_WATCH, WEATHER_ICON_KEY, 0, (unsigned char *)&icon_type, sizeof(int8_t));
#else
	icon_read_size = app_persist_read_data( WEATHER_ICON_KEY, 0, (unsigned char *)&icon_type, sizeof(int8_t));
#endif

	/*第一次使用, 显示默认值*/
	if (icon_read_size == 0)
	{
		icon_type = WEATHER_ICON_SPEC;

#ifdef LINUX
		app_persist_write_data_extend( USER_APP_ID_WEATHER_WATCH, WEATHER_ICON_KEY,(unsigned char *)&icon_type, sizeof(int8_t));
#else
		app_persist_write_data_extend( WEATHER_ICON_KEY,(unsigned char *)&icon_type, sizeof(int8_t));
#endif

	}





	/*创建保存温度文件*/
#ifdef LINUX
	app_persist_create( USER_APP_ID_WEATHER_WATCH, WEATHER_TEMP_KEY, 1);
#else
	app_persist_create( WEATHER_TEMP_KEY, 1);
#endif

	
	/*先读取温度, 如果第一次，设置默认值*/	
	int8_t temp = 0;
	int8_t temp_read_size = 0;	

#ifdef LINUX
	temp_read_size = app_persist_read_data( USER_APP_ID_WEATHER_WATCH, WEATHER_TEMP_KEY, 0, (unsigned char *)&temp, sizeof(int8_t));
#else
	temp_read_size = app_persist_read_data( WEATHER_TEMP_KEY, 0, (unsigned char *)&temp, sizeof(int8_t));
#endif

	if (temp_read_size == 0)
	{
	
		temp = WEATHER_TEMP_SPEC;
#ifdef LINUX
		app_persist_write_data_extend( USER_APP_ID_WEATHER_WATCH, WEATHER_TEMP_KEY,(unsigned char *)&temp, sizeof(int8_t));
#else
		app_persist_write_data_extend( WEATHER_TEMP_KEY,(unsigned char *)&temp, sizeof(int8_t));
#endif

	}


}


/*根据图标类型获取对应的天气图标*/
int32_t weather_get_icon_key(int32_t icon_type)
{

	int32_t type = RES_BITMAP_WATCHFACE_WEATHER_CLOUDY; 
	if (icon_type == WEATHER_ICON_NO_DATA)
	{
		type = RES_BITMAP_WATCHFACE_WEATHER_NODATA;
	}	
	else if (icon_type == WEATHER_ICON_CLEAR_SKY)
	{
		type = RES_BITMAP_WATCHFACE_WEATHER_SUNNY;
	}
	else if (icon_type == WEATHER_ICON_FEW_CLOUDS)
	{
		type = RES_BITMAP_WATCHFACE_WEATHER_CLOUDY;
	}	
	else if (icon_type == WEATHER_ICON_SCATTERED_CLOUDS)
	{
		type = RES_BITMAP_WATCHFACE_WEATHER_CLOUDY;
	}
	else if (icon_type == WEATHER_ICON_BROKEN_CLOUDS)
	{
		type = RES_BITMAP_WATCHFACE_WEATHER_OVERCAST;
	}
	else if(icon_type == WEATHER_ICON_SHOWER_RAIN)
	{
		type = RES_BITMAP_WATCHFACE_WEATHER_RAIN_1;
	}
	else if (icon_type == WEATHER_ICON_RAIN)
	{
		type = RES_BITMAP_WATCHFACE_WEATHER_RAIN_2;
	}
	else if (icon_type == WEATHER_ICON_THUNDERSTORM)
	{
		type = RES_BITMAP_WATCHFACE_WEATHER_RAIN_3;
	}
	else if (icon_type == WEATHER_ICON_SNOW)
	{
		type = RES_BITMAP_WATCHFACE_WEATHER_SNOW_2;
	}
	else if (icon_type == WEATHER_ICON_MIST)
	{
		type = RES_BITMAP_WATCHFACE_WEATHER_FOG;
	}
	else
	{
		type = RES_BITMAP_WATCHFACE_WEATHER_CLOUDY; 
	}

	//printf("type :%d\n", type);

	return type;
}




/*
 *--------------------------------------------------------------------------------------
 *     function:  weather_request_web
 *    parameter: 
 *       return:
 *  description:  请求网络数据
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
void weather_request_web()
{
	/*拼接url请求地址, 注意url的缓存大小*/
	char url[200] = "";		
	sprintf(url, "%slat=%s&lon=%s", WEATHER_WEB, g_gps_altitude.lat, g_gps_altitude.lon);		
	/*拼接过滤参数，即只接受和过滤参数匹配的返回值*/
	char param[20] = "";
	sprintf(param, "%s,%s", "temp","icon");
	/*15分钟同步一次天气数据*/
	g_comm_id_web = maibu_comm_request_web(url, param, 60*15);	
}



/*
 *--------------------------------------------------------------------------------------
 *     function:  weather_phone_recv_callback
 *    parameter: 
 *       return:
 *  description:  接受手机数据回调
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
void weather_phone_recv_callback(enum ERequestPhone type, void *context)
{
	uint8_t* ptr = context;
	uint8_t i = 0;
	/*如果不是GPS海拔数据， 退出*/
	if (type != ERequestPhoneGPSAltitude)
	{
		return;
	}

	/*提取经度纬度*/
	memcpy(&g_gps_altitude, (SGpsAltitude *)context, sizeof(SGpsAltitude));

	/*请求Web获取天气数据*/
	weather_request_web();
}


/*
 *--------------------------------------------------------------------------------------
 *     function:  
 *    parameter: 
 *       return:
 *  description:  接受WEB数据回调
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
void weather_web_recv_callback(const uint8_t *buff, uint16_t size)
{
	int8_t icon = 0;
	char temp[5] = "";
	int tempera = 0 ;
	uint8_t i = 0;
	//printf("aa---%s\n", buff);

	/*提取温度及对应的图片ID*/
	int32_t icon_tmp = 0, tempera_tmp = 0;
	if ((-1 == maibu_get_json_int(buff, "icon", &icon_tmp)) || (-1 == maibu_get_json_int(buff, "temp", &tempera_tmp)))
	{
		return;
	}
	icon = icon_tmp;
	tempera = tempera_tmp;
	tempera -= 273;
	g_update_time = 0;


	/*保存新的温度及图标序号*/
#ifdef LINUX
	app_persist_write_data_extend( USER_APP_ID_WEATHER_WATCH, WEATHER_ICON_KEY,(unsigned char *)&icon, sizeof(int8_t));
#else
	app_persist_write_data_extend( WEATHER_ICON_KEY,(unsigned char *)&icon, sizeof(int8_t));
#endif


#ifdef LINUX
	app_persist_write_data_extend( USER_APP_ID_WEATHER_WATCH, WEATHER_TEMP_KEY,(unsigned char *)&tempera, sizeof(int8_t));
#else
	app_persist_write_data_extend( WEATHER_TEMP_KEY, (unsigned char *)&tempera, sizeof(int8_t));
#endif


	/*根据窗口ID获取窗口句柄*/
	P_Window p_window = app_window_stack_get_window_by_id(g_window_id);	
	if (NULL != p_window)
	{
		/*获取原来显示的温度及天气图标图层*/
		P_Layer p_layer_temp = app_window_get_layer_by_id(p_window, g_layer_id_temp);
		P_Layer p_layer_icon = app_window_get_layer_by_id(p_window, g_layer_id_icon);

		if (NULL != p_layer_temp)
		{
			/*修改温度*/
			sprintf(temp ,"%d" , tempera);
			app_layer_set_text_text(p_layer_temp, temp);	
		}
	
		if (NULL != p_layer_icon)
		{	
			/*修改天气图标*/
			GBitmap bitmap_icon;
			res_get_user_bitmap( weather_get_icon_key(icon), &bitmap_icon);
			app_layer_set_bitmap_bitmap(p_layer_icon, &bitmap_icon);
		}	

		/*更新窗口*/	
		app_window_update(p_window);

	}

}



void weather_time_change(enum SysEventType type, void *context)
{
	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		/*根据窗口ID获取窗口句柄*/
		P_Window p_window = app_window_stack_get_window_by_id(g_window_id);	
		if (NULL == p_window)
		{
			return;
		}
		
		/*获取当前时间*/
		struct date_time datetime;
		app_service_get_datetime(&datetime);
		char str[20] = "";
	
		/*获取小时分钟图层*/	
		P_Layer p_layer_hm = app_window_get_layer_by_id(p_window, g_layer_id_hm);
		if (NULL ==p_layer_hm)
		{
			return;	
		}

		/*设置图层内容*/
		sprintf(str, "%02d:%02d", datetime.hour, datetime.min);
		app_layer_set_text_text(p_layer_hm, str);	


		/*每天凌晨更新日期,这里可以优化,你懂的*/	
		if ((datetime.hour == 0))
		{
			P_Layer p_layer_wmd = app_window_get_layer_by_id(p_window, g_layer_id_wmd);
			if (p_layer_wmd)
			{
				char wday[7][8]={"周日","周一","周二","周三","周四","周五","周六"}; 
				sprintf(str, "%s  %d月%d日", wday[datetime.wday], datetime.mon, datetime.mday);
				app_layer_set_text_text(p_layer_wmd, str);	
			}
		}

		/*更新窗口显示*/
		app_window_update(p_window);
	}
}





void app_weather_watch_timer_callback(date_time_t tick_time, uint32_t millis, void *context)
{
	g_update_time++;

	/*每两个小时检测一次，如要数据没有更新, 需要显示默认界面*/
	if (g_update_time > TIME_LIMIT)
	{
		g_update_time = 0;

		/*默认图标及温度*/
		int8_t icon = WEATHER_ICON_SPEC;
		int8_t tempera = WEATHER_TEMP_SPEC;	
		char temp[5] = "";

		/*保存新的温度及图标序号*/
	#ifdef LINUX
		app_persist_write_data_extend( USER_APP_ID_WEATHER_WATCH, WEATHER_ICON_KEY,(unsigned char *)&icon, sizeof(int8_t));
	#else
		app_persist_write_data_extend( WEATHER_ICON_KEY,(unsigned char *)&icon, sizeof(int8_t));
	#endif


	#ifdef LINUX
		app_persist_write_data_extend( USER_APP_ID_WEATHER_WATCH, WEATHER_TEMP_KEY,(unsigned char *)&tempera, sizeof(int8_t));
	#else
		app_persist_write_data_extend( WEATHER_TEMP_KEY, (unsigned char *)&tempera, sizeof(int8_t));
	#endif


		/*根据窗口ID获取窗口句柄*/
		P_Window p_window = app_window_stack_get_window_by_id(g_window_id);	
		if (NULL != p_window)
		{
			/*获取原来显示的温度及天气图标图层*/
			P_Layer p_layer_temp = app_window_get_layer_by_id(p_window, g_layer_id_temp);
			P_Layer p_layer_icon = app_window_get_layer_by_id(p_window, g_layer_id_icon);

			if (NULL != p_layer_temp)
			{
				/*修改温度*/
				strcpy(temp ,"--");
				app_layer_set_text_text(p_layer_temp, temp);	
			}
		
			if (NULL != p_layer_icon)
			{	
				/*修改天气图标*/
				GBitmap bitmap_icon;

				res_get_user_bitmap( weather_get_icon_key(WEATHER_ICON_SPEC), &bitmap_icon);
				app_layer_set_bitmap_bitmap(p_layer_icon, &bitmap_icon);
			}	
	
			/*更新窗口*/	
			app_window_update(p_window);

		}


	}	


}




P_Window init_weather_window()
{
	P_Window p_window = NULL;
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	/*添加背景图片图层*/
	GRect frame_bg = {{LAYER_BG_ORIGIN_X, LAYER_BG_ORIGIN_Y}, {LAYER_BG_SIZE_H, LAYER_BG_SIZE_W}};
	GBitmap bitmap_bg;
	res_get_user_bitmap(RES_BITMAP_WATCHFACE_WEATHER_BG, &bitmap_bg);
	LayerBitmap lb_bg = {bitmap_bg, frame_bg, GAlignLeft};	
	P_Layer layer_bitmap_bg = app_layer_create_bitmap(&lb_bg);
	if(layer_bitmap_bg != NULL)
	{
		app_window_add_layer(p_window, layer_bitmap_bg);
	}


	/*添加小时分钟图层*/
	GRect frame_hm = {{LAYER_HM_ORIGIN_X, LAYER_HM_ORIGIN_Y}, {LAYER_HM_SIZE_H, LAYER_HM_SIZE_W}};
	struct date_time t;
	app_service_get_datetime(&t);
	char time_str[20] = "";
	sprintf(time_str, "%02d:%02d", t.hour, t.min);
	LayerText lt_hm = {time_str, frame_hm, GAlignCenter, U_ASCII_ARIALBD_30, 0};
	P_Layer layer_text_hm = app_layer_create_text(&lt_hm);
	app_layer_set_bg_color(layer_text_hm, GColorWhite);
	if(layer_text_hm != NULL)
	{
		app_layer_set_bg_color(layer_text_hm, GColorBlack);
		g_layer_id_hm = app_window_add_layer(p_window, layer_text_hm);
	}


	/*添加星期月图层*/
	GRect frame_wmd = {{LAYER_WMD_ORIGIN_X, LAYER_WMD_ORIGIN_Y}, {LAYER_WMD_SIZE_H, LAYER_WMD_SIZE_W}};
	memset(time_str, 0, sizeof(time_str));
	char wday[7][8]={"周日","周一","周二","周三","周四","周五","周六"}; 
	sprintf(time_str, "%s  %d月%d日", wday[t.wday], t.mon, t.mday);
	LayerText lt_wmd = { time_str,frame_wmd, GAlignCenter,  U_ASCII_ARIAL_14, 0};
	P_Layer layer_text_wmd = app_layer_create_text(&lt_wmd);
	if(layer_text_wmd != NULL)
	{
		app_layer_set_bg_color(layer_text_wmd, GColorBlack);
		g_layer_id_wmd = app_window_add_layer(p_window, layer_text_wmd);
	}


	
	/*添加天气图标*/
#ifdef LINUX
	app_persist_create( USER_APP_ID_WEATHER_WATCH, WEATHER_ICON_KEY, 1);
#else
	app_persist_create( WEATHER_ICON_KEY, 1);
#endif
	
	/*读取天气图标*/	
	int8_t icon_type = 0;
#ifdef LINUX
	app_persist_read_data( USER_APP_ID_WEATHER_WATCH, WEATHER_ICON_KEY, 0, (unsigned char *)&icon_type, sizeof(int8_t));
#else
	app_persist_read_data( WEATHER_ICON_KEY, 0, (unsigned char *)&icon_type, sizeof(int8_t));
#endif


	/*如果icon_type大于0，显示天气图标及温度*/
	if (icon_type >= 0)
	{
		/*添加天气图标*/
		GRect frame_icon = {{LAYER_WICON_ORIGIN_X, LAYER_WICON_ORIGIN_Y}, {LAYER_WICON_SIZE_H, LAYER_WICON_SIZE_W}};	
		GBitmap bitmap_icon;
		res_get_user_bitmap( weather_get_icon_key(icon_type), &bitmap_icon);
		LayerBitmap lb_icon = {bitmap_icon, frame_icon, GAlignLeft};
		P_Layer layer_bitmap_icon = app_layer_create_bitmap(&lb_icon);
		if(layer_bitmap_icon != NULL)
		{
			g_layer_id_icon = app_window_add_layer(p_window, layer_bitmap_icon);
		}

		
		/*添加温度*/
		GRect frame_temp = {{LAYER_TEMP_ORIGIN_X, LAYER_TEMP_ORIGIN_Y}, {LAYER_TEMP_SIZE_H, LAYER_TEMP_SIZE_W}};
		char temp_str[5] = "";
		int8_t temp_int = 0;
		#ifdef LINUX
			app_persist_read_data( USER_APP_ID_WEATHER_WATCH, WEATHER_TEMP_KEY, 0, (unsigned char *)&temp_int, sizeof(int8_t));
		#else
			app_persist_read_data( WEATHER_TEMP_KEY, 0, (unsigned char *)&temp_int, sizeof(int8_t));
		#endif

		if (temp_int == WEATHER_TEMP_SPEC)
		{
			strcpy(temp_str, "--");
		}
		else
		{
			sprintf(temp_str, "%d", temp_int);
		}
		LayerText lt_temp = {temp_str, frame_temp, GAlignRight, U_ASCII_ARIALBD_30, 0};
		P_Layer layer_text_temp = app_layer_create_text(&lt_temp);
		if(layer_text_temp != NULL)
		{
			app_layer_set_bg_color(layer_text_temp, GColorWhite);
			g_layer_id_temp = app_window_add_layer(p_window, layer_text_temp);
		}


	}



	return p_window;

}


void weather_comm_result_callback(enum ECommResult result, uint32_t comm_id, void *context)
{
	/*如果上一次请求GPS通讯失败，并且通讯ID相同，则重新发送*/
	if ((result == ECommResultFail) && (comm_id == g_comm_id_gps))
	{
		g_comm_id_gps = maibu_comm_request_phone(ERequestPhoneGPSAltitude,  NULL,0);
	}

	/*如果上一次请求WEB通讯失败，并且通讯ID相同，则重新发送*/
	if ((result == ECommResultFail) && (comm_id == g_comm_id_web))
	{
		weather_request_web();
	}

} 



int main()
{
#ifdef LINUX	
	/*非APP编写*/	
	screen_init(SCREEN_ROW_NUMS,SCREEN_COL_NUMS);
	os_store_manage_init();	
	window_stack_init();
#endif

	/*APP编写*/

	/*初始化天气温度及图标数据*/
	weather_init_store();
	/*创建消息列表窗口*/
	P_Window p_window = init_weather_window(); 
	/*放入窗口栈显示*/
	g_window_id = app_window_stack_push(p_window);
	/*注册接受WEB数据回调函数*/
	maibu_comm_register_web_callback(weather_web_recv_callback);
	/*注册接受手机数据回调函数*/
	maibu_comm_register_phone_callback(weather_phone_recv_callback);
	/*注册定时器，定时更新天气数据*/
	app_service_timer_subscribe(TIME_INTERVAL*60*1000, app_weather_watch_timer_callback, NULL);	
	/*注册一个事件通知回调，当有时间改变时，立即更新时间*/
	maibu_service_sys_event_subscribe(weather_time_change);
	/*请求GPS数据*/
	g_comm_id_gps = maibu_comm_request_phone(ERequestPhoneGPSAltitude,  NULL,0);
	/*注册通讯结果回调*/
	maibu_comm_register_result_callback(weather_comm_result_callback);


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
			static int s_index = 0;
			char buff[50] = "";
			sprintf(buff, "{\"temp\":\"200\",\"icon\":\"%d\"}", ((s_index++)%14 + 1));
			weather_web_recv_callback((const uint8_t *)buff, strlen(buff));

			//printf("buf0:%d, buf1:%d, buf2:%d, buf:%s\n", buff[0], buff[1], buff[2], &buff[3]);
	
		}
		else if (input == 'g')
		{
			SGpsAltitude g_gps_altitude;
			strcpy(g_gps_altitude.lon, "116.305145");
			strcpy(g_gps_altitude.lat, "39.982368");
			weather_phone_recv_callback(ERequestPhoneGPSAltitude, (void *)&g_gps_altitude);

		}
	
	}	

	SHOW;
	app_window_stack_pop(p_window);
	window_stack_destory();
	screen_destory();
	os_store_manage_destory();

	SHOW;

#endif

	return 0;

}


