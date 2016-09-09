
#include "maibu_sdk.h"
#include "maibu_res.h"



/*标志位*/

//窗口ID
static int32_t g_window_id  = -1;
static uint8_t g_timer_id = -1;
static uint8_t g_callback_flag = 0;//用来标志是否是快速查询状态
static uint8_t g_flag = 0;


#define Pi 3.141593
#define Point_Big_Offset_x 0.5
#define Point_Big_Offset_y 0.5
#define Point_Start_X 36
#define Point_Start_Y 37
#define Point_Origin_X 36
#define Point_Origin_Y 55


/*图层位置*/

/*背景图片图层位置*/
#define MARS_BG_ORIGIN_X			0	
#define MARS_BG_ORIGIN_Y			0
#define MARS_BG_SIZE_H				128	
#define MARS_BG_SIZE_W				128	

/*时间文本图层位置*/
#define MARS_TIME_ORIGIN_X			70	
#define MARS_TIME_ORIGIN_Y			103
#define MARS_TIME_SIZE_H			20	
#define MARS_TIME_SIZE_W			50

/*地球年月日文本图层位置*/
#define MARS_EARTH_DAY_ORIGIN_X			4	
#define MARS_EARTH_DAY_ORIGIN_Y			13
#define MARS_EARTH_DAY_SIZE_H			12	
#define MARS_EARTH_DAY_SIZE_W			70	

/*火星日文本图层位置*/
#define MARS_MARS_DAY_ORIGIN_X			76	
#define MARS_MARS_DAY_ORIGIN_Y			13
#define MARS_MARS_DAY_SIZE_H			12	
#define MARS_MARS_DAY_SIZE_W			44	

/*最高温度文本图层位置*/
#define MARS_MAX_TEMPERATURE_ORIGIN_X	74	
#define MARS_MAX_TEMPERATURE_ORIGIN_Y	47
#define MARS_MAX_TEMPERATURE_SIZE_H		12
#define MARS_MAX_TEMPERATURE_SIZE_W		36	

/*最低温度文本图层位置*/
#define MARS_MIN_TEMPERATURE_ORIGIN_X	74	
#define MARS_MIN_TEMPERATURE_ORIGIN_Y	67
#define MARS_MIN_TEMPERATURE_SIZE_H		12
#define MARS_MIN_TEMPERATURE_SIZE_W		36	

/*气压文本图层位置*/
#define MARS_PRESSURE_ORIGIN_X			74
#define MARS_PRESSURE_ORIGIN_Y			87
#define MARS_PRESSURE_SIZE_H			12
#define MARS_PRESSURE_SIZE_W			36	

/*日面经度文本图层位置*/
#define MARS_HELIOLONGITUDE_ORIGIN_X	16
#define MARS_HELIOLONGITUDE_ORIGIN_Y	95
#define MARS_HELIOLONGITUDE_SIZE_H		12
#define MARS_HELIOLONGITUDE_SIZE_W		30	


/*初始数据*/

//天气相关数据
#define WEATHER_KEY			"sol,max_temp,min_temp,pressure,ls"
#define WEATHER_URL			"http://marsweather.ingenology.com/v1/latest/?format=json"

#define JSON_ERROR			-1


//字符串
//最高温度
static char g_MARS_str_max[4] = {"--"};
//最低温度
static char g_MARS_str_min[4] = {"--"};
//气压
static char g_MARS_str_pressure[4] = {"--"};
//日面经度
static char g_MARS_str_heliolongitude[8] = {"000"};
//火星日
static char g_MARS_str_mars_day[6] = {"--"};
//地球日
static char g_MARS_str_earth_day[12] = {"--"};
//时间
static char g_MARS_str_time[6] ={0};

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
int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,enum GAlign how_to_align,enum GColor black_or_white,int32_t bmp_name_key)
{	

	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_name_key, &bmp_point);
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


