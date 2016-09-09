/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  watchface_bicycle.c
 *
 *    Description:  骑行表盘
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
#include "os_sys_notify.h"
#else
#include "maibu_sdk.h"
#include "maibu_res.h"
#endif




/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_bicycle_window_id = -1;

/*图层ID，通过该图层ID获取图层句柄*/
static int8_t g_bicycle_distance_layer_id = -1;
static int8_t g_bicycle_time_layer_id = -1;
static int8_t g_bicycle_real_time_speed_layer_id = -1;
static int8_t g_bicycle_curr_time_layer_id = -1;
static int8_t g_bicycle_real_time_altitude_layer_id = -1;

/*定义各个图层的位置*/


/*背景图层*/
#define BICYCLE_BG_ORIGIN_X		0
#define BICYCLE_BG_ORIGIN_Y		0
#define BICYCLE_BG_SIZE_H		128
#define BICYCLE_BG_SIZE_W		128


/*距离图层*/
#define BICYCLE_DISTANCE_ORIGIN_X	0
#define BICYCLE_DISTANCE_ORIGIN_Y	22
#define BICYCLE_DISTANCE_SIZE_H		30
#define BICYCLE_DISTANCE_SIZE_W		124

/*计时图层*/
#define BICYCLE_TIME_ORIGIN_X		0
#define BICYCLE_TIME_ORIGIN_Y		94
#define BICYCLE_TIME_SIZE_H			30
#define BICYCLE_TIME_SIZE_W			124

/*速度图层*/
#define BICYCLE_REAL_TIME_SPEED_ORIGIN_X	0
#define BICYCLE_REAL_TIME_SPEED_ORIGIN_Y	58
#define BICYCLE_REAL_TIME_SPEED_SIZE_H		30
#define BICYCLE_REAL_TIME_SPEED_SIZE_W		124


/*当前时间图层*/
#define BICYCLE_CURR_TIME_ORIGIN_X	2
#define BICYCLE_CURR_TIME_ORIGIN_Y	1
#define BICYCLE_CURR_TIME_SIZE_H	16
#define BICYCLE_CURR_TIME_SIZE_W	48

/*海拔图层*/
#define BICYCLE_ALTITUDE_ORIGIN_X	60
#define BICYCLE_ALTITUDE_ORIGIN_Y	1
#define BICYCLE_ALTITUDE_SIZE_H		16
#define BICYCLE_ALTITUDE_SIZE_W		48



#define BICYCLE_KEY_DISTANCE_4		0x01//m
#define BICYCLE_KEY_TIME_4		0x02		//s
#define BICYCLE_KEY_AVERAGE_SPEED_2     0x03		//m/s
#define BICYCLE_KEY_MAX_SPEED_2		0x04//m/s
#define BICYCLE_KEY_CLIMB_2		0x05//cm
#define BICYCLE_KEY_CALORIE_2		0x06//kcal  
#define BICYCLE_KEY_REAL_TIME_SPEED_2	0x21//m/s
#define BICYCLE_KEY_REAL_TIME_TREAD_2	0x22//rpm
#define BICYCLE_KEY_REAL_TIME_HEART_1	0x23//bpm
#define BICYCLE_KEY_REAL_TIME_ALTITUDE_2	0x24	//cm,偏移20000,如15000对应负50米，25000对应50米	
#define BICYCLE_KEY_REAL_SLOPE_1	0x25//
#define BICYCLE_KEY_GPS_1		0x26		
#define BICYCLE_KEY_SPORT_STATUS_1	0xa1	//
#define BICYCLE_KEY_SPORT_TYPE_1	0xa2



typedef struct tag_BicycleData
{
	uint32_t distance;	//m
	uint32_t time;		//s
	uint16_t average_speed;	//m/s
	uint16_t max_speed;	//m/s
	uint16_t climb;		//cm
	uint16_t calorie;	//kcal  
	uint16_t real_time_speed;	//m/s
	uint16_t real_time_tread;	//rpm
	uint8_t real_time_heart;	//bpm
	uint16_t real_time_altitude;	//cm,偏移20000,如15000对应负50米，25000对应50米	
	uint8_t real_time_slope;	//
	uint8_t gps;			
	uint8_t sport_status;	//
	uint8_t sport_type;	//	
}BicycleData;

