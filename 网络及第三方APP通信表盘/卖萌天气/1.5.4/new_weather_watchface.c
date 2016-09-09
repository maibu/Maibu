
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "maibu_sdk.h"
#include "maibu_res.h"

static int32_t g_window_id             = -1;

static int8_t g_layer_id_waiting      = -1;
static int8_t g_layer_id_waiting_str  = -1;

static uint8_t g_callback_request_flag = 0;//标志是否是快速查询状态
static uint8_t g_auto_weather_flag     = 1;//标志是否自动查询天气
static char    get_city_context[35]    = {0x01};//协议ID
static uint32_t g_start_seconds		   = 0;//标志记录开始的秒数
static uint8_t g_start_seconds_flag    = 0;//标志记录是刚安装表盘还是切换表盘
static uint32_t g_comm_id_get_city      = 0;//获取城市的通讯ID
static uint32_t g_comm_id_web_weather   = 0;//获取天气的通讯ID


static char    g_city[20]              = {0};
static char    g_city_temp[20]         = {0};

static int32_t g_temperature           = 0;
static int32_t g_curPm                 = 0;
static int32_t g_level				   = 0;

static char weather_info[20] 	       = {0};

static uint32_t g_weather_icon_key     = 0;

static uint8_t g_request_count        = 0;

static char wday[7][8]={"周日","周一","周二","周三","周四","周五","周六"}; 

static char level_aqi_str[7][19] = {"暂无空气数据","空气质量优","空气质量良好","轻度污染","中度污染","重度污染","严重污染"};

static uint8_t g_request_timer		   = 0;

#define CITY_EMPTY					3

#define WEATHER_KEY			"temperature,pm25,info,level"

#define WEATHER_URL		"http://op.juhe.cn/onebox/weather/query?key=这里替换成你申请的KEY&cityname="


#define STR_PM25		"PM2.5"


#define WEATHER_CLOUDY		"多云"
#define WEATHER_FOG_1		"雾"
#define WEATHER_FOG_2		"霾"
#define WEATHER_OVERCAST	"阴"
#define WEATHER_RAIN		"雨"
#define WEATHER_SNOW		"雪"
#define WEATHER_SUNNY		"晴"

#define WAITING_FOR_WEATHER		"正在刷新天气"

#define JSON_ERROR			-1

#define CITY_PRESIST_DATA_KEY 1

P_Window init_window(void);

void show_waitings(P_Window p_window,bool show_status)
{
	maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_layer_id_waiting),show_status);
	maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_layer_id_waiting_str),show_status);
}

uint32_t get_weather_icon(char * str,int32_t day_or_night)
{
	uint32_t weather_icon_key = 0;
	if( strlen(str) == 0)
	{

	}
	else if(memcmp(str,WEATHER_SUNNY,strlen(WEATHER_SUNNY)) == 0 )
	{
		weather_icon_key = (day_or_night)?RES_BITMAP_WATCHFACE_WEATHER_SUNNY_DAY:RES_BITMAP_WATCHFACE_WEATHER_SUNNY_NIGHT;
	}
	else if(memcmp(str,WEATHER_OVERCAST,strlen(WEATHER_OVERCAST)) == 0 )
	{
		weather_icon_key = RES_BITMAP_WATCHFACE_WEATHER_OVERCAST;
	}
	else if(memcmp(str,WEATHER_CLOUDY,strlen(WEATHER_CLOUDY)) == 0 )
	{
		weather_icon_key = (day_or_night)?RES_BITMAP_WATCHFACE_WEATHER_CLOUDY_DAY:RES_BITMAP_WATCHFACE_WEATHER_CLOUDY_NIGHT;
	}
	else if((memcmp(str,WEATHER_FOG_1,strlen(WEATHER_FOG_1)) == 0 )||(memcmp(str,WEATHER_FOG_2,strlen(WEATHER_FOG_2)) == 0 ))
	{
		weather_icon_key = RES_BITMAP_WATCHFACE_WEATHER_FOG;
	}
	else
	{
		if(strstr(str,WEATHER_SNOW) != NULL)
		{
			weather_icon_key = RES_BITMAP_WATCHFACE_WEATHER_SNOW;
		}
		else
		{
			weather_icon_key = RES_BITMAP_WATCHFACE_WEATHER_RAIN;
		}
	}

	return weather_icon_key;
}


