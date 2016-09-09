#include "maibu_sdk.h"
#include "maibu_res.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int32_t g_window_id              = -1;

static uint8_t g_callback_request_flag = 0;//用来标志是否是快速查询状态
static char get_city_context[35]	    = {0x01};//协议ID
static uint8_t g_set_city_number	 	= 0;//标志设置了几个城市
static uint32_t g_start_seconds		    = 0;//标志记录开始的秒数
static uint8_t g_start_seconds_flag     = 0;//标志记录是刚安装表盘还是切换表盘
static uint32_t g_comm_id_get_city      = 0;//获取城市的通讯ID
static uint32_t g_comm_id_web_weather   = 0;//获取天气的通讯ID


#define CITY_BEIJING		"北京"
#define CITY_SHANGHAI       "上海"

#define CITY0_PRESIST_DATA_KEY 0x2100
#define CITY1_PRESIST_DATA_KEY 0x2101



/*天气信息结构体*/
static struct weather_info_pack
{
	char city[20];
	char temperature[4];
	char curPm[4];
	char weather_info[20];
	uint32_t weather_bmp_key;
}g_weather_data[2]={
		{"","","","",0},
		{CITY_BEIJING,"","","",0}
};


static uint8_t g_request_count        = 0;

static uint8_t delay_time_request_weather_info_timer = 0;
static uint8_t common_timer            = 0;

static uint8_t g_lock = 0;


/*背景图片图层位置*/
#define DOUBLE_WEATHER_ZONE_BG_ORIGIN_X			0	
#define DOUBLE_WEATHER_ZONE_BG_ORIGIN_Y			0
#define DOUBLE_WEATHER_ZONE_BG_SIZE_H			128	
#define DOUBLE_WEATHER_ZONE_BG_SIZE_W			128	

/*日期图片图层起始位置*/
#define DOUBLE_WEATHER_ZONE_MONTH_DAY_ORIGIN_X	4	
#define DOUBLE_WEATHER_ZONE_MONTH_DAY_ORIGIN_Y	5
#define DOUBLE_WEATHER_ZONE_MONTH_DAY_SIZE_H	10	
#define DOUBLE_WEATHER_ZONE_MONTH_DAY_SIZE_W	7

/*时间图片图层起始位置*/
#define DOUBLE_WEATHER_ZONE_TIME_ORIGIN_X		93	
#define DOUBLE_WEATHER_ZONE_TIME_ORIGIN_Y		5
#define DOUBLE_WEATHER_ZONE_TIME_SIZE_H			10	
#define DOUBLE_WEATHER_ZONE_TIME_SIZE_W			7

/*天气图片上图层位置*/
#define DOUBLE_WEATHER_ZONE_WEATHER_TOP_ORIGIN_X	5	
#define DOUBLE_WEATHER_ZONE_WEATHER_TOP_ORIGIN_Y	22
#define DOUBLE_WEATHER_ZONE_WEATHER_TOP_SIZE_H		38
#define DOUBLE_WEATHER_ZONE_WEATHER_TOP_SIZE_W		38	

/*天气图片下图层位置*/
#define DOUBLE_WEATHER_ZONE_WEATHER_BOTTOM_ORIGIN_X	5	
#define DOUBLE_WEATHER_ZONE_WEATHER_BOTTOM_ORIGIN_Y	80
#define DOUBLE_WEATHER_ZONE_WEATHER_BOTTOM_SIZE_H	38
#define DOUBLE_WEATHER_ZONE_WEATHER_BOTTOM_SIZE_W	38	

/*城市文本上图层位置*/
#define DOUBLE_WEATHER_ZONE_CITY_TOP_ORIGIN_X		49	
#define DOUBLE_WEATHER_ZONE_CITY_TOP_ORIGIN_Y		22
#define DOUBLE_WEATHER_ZONE_CITY_TOP_SIZE_H			14
#define DOUBLE_WEATHER_ZONE_CITY_TOP_SIZE_W			76	

/*城市文本下图层位置*/
#define DOUBLE_WEATHER_ZONE_CITY_BOTTOM_ORIGIN_X	49
#define DOUBLE_WEATHER_ZONE_CITY_BOTTOM_ORIGIN_Y	80
#define DOUBLE_WEATHER_ZONE_CITY_BOTTOM_SIZE_H		14
#define DOUBLE_WEATHER_ZONE_CITY_BOTTOM_SIZE_W		76	


