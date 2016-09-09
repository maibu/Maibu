#include "maibu_sdk.h"
#include "maibu_res.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int32_t g_window_id             = -1;
static int32_t g_layer_id_waiting      = -1;
static int32_t g_layer_id_noweather    = -1;

static uint32_t g_callback_request_flag = 0;//用来标志是否是快速查询状态
static char get_city_context[35]	   = {0x01};//协议ID
static uint32_t g_start_seconds		   = 0;//标志记录开始的秒数
static uint32_t g_start_seconds_flag   = 0;//标志记录是刚安装表盘还是切换表盘


static char    g_city[20]              = {0};

static uint32_t g_comm_id_get_city     = 0;
static uint32_t g_comm_id_web_city     = 0;
static uint32_t g_comm_id_web_weather  = 0;

static int32_t g_temperature           = 0;
static int32_t g_curPm                 = 0;
static int32_t g_level				   = 0;
static char weather_info[20] 	       = {0};


static uint32_t g_weather_icon_key     = 0;

static uint32_t g_airquality_icon_key  = 0;

static uint32_t g_request_count        = 0;

static uint8_t common_timer            = 0;


#define CITY_EMPTY					3




#define WEATHER_KEY			"temperature,pm25,info,level"
#define WEATHER_URL			"http://op.juhe.cn/onebox/weather/query?key=这里替换成你申请的KEY&cityname="

#define WEATHER_CLOUDY		"多云"
#define WEATHER_FOG_1		"雾"
#define WEATHER_FOG_2		"霾"
#define WEATHER_OVERCAST	"阴"
#define WEATHER_RAIN		"雨"
#define WEATHER_SNOW		"雪"
#define WEATHER_SUNNY		"晴"

#define JSON_ERROR			-1


//小号数字数组
uint32_t get_LIT_icon_key[12] =
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
	RES_BITMAP_WATCHFACE_NUMBER_LIT_FUHAO,
	RES_BITMAP_WATCHFACE_NUMBER_LIT_DUHAO
};
//大号数字数组
uint32_t get_BIG_icon_key[10] =
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
//空气质量图层
uint32_t get_airquality_icon_array[7] = 
{	
	0,
	RES_BITMAP_WATCHFACE_AIRQUALITY_GOOD,
	RES_BITMAP_WATCHFACE_AIRQUALITY_MODERATE,
	RES_BITMAP_WATCHFACE_AIRQUALITY_LIGHTLY_POLLUTED,
	RES_BITMAP_WATCHFACE_AIRQUALITY_MODERATELY_POLLUTED,
	RES_BITMAP_WATCHFACE_AIRQUALITY_HEAVILY_POLLUTED,
	RES_BITMAP_WATCHFACE_AIRQUALITY_SEVERELY_POLLUTED
};
//等待更新图层
uint32_t get_others_icon_array[4] = 
{
	RES_BITMAP_WATCHFACE_WEATHER_BG,
	RES_BITMAP_WATCHFACE_WEATHER_WAITING,
	RES_BITMAP_WATCHFACE_WEATHER_NOWEATHER,
	0
};

#define OTHER_ICON_ARRAY_BG_ICON_KEY 0
#define OTHER_ICON_ARRAY_WAITING_ICON_KEY 1
#define OTHER_ICON_ARRAY_NOWEATHER_ICON_KEY 2
#define OTHER_ICON_ARRAY_WEATHER_ICON_KEY 3

P_Window init_window(void);


void show_waitings(P_Window p_window,bool show_status)
{
	
	maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_layer_id_waiting),show_status);
	maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_layer_id_noweather),show_status);
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