int32_t get_date_flag(void)
{
	struct date_time datetime;
	app_service_get_datetime(&datetime);

	if((datetime.hour >= 7)&&(datetime.hour < 20))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int32_t get_front_or_back_flag(void)
{
	if(NULL == app_window_stack_get_window_by_id(g_window_id))
		return 0;
	else
		return 1;
}

int32_t get_time_out_flag(uint32_t time_out_min)
{
	struct date_time t_now;
	uint32_t t_now_second;
	
	app_service_get_datetime(&t_now);
	t_now_second = app_get_time(&t_now);

	if((t_now_second - g_start_seconds) > time_out_min*60)
		return 1;
	else
		return 0;
}

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


//请求天气数据回调
void weather_info_callback(const uint8_t *buff,uint16_t size)
{
	char buffer[20] = {0};
	
	struct date_time tt;
	app_service_get_datetime(&tt);
	g_start_seconds = app_get_time(&tt);
	
	
	g_request_count = 0;//清空请求计数
	g_callback_request_flag = 0; //清空快速请求标志
	
	maibu_get_json_int(buff, "temperature", &g_temperature);
	maibu_get_json_int(buff, "level",&g_level);
	maibu_get_json_int(buff, "pm25", &g_curPm);
			
	if(maibu_get_json_str(buff, "info", buffer,sizeof(weather_info)) != JSON_ERROR )
	{//防止数据为空导致的数据清空
		memset(weather_info,0,sizeof(weather_info));
		maibu_get_json_str(buff, "info", weather_info,sizeof(weather_info));
		g_weather_icon_key = get_weather_icon(weather_info,get_date_flag());
	}
	else
	{
		return;
	}
		
	//刷新界面
	window_reloading();

}


//请求天气数据
void request_weather_info(char * city_name)
{
	char url[256] = {0};
	sprintf(url,"%s",WEATHER_URL);
			
	if(city_name != NULL)
	{
		int i = 0,j = 0;
		for(i;(city_name[i] != '\0')&&(j < sizeof(url)) ;++i)
		{
			sprintf(url,"%s%%%x",url,(unsigned char)city_name[i]);
		}
		
	}
	else if(strlen(city_name) < CITY_EMPTY)
	{
		request_get_city_info();
		return;
	}
		
	maibu_comm_register_web_callback(weather_info_callback);


	g_comm_id_web_weather = maibu_comm_request_web(url,WEATHER_KEY,5*60*get_front_or_back_flag());

}


//请求城市数据
void request_get_city_info(void)
{
	g_comm_id_get_city = maibu_comm_request_phone(ERequestPhoneSelfDefine,(void *)&get_city_context,34);
}


//请求城市数据回调
void get_city_info_callback(enum ERequestPhone  type,void * context, uint16_t context_size)
{
	if(type == ERequestPhoneSelfDefine)
	{	
		if(*(int16_t *)context == 1)
		{
			int8_t *context_city_name_point = (int8_t *)context + 4;	
					
			memcpy(g_city,context_city_name_point,20);
			g_city[19]='\0';
		
			request_weather_info(g_city);
		}
	}
}



void time_change(enum SysEventType type, void *context)
{
	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		window_reloading();
	}
}

int32_t set_temp_layerText(LayerText* temp_LayerText,char * str,GRect rect,enum GAlign alignment, uint8_t font_type,P_Window p_window,P_Layer p_layer,enum GColor color)
{
	temp_LayerText->text = str;
	temp_LayerText->frame = rect;
	temp_LayerText->alignment = alignment;
	temp_LayerText->font_type = font_type;
	
	p_layer = app_layer_create_text(temp_LayerText);
	
	if(p_layer != NULL)
	{
		app_layer_set_bg_color(p_layer, color);
		return app_window_add_layer(p_window, p_layer);
	}
	return 0;
}

/*创建并显示图层，需要坐标值，得到icon_key的数组，数组需要的参数值，P_Window*/
int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,int32_t bmp_array_name_key)
{	
	int32_t g_layer_id_temporary;

	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_array_name_key, &bmp_point);
	LayerBitmap layer_bitmap_struct_l = {bmp_point, *temp_p_frame, GAlignLeft};
 	temp_P_Layer = app_layer_create_bitmap(&layer_bitmap_struct_l);
	
	if(temp_P_Layer != NULL)
	{
		app_layer_set_bg_color(temp_P_Layer, GColorWhite);
		g_layer_id_temporary= app_window_add_layer(p_window, temp_P_Layer);
	}

	return g_layer_id_temporary;
}