typedef struct tag_BicycleDataSize
{
	uint8_t key;
	uint8_t value_size;
}BicycleDataSize; 



BicycleData g_bicycle_data = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};


typedef union tag_union
{
	uint8_t int8;
	uint16_t int16;	
	uint32_t int32;	
}int_union;


int8_t app_bicycle_get_value_size(uint8_t key)
{
	BicycleDataSize data_size[14] = {
		{BICYCLE_KEY_DISTANCE_4,		4},//m
		{BICYCLE_KEY_TIME_4,			4},		//s
		{BICYCLE_KEY_AVERAGE_SPEED_2,    	2},		//m/s
		{BICYCLE_KEY_MAX_SPEED_2,		2},//m/s
		{BICYCLE_KEY_CLIMB_2,			2},//cm
		{BICYCLE_KEY_CALORIE_2,			2},//kcal  
		{BICYCLE_KEY_REAL_TIME_SPEED_2,		2},//m/s
		{BICYCLE_KEY_REAL_TIME_TREAD_2,		2},//rpm
		{BICYCLE_KEY_REAL_TIME_HEART_1,		1},//bpm
		{BICYCLE_KEY_REAL_TIME_ALTITUDE_2,	2},	//cm,偏移20000,如15000对应负50米，25000对应50米	
		{BICYCLE_KEY_REAL_SLOPE_1,		1},//
		{BICYCLE_KEY_GPS_1,			1},		
		{BICYCLE_KEY_SPORT_STATUS_1,		1},	//
		{BICYCLE_KEY_SPORT_TYPE_1,		1}
		};

	int8_t i = 0;
	for (i = 0; i < sizeof(data_size)/sizeof(BicycleDataSize);i++)
	{
		if (key == data_size[i].key)
		{
			return (data_size[i].value_size);
		}
	}

	return 0;	
}



int8_t app_bicycle_get_value_by_id(uint8_t *data, uint8_t data_size, uint8_t key, uint32_t *value)
{
	uint8_t pos = 0, tmp_key = 0, len = 0;
	uint8_t *ptr = data;
	
	while(pos < data_size)
	{
		tmp_key = ptr[pos];
		len = app_bicycle_get_value_size(tmp_key);
		if ((len == 0) || (len > 4))
		{
			return 0;
		}
		if (tmp_key == key)/*找到*/
		{
	
			/*网络字节序转换为主机字节序*/	
			int8_t i = 0;	
			for (i = 0; i < len; i++)
			{
				*(((char *)value)+i) = *(ptr+pos+len-i);
			}
			return 1;			
		}
		pos += (len+1);	
	}

	return 0;
}