/*温度图片上图层起始位置*/
#define DOUBLE_WEATHER_ZONE_TEMPERATURE_TOP_ORIGIN_X		50	
#define DOUBLE_WEATHER_ZONE_TEMPERATURE_TOP_ORIGIN_Y		43
#define DOUBLE_WEATHER_ZONE_TEMPERATURE_TOP_SIZE_H			15
#define DOUBLE_WEATHER_ZONE_TEMPERATURE_TOP_SIZE_W			47

/*温度图片下图层起始位置*/
#define DOUBLE_WEATHER_ZONE_TEMPERATURE_BOTTOM_ORIGIN_X		50
#define DOUBLE_WEATHER_ZONE_TEMPERATURE_BOTTOM_ORIGIN_Y		101
#define DOUBLE_WEATHER_ZONE_TEMPERATURE_BOTTOM_SIZE_H		15
#define DOUBLE_WEATHER_ZONE_TEMPERATURE_BOTTOM_SIZE_W		47	

/*PM2.5图片上图层起始位置*/
#define DOUBLE_WEATHER_ZONE_PM25_TOP_ORIGIN_X				106	
#define DOUBLE_WEATHER_ZONE_PM25_TOP_ORIGIN_Y				51
#define DOUBLE_WEATHER_ZONE_PM25_TOP_SIZE_H					7
#define DOUBLE_WEATHER_ZONE_PM25_TOP_SIZE_W					18

/*PM2.5图片下图层起始位置*/
#define DOUBLE_WEATHER_ZONE_PM25_BOTTOM_ORIGIN_X			106
#define DOUBLE_WEATHER_ZONE_PM25_BOTTOM_ORIGIN_Y			109
#define DOUBLE_WEATHER_ZONE_PM25_BOTTOM_SIZE_H				7
#define DOUBLE_WEATHER_ZONE_PM25_BOTTOM_SIZE_W				18	


#define CITY_EMPTY					3



#define WEATHER_KEY			"temperature,pm25,info,city_name"
#define WEATHER_URL			"http://op.juhe.cn/onebox/weather/query?key=这里替换成你申请的Key&cityname="

#define WEATHER_CLOUDY		"多云"
#define WEATHER_FOG_1		"雾"
#define WEATHER_FOG_2		"霾"
#define WEATHER_OVERCAST	"阴"
#define WEATHER_RAIN		"雨"
#define WEATHER_SNOW		"雪"
#define WEATHER_SUNNY		"晴"



#define JSON_ERROR			-1

#define TOP_DATA_NUM		0
#define BOTTOM_DATA_NUM		1


//小号数字图片数组
uint32_t get_LIT_icon_key[] =
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
	RES_BITMAP_WATCHFACE_NUMBER_LIT_9
	
};
//中号数字图片数组
uint32_t get_MID_icon_key[] =
{
	RES_BITMAP_WATCHFACE_NUMBER_MID_0,
	RES_BITMAP_WATCHFACE_NUMBER_MID_1,
	RES_BITMAP_WATCHFACE_NUMBER_MID_2,
	RES_BITMAP_WATCHFACE_NUMBER_MID_3,
	RES_BITMAP_WATCHFACE_NUMBER_MID_4,
	RES_BITMAP_WATCHFACE_NUMBER_MID_5,
	RES_BITMAP_WATCHFACE_NUMBER_MID_6,
	RES_BITMAP_WATCHFACE_NUMBER_MID_7,
	RES_BITMAP_WATCHFACE_NUMBER_MID_8,
	RES_BITMAP_WATCHFACE_NUMBER_MID_9
	
};

//大号数字图片数组
uint32_t get_BIG_icon_key[] =
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
	RES_BITMAP_WATCHFACE_NUMBER_BIG_9,
	RES_BITMAP_WATCHFACE_NUMBER_FU_HAO,
	RES_BITMAP_WATCHFACE_NUMBER_DU_HAO
};
//其他图层图片数组
uint32_t get_others_icon_key_array[] = 
{
	RES_BITMAP_WATCHFACE_WEATHER_BG,
	RES_BITMAP_WATCHFACE_WEATHER_REFRESHING,
	RES_BITMAP_WATCHFACE_WEATHER_REFRESHING_TEMPERATURE,
	RES_BITMAP_WATCHFACE_WEATHER_REFRESHING_PM25,
	0
};