int32_t get_date_flag(void)
{
	struct date_time datetime;
	app_service_get_datetime(&datetime);

	if((datetime.hour >= 6)&&(datetime.hour < 18))
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



uint32_t get_weather_icon(char * str)
{
	int32_t day_or_night = get_date_flag();
	uint32_t weather_icon_key = 0;
	if(strlen(str) == 0)
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



/*创建并显示图层，需要坐标X值，得到icon_key的数组，数组需要的参数值，P_Window*/
int32_t display_target_layer(GRect temp_frame, int target_wei_value,P_Window p_window,uint32_t get_target_icon_array[],enum GAlign how_to_align)
{	
	int32_t g_layer_id_temporary;
	GBitmap temp_GBitmap = {0};
	P_Layer temp_P_Layer = NULL;
		
	res_get_user_bitmap(get_target_icon_array[target_wei_value], &temp_GBitmap);
	LayerBitmap lb_curPM_bai_icon = {temp_GBitmap, temp_frame, how_to_align};
	temp_P_Layer = app_layer_create_bitmap(&lb_curPM_bai_icon);
	if(temp_P_Layer != NULL)
	{
		g_layer_id_temporary= app_window_add_layer(p_window, temp_P_Layer);
	}
	
	return g_layer_id_temporary;
}


/*分析curPM数据，并显示*/
void check_display_curPM_number(P_Window p_window)
{
	uint32_t curPM_wholesize_w = 7;

	GRect temp_frame;
	temp_frame.origin.x = 2;
	temp_frame.origin.y = 113;
	temp_frame.size.h = 10;
	temp_frame.size.w = 7;

	if(g_curPm >=10)
	{
		curPM_wholesize_w = curPM_wholesize_w + 7;
	}
	if(g_curPm >=100)
	{
		curPM_wholesize_w = curPM_wholesize_w + 7;
	}
	temp_frame.origin.x = temp_frame.origin.x + (25 - curPM_wholesize_w)/2;

	if(g_curPm >=100)
	{	
		display_target_layer(temp_frame,((g_curPm/100)%10),p_window,&get_LIT_icon_key[0],GAlignCenter);
		temp_frame.origin.x = temp_frame.origin.x + 7;
	}
	if(g_curPm >=10)
	{	
		display_target_layer(temp_frame,((g_curPm/10)%10),p_window,&get_LIT_icon_key[0],GAlignCenter);
		temp_frame.origin.x = temp_frame.origin.x + 7;
	}
	
	display_target_layer(temp_frame,(g_curPm%10),p_window,&get_LIT_icon_key[0],GAlignCenter);
}

/*分析temperature数据，并显示*/
void check_display_temperature_number(P_Window p_window)
{
	uint32_t temperature_wholesize_w = 7;
	
	GRect temp_frame;
	temp_frame.origin.x = 99;
	temp_frame.origin.y = 113;
	temp_frame.size.h = 10;
	temp_frame.size.w = 7;
	
	if(g_temperature >= 10)
	{
		temperature_wholesize_w = temperature_wholesize_w + 7;
	}
	if(g_temperature <= 0)
	{
		temperature_wholesize_w = temperature_wholesize_w + 7;
	}
	if(g_temperature <= (-10))
	{
		temperature_wholesize_w = temperature_wholesize_w + 7;
	}
	temp_frame.origin.x = temp_frame.origin.x + (27 - 6 - temperature_wholesize_w)/2;

	if(g_temperature >= 0)
	{
		if(g_temperature >= 10)
		{	
			display_target_layer(temp_frame,((g_temperature/10)%10),p_window,&get_LIT_icon_key[0],GAlignCenter);
			temp_frame.origin.x = temp_frame.origin.x + 7;
		}

		display_target_layer(temp_frame,(g_temperature%10),p_window,&get_LIT_icon_key[0],GAlignCenter);
	}
	else
	{	
		
		display_target_layer(temp_frame,10,p_window,&get_LIT_icon_key[0],GAlignCenter);
		temp_frame.origin.x = temp_frame.origin.x + 7;
			
		if(g_temperature <= (-10))
		{	
			display_target_layer(temp_frame,((g_temperature*(-1)/10)%10),p_window,&get_LIT_icon_key[0],GAlignCenter);
			temp_frame.origin.x = temp_frame.origin.x + 7;
		}

		display_target_layer(temp_frame,(g_temperature*(-1)%10),p_window,&get_LIT_icon_key[0],GAlignCenter);
	}
	
	temp_frame.origin.x = temp_frame.origin.x + 6;
	display_target_layer(temp_frame,11,p_window,&get_LIT_icon_key[0],GAlignCenter);
	
}



/*添加小时分钟图层*/

void check_display_time_number(P_Window p_window)
{
	struct date_time datetime;
	app_service_get_datetime(&datetime);
		
	GRect temp_frame;

//hour
	temp_frame.origin.x = 25;
	temp_frame.origin.y = 83;
	temp_frame.size.h = 22;
	temp_frame.size.w = 18;
	display_target_layer(temp_frame,(datetime.hour / 10),p_window,&get_BIG_icon_key[0],GAlignRight);

	temp_frame.origin.x = 43;
	display_target_layer(temp_frame,(datetime.hour % 10),p_window,&get_BIG_icon_key[0],GAlignRight);
	
//min
	temp_frame.origin.x = 68;
	
	display_target_layer(temp_frame,(datetime.min / 10),p_window,&get_BIG_icon_key[0],GAlignLeft);


	temp_frame.origin.x = 86;
	
	display_target_layer(temp_frame,(datetime.min % 10),p_window,&get_BIG_icon_key[0],GAlignLeft);
	

	
}


P_Window init_window(void)
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	/*添加背景图片图层*/
	GRect temp_frame = {{0, 0}, {128, 128}};
	display_target_layer(temp_frame,OTHER_ICON_ARRAY_BG_ICON_KEY,p_window,get_others_icon_array,GAlignLeft);
		
	/*添加小时分钟图层*/
	check_display_time_number(p_window);
	
	//添加天气图片图层
	temp_frame.origin.x = 0;
	temp_frame.origin.y = 0;
	temp_frame.size.h = 76;
	temp_frame.size.w = 128;

	
	g_weather_icon_key = get_weather_icon(weather_info);
	get_others_icon_array[OTHER_ICON_ARRAY_WEATHER_ICON_KEY] = g_weather_icon_key;
	
	display_target_layer(temp_frame,OTHER_ICON_ARRAY_WEATHER_ICON_KEY,p_window,get_others_icon_array,GAlignCenter);

	//添加待更新天气图层
	g_layer_id_noweather = display_target_layer(temp_frame,OTHER_ICON_ARRAY_NOWEATHER_ICON_KEY,p_window,get_others_icon_array,GAlignCenter);

	
	/*添加温度图层*/
	check_display_temperature_number(p_window);	
	
	/*添加空气质量指数图层*/
	check_display_curPM_number(p_window);
		
	
	/*添加空气质量图层*/
	temp_frame.origin.x = 28;
	temp_frame.origin.y = 112;
	temp_frame.size.h = 11;
	temp_frame.size.w = 63;
	
	display_target_layer(temp_frame,g_level,p_window,get_airquality_icon_array,GAlignCenter);

	
	/*添加等待更新图片图层*/
    temp_frame.origin.x = 2;
    temp_frame.origin.y = 109;
    temp_frame.size.h = 17;
    temp_frame.size.w = 124;
	
	g_layer_id_waiting = display_target_layer(temp_frame,OTHER_ICON_ARRAY_WAITING_ICON_KEY,p_window,get_others_icon_array,GAlignLeft);



	
    if(g_weather_icon_key != 0)
	{
		show_waitings(p_window,false);
    }

	return p_window;
}


//请求天气数据回调
void weather_info_callback(const uint8_t *buff,uint16_t size)
{
	
	char buffer[40] = {0};

	struct date_time tt;
	app_service_get_datetime(&tt);
	g_start_seconds = app_get_time(&tt);

	
	g_request_count = 0;//清空请求计数
	g_callback_request_flag = 0; //清空快速查询标志

	
	if(maibu_get_json_str(buff, "info", buffer,sizeof(weather_info)) != JSON_ERROR )
	{
		//防止数据为空导致的数据清空
		maibu_get_json_str(buff, "info", weather_info,sizeof(weather_info));
		g_weather_icon_key = get_weather_icon(weather_info);
		
	}
	else
	{	
		return;
	}
	maibu_get_json_int(buff, "temperature", &g_temperature);
	maibu_get_json_int(buff, "pm25", &g_curPm);
	maibu_get_json_int(buff, "level",&g_level);

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

//请求城市数据回调
void get_city_info_callback(enum ERequestPhone  type,void * context)
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

//请求城市数据
void request_get_city_info()
{
	g_comm_id_get_city = maibu_comm_request_phone(ERequestPhoneSelfDefine,(void *)&get_city_context,34);
}


void  time_change (enum SysEventType type, void *context)
{
	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		window_reloading();
	}
}

void app_weather_update_timer_callback(date_time_t tick_time, uint32_t millis, void *context)
{	

	
	if((get_time_out_flag(55))&&(g_callback_request_flag == 0))
    {
    	//正常查询状态请求时间大于一小时就显示提示信息
		//清空历史数据

		g_weather_icon_key = 0;
		memcpy(weather_info,0,sizeof(weather_info));

		/*根据窗口ID获取窗口句柄*/
		P_Window p_window = app_window_stack_get_window_by_id(g_window_id); 
		if (NULL != p_window)
		{
			show_waitings(p_window,true);
			app_window_update(p_window);
		}
		
	}
	else if((g_request_count >= 12)&&(g_callback_request_flag == 1))
	{   //快速查询的状态下大于12次(1分钟)就退出快速查询
        
        g_request_count = 0;
		g_callback_request_flag = 0;
	}
		    	
	if(g_callback_request_flag == 0)
	{
       	app_service_timer_unsubscribe(common_timer);
	   	common_timer = app_service_timer_subscribe((30-(15*get_front_or_back_flag()))*60*1000, app_weather_update_timer_callback, NULL);
	}
	

	request_get_city_info();
	
	g_request_count++;
}

#if 1
void weather_comm_result_callback(enum ECommResult result, uint32_t comm_id, void *context)
{
	/*如果上一次请求城市通讯失败，并且通讯ID相同，则重新发送*/
		if ((result == ECommResultFail) && (comm_id == g_comm_id_get_city))
		{
			
			if(strlen(g_city)>= CITY_EMPTY)
			{
				request_weather_info(g_city);
			}
			else
			{
				g_comm_id_get_city = maibu_comm_request_phone(ERequestPhoneSelfDefine,(void *)&get_city_context,34);
			}
		}

		/*如果上一次请求WEB通讯失败，并且通讯ID相同，则重新发送*/
		if ((result == ECommResultFail) && (comm_id == g_comm_id_web_weather))
		{
	
			request_weather_info(g_city);
		}
	
}
#endif

int main(void)
{
	
//	simulator_init();
	/*创建消息列表窗口*/
	
	P_Window p_window = init_window(); 
	/*放入窗口栈显示*/
	g_window_id = app_window_stack_push(p_window);
	
		
	/*注册接受手机数据回调函数*/
	maibu_comm_register_phone_callback(get_city_info_callback);

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
	

	//判断是否距离上次成功获得数据超过5分钟
	if(get_time_out_flag(5)||(g_weather_icon_key == 0))
	{
		//请求GPS数据并注册GPS数据请求超时回调
		request_get_city_info();
	}

	
	//注册定时查询函数
	if(common_timer == 0)
	{
    	common_timer = app_service_timer_subscribe(request_time, app_weather_update_timer_callback, NULL);
	}
	
	/*注册一个事件通知回调，当有时间改变时，立即更新时间*/
    maibu_service_sys_event_subscribe(time_change);
	/*注册通讯结果回调*/
    maibu_comm_register_result_callback(weather_comm_result_callback);
	

//	simulator_wait();
	
	return 0;

}















