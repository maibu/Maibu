#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "maibu_sdk.h"
#include "maibu_res.h"



/*标志位*/

//窗口ID
static int32_t g_window_id = -1;
//刷新图片图层ID
static int32_t g_layer_id_refreshing = -1;
//温度宽度
static int32_t g_temperature_width = -1;
//天气图片KEY
static int32_t g_weather_bmp_key = -1;

//表盘的记录的当天日期，用于比较日期是否变为第二天
static uint8_t g_today_num = -1;

static uint32_t g_callback_request_flag = 0;//用来标志是否是快速查询状态
static char 	get_city_context[35]    = {0x01};//协议ID
static uint32_t g_start_seconds		    = 0;//标志记录开始的秒数
static int32_t  g_start_seconds_flag    = 0;//标志记录是刚安装表盘还是切换表盘
static char		common_timer            = 0;//定时器id
static uint32_t g_comm_id_get_city      = 0;//获取城市的通讯ID
static uint32_t g_comm_id_web_weather   = 0;//获取天气的通讯ID
static uint32_t g_request_count         = 0;//请求次数
static char		weather_info[20] 	    = {0};//天气数据


/*背景图片图层位置*/
#define SHOW_MORE_BG_ORIGIN_X			0	
#define SHOW_MORE_BG_ORIGIN_Y			0
#define SHOW_MORE_BG_SIZE_H				128	
#define SHOW_MORE_BG_SIZE_W				128	

/*海拔文本图层位置*/
#define SHOW_MORE_ALTITUDE_ORIGIN_X		15	
#define SHOW_MORE_ALTITUDE_ORIGIN_Y		1
#define SHOW_MORE_ALTITUDE_SIZE_H		12	
#define SHOW_MORE_ALTITUDE_SIZE_W		54	

/*电池剩余百分比文本图层位置*/
#define SHOW_MORE_BATTERY_PERCENT_ORIGIN_X		83	
#define SHOW_MORE_BATTERY_PERCENT_ORIGIN_Y		1
#define SHOW_MORE_BATTERY_PERCENT_SIZE_H		12	
#define SHOW_MORE_BATTERY_PERCENT_SIZE_W		24	

/*时间文本图层位置*/
#define SHOW_MORE_TIME_ORIGIN_X			0	
#define SHOW_MORE_TIME_ORIGIN_Y			14
#define SHOW_MORE_TIME_SIZE_H			30	
#define SHOW_MORE_TIME_SIZE_W			128	


/*月日周文本图层位置*/
#define SHOW_MORE_M_D_W_ORIGIN_X		0	
#define SHOW_MORE_M_D_W_ORIGIN_Y		46
#define SHOW_MORE_M_D_W_SIZE_H			12	
#define SHOW_MORE_M_D_W_SIZE_W			128	


/*农历文本图层位置*/
#define SHOW_MORE_LUNAR_ORIGIN_X		0	
#define SHOW_MORE_LUNAR_ORIGIN_Y		61
#define SHOW_MORE_LUNAR_SIZE_H			12	
#define SHOW_MORE_LUNAR_SIZE_W			128	


/*步数文本图层位置*/
#define SHOW_MORE_STEPS_ORIGIN_X		89	
#define SHOW_MORE_STEPS_ORIGIN_Y		90
#define SHOW_MORE_STEPS_SIZE_H			12	
#define SHOW_MORE_STEPS_SIZE_W			36	

/*楼层文本图层位置*/
#define SHOW_MORE_FLOORS_ORIGIN_X		93	
#define SHOW_MORE_FLOORS_ORIGIN_Y		114
#define SHOW_MORE_FLOORS_SIZE_H			12	
#define SHOW_MORE_FLOORS_SIZE_W			32	

/*天气刷新图片图层位置*/
#define SHOW_MORE_REFRESHING_ORIGIN_X	5
#define SHOW_MORE_REFRESHING_ORIGIN_Y	83
#define SHOW_MORE_REFRESHING_SIZE_H		41	
#define SHOW_MORE_REFRESHING_SIZE_W		83

/*天气图片图层位置*/
#define SHOW_MORE_WEATHER_ORIGIN_X		5	
#define SHOW_MORE_WEATHER_ORIGIN_Y		83
#define SHOW_MORE_WEATHER_SIZE_H		40
#define SHOW_MORE_WEATHER_SIZE_W		40	