P_Window init_window(void);



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
	uint32_t weather_icon_key = 0;

	if(memcmp(str,WEATHER_SUNNY,strlen(WEATHER_SUNNY)) == 0 )
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




/*创建并显示图片图层，需要坐标值，得到icon_key的数组，数组需要的参数值，P_Window*/
void display_target_layer(P_Window p_window,GRect *temp_p_frame,enum GAlign how_to_align,enum GColor black_or_white,int32_t bmp_array_name_key[],int bmp_id_number)
{	

	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_array_name_key[bmp_id_number], &bmp_point);
	LayerBitmap layer_bitmap_struct_l = {bmp_point, *temp_p_frame, how_to_align};
 	temp_P_Layer = app_layer_create_bitmap(&layer_bitmap_struct_l);
	if(temp_P_Layer != NULL)
	{
		app_layer_set_bg_color(temp_P_Layer, black_or_white);
		app_window_add_layer(p_window, temp_P_Layer);
	}

	return;
}


/*创建并显示文本图层*/
void display_target_layerText(P_Window p_window,GRect  *temp_p_frame,enum GAlign how_to_align,enum GColor color,char * str,uint8_t font_type)
{
	LayerText temp_LayerText = {0};
	temp_LayerText.text = str;
	temp_LayerText.frame = *temp_p_frame;
	temp_LayerText.alignment = how_to_align;
	temp_LayerText.font_type = font_type;
	temp_LayerText.bound_width = 0;
	P_Layer p_layer = app_layer_create_text(&temp_LayerText);
	
	if(p_layer != NULL)
	{
		app_layer_set_bg_color(p_layer, color);
		app_window_add_layer(p_window, p_layer);
	}
	return;
}


/*分析curPM数据，并显示*/
void check_display_curPM_number(P_Window p_window,GRect  *temp_p_frame,char weather_data_num)
{

	int8_t curPM_number_show_pic_count = 0;
	
	curPM_number_show_pic_count = strlen(g_weather_data[weather_data_num].curPm);

	
	if(curPM_number_show_pic_count <= 0)
	{
		display_target_layer(p_window,temp_p_frame,GAlignLeft,1-weather_data_num,&get_others_icon_key_array[0],3);
	}
	else
	{
		int8_t i = curPM_number_show_pic_count-1;
		temp_p_frame->size.w = 6;
		temp_p_frame->origin.x = DOUBLE_WEATHER_ZONE_PM25_TOP_ORIGIN_X + 2*(temp_p_frame->size.w);
		for(; i >= 0 ; i--)
		{
			display_target_layer(p_window,temp_p_frame,GAlignLeft,1-weather_data_num,&get_LIT_icon_key[0],g_weather_data[weather_data_num].curPm[i]-'0');
			temp_p_frame->origin.x = (temp_p_frame->origin.x) - (temp_p_frame->size.w);
		}
	}
}

/*分析temperature数据，并显示*/
void check_display_temperature_number(P_Window p_window,GRect  *temp_p_frame,char weather_data_num)
{

	uint8_t temperature_show_pic_count = 0;
	
	temperature_show_pic_count = strlen(g_weather_data[weather_data_num].temperature);
	
	
	if(temperature_show_pic_count <= 0)
	{
		display_target_layer(p_window,temp_p_frame,GAlignLeft,1-weather_data_num,&get_others_icon_key_array[0],2);
	}
	else
	{
		uint8_t i = 0;
		if(g_weather_data[weather_data_num].temperature[0] == '-')
		{
			temp_p_frame->size.w = 11;
			display_target_layer(p_window,temp_p_frame,GAlignLeft,weather_data_num,&get_BIG_icon_key[0],10);
			temp_p_frame->origin.x = temp_p_frame->origin.x + 11;
			i = 1;
		}
		temp_p_frame->size.w = 15;
		for(; i < temperature_show_pic_count ; i++)
		{
			display_target_layer(p_window,temp_p_frame,GAlignLeft,weather_data_num,&get_BIG_icon_key[0],g_weather_data[weather_data_num].temperature[i] - '0');
			temp_p_frame->origin.x = temp_p_frame->origin.x + 15;
		}
		temp_p_frame->size.w = 6;
		display_target_layer(p_window,temp_p_frame,GAlignLeft,weather_data_num,&get_BIG_icon_key[0],11);
	}

}