P_Window init_window(void)
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}
	
	LayerText temp_LayerText = {0};
	temp_LayerText.bound_width = 0;
	
	GBitmap temp_GBitmap = {0};
	P_Layer temp_P_Layer = NULL;

	/*添加背景图片图层*/
	GRect temp_frame = {{0, 0}, {128, 128}};
	uint32_t *temp_frame_p = (uint32_t *)&temp_frame;
	
	display_target_layer(p_window,&temp_frame,RES_BITMAP_WATCHFACE_WEATHER_BG);
   
	//添加天气图片
	//temp_frame = {{0, 28}, {60, 80}};
	*temp_frame_p = 0x503c1c00;
	g_weather_icon_key = get_weather_icon(weather_info,get_date_flag());
	display_target_layer(p_window,&temp_frame,g_weather_icon_key);

	char buffer[24] = {0};
	/*添加温度图层*/
	//temp_frame = {{71, 52}, {30, 46}};
	*temp_frame_p = 0x2e1e3447;
	sprintf(buffer, "%d",g_temperature);
	set_temp_layerText(&temp_LayerText,buffer, temp_frame, GAlignRight, U_ASCII_ARIAL_30,p_window,temp_P_Layer,GColorWhite);


	/*添加空气质量指数标识图层*/
	//temp_frame = {{91,93},{12,36}};
	*temp_frame_p = 0x240c5d5b;
	char * pm_str = STR_PM25;
	set_temp_layerText(&temp_LayerText,pm_str,temp_frame,GAlignRight,U_ASCII_ARIAL_12,p_window,temp_P_Layer,GColorBlack);
	
	
	/*添加空气质量指数图层*/
	//temp_frame = {{87,103},{24,40}};
	*temp_frame_p = 0x28186757;
	sprintf(buffer,"%d",g_curPm);
	set_temp_layerText(&temp_LayerText,buffer,temp_frame,GAlignRight,U_ASCII_ARIAL_24,p_window,temp_P_Layer,GColorBlack);
	
	
	/*添加空气质量图层*/
	//temp_frame = {{4,112},{12,78}};
	*temp_frame_p = 0x4e0c7004;
	sprintf(buffer,"%s",level_aqi_str[g_level]);
	set_temp_layerText(&temp_LayerText,buffer,temp_frame,GAlignLeft,U_ASCII_ARIAL_12,p_window,temp_P_Layer,GColorBlack);
	
	/*添加城市名称图层*/
	//temp_frame = {{12,93},{14,82}};
	*temp_frame_p = 0x500e5d0c;
	sprintf(buffer,"%s",g_city);
	set_temp_layerText(&temp_LayerText,buffer,temp_frame,GAlignLeft,U_ASCII_ARIAL_14,p_window,temp_P_Layer,GColorBlack);
	

	/*添加等待更新图片图层*/
	//temp_frame = {{0, 28}, {100, 128}};
	*temp_frame_p = 0x80641c00;
	g_layer_id_waiting = display_target_layer(p_window,&temp_frame,RES_BITMAP_WATCHFACE_WEATHER_UPDATING);

	/*添加等待更新提示图层*/
	//temp_frame = {{0,101},{14,128}};
	*temp_frame_p = 0x800e6500;
	//g_waiting_str = (g_waiting_str == NULL)?(WAITING_FOR_GPS):(g_waiting_str);//保持上一次的提示
	g_layer_id_waiting_str = set_temp_layerText(&temp_LayerText,WAITING_FOR_WEATHER, temp_frame, GAlignCenter, U_ASCII_ARIAL_14,p_window,temp_P_Layer,GColorBlack);
	
	if(g_weather_icon_key != 0)
	{	
		show_waitings(p_window,false);
	}
	
	struct date_time t;
	app_service_get_datetime(&t);

	/*添加小时分钟图层*/
	//temp_frame = {{73,2}, {20,53}};
	*temp_frame_p = 0x35140249;
	
	sprintf(buffer, "%02d:%02d", t.hour, t.min);
	set_temp_layerText(&temp_LayerText,buffer, temp_frame, GAlignRight, U_ASCII_ARIALBD_20,p_window,temp_P_Layer,GColorWhite);


	/*添加星期月图层*/
	//temp_frame = {{4, 4}, {12, 64}};
	*temp_frame_p = 0x400c0404;
//	memset(buffer, 0, sizeof(buffer));
	
	sprintf(buffer, "%02d-%02d %s", t.mon, t.mday, wday[t.wday]);
	set_temp_layerText(&temp_LayerText, buffer,temp_frame, GAlignLeft,	U_ASCII_ARIAL_12,p_window,temp_P_Layer,GColorWhite);

	return p_window;
}
 

