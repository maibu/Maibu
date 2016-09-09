#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "maibu_sdk.h"
#include "maibu_res.h"



/*标志位*/

//窗口ID
static int32_t g_window_id = -1;
//特殊节日图片图层ID
static int32_t g_layer_id_festival = -1;
//温度宽度
static int32_t g_temperature_width = -1;
//天气图片KEY
static int32_t g_weather_bmp_key = -1;

static int32_t g_weather_first_flag = 0;

static uint8_t common_timer = 0;
static uint8_t common_timer_count = 0;

//表盘的记录的当天日期，用于比较日期是否变为第二天
static uint8_t g_today_num = 0;
//标志是否请求web数据
static int8_t g_request_web_info_flag   = 1;

static int32_t  g_start_seconds_flag    = 0;//标志记录是刚安装表盘还是切换表盘

static int8_t     g_time_bmp_num[4]		= {0};


/*背景图片图层位置*/
#define ALMANAC_BG_ORIGIN_X			0	
#define ALMANAC_BG_ORIGIN_Y			0
#define ALMANAC_BG_SIZE_H			128	
#define ALMANAC_BG_SIZE_W			128	

/*日期周文本图层位置*/
#define ALMANAC_DAY_ORIGIN_X		18 		
#define ALMANAC_DAY_ORIGIN_Y		4 
#define ALMANAC_DAY_SIZE_H			12 	
#define ALMANAC_DAY_SIZE_W			92 

/*时间图片图层起始位置*/
#define ALMANAC_TIME_ORIGIN_X		23	
#define ALMANAC_TIME_ORIGIN_Y		27
#define ALMANAC_TIME_SIZE_H			34	
#define ALMANAC_TIME_SIZE_W			19

/*农历文本图层位置*/
#define ALMANAC_M_D_W_ORIGIN_X		1	
#define ALMANAC_M_D_W_ORIGIN_Y		70
#define ALMANAC_M_D_W_SIZE_H		12	
#define ALMANAC_M_D_W_SIZE_W		126	

/*农历宜文本图层位置*/
#define ALMANAC_PROPER_DO_ORIGIN_X		37	
#define ALMANAC_PROPER_DO_ORIGIN_Y		91
#define ALMANAC_PROPER_DO_SIZE_H		12	
#define ALMANAC_PROPER_DO_SIZE_W		86	

/*农历忌文本图层位置*/
#define ALMANAC_TABOO_ORIGIN_X		37	
#define ALMANAC_TABOO_ORIGIN_Y		111
#define ALMANAC_TABOO_SIZE_H		12	
#define ALMANAC_TABOO_SIZE_W		86

#define JSON_ERROR 	-1

/*初始数据*/

//天气相关数据
#define WEATHER_KEY			"yi,ji"
#define WEATHER_URL			"http://v.juhe.cn/laohuangli/d?key=625cc68bc3b2c11b4253e63f7de58d40&date="


//字符串
//年月日 周
char g_almanac_str_ymd[18] = {""};
//农历
char g_almanac_str_lunar[23] = {"--"};
//宜
char g_almanac_str_proper[20] = {"--      --      --"};
//忌
char g_almanac_str_taboo[20]  = {"--      --      --"};



static char wday[7][8]={"周日","周一","周二","周三","周四","周五","周六"}; 


//图片ID数组
int32_t bmp_array_name[11] = {
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
	RES_BITMAP_WATCHFACE_ALMANAC_BG
};