P_Window init_window(void)
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}  

	//添加背景图层
	GRect temp_frame ={{DOUBLE_WEATHER_ZONE_BG_ORIGIN_X,DOUBLE_WEATHER_ZONE_BG_ORIGIN_Y},
						{DOUBLE_WEATHER_ZONE_BG_SIZE_H,DOUBLE_WEATHER_ZONE_BG_SIZE_W}};
	
	display_target_layer(p_window,&temp_frame,GAlignLeft,GColorWhite,get_others_icon_key_array,0);


	struct date_time datetime;
	app_service_get_datetime(&datetime);
#if 1
	temp_frame.origin.x = 2;
	temp_frame.origin.y = 2;
	temp_frame.size.h = 12;
	temp_frame.size.w = 124;

	char buffer_time_str[32]={0};
	
	
	sprintf(buffer_time_str, "%02d-%02d                    %02d:%02d", datetime.mon,datetime.mday,datetime.hour,datetime.min);
	display_target_layerText(p_window,&temp_frame,GAlignCenter,GColorBlack,buffer_time_str,U_ASCII_ARIAL_12);
#else
	//month
	temp_frame.origin.x = DOUBLE_WEATHER_ZONE_MONTH_DAY_ORIGIN_X;
	temp_frame.origin.y = DOUBLE_WEATHER_ZONE_MONTH_DAY_ORIGIN_Y;
	temp_frame.size.h = DOUBLE_WEATHER_ZONE_MONTH_DAY_SIZE_H;
	temp_frame.size.w = DOUBLE_WEATHER_ZONE_MONTH_DAY_SIZE_W;

	display_target_layer(p_window,&temp_frame,GAlignRight,GColorWhite,&get_MID_icon_key[0],(datetime.mon / 10));

	temp_frame.origin.x = temp_frame.origin.x + DOUBLE_WEATHER_ZONE_TIME_SIZE_W;
	display_target_layer(p_window,&temp_frame,GAlignRight,GColorWhite,&get_MID_icon_key[0],(datetime.mon % 10));
	
	//day
	temp_frame.origin.x = temp_frame.origin.x + DOUBLE_WEATHER_ZONE_TIME_SIZE_W + 7;
	display_target_layer(p_window,&temp_frame,GAlignRight,GColorWhite,&get_MID_icon_key[0],(datetime.mday / 10));
	
	temp_frame.origin.x = temp_frame.origin.x + DOUBLE_WEATHER_ZONE_TIME_SIZE_W;
	display_target_layer(p_window,&temp_frame,GAlignRight,GColorWhite,&get_MID_icon_key[0],(datetime.mday % 10));
	
	
	//hour
	temp_frame.origin.x = DOUBLE_WEATHER_ZONE_TIME_ORIGIN_X;
	temp_frame.origin.y = DOUBLE_WEATHER_ZONE_TIME_ORIGIN_Y;
	temp_frame.size.h = DOUBLE_WEATHER_ZONE_TIME_SIZE_H;
	temp_frame.size.w = DOUBLE_WEATHER_ZONE_TIME_SIZE_W;

	display_target_layer(p_window,&temp_frame,GAlignRight,GColorWhite,&get_MID_icon_key[0],(datetime.hour / 10));
	
	temp_frame.origin.x = temp_frame.origin.x + DOUBLE_WEATHER_ZONE_TIME_SIZE_W;
	display_target_layer(p_window,&temp_frame,GAlignRight,GColorWhite,&get_MID_icon_key[0],(datetime.hour % 10));
	
	//min
	temp_frame.origin.x = temp_frame.origin.x + DOUBLE_WEATHER_ZONE_TIME_SIZE_W + 4;
	display_target_layer(p_window,&temp_frame,GAlignRight,GColorWhite,&get_MID_icon_key[0],(datetime.min / 10));

	temp_frame.origin.x = temp_frame.origin.x + DOUBLE_WEATHER_ZONE_TIME_SIZE_W;
	display_target_layer(p_window,&temp_frame,GAlignRight,GColorWhite,&get_MID_icon_key[0],(datetime.min % 10));