void rotare_angle_x_y_offset(uint8_t *target_point_x,uint8_t* target_point_y,int32_t helio_rotation)
{
	double angle_rotation = (double)helio_rotation;
    angle_rotation = -angle_rotation*Pi/180;
    double target_coordinates_x;
    double target_coordinates_y;
    //点绕点旋转公式
    target_coordinates_x = ((Point_Start_X) - (Point_Origin_X))*cos(angle_rotation) - ((Point_Start_Y) - (Point_Origin_Y))*sin(angle_rotation);
    target_coordinates_y = ((Point_Start_X) - (Point_Origin_X))*sin(angle_rotation) + ((Point_Start_Y) - (Point_Origin_Y))*cos(angle_rotation);
    *target_point_x = Point_Origin_X + (int)(Point_Big_Offset_x + target_coordinates_x); 
    *target_point_y = Point_Origin_Y + (int)(Point_Big_Offset_y + target_coordinates_y); 

}


static int8_t mars_get_heliolongitude_layer(P_Window p_window,char *heliolongitude_str_p)
{
	int32_t heliolongitude_num = atoi(heliolongitude_str_p);
	uint8_t target_point_x = 0;
	uint8_t target_point_y = 0;
	
	rotare_angle_x_y_offset(&target_point_x,&target_point_y,heliolongitude_num);
	
	LayerGeometry layer_geometry;
	memset(&layer_geometry, 0, sizeof(LayerGeometry));

	Line line_X_Y = {{target_point_x,target_point_y},{Point_Origin_X, Point_Origin_Y}};
	Geometry pg = {GeometryTypeLine, FillArea, GColorWhite, (void*)&line_X_Y}; 
	P_Geometry p_pg[1];
	p_pg[layer_geometry.num++] = &pg;

	layer_geometry.p_g = p_pg;


	/*图层1*/
	P_Layer	 layer1 = NULL;
	layer1 = app_layer_create_geometry(&layer_geometry);


	return app_window_add_layer(p_window, layer1);;


}