void app_bicycle_recv_callback(const char *link_id, const uint8_t *buff, uint8_t size)
{
		int8_t res  = 0;
		//uint8_t *data_ptr = (uint8_t *)(buff + 1 + buff[1] + 2 + 1);
		uint8_t *data_ptr = (uint8_t *)buff;
		int16_t data_size = size;
		uint32_t value;
		static int8_t pulse_flag = 0, pulse_time  = 0;
		BicycleData data = g_bicycle_data;

		res = app_bicycle_get_value_by_id(data_ptr, data_size,  BICYCLE_KEY_DISTANCE_4, &value);
		if (res != 0)
		{
			g_bicycle_data.distance = value;
		}

		res = app_bicycle_get_value_by_id(data_ptr, data_size,  BICYCLE_KEY_TIME_4, &value);
		if (res != 0)
		{
			g_bicycle_data.time = value;
		}

		res = app_bicycle_get_value_by_id(data_ptr, data_size,  BICYCLE_KEY_REAL_TIME_SPEED_2, &value);
		if (res != 0)
		{
			g_bicycle_data.real_time_speed = value;
		}

		res = app_bicycle_get_value_by_id(data_ptr, data_size,  BICYCLE_KEY_REAL_TIME_ALTITUDE_2, &value);
		if (res != 0)
		{
			g_bicycle_data.real_time_altitude = value;	//cm,偏移20000,如15000对应负50米，25000对应50米	
		}

		//printf("---------:%d\n", g_bicycle_data.real_time_altitude);	//cm,偏移20000,如15000对应负50米，25000对应50米	
		
	
		/*根据窗口ID获取窗口句柄*/
		P_Window p_window = app_window_stack_get_window_by_id(g_bicycle_window_id);	
		if (NULL != p_window)
		{
			/*获取图层指针*/
			P_Layer p_layer_distance = app_window_get_layer_by_id(p_window, g_bicycle_distance_layer_id);
			P_Layer p_layer_time = app_window_get_layer_by_id(p_window, g_bicycle_time_layer_id);
			P_Layer p_layer_real_time_speed = app_window_get_layer_by_id(p_window, g_bicycle_real_time_speed_layer_id);
			P_Layer p_layer_real_time_altitude = app_window_get_layer_by_id(p_window, g_bicycle_real_time_altitude_layer_id);
			char tmp_buf[20] = "";

			/*修改距离*/
			if ((NULL != p_layer_distance) && (g_bicycle_data.distance != data.distance))
			{
				sprintf(tmp_buf, "%d.%d", g_bicycle_data.distance/1000, (g_bicycle_data.distance%1000)/100);
				app_layer_set_text_text(p_layer_distance, tmp_buf);	
			}
		
			/*修改时间*/	
			if ((NULL != p_layer_time) && (g_bicycle_data.time != data.time))
			{	
				/*如果时间大于1个小时，则只显示小时和分钟*/
				if (g_bicycle_data.time >= 3600)
				{
					sprintf(tmp_buf, "%d:%02d", g_bicycle_data.time/3600, (g_bicycle_data.time%3600)/60);
				}
				else
				{
					sprintf(tmp_buf, "%d:%02d", (g_bicycle_data.time%3600)/60, (g_bicycle_data.time%60));
				}
				app_layer_set_text_text(p_layer_time, tmp_buf);
			}	

			/*修改速度*/	
			if ((NULL != p_layer_real_time_speed) && (g_bicycle_data.real_time_speed != data.real_time_speed))
			{	
				int32_t tmp_speed = (g_bicycle_data.real_time_speed * 3600)/100;
				sprintf(tmp_buf, "%d.%d", tmp_speed/1000, (tmp_speed%1000)/100);
				app_layer_set_text_text(p_layer_real_time_speed, tmp_buf);

//				printf("tmp_speed:%d, pulse_time:%d, pulse_flag:%d\n", tmp_speed, pulse_time, pulse_flag);

				/*超速震动*/
				if((tmp_speed > 120000) && ((pulse_time == 2) || (pulse_flag == 0)))
				{
					pulse_flag = 0;
					pulse_time = 3;		
				}
				else if ((tmp_speed > 80000) && ((pulse_time == 1) || (pulse_flag == 0)))
				{
					pulse_flag = 0;
					pulse_time = 2;		

				}
				else if ((tmp_speed > 60000) && (pulse_flag == 0))
				{
					pulse_time = 1;
				}
				else if (tmp_speed <= 60000)
				{
					pulse_flag = 0;
					pulse_time = 0;
				}

				if (pulse_time && (0 == pulse_flag))
				{
					maibu_service_vibes_pulse(VibesPulseTypeMiddle, pulse_time);	
					pulse_flag = 1;

//					printf("ddddddddddddddddddd :%d\n", pulse_time);
				}
			}	

			/*修改海拔, 如果为0，说明没有取到数据*/	
			if((NULL != p_layer_real_time_altitude) && (g_bicycle_data.real_time_altitude != data.real_time_altitude) && (g_bicycle_data.real_time_altitude != 0))
			{
				sprintf(tmp_buf, "%d", (g_bicycle_data.real_time_altitude - 20000)/100);
				app_layer_set_text_text(p_layer_real_time_altitude, tmp_buf);
			}	


			/*更新窗口*/	
			app_window_update(p_window);

		}
}