void app_weather_update_timer_callback(date_time_t tick_time, uint32_t millis, void *context)
{	
	if((get_time_out_flag(58))&&(g_callback_request_flag == 0))
	{
		//正常查询状态请求次数大于5次就显示提示信息
		//清空历史数据
		g_weather_icon_key = 0;
		memset(weather_info,0,sizeof(weather_info));
	
		/*根据窗口ID获取窗口句柄*/
		P_Window p_window = app_window_stack_get_window_by_id(g_window_id); 
		if (NULL != p_window)
		{
			show_waitings(p_window,true);
			app_window_update(p_window);
		}
		
	}
	else if((g_request_count >= 12)&&(g_callback_request_flag == 1))
	{
		//快速查询的状态下大于12次(1分钟)就退出快速查询
		g_request_count = 0;
		g_callback_request_flag = 0;
	}
	if(g_callback_request_flag == 0)
	{
		app_service_timer_unsubscribe(g_request_timer);
		g_request_timer = app_service_timer_subscribe((30-(15*get_front_or_back_flag()))*60*1000, app_weather_update_timer_callback, NULL);
	
	}
	//请求城市
	if(g_auto_weather_flag == 1)
		request_get_city_info();
	else
		request_weather_info(g_city);
	
	g_request_count++;
}

#if 1
void weather_comm_result_callback(enum ECommResult result, uint32_t comm_id, void *context)
{
	/*如果上一次请求GPS通讯失败，并且通讯ID相同，则重新发送*/
	if (result == ECommResultFail) 
	{
		if (comm_id == g_comm_id_get_city)
		{
			g_comm_id_get_city = maibu_comm_request_phone(ERequestPhoneSelfDefine,(void *)&get_city_context,34);
		}

		/*如果上一次请求WEB通讯失败，并且通讯ID相同，则重新发送*/
		if (comm_id == g_comm_id_web_weather)
		{
			request_weather_info(g_city);
		
		}
	}
	
}

#endif

void watchapp_comm_callback(enum ESyncWatchApp type, uint8_t *buf, uint16_t len)
{
	
	if(type == ESyncWatchAppUpdateParam)
    {

    	P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
		if (NULL != p_window)
		{
			show_waitings(p_window,true);
			app_window_update(p_window);
		}
	    	
		if((len >= 4) && (len <=20))
		{
			g_auto_weather_flag = 0;
			g_weather_icon_key = 0;
			memset(weather_info,0,sizeof(weather_info));
			
			memcpy(g_city,buf,len);
			g_city[len]='\0';
	
			app_persist_write_data_extend(CITY_PRESIST_DATA_KEY, g_city, sizeof(g_city));
			//根据城市信息获取天气信息
			request_weather_info(g_city);
			
		}
		else//恢复自动查询城市
		{
			app_persist_delete_data(CITY_PRESIST_DATA_KEY);
		
			g_auto_weather_flag = 1;
			g_weather_icon_key = 0;
			memset(weather_info,0,sizeof(weather_info));
			memset(g_city,0,sizeof(g_city));
			request_get_city_info();
		}
	}
}
void init_register_handle(void)
{
	//创建空间储存城市配置
	app_persist_create(CITY_PRESIST_DATA_KEY, sizeof(g_city));
	//读取城市配置
	memset(g_city_temp,0,sizeof(g_city_temp));
	app_persist_read_data(CITY_PRESIST_DATA_KEY, 0, g_city_temp, sizeof(g_city_temp));

	uint32_t request_time = 15*60*1000;//默认15分钟请求数据
	
	//是安装表盘的情况,则记录时间,并设置快速查询
	if(g_start_seconds_flag == 0)
	{
		struct date_time tt1;
		app_service_get_datetime(&tt1);
		g_start_seconds = app_get_time(&tt1);
			
		request_time = 5 * 1000;//在没有获取过数据的情况下5秒获取一次数据
		g_callback_request_flag = 1;//设置快速查询标志
	}
	g_start_seconds_flag = 1;

	//注册手机设置回调函数
	maibu_comm_register_watchapp_callback(watchapp_comm_callback);
	
	//注册接受请求城市数据回调函数
	maibu_comm_register_phone_callback(get_city_info_callback);

	/*注册一个事件通知回调，当有时间改变时，立即更新时间*/
	maibu_service_sys_event_subscribe(time_change);

	/*注册通讯结果回调*/
	maibu_comm_register_result_callback(weather_comm_result_callback);
	
	//判断是否自动查询
	if(strlen(g_city_temp) == 0)
	{
		g_auto_weather_flag = 1;
		if(get_time_out_flag(5)||(g_weather_icon_key == 0))
		{
			
			request_get_city_info();
		}
	}
	else 
	{
		
		g_auto_weather_flag = 0;
		if(get_time_out_flag(5)||(g_weather_icon_key == 0))
		{
			
			memcpy(g_city,g_city_temp,sizeof(g_city_temp));
			request_weather_info(g_city);
		}
	}

	//注册定时查询函数
	g_request_timer = app_service_timer_subscribe(request_time, app_weather_update_timer_callback, NULL);	
	
}


int main()
{
	/*创建显示窗口*/
	P_Window p_window = init_window(); 
	/*放入窗口栈显示*/
	g_window_id = app_window_stack_push(p_window);

	init_register_handle();

	return 0;

}