/*PM2.5文本图层位置*/
#define SHOW_MORE_PM2_5_ORIGIN_X		51	
#define SHOW_MORE_PM2_5_ORIGIN_Y		90
#define SHOW_MORE_PM2_5_SIZE_H			12
#define SHOW_MORE_PM2_5_SIZE_W			24	

/*温度文本图层位置*/
#define SHOW_MORE_TEMPERATURE_ORIGIN_X	51	
#define SHOW_MORE_TEMPERATURE_ORIGIN_Y	107
#define SHOW_MORE_TEMPERATURE_SIZE_H	20
#define SHOW_MORE_TEMPERATURE_SIZE_W	38	

/*度号文本图层位置*/
#define SHOW_MORE_DU_HAO_ORIGIN_X		(SHOW_MORE_TEMPERATURE_ORIGIN_X+g_temperature_width+1)
#define SHOW_MORE_DU_HAO_ORIGIN_Y		109
#define SHOW_MORE_DU_HAO_SIZE_H			7
#define SHOW_MORE_DU_HAO_SIZE_W			7	


/*初始数据*/

//天气相关数据
#define WEATHER_KEY			"temperature,pm25,info"
#define WEATHER_URL			"http://op.juhe.cn/onebox/weather/query?key=这里替换成你申请的KEY&cityname="

#define WEATHER_CLOUDY		"多云"
#define WEATHER_FOG_1		"雾"
#define WEATHER_FOG_2		"霾"
#define WEATHER_OVERCAST	"阴"
#define WEATHER_RAIN		"雨"
#define WEATHER_SNOW		"雪"
#define WEATHER_SUNNY		"晴"

#define CITY_EMPTY			3
#define JSON_ERROR			-1

static char    g_city[20]   = {0};

//字符串
//电量百分比
char g_show_more_str_battery_percent[4] = {0};
//海拔高度
char g_show_more_str_altitude[8] = {"0"};
//时间
char g_show_more_str_time[6] = {"0"};
//月日星期
char g_show_more_str_month_day_week[21] = {"0"};
//农历月日
char g_show_more_str_lunar[13] = {0};
//步数
char g_show_more_str_steps[6] = {0};
//楼层
char g_show_more_str_floors[5] = {0};
//pm2.5
char g_show_more_str_PM2_5[4] = {0};
//温度
char g_show_more_str_temperature[4] = {0};

/*其他图片数组*/
static int32_t bmp_array_name_key[4]=
{
	RES_BITMAP_WATCHFACE_WEATHER_BG,
	RES_BITMAP_WATCHFACE_WEATHER_REFRESHING,
	RES_BITMAP_WATCHFACE_WEATHER_DU_HAO,
	0
};

static char wday_str[7][11]={"星期天","星期一","星期二","星期三","星期四","星期五","星期六"}; 

P_Window init_window(void);
void request_weather_info(char * city_name);