/*
 *--------------------------------------------------------------------------------------
 *     function:  create_layer_text
 *    parameter: 
 *       return:
 *  description:  创建文本图层
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
int8_t create_layer_text(P_Window p_window, char *str, GRect frame, enum GAlign align, int8_t font, enum GColor color)
{
	LayerText lt = {str, frame, align, font, 0};
	P_Layer layer_text = app_layer_create_text(&lt);
	app_layer_set_bg_color(layer_text, color);
	return(app_window_add_layer(p_window, layer_text));
}





static P_Window init_bicycle_window()
{
	P_Window p_window = NULL;
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}


	/*创建文字图片图层*/
	GRect bmp_frame = {{BICYCLE_BG_ORIGIN_X, BICYCLE_BG_ORIGIN_Y}, {BICYCLE_BG_SIZE_H, BICYCLE_BG_SIZE_W}};
	GBitmap bmp;
	res_get_user_bitmap(RES_BITMAP_WATCHFACE_BICYCLE_TEXT, &bmp);
	LayerBitmap layer_bitmap = {bmp, bmp_frame, GAlignCenter};
	P_Layer bmp_layer = app_layer_create_bitmap(&layer_bitmap);
	app_window_add_layer(p_window, bmp_layer);


	/*当前时间*/
	GRect frame_now = {{BICYCLE_CURR_TIME_ORIGIN_X, BICYCLE_CURR_TIME_ORIGIN_Y}, {BICYCLE_CURR_TIME_SIZE_H, BICYCLE_CURR_TIME_SIZE_W}};
	char buf[10] = "";
	struct date_time datetime;
	app_service_get_datetime(&datetime);
	sprintf(buf, "%02d:%02d", datetime.hour, datetime.min);

#if 0
	LayerText text_now = {buf, frame_now, GAlignLeft, U_ASCII_ARIALBD_16};
	P_Layer layer_now = app_layer_create_text(&text_now);
	app_layer_set_bg_color(layer_now, GColorBlack);
	g_bicycle_curr_time_layer_id = app_window_add_layer(p_window, layer_now);
#endif

	g_bicycle_curr_time_layer_id =  create_layer_text(p_window, buf, frame_now, GAlignLeft, U_ASCII_ARIALBD_16, GColorBlack);



	/*当前海拔*/
	GRect frame_altitude = {{BICYCLE_ALTITUDE_ORIGIN_X, BICYCLE_ALTITUDE_ORIGIN_Y}, {BICYCLE_ALTITUDE_SIZE_H, BICYCLE_ALTITUDE_SIZE_W}};
	if (0 != g_bicycle_data.real_time_altitude)
	{
		sprintf(buf, "%d", (g_bicycle_data.real_time_altitude - 20000)/100) ;
	}
	else
	{
		sprintf(buf, "%d", g_bicycle_data.real_time_altitude) ;
	}
#if 0
	LayerText text_altitude = {buf, frame_altitude, GAlignRight, U_ASCII_ARIALBD_16};
	P_Layer layer_altitude = app_layer_create_text(&text_altitude);
	app_layer_set_bg_color(layer_altitude, GColorBlack);
	g_bicycle_real_time_altitude_layer_id = app_window_add_layer(p_window, layer_altitude);
#endif

	g_bicycle_real_time_altitude_layer_id =  create_layer_text(p_window, buf, frame_altitude, GAlignRight, U_ASCII_ARIALBD_16, GColorBlack);



	/*距离*/
	GRect frame_distance = {{BICYCLE_DISTANCE_ORIGIN_X, BICYCLE_DISTANCE_ORIGIN_Y}, {BICYCLE_DISTANCE_SIZE_H, BICYCLE_DISTANCE_SIZE_W}};
	sprintf(buf, "%d.%d", g_bicycle_data.distance/1000, (g_bicycle_data.distance%1000)/100);
#if 0
	LayerText text_distance = {buf, frame_distance, GAlignRight, U_ASCII_ARIALBD_24};
	P_Layer layer_distance = app_layer_create_text(&text_distance);
	g_bicycle_distance_layer_id = app_window_add_layer(p_window, layer_distance);
#endif

	g_bicycle_distance_layer_id =  create_layer_text(p_window, buf, frame_distance, GAlignRight, U_ASCII_ARIALBD_30, GColorWhite);


	/*时间*/
	GRect frame_time = {{BICYCLE_TIME_ORIGIN_X, BICYCLE_TIME_ORIGIN_Y}, {BICYCLE_TIME_SIZE_H, BICYCLE_TIME_SIZE_W}};

	/*如果时间大于1个小时，则只显示小时和分钟*/
	if (g_bicycle_data.time >= 3600)
	{
		sprintf(buf, "%d:%02d", g_bicycle_data.time/3600, (g_bicycle_data.time%3600)/60);
	}
	else
	{
		sprintf(buf, "%d:%02d", (g_bicycle_data.time%3600)/60, (g_bicycle_data.time%60));
	}