P_Window init_window(void);
void request_weather_info(char * city_name);


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
	GRect temp_frame ={{ALMANAC_BG_ORIGIN_X,ALMANAC_BG_ORIGIN_Y},
						{ALMANAC_BG_SIZE_H,ALMANAC_BG_SIZE_W}};
	
	display_target_layer(p_window,&temp_frame,GAlignCenter,GColorWhite,bmp_array_name,10);

	//添加日期周文本图层
	temp_frame.origin.x = ALMANAC_DAY_ORIGIN_X;
	temp_frame.origin.y = ALMANAC_DAY_ORIGIN_Y;
	temp_frame.size.h = ALMANAC_DAY_SIZE_H;
	temp_frame.size.w = ALMANAC_DAY_SIZE_W;
	
	display_target_layerText(p_window,&temp_frame,GAlignCenter,GColorWhite,g_almanac_str_ymd,U_ASCII_ARIAL_12);

	//添加日期图片图层
	temp_frame.origin.x = ALMANAC_TIME_ORIGIN_X;
	temp_frame.origin.y = ALMANAC_TIME_ORIGIN_Y;
	temp_frame.size.h = ALMANAC_TIME_SIZE_H;
	temp_frame.size.w = ALMANAC_TIME_SIZE_W;

	uint8_t ii;
	for(ii = 0;ii <=3 ;ii++)
	{
		display_target_layer(p_window,&temp_frame,GAlignLeft,GColorWhite,bmp_array_name,g_time_bmp_num[ii]);
		temp_frame.origin.x = temp_frame.origin.x + 19;
		if(ii == 1)
		{
			temp_frame.origin.x = temp_frame.origin.x + 9;
		}
	}
	
	
	//添加农历文本图层
	temp_frame.origin.x = ALMANAC_M_D_W_ORIGIN_X;
	temp_frame.origin.y = ALMANAC_M_D_W_ORIGIN_Y;
	temp_frame.size.h = ALMANAC_M_D_W_SIZE_H;
	temp_frame.size.w = ALMANAC_M_D_W_SIZE_W;
	
	display_target_layerText(p_window,&temp_frame,GAlignCenter,GColorBlack,g_almanac_str_lunar,U_ASCII_ARIAL_12);
	
	//添加农历宜文本图层
	temp_frame.origin.x = ALMANAC_PROPER_DO_ORIGIN_X;
	temp_frame.origin.y = ALMANAC_PROPER_DO_ORIGIN_Y;
	temp_frame.size.h = ALMANAC_PROPER_DO_SIZE_H;
	temp_frame.size.w = ALMANAC_PROPER_DO_SIZE_W;
	
	display_target_layerText(p_window,&temp_frame,GAlignLeft,GColorWhite,g_almanac_str_proper,U_ASCII_ARIAL_12);

	//添加农历忌文本图层
	temp_frame.origin.x = ALMANAC_TABOO_ORIGIN_X;
	temp_frame.origin.y = ALMANAC_TABOO_ORIGIN_Y;
	temp_frame.size.h = ALMANAC_TABOO_SIZE_H;
	temp_frame.size.w = ALMANAC_TABOO_SIZE_W;

	display_target_layerText(p_window,&temp_frame,GAlignLeft,GColorWhite,g_almanac_str_taboo,U_ASCII_ARIAL_12);

	return p_window;
}



void data_handler_per_minute()
{
	//获取时间数据
	struct date_time datetime_perminute;
	app_service_get_datetime(&datetime_perminute);
	g_today_num = datetime_perminute.mday;

	g_time_bmp_num[0] = datetime_perminute.hour/10;
	g_time_bmp_num[1] = datetime_perminute.hour%10;
	
	g_time_bmp_num[2] = datetime_perminute.min/10;
	g_time_bmp_num[3] = datetime_perminute.min%10;
}

void data_handler_per_day()
{

	//获取时间月日周数据
	struct date_time datetime_perday;
	app_service_get_datetime(&datetime_perday);
	
	sprintf(g_almanac_str_ymd, "%d.%d.%d %s", datetime_perday.year,datetime_perday.mon,datetime_perday.mday,wday[datetime_perday.wday]);


	//获取农历数据
	SLunarData lunar_calendar_data_struct = {0};

	uint8_t retval = 1;
	retval = maibu_get_lunar_calendar(NULL,&lunar_calendar_data_struct);

	
	memset(g_almanac_str_lunar,0,sizeof(g_almanac_str_lunar));
	if(retval == -1)
	{
		strcpy(g_almanac_str_lunar,"--");
	}
	else
	{
		sprintf(g_almanac_str_lunar, "%s%s",lunar_calendar_data_struct.mon,lunar_calendar_data_struct.day);
		if(strlen(lunar_calendar_data_struct.festival) >= 6)
		{
			strcat(g_almanac_str_lunar," ");
			strcat(g_almanac_str_lunar,lunar_calendar_data_struct.festival);
		}
		else if(strlen(lunar_calendar_data_struct.solar_term) >= 6)
		{
			strcat(g_almanac_str_lunar," ");
			strcat(g_almanac_str_lunar,lunar_calendar_data_struct.solar_term);
		}

	}
	
		
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
			g_request_web_info_flag = 1;

		}
		if(g_request_web_info_flag == 1)
		{	

			if(maibu_get_ble_status() == BLEStatusConnected)
			{
				request_web_info();
			}
				
		}

		data_handler_per_minute();

		window_reloading();
	}
	
}
/*获取宜忌信息*/