int32_t get_date_flag(void)
{

	struct date_time datetime;
	app_service_get_datetime(&datetime);

	if((datetime.hour >= 7)&&(datetime.hour < 19))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint32_t get_weather_icon(char * str)
{
	uint32_t weather_icon_key = 0;
	if(strlen(str) == 0)
	{

	}
	else if(memcmp(str,WEATHER_SUNNY,strlen(WEATHER_SUNNY)) == 0 )
	{
		weather_icon_key = RES_BITMAP_WATCHFACE_WEATHER_SUNNY;
	}
	else if(memcmp(str,WEATHER_OVERCAST,strlen(WEATHER_OVERCAST)) == 0 )
	{
		weather_icon_key = RES_BITMAP_WATCHFACE_WEATHER_OVERCAST;
	}
	else if(memcmp(str,WEATHER_CLOUDY,strlen(WEATHER_CLOUDY)) == 0 )
	{
		weather_icon_key = RES_BITMAP_WATCHFACE_WEATHER_CLOUDY;
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

/*创建并显示图片图层，需要坐标值，得到icon_key的数组，数组需要的参数值，P_Window*/
int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,enum GAlign how_to_align,enum GColor black_or_white,int32_t bmp_array_name_key[],int bmp_id_number)
{	

	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_array_name_key[bmp_id_number], &bmp_point);
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

P_Window init_window(void)
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	//添加背景图层
	GRect temp_frame ={{SHOW_MORE_BG_ORIGIN_X,SHOW_MORE_BG_ORIGIN_Y},
						{SHOW_MORE_BG_SIZE_H,SHOW_MORE_BG_SIZE_W}};
	
	display_target_layer(p_window,&temp_frame,GAlignLeft,GColorWhite,bmp_array_name_key,0);

	//添加海拔文本图层
	temp_frame.origin.x = SHOW_MORE_ALTITUDE_ORIGIN_X;
	temp_frame.origin.y = SHOW_MORE_ALTITUDE_ORIGIN_Y;
	temp_frame.size.h = SHOW_MORE_ALTITUDE_SIZE_H;
	temp_frame.size.w = SHOW_MORE_ALTITUDE_SIZE_W;
	
	display_target_layerText(p_window,&temp_frame,GAlignLeft,GColorWhite,g_show_more_str_altitude,U_ASCII_ARIAL_12);

	//电池剩余百分比文本图层
	temp_frame.origin.x = SHOW_MORE_BATTERY_PERCENT_ORIGIN_X;
	temp_frame.origin.y = SHOW_MORE_BATTERY_PERCENT_ORIGIN_Y;
	temp_frame.size.h = SHOW_MORE_BATTERY_PERCENT_SIZE_H;
	temp_frame.size.w = SHOW_MORE_BATTERY_PERCENT_SIZE_W;
	
	display_target_layerText(p_window,&temp_frame,GAlignRight,GColorWhite,&g_show_more_str_battery_percent,U_ASCII_ARIAL_12);
	

	//添加时间文本图层
	temp_frame.origin.x = SHOW_MORE_TIME_ORIGIN_X;
	temp_frame.origin.y = SHOW_MORE_TIME_ORIGIN_Y;
	temp_frame.size.h = SHOW_MORE_TIME_SIZE_H;
	temp_frame.size.w = SHOW_MORE_TIME_SIZE_W;
	
	display_target_layerText(p_window,&temp_frame,GAlignCenter,GColorWhite,g_show_more_str_time,U_ASCII_ARIALBD_30);
	
	//添加月日周文本图层
	temp_frame.origin.x = SHOW_MORE_M_D_W_ORIGIN_X;
	temp_frame.origin.y = SHOW_MORE_M_D_W_ORIGIN_Y;
	temp_frame.size.h = SHOW_MORE_M_D_W_SIZE_H;
	temp_frame.size.w = SHOW_MORE_M_D_W_SIZE_W;
	
	display_target_layerText(p_window,&temp_frame,GAlignCenter,GColorWhite,g_show_more_str_month_day_week,U_ASCII_ARIAL_12);

	//添加农历文本图层
	temp_frame.origin.x = SHOW_MORE_LUNAR_ORIGIN_X;
	temp_frame.origin.y = SHOW_MORE_LUNAR_ORIGIN_Y;
	temp_frame.size.h = SHOW_MORE_LUNAR_SIZE_H;
	temp_frame.size.w = SHOW_MORE_LUNAR_SIZE_W;

	display_target_layerText(p_window,&temp_frame,GAlignCenter,GColorWhite,g_show_more_str_lunar,U_ASCII_ARIAL_12);
	
	//添加步数文本图层
	temp_frame.origin.x = SHOW_MORE_STEPS_ORIGIN_X;
	temp_frame.origin.y = SHOW_MORE_STEPS_ORIGIN_Y;
	temp_frame.size.h = SHOW_MORE_STEPS_SIZE_H;
	temp_frame.size.w = SHOW_MORE_STEPS_SIZE_W;

	display_target_layerText(p_window,&temp_frame,GAlignRight,GColorBlack,g_show_more_str_steps,U_ASCII_ARIALBD_12);

	//添加楼层文本图层
	temp_frame.origin.x = SHOW_MORE_FLOORS_ORIGIN_X;
	temp_frame.origin.y = SHOW_MORE_FLOORS_ORIGIN_Y;
	temp_frame.size.h = SHOW_MORE_FLOORS_SIZE_H;
	temp_frame.size.w = SHOW_MORE_FLOORS_SIZE_W;

	display_target_layerText(p_window,&temp_frame,GAlignRight,GColorBlack,g_show_more_str_floors,U_ASCII_ARIALBD_12);

	//添加天气刷新图片图层
	temp_frame.origin.x = SHOW_MORE_REFRESHING_ORIGIN_X;
	temp_frame.origin.y = SHOW_MORE_REFRESHING_ORIGIN_Y;
	temp_frame.size.h = SHOW_MORE_REFRESHING_SIZE_H;
	temp_frame.size.w = SHOW_MORE_REFRESHING_SIZE_W;
		
	g_layer_id_refreshing = display_target_layer(p_window,&temp_frame,GAlignLeft,GColorWhite,bmp_array_name_key,1);

	if(g_weather_bmp_key != -1)
	{
		maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_layer_id_refreshing),false);
	}
	else
	{
		maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_layer_id_refreshing),true);
		return p_window;
	}
	
	//添加天气图片图层
	temp_frame.origin.x = SHOW_MORE_WEATHER_ORIGIN_X;
	temp_frame.origin.y = SHOW_MORE_WEATHER_ORIGIN_Y;
	temp_frame.size.h = SHOW_MORE_WEATHER_SIZE_H;
	temp_frame.size.w = SHOW_MORE_WEATHER_SIZE_W;

	bmp_array_name_key[3] = g_weather_bmp_key;
	display_target_layer(p_window,&temp_frame,GAlignLeft,GColorWhite,bmp_array_name_key,3);
	
	//添加PM2.5文本图层
	temp_frame.origin.x = SHOW_MORE_PM2_5_ORIGIN_X;
	temp_frame.origin.y = SHOW_MORE_PM2_5_ORIGIN_Y;
	temp_frame.size.h = SHOW_MORE_PM2_5_SIZE_H;
	temp_frame.size.w = SHOW_MORE_PM2_5_SIZE_W;
	
	display_target_layerText(p_window,&temp_frame,GAlignLeft,GColorBlack,g_show_more_str_PM2_5,U_ASCII_ARIALBD_12);

	//添加温度文本图层
	temp_frame.origin.x = SHOW_MORE_TEMPERATURE_ORIGIN_X;
	temp_frame.origin.y = SHOW_MORE_TEMPERATURE_ORIGIN_Y;
	temp_frame.size.h = SHOW_MORE_TEMPERATURE_SIZE_H;
	temp_frame.size.w = SHOW_MORE_TEMPERATURE_SIZE_W;
			
	display_target_layerText(p_window,&temp_frame,GAlignLeft,GColorBlack,g_show_more_str_temperature,U_ASCII_ARIALBD_20);

	//添加度号图片图层
	temp_frame.origin.x = SHOW_MORE_DU_HAO_ORIGIN_X;
	temp_frame.origin.y = SHOW_MORE_DU_HAO_ORIGIN_Y;
	temp_frame.size.h = SHOW_MORE_DU_HAO_SIZE_H;
	temp_frame.size.w = SHOW_MORE_DU_HAO_SIZE_W;
					
	display_target_layer(p_window,&temp_frame,GAlignLeft,GColorWhite,bmp_array_name_key,2);


	return p_window;
}