#if 0
	LayerText text_time = {buf, frame_time, GAlignRight, U_ASCII_ARIALBD_24};
	P_Layer layer_time = app_layer_create_text(&text_time);
	g_bicycle_time_layer_id = app_window_add_layer(p_window, layer_time);
#endif

	g_bicycle_time_layer_id =  create_layer_text(p_window, buf, frame_time, GAlignRight, U_ASCII_ARIALBD_30, GColorWhite);



	/*实时速度*/
	GRect frame_real_time_speed = {{BICYCLE_REAL_TIME_SPEED_ORIGIN_X, BICYCLE_REAL_TIME_SPEED_ORIGIN_Y}, {BICYCLE_REAL_TIME_SPEED_SIZE_H, BICYCLE_REAL_TIME_SPEED_SIZE_W}};
	int32_t tmp_speed = (g_bicycle_data.real_time_speed * 3600)/100;
	sprintf(buf, "%d.%d", tmp_speed/1000, (tmp_speed%1000)/100);
#if 0
	LayerText text_real_time_speed = {buf, frame_real_time_speed, GAlignRight, U_ASCII_ARIALBD_24};
	P_Layer layer_real_time_speed = app_layer_create_text(&text_real_time_speed);
	g_bicycle_real_time_speed_layer_id = app_window_add_layer(p_window, layer_real_time_speed);
#endif

	g_bicycle_real_time_speed_layer_id =  create_layer_text(p_window, buf, frame_real_time_speed, GAlignRight, U_ASCII_ARIALBD_30, GColorWhite);


	return p_window;
}




static void bicycle_time_change(enum SysEventType type, void *context)
{

	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		P_Window p_window = NULL;

		/*根据窗口ID获取窗口句柄*/
		p_window = app_window_stack_get_window_by_id(g_bicycle_window_id);
		if (p_window == NULL)
		{
			return;
		}
		
		/*获取窗口中分钟图层句柄*/
		P_Layer p_layer = app_window_get_layer_by_id(p_window, g_bicycle_curr_time_layer_id);	
		if (p_layer != NULL)
		{
			char str[20] = "";
			struct date_time datetime;
			app_service_get_datetime(&datetime);
			sprintf(str, "%02d:%02d", datetime.hour, datetime.min);
			app_layer_set_text_text(p_layer, str);

			/*窗口显示*/	
			app_window_update(p_window);
		}

	}
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



	P_Window p_window = init_bicycle_window(); 
	if(p_window != NULL)
	{
		/*放入窗口栈显示*/
		g_bicycle_window_id = app_window_stack_push(p_window);

		/*注册接受数据回调函数*/
		maibu_comm_register_msg_callback(app_bicycle_recv_callback);

		/*注册一个事件通知回调，当有时间改变是，立即更新时间*/
		maibu_service_sys_event_subscribe(bicycle_time_change);
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
			static int s_index = 0;
			s_index++;
			uint8_t data[] = {
						0x01,0x00,0x00,0x00,0x00,	//里程
						0x02,0x00,0x00,0x00,0x00,	//运动时间
						0x21,0x00,0x00,		//实时速度
						0x26,0x04,		//GPS信号
						0xa1,0x11,		//运动状态
						0xa2,0x03,		//运动类型
						0x24,0x50,0x00		//海拔	
					};
			//uint8_t data[] = {0x01,0x00,0x00,0x03, 0xe8, 0x02,0x00,0x01,0x00,0xc8};//,0x21,0x02,0x05,0x26,0x04,0xa1,0x11};
			data[3] = s_index;
			data[8] = s_index;
			data[11] = 0x06+s_index;
			if (s_index > 5 && s_index < 8)
			{
				data[11] = 0x02;
			}
	
			app_bicycle_recv_callback("maibu", data, sizeof(data));
	
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