#endif	

	//添加天气上图片图层
	temp_frame.origin.x = DOUBLE_WEATHER_ZONE_WEATHER_TOP_ORIGIN_X;
	temp_frame.origin.y = DOUBLE_WEATHER_ZONE_WEATHER_TOP_ORIGIN_Y;
	temp_frame.size.h = DOUBLE_WEATHER_ZONE_WEATHER_TOP_SIZE_H;
	temp_frame.size.w = DOUBLE_WEATHER_ZONE_WEATHER_TOP_SIZE_W;
	
	get_others_icon_key_array[4] = g_weather_data[TOP_DATA_NUM].weather_bmp_key;
	if(g_weather_data[TOP_DATA_NUM].weather_bmp_key == 0)
	{
		get_others_icon_key_array[4] = get_others_icon_key_array[1];
	}


	display_target_layer(p_window,&temp_frame,GAlignLeft,GColorBlack,get_others_icon_key_array,4);

	//添加城市上文本图层
	temp_frame.origin.x = DOUBLE_WEATHER_ZONE_CITY_TOP_ORIGIN_X;
	temp_frame.origin.y = DOUBLE_WEATHER_ZONE_CITY_TOP_ORIGIN_Y;
	temp_frame.size.h = DOUBLE_WEATHER_ZONE_CITY_TOP_SIZE_H;
	temp_frame.size.w = DOUBLE_WEATHER_ZONE_CITY_TOP_SIZE_W;

	display_target_layerText(p_window,&temp_frame,GAlignLeft,GColorBlack,g_weather_data[TOP_DATA_NUM].city,U_ASCII_ARIAL_14);


	//添加温度上图片图层
	temp_frame.origin.x = DOUBLE_WEATHER_ZONE_TEMPERATURE_TOP_ORIGIN_X;
	temp_frame.origin.y = DOUBLE_WEATHER_ZONE_TEMPERATURE_TOP_ORIGIN_Y;
	temp_frame.size.h = DOUBLE_WEATHER_ZONE_TEMPERATURE_TOP_SIZE_H;
	temp_frame.size.w = DOUBLE_WEATHER_ZONE_TEMPERATURE_TOP_SIZE_W;

	check_display_temperature_number(p_window,&temp_frame,TOP_DATA_NUM);
	
	//添加PM2.5上图片图层
	temp_frame.origin.x = DOUBLE_WEATHER_ZONE_PM25_TOP_ORIGIN_X;
	temp_frame.origin.y = DOUBLE_WEATHER_ZONE_PM25_TOP_ORIGIN_Y;
	temp_frame.size.h = DOUBLE_WEATHER_ZONE_PM25_TOP_SIZE_H;
	temp_frame.size.w = DOUBLE_WEATHER_ZONE_PM25_TOP_SIZE_W;
	
	check_display_curPM_number(p_window,&temp_frame,TOP_DATA_NUM);

	//添加天气下图片图层
	temp_frame.origin.x = DOUBLE_WEATHER_ZONE_WEATHER_BOTTOM_ORIGIN_X;
	temp_frame.origin.y = DOUBLE_WEATHER_ZONE_WEATHER_BOTTOM_ORIGIN_Y;
	temp_frame.size.h = DOUBLE_WEATHER_ZONE_WEATHER_BOTTOM_SIZE_H;
	temp_frame.size.w = DOUBLE_WEATHER_ZONE_WEATHER_BOTTOM_SIZE_W;

	get_others_icon_key_array[4] = g_weather_data[BOTTOM_DATA_NUM].weather_bmp_key;
	if(g_weather_data[BOTTOM_DATA_NUM].weather_bmp_key == 0)
	{
		get_others_icon_key_array[4] = get_others_icon_key_array[1];
	}
	
	
	display_target_layer(p_window,&temp_frame,GAlignLeft,GColorWhite,get_others_icon_key_array,4);

	//添加城市下文本图层
	temp_frame.origin.x = DOUBLE_WEATHER_ZONE_CITY_BOTTOM_ORIGIN_X;
	temp_frame.origin.y = DOUBLE_WEATHER_ZONE_CITY_BOTTOM_ORIGIN_Y;
	temp_frame.size.h = DOUBLE_WEATHER_ZONE_CITY_BOTTOM_SIZE_H;
	temp_frame.size.w = DOUBLE_WEATHER_ZONE_CITY_BOTTOM_SIZE_W;

	display_target_layerText(p_window,&temp_frame,GAlignLeft,GColorWhite,g_weather_data[BOTTOM_DATA_NUM].city,U_ASCII_ARIAL_14);

	//添加温度下图片图层
	temp_frame.origin.x = DOUBLE_WEATHER_ZONE_TEMPERATURE_BOTTOM_ORIGIN_X;
	temp_frame.origin.y = DOUBLE_WEATHER_ZONE_TEMPERATURE_BOTTOM_ORIGIN_Y;
	temp_frame.size.h = DOUBLE_WEATHER_ZONE_TEMPERATURE_BOTTOM_SIZE_H;
	temp_frame.size.w = DOUBLE_WEATHER_ZONE_TEMPERATURE_BOTTOM_SIZE_W;

	check_display_temperature_number(p_window,&temp_frame,BOTTOM_DATA_NUM);

	//添加PM2.5下图片图层
	temp_frame.origin.x = DOUBLE_WEATHER_ZONE_PM25_BOTTOM_ORIGIN_X;
	temp_frame.origin.y = DOUBLE_WEATHER_ZONE_PM25_BOTTOM_ORIGIN_Y;
	temp_frame.size.h = DOUBLE_WEATHER_ZONE_PM25_BOTTOM_SIZE_H;
	temp_frame.size.w = DOUBLE_WEATHER_ZONE_PM25_BOTTOM_SIZE_W;

	check_display_curPM_number(p_window,&temp_frame,BOTTOM_DATA_NUM);

	
	return p_window;
}