void data_handler_per_minute()
{
	//获取海拔数据
	float altitude, accuracy;
    maibu_get_altitude(&altitude, &accuracy);  
	double zhuan = (double)altitude;
	int zhuan2 = (int)zhuan;
	//将浮点型数据转变成整形数据
	sprintf(g_show_more_str_altitude, "%d", zhuan2);  
	char MMM[2] = {'m','\0'};
	strcat(g_show_more_str_altitude,&MMM);

	//获取电量数据
	char battery_temp = 0;
    maibu_get_battery_percent(&battery_temp);

	sprintf(g_show_more_str_battery_percent,"%d",(int)battery_temp);

	//获取时间数据
	struct date_time datetime_perminute;
	app_service_get_datetime(&datetime_perminute);
	g_today_num = datetime_perminute.mday;
	
	sprintf(g_show_more_str_time, "%02d:%02d", datetime_perminute.hour, datetime_perminute.min);

	//获取步数楼层
	SportData sport_datas;
    maibu_get_sport_data(&sport_datas, 0);   

	sprintf(g_show_more_str_floors, "%d", sport_datas.floor);
	sprintf(g_show_more_str_steps, "%d", sport_datas.step);
}

void data_handler_per_day()
{
	//获取时间月日周数据
	struct date_time datetime_perday;
	app_service_get_datetime(&datetime_perday);
	
	sprintf(g_show_more_str_month_day_week, "%d月%02d日 %s",datetime_perday.mon,datetime_perday.mday,wday_str[datetime_perday.wday]);

	//获取农历数据

	SLunarData lunar_calendar_data_struct = {
		.mon = "",
		.day = "",
		};
	
	maibu_get_lunar_calendar(NULL,&lunar_calendar_data_struct);

	sprintf(g_show_more_str_lunar, "%s%s",lunar_calendar_data_struct.mon,lunar_calendar_data_struct.day);

}