P_Window init_window(void)
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	//添加背景图层
	GRect temp_frame ={{MARS_BG_ORIGIN_X,MARS_BG_ORIGIN_Y},
						{MARS_BG_SIZE_H,MARS_BG_SIZE_W}};
	
	display_target_layer(p_window,&temp_frame,GAlignLeft,GColorWhite,RES_BITMAP_WATCHFACE_MARS_BG);


	//添加时间文本图层
	temp_frame.origin.x = MARS_TIME_ORIGIN_X;
	temp_frame.origin.y = MARS_TIME_ORIGIN_Y;
	temp_frame.size.h = MARS_TIME_SIZE_H;
	temp_frame.size.w = MARS_TIME_SIZE_W;
	
	display_target_layerText(p_window,&temp_frame,GAlignRight,GColorBlack,g_MARS_str_time,U_ASCII_ARIAL_20);
	
	//添加气压文本图层
	temp_frame.origin.x = MARS_PRESSURE_ORIGIN_X;
	temp_frame.origin.y = MARS_PRESSURE_ORIGIN_Y;
	temp_frame.size.h = MARS_PRESSURE_SIZE_H;
	temp_frame.size.w = MARS_PRESSURE_SIZE_W;
	
	display_target_layerText(p_window,&temp_frame,GAlignRight,GColorBlack,g_MARS_str_pressure,U_ASCII_ARIAL_12);

	//添加最低温度文本图层
	temp_frame.origin.x = MARS_MIN_TEMPERATURE_ORIGIN_X;
	temp_frame.origin.y = MARS_MIN_TEMPERATURE_ORIGIN_Y;
	temp_frame.size.h = MARS_MIN_TEMPERATURE_SIZE_H;
	temp_frame.size.w = MARS_MIN_TEMPERATURE_SIZE_W;

	display_target_layerText(p_window,&temp_frame,GAlignRight,GColorBlack,g_MARS_str_min,U_ASCII_ARIAL_12);
	
	//添加最高温度文本图层
	temp_frame.origin.x = MARS_MAX_TEMPERATURE_ORIGIN_X;
	temp_frame.origin.y = MARS_MAX_TEMPERATURE_ORIGIN_Y;
	temp_frame.size.h = MARS_MAX_TEMPERATURE_SIZE_H;
	temp_frame.size.w = MARS_MAX_TEMPERATURE_SIZE_W;

	display_target_layerText(p_window,&temp_frame,GAlignRight,GColorBlack,g_MARS_str_max,U_ASCII_ARIAL_12);

	//添加火星日文本图层
	temp_frame.origin.x = MARS_MARS_DAY_ORIGIN_X;
	temp_frame.origin.y = MARS_MARS_DAY_ORIGIN_Y;
	temp_frame.size.h = MARS_MARS_DAY_SIZE_H;
	temp_frame.size.w = MARS_MARS_DAY_SIZE_W;

	display_target_layerText(p_window,&temp_frame,GAlignRight,GColorBlack,g_MARS_str_mars_day,U_ASCII_ARIAL_12);

	//添加地球日文本图层
	temp_frame.origin.x = MARS_EARTH_DAY_ORIGIN_X;
	temp_frame.origin.y = MARS_EARTH_DAY_ORIGIN_Y;
	temp_frame.size.h = MARS_EARTH_DAY_SIZE_H;
	temp_frame.size.w = MARS_EARTH_DAY_SIZE_W;
	
	display_target_layerText(p_window,&temp_frame,GAlignCenter,GColorBlack,g_MARS_str_earth_day,U_ASCII_ARIAL_12);

	//添加日面经度文本图层
	temp_frame.origin.x = MARS_HELIOLONGITUDE_ORIGIN_X;
	temp_frame.origin.y = MARS_HELIOLONGITUDE_ORIGIN_Y;
	temp_frame.size.h = MARS_HELIOLONGITUDE_SIZE_H;
	temp_frame.size.w = MARS_HELIOLONGITUDE_SIZE_W;
			
	display_target_layerText(p_window,&temp_frame,GAlignRight,GColorBlack,g_MARS_str_heliolongitude,U_ASCII_ARIAL_12);

	mars_get_heliolongitude_layer(p_window,g_MARS_str_heliolongitude);


	return p_window;
}


void data_handler_per_minute()
{
	//获取时间数据
	struct date_time datetime_perminute;
	app_service_get_datetime(&datetime_perminute);
	
	sprintf(g_MARS_str_time, "%02d:%02d", datetime_perminute.hour, datetime_perminute.min);

	sprintf(g_MARS_str_earth_day, "%d-%02d-%02d",datetime_perminute.year,datetime_perminute.mon,datetime_perminute.mday);
}



void  time_change (enum SysEventType type, void *context)
{
	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		data_handler_per_minute();

		window_reloading();
	}
	
}

int8_t mars_day_handler (int32_t *m_webback_mars_day_p,int32_t *m_heliolongitude_p)
{	
	uint32_t mars_day_sec = 1344259800;
	uint32_t datetime_now_sec = 0;
	int32_t mars_days_calculated_value = 0;
	
	struct date_time datetime_now;
	app_service_get_datetime(&datetime_now);
	datetime_now_sec = app_get_time(&datetime_now);
	
	mars_days_calculated_value = (datetime_now_sec-mars_day_sec)/((24*60+39)*60+35);

	if(*m_webback_mars_day_p >= mars_days_calculated_value)
	{
		return 0;
	}
	else
	{
		*m_heliolongitude_p = (int)(*m_heliolongitude_p + (360/668.59 * (mars_days_calculated_value - *m_webback_mars_day_p)))%360;
		*m_webback_mars_day_p = mars_days_calculated_value;
		return 1;
	}
	
}