//请求天气数据回调
void weather_info_callback(const uint8_t *buff,uint16_t size)
{	
	if(g_lock == 1)
	{
		return;
	}
	g_lock = 1;
	
	char m_back_city[20] = {0};
	char m_weather_data_num = -1;

	maibu_get_json_str(buff, "city_name", &m_back_city ,sizeof(m_back_city));
		
	if(strstr(g_weather_data[TOP_DATA_NUM].city,m_back_city) != NULL)
	{
		m_weather_data_num = TOP_DATA_NUM;
	}
	else if(strstr(g_weather_data[BOTTOM_DATA_NUM].city,m_back_city) != NULL)
	{
		m_weather_data_num = BOTTOM_DATA_NUM;
	}
	else
	{
		g_lock = 0;
		return;
	}
		

	char buffer[16] = {0};
	if(maibu_get_json_str(buff, "info", buffer,sizeof(g_weather_data[m_weather_data_num].weather_info)) != JSON_ERROR )
	{
		//防止数据为空导致的数据清空
		maibu_get_json_str(buff, "info", g_weather_data[m_weather_data_num].weather_info,sizeof(g_weather_data[m_weather_data_num].weather_info));
		g_weather_data[m_weather_data_num].weather_bmp_key = get_weather_icon(g_weather_data[m_weather_data_num].weather_info);
		
	}
	else
	{	
		g_lock = 0;
		return;
	}

	struct date_time tt;
	app_service_get_datetime(&tt);
	g_start_seconds = app_get_time(&tt);

	g_request_count = 0;//清空请求计数
	g_callback_request_flag = 0; //清空快速查询标志
	
	maibu_get_json_str(buff, "temperature",g_weather_data[m_weather_data_num].temperature,sizeof(g_weather_data[m_weather_data_num].temperature));
	maibu_get_json_str(buff, "pm25",g_weather_data[m_weather_data_num].curPm,sizeof(g_weather_data[m_weather_data_num].curPm));



	if(strcmp(g_weather_data[TOP_DATA_NUM].city,g_weather_data[BOTTOM_DATA_NUM].city) == 0)
	{
		g_weather_data[BOTTOM_DATA_NUM] = g_weather_data[TOP_DATA_NUM];
	}
		
	g_lock = 0;
	window_reloading();
	
		
}

//请求天气数据
void request_weather_info(char * city_name)
{
	char url[160] = {0};
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
		return;
	}


	maibu_comm_register_web_callback(weather_info_callback);
	g_comm_id_web_weather = maibu_comm_request_web(url,WEATHER_KEY,5*60*get_front_or_back_flag());

	
}
//延时请求
void delay_time_request_weather_info()
{
	app_service_timer_unsubscribe(delay_time_request_weather_info_timer);
	request_weather_info(g_weather_data[BOTTOM_DATA_NUM].city);
}
//请求天气数据函数入口
void request_weather_info_entry()
{
	request_weather_info(g_weather_data[TOP_DATA_NUM].city);
	app_service_timer_unsubscribe(delay_time_request_weather_info_timer);
	delay_time_request_weather_info_timer = app_service_timer_subscribe(10*1000,delay_time_request_weather_info, NULL);

}