void  time_change (enum SysEventType type, void *context)
{
	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		struct date_time datetime_now_change;
		app_service_get_datetime(&datetime_now_change);

		if(g_today_num != datetime_now_change.mday)
		{
			data_handler_per_day();
		}

		data_handler_per_minute();

		window_reloading();
	}
	
}
/*获取天气时间*/

//判断前后台
int32_t get_front_or_back_flag(void)
{
	if(NULL == app_window_stack_get_window_by_id(g_window_id))
		return 0;
	else
		return 1;
}

//判断超时分钟函数
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

//请求城市数据
void request_get_city_info()
{
	g_comm_id_get_city = maibu_comm_request_phone(ERequestPhoneSelfDefine,(void *)&get_city_context,34);
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
		g_weather_bmp_key = get_weather_icon(weather_info);
		
	}
	else
	{	
		return;
	}

	memset(g_show_more_str_temperature,0,sizeof(g_show_more_str_temperature));
	memset(g_show_more_str_PM2_5,0,sizeof(g_show_more_str_PM2_5));
	
	maibu_get_json_str(buff, "temperature",g_show_more_str_temperature,sizeof(g_show_more_str_temperature));
	maibu_get_json_str(buff, "pm25", g_show_more_str_PM2_5,sizeof(g_show_more_str_PM2_5));
	
	if((g_show_more_str_PM2_5[0]<'0')||(g_show_more_str_PM2_5[0]>'9'))
	{
		uint8_t unknow_str[3] = "--";
		memcpy(g_show_more_str_PM2_5,unknow_str,sizeof(unknow_str));
	}
	
	if(atoi(g_show_more_str_temperature) >= 0)
	{
		g_temperature_width = strlen(g_show_more_str_temperature)*11;
	}
	else
	{
		g_temperature_width = (strlen(g_show_more_str_temperature)-1)*11+7;
	}
	
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


//定时器回调函数
void app_weather_update_timer_callback(date_time_t tick_time, uint32_t millis, void *context)
{	

	if((get_time_out_flag(55))&&(g_callback_request_flag == 0))
    {
    	//正常查询状态请求时间大于一小时就显示提示信息
		//清空历史数据
		g_weather_bmp_key = -1;
		memcpy(weather_info,0,sizeof(weather_info));

		/*根据窗口ID获取窗口句柄*/
		P_Window p_window = app_window_stack_get_window_by_id(g_window_id); 
		if (NULL != p_window)
		{
			maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_layer_id_refreshing),true);
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

	/*注册一个事件通知回调，当有改变时，改变表盘显示数据*/
    maibu_service_sys_event_subscribe(time_change);
	/*注册接受手机数据回调函数*/
	maibu_comm_register_phone_callback(get_city_info_callback);
	
	/*注册通讯结果回调*/
    maibu_comm_register_result_callback(weather_comm_result_callback);
	
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

	time_change(SysEventTypeTimeChange,NULL);
	
	//判断是否距离上次成功获得数据超过5分钟
	if(get_time_out_flag(5)||(g_weather_bmp_key == -1))
	{
		//请求GPS数据并注册GPS数据请求超时回调
		request_get_city_info();
		
	}

	//注册定时查询函数
	if(common_timer == 0)
	{
		common_timer = app_service_timer_subscribe(request_time, app_weather_update_timer_callback, NULL);
	}

	P_Window p_window = init_window(); 

	/*放入窗口栈显示*/
	g_window_id = app_window_stack_push(p_window);

	return 0;

}