void del_char(char* str)
{
    char *p = str;
    char *q = str;
    while(*q)
    {
        if ((*q !=' ')||((*q ==' ')&&(*(q+1) !=' ')))
        {
            *p = *q;
			p++;
        }
        q++;
    }
    *p='\0';
}

//请求宜忌信息数据回调
void request_web_info_callback(const uint8_t *buff,uint16_t size)
{
	char buffer[100] = {0};
	int ret = JSON_ERROR;
	ret = maibu_get_json_str(buff, "ji", buffer,sizeof(buffer));
	
	if( ret != JSON_ERROR )
	{
		common_timer_count = 8;
		
		uint32_t len = strlen(buffer)>sizeof(g_almanac_str_taboo)?sizeof(g_almanac_str_taboo):strlen(buffer);
		memset(g_almanac_str_taboo,0,sizeof(g_almanac_str_taboo));
		del_char(buffer);
		memcpy(g_almanac_str_taboo,buffer,len);
		
		memset(buffer,0,sizeof(buffer));
		ret = maibu_get_json_str(buff, "yi", buffer,sizeof(buffer));
		len = strlen(buffer)>sizeof(g_almanac_str_taboo)?sizeof(g_almanac_str_taboo):strlen(buffer);
		memset(g_almanac_str_proper,0,sizeof(g_almanac_str_proper));
		del_char(buffer);
		memcpy(g_almanac_str_proper,buffer,len);
	}
	else
	{	
		return;
	}

	g_request_web_info_flag = 0;
	window_reloading();
	
}

//请求宜忌信息数据
void request_web_info(void)
{
	
	struct date_time datetime_perday;
	app_service_get_datetime(&datetime_perday);

	//请求web年月日字符串
	char m_almanac_str_web_ymd[12] = {""};
	sprintf(m_almanac_str_web_ymd, "%d-%02d-%02d", datetime_perday.year,datetime_perday.mon,datetime_perday.mday);

	char url[256] = {0};
	sprintf(url,"%s%s",WEATHER_URL,m_almanac_str_web_ymd);

	maibu_comm_register_web_callback(request_web_info_callback);
	maibu_comm_request_web(url,WEATHER_KEY,0);


	memset(g_almanac_str_proper,0,sizeof(g_almanac_str_proper));
	strcpy(g_almanac_str_proper,"--      --      --");
	memset(g_almanac_str_taboo,0,sizeof(g_almanac_str_taboo));
	strcpy(g_almanac_str_taboo,"--      --      --");
}

void app_timer_callback()
{
	common_timer_count ++;

	if(common_timer_count <= 8)
	{
		if(maibu_get_ble_status() == BLEStatusConnected)
		{
			request_web_info();
		}
	}else
	{
		app_service_timer_unsubscribe(common_timer);
	}

}

int main(void)
{

	/*注册一个事件通知回调，当有改变时，改变表盘显示数据*/
    maibu_service_sys_event_subscribe(time_change);

	time_change(SysEventTypeTimeChange,NULL);

	P_Window p_window = init_window(); 

	/*放入窗口栈显示*/
	g_window_id = app_window_stack_push(p_window);

	common_timer = app_service_timer_subscribe(7*1000, app_timer_callback, NULL);
	
	return 0;

}