//请求城市数据回调
void get_city_info_callback(enum ERequestPhone  type,void * context)
{
	
	if(type == ERequestPhoneSelfDefine)
	{	
		if(*(int16_t *)context == 1)
		{
			int8_t *context_city_name_point = (int8_t *)context + 4;	
	
			memcpy(g_weather_data[TOP_DATA_NUM].city,context_city_name_point,20);
			g_weather_data[TOP_DATA_NUM].city[19]='\0';

			if(g_set_city_number == 0)
			{
				if(strstr(g_weather_data[TOP_DATA_NUM].city,CITY_BEIJING) != NULL)
				{
					strcpy(g_weather_data[BOTTOM_DATA_NUM].city,CITY_SHANGHAI);
				}
				else
				{
					strcpy(g_weather_data[BOTTOM_DATA_NUM].city,CITY_BEIJING);
				}
			}
			
			window_reloading();
			request_weather_info_entry();
		}
	}
}

//请求城市数据
void request_get_city_info()
{
	
	g_comm_id_get_city = maibu_comm_request_phone(ERequestPhoneSelfDefine,(void *)&get_city_context,34);
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
			app_service_timer_unsubscribe(delay_time_request_weather_info_timer);
			request_weather_info_entry();
		}
	}
	
}

#endif

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
	if((get_time_out_flag(58))&&(g_callback_request_flag == 0))
    {
    	//正常查询状态请求时间大于一小时就显示提示信息
		//清空历史数据

		memset((char *)g_weather_data[TOP_DATA_NUM].temperature,0,32);
		
		memset((char *)g_weather_data[BOTTOM_DATA_NUM].temperature,0,32);

		/*根据窗口ID获取窗口句柄*/
		window_reloading();
		
	}
	else if((g_request_count >= 6)&&(g_callback_request_flag == 1))
	{   //快速查询的状态下大于12次(1分钟)就退出快速查询
        
        g_request_count = 0;
		g_callback_request_flag = 0;
	}
		    	
	if(g_callback_request_flag == 0)
	{
       	app_service_timer_unsubscribe(common_timer);
	   	common_timer = app_service_timer_subscribe((30-(15*get_front_or_back_flag()))*60*1000, app_weather_update_timer_callback, NULL);
	}
	
	

	//根据城市信息获取天气信息
	if(g_set_city_number == 2)
	{
		request_weather_info_entry();
	}
	else
	{
		
		request_get_city_info();
	}
	
	g_request_count++;
}

void phone_back_analysis(uint8_t *buf, uint16_t len)
{

	char m_city0[20] = {0};
	char m_city1[20] = {0};
	char m_get_city_number = 0;
		
	char *m_p = buf;
	char *m_q = m_p;
	uint8_t count_len = 0;
	//去掉空格和回车
	while(((*m_p == ' ')||(*m_p == '\n'))&&(count_len < len))
	{	
		m_p++;
		count_len++;
	}
	//取数据
	while(count_len < len)
	{
		if((*m_p != ' ')&&(*m_p != '\n'))
		{
			*m_p++;
			count_len++;
		}
		else if(m_get_city_number == 0)
		{
			memcpy(m_city0,m_q,(m_p-m_q));
			m_city0[m_p-m_q]='\0';
			m_get_city_number = 1;
			
			while(((*m_p == ' ')||(*m_p == '\n'))&&(count_len < len))
			{	
				m_p++;
				count_len++;
			}
			m_q = m_p;
			
		}
		else 
		{
			break;
		}
		
	}
	memcpy(m_city1,m_q,(m_p-m_q));
	m_city1[m_p-m_q]='\0';

	
	memset((char *)&(g_weather_data[1]),0,sizeof(struct weather_info_pack));
	memset((char *)&(g_weather_data[0]),0,sizeof(struct weather_info_pack));

	//判断设置了几个城市
	if((strlen(m_city0) > 1)&&(strlen(m_city1) > 1))
	{
		strcpy(g_weather_data[0].city,m_city0);
		strcpy(g_weather_data[1].city,m_city1);
		g_set_city_number = 2;
	}
	else if((strlen(m_city0) > 1)&&(strlen(m_city1) <= 1))
	{	
		strcpy(g_weather_data[1].city,m_city0);
		g_set_city_number = 1;
	}
	else if((strlen(m_city0) <= 1)&&(strlen(m_city1) > 1))
	{
		strcpy(g_weather_data[1].city,m_city1);
		g_set_city_number = 1;
	}

}