int32_t myrandom(int32_t num_max)
{
	struct date_time datetime_now;
	app_service_get_datetime(&datetime_now);
	
	int32_t ret = 0;
		
	ret = (datetime_now.hour * datetime_now.wday * datetime_now.sec)%num_max;
	return ret;
}

void temp_pressure_handler(int32_t *m_MARS_num_max,int32_t *m_MARS_num_min,int32_t *m_MARS_num_pressure)
{	
	
	int32_t temp_max = *m_MARS_num_max;
	int32_t temp_min = *m_MARS_num_min;
	int32_t temp_pre = *m_MARS_num_pressure;
	
	*m_MARS_num_max = (temp_max * (myrandom(10)-5))/100 + temp_max;
	*m_MARS_num_min = (temp_min * (myrandom(10)-5))/100 + temp_min;
	*m_MARS_num_pressure = (temp_pre * (myrandom(10)-5))/100 + temp_pre;
}


//请求web数据回调
void mars_info_callback(const uint8_t *buff,uint16_t size)
{
	g_callback_flag = 1;
	int8_t	ret = -1;
	int32_t test_num = 0;
	
	int32_t m_MARS_num_mars_day = 0;
	int32_t m_MARS_num_max = 0;
	int32_t m_MARS_num_min = 0;
	int32_t m_MARS_num_pressure = 0;
	int32_t m_MARS_num_heliolongitude = 0;
	
	if(maibu_get_json_int(buff, "sol", &test_num) != JSON_ERROR )
	{
		//防止数据为空导致的数据清空
		maibu_get_json_int(buff, "sol", &m_MARS_num_mars_day);
	}
	else
	{	
		return;
	}
	maibu_get_json_int(buff, "ls", &m_MARS_num_heliolongitude);

	maibu_get_json_int(buff, "max_temp", &m_MARS_num_max);
	
	maibu_get_json_int(buff, "min_temp", &m_MARS_num_min);

	maibu_get_json_int(buff, "pressure", &m_MARS_num_pressure);

	
	ret = mars_day_handler(&m_MARS_num_mars_day,&m_MARS_num_heliolongitude);
	if((ret == 1)&&(g_flag == 1))
	{
		temp_pressure_handler(&m_MARS_num_max,&m_MARS_num_min,&m_MARS_num_pressure);
	}


	sprintf(g_MARS_str_mars_day,"%d",m_MARS_num_mars_day);
	sprintf(g_MARS_str_max,"%d",m_MARS_num_max);
	sprintf(g_MARS_str_min,"%d",m_MARS_num_min);
	sprintf(g_MARS_str_pressure,"%d",m_MARS_num_pressure);
	sprintf(g_MARS_str_heliolongitude,"%03d",m_MARS_num_heliolongitude);
	
	window_reloading();
	
}

//请求数据
void request_mars_info(void)
{
	maibu_comm_request_web(WEATHER_URL,WEATHER_KEY,0);
}


//定时器回调函数
void app_weather_update_timer_callback(date_time_t tick_time, uint32_t millis, void *context)
{	
	if(g_callback_flag == 1)
	{
       	app_service_timer_unsubscribe(g_timer_id);
		//3小时请求一次
	   	g_timer_id = app_service_timer_subscribe(3*60*60*1000, app_weather_update_timer_callback, NULL);
		g_flag = 1;
	}
	request_mars_info();
}

int main(void)
{
	data_handler_per_minute();
	P_Window p_window = init_window(); 

	/*放入窗口栈显示*/
	g_window_id = app_window_stack_push(p_window);

	
	//刚安装或重启手表后,5秒请求数据,直到数据返回 
	g_timer_id = app_service_timer_subscribe(5*1000, app_weather_update_timer_callback, NULL);


	/*注册一个事件通知回调，当有改变时，改变表盘显示数据*/
    maibu_service_sys_event_subscribe(time_change);
	maibu_comm_register_web_callback(mars_info_callback);

	return 0;

}