#if 1
void watchapp_comm_callback(enum ESyncWatchApp type, uint8_t *buf, uint16_t len)
{
	
	if(type == ESyncWatchAppUpdateParam)
    {
/*
	    P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
		if (NULL == p_window)
		{
			return;
		}
*/		
		if((len >= 3) && (len <=40))
		{
			phone_back_analysis(buf,len);

			app_persist_write_data_extend(CITY0_PRESIST_DATA_KEY, g_weather_data[0].city, sizeof(g_weather_data[0].city));
			app_persist_write_data_extend(CITY1_PRESIST_DATA_KEY, g_weather_data[1].city, sizeof(g_weather_data[1].city));

						
			//根据城市信息获取天气信息
			if(g_set_city_number == 2)
			{
				request_weather_info_entry();
			}
			else
			{
				
				request_get_city_info();
			}
		}
		else
		{
			//恢复初始查询值
			g_set_city_number = 0;

			memset((char *)&(g_weather_data[0]),0,sizeof(struct weather_info_pack));
			memset((char *)&(g_weather_data[1]),0,sizeof(struct weather_info_pack));

			app_persist_delete_data(CITY0_PRESIST_DATA_KEY);
			app_persist_delete_data(CITY1_PRESIST_DATA_KEY);

			
			request_get_city_info();
		}

		window_reloading();
	}
}

#endif
int main(void)
{
	//os_printk("g_weather_data[TOP_DATA_NUM].city = %s\n",g_weather_data[TOP_DATA_NUM].city);
	//创建配置存储
	app_persist_create(CITY0_PRESIST_DATA_KEY, sizeof(g_weather_data[0].city));
	app_persist_create(CITY1_PRESIST_DATA_KEY, sizeof(g_weather_data[1].city));
	//读取
	
	if(g_set_city_number != 1)
	{
		app_persist_read_data(CITY0_PRESIST_DATA_KEY, 0, g_weather_data[0].city, sizeof(g_weather_data[0].city));
		
	}
	app_persist_read_data(CITY1_PRESIST_DATA_KEY, 0, g_weather_data[1].city, sizeof(g_weather_data[1].city));

#if 1 
	if(g_set_city_number == 0)
	{
		if(strlen(g_weather_data[0].city) > 1)
		{
			g_set_city_number++;
		}


		if(strlen(g_weather_data[1].city) > 1)
		{	
			g_set_city_number++;
		}
	}	
#endif

	/*创建消息列表窗口*/
	P_Window p_window = init_window(); 

	/*放入窗口栈显示*/
	g_window_id = app_window_stack_push(p_window);
			
	/*注册接受手机数据回调函数*/
	maibu_comm_register_phone_callback(get_city_info_callback);

	//注册手机设置回调函数
	maibu_comm_register_watchapp_callback(watchapp_comm_callback);

	/*注册通讯结果回调*/
	maibu_comm_register_result_callback(weather_comm_result_callback);

	uint32_t request_time = 15*60*1000;//默认15分钟请求数据	
	
    //是安装表盘的情况,则记录时间,并设置快速查询
	if(g_start_seconds_flag == 0)
	{
		struct date_time tt1;
		app_service_get_datetime(&tt1);
		g_start_seconds = app_get_time(&tt1);
		
		request_time = 25 * 1000;//在没有获取过数据的情况下25秒获取一次数据
		g_callback_request_flag = 1;//设置快速查询标志
	}
	//判断是否距离上次成功获得数据超过5分钟
	else if(get_time_out_flag(5)||(g_weather_data[TOP_DATA_NUM].weather_bmp_key == 0)||(g_weather_data[BOTTOM_DATA_NUM].weather_bmp_key == 0))
	{
		//根据城市信息获取天气信息
		if(g_set_city_number == 2)
		{
			request_weather_info_entry();
		}
		else
		{
			request_get_city_info();
		}
	}
	g_start_seconds_flag = 1;
			
    common_timer = app_service_timer_subscribe(request_time, app_weather_update_timer_callback, NULL);
		
	/*注册一个事件通知回调，当有时间改变时，立即更新时间*/
    maibu_service_sys_event_subscribe(time_change);
	/*注册通讯结果回调*/
	return 0;

}















