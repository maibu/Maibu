/*
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


#include "maibu_sdk.h"
#include "maibu_res.h"

#define LAYER_NUM 11

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

static uint8_t g_request_timer		   = 0;

static int32_t g_app_window_id = -1;
static uint32_t g_app_timer_change_id = -1;
static int32_t g_app_mwd_layer_step_id[LAYER_NUM];
static int32_t get_icon_key[1] =
{
	RES_BITMAP_WATCHFACE_BG,
};

typedef struct tag_BicycleData
{
	uint32_t distance;	//m		距离
	uint32_t time;		//s		时间
	uint16_t average_speed;	//m/s	平均速度
	uint16_t max_speed;	//m/s		最大速度
	uint16_t climb;		//cm		上升
	uint16_t calorie;	//kcal		热卡
	uint16_t real_time_speed;	//m/s	实时速度
	uint16_t real_time_tread;	//rpm	踏频
	uint8_t real_time_heart;	//bpm	心率
	uint32_t real_time_altitude;	//cm	海拔,偏移20000,如15000对应负50米，25000对应50米	
	uint8_t real_time_slope;	//坡度
	uint8_t gps;			//gps
	uint8_t sport_status;	//运动状态
	uint8_t sport_type;	//运动类型
}BicycleData;

typedef struct tag_BicycleDataSize
{
	uint8_t key;
	uint8_t value_size;
}BicycleDataSize; 



BicycleData g_bicycle_data = {0,0,0,0,0,0,0,0,0,20000,0,0,0,0};



static GRect g_txt_frame[LAYER_NUM] = {
	{{0, 0}, {24, 72}}         //时间
	, {{0, 32}, {20, 72}}      //运动状态
	, {{72, 16}, {42, 104}}    //实时速度
	, {{70, 0}, {16, 98}}      //电量温度
	, {{0, 148}, {24, 103}}    
	, {{21, 59}, {24, 55}}     //最大速度
	, {{110, 59}, {24, 55}}    //平均速度
	, {{16, 114}, {30, 69}}    //距离
	, {{16, 84}, {30, 69}}     //海拔
	, {{103, 87}, {42, 73}}    //心率
	, {{103, 134}, {42, 73}}   //踏频
};


//U_ASCII_ARIAL_12, U_ASCII_ARIAL_14, U_ASCII_ARIAL_16,U_ASCII_ARIAL_20
//U_ASCII_ARIAL_24, U_ASCII_ARIAL_30, U_ASCII_ARIAL_42
static uint8_t g_txt_f[LAYER_NUM] = {
	U_ASCII_ARIALBD_24
	, U_GBK_SIMSUN_20
	, U_ASCII_ARIALBD_42
	, U_ASCII_ARIAL_16
	, U_ASCII_ARIALBD_24
	, U_ASCII_ARIAL_24
	, U_ASCII_ARIAL_24
	, U_ASCII_ARIALBD_30
	, U_ASCII_ARIALBD_30
	, U_ASCII_ARIALBD_42
	, U_ASCII_ARIALBD_42
};
static uint8_t g_txt_ale[LAYER_NUM] = {
	GAlignCenter
	, GAlignCenter
	, GAlignCenter
	, GAlignRight
	, GAlignCenter
	, GAlignRight
	, GAlignRight
	, GAlignRight
	, GAlignRight
	, GAlignRight
	, GAlignRight
};

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
		{BICYCLE_KEY_REAL_TIME_TREAD_2,		2},//rpm踏频
		{BICYCLE_KEY_REAL_TIME_HEART_1,		1},//bpm心率
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


void getSportStr(int id, char* str)
{
	int k,m;
	struct date_time t;
	app_service_get_datetime(&t);
	
	if(0==id)//时间:
	{
		sprintf(str, "%02d:%02d", t.hour, t.min);
	}	
	else if(1==id)//状态:
	{
		if (1==g_bicycle_data.sport_type)
		{
			sprintf(str, "徒步");
		}
		if (2==g_bicycle_data.sport_type)
		{
			sprintf(str, "跑步");
		}
		if (3==g_bicycle_data.sport_type)
		{
			sprintf(str, "骑行");
		}
/*		if (17==g_bicycle_data.sport_status)
		{
			sprintf(str, "%s中.",str);
		}
		if (16==g_bicycle_data.sport_status)
		{
			sprintf(str, "定位中.");
		}
*/
		if (18==g_bicycle_data.sport_status)
		{
			sprintf(str, "暂停");
		}
		if (0==g_bicycle_data.sport_status||16==g_bicycle_data.sport_status)
		{
		//char battery_new;
		//maibu_get_battery_percent(&battery_new);        //获取电量数据
		//sprintf(str, "%d%%", battery_new);
		sprintf(str, "停止");
		}

			//sprintf(str, "%d/%d/%d", g_bicycle_data.sport_type, g_bicycle_data.sport_status, g_bicycle_data.gps);
	}	
	else if(2==id)//实时速度:
	{
	//char battery_new;
    	//maibu_get_battery_percent(&battery_new);        //获取电量数据
		//sprintf(str, "%d%%", battery_new);
		int32_t tmp_speed = (g_bicycle_data.real_time_speed * 3600)/100;
		if (tmp_speed>100*1000)
		{
		sprintf(str, "%d.%01d", tmp_speed/1000, (tmp_speed%1000)/100);
		}
		else
		{
		sprintf(str, "%d.%02d", tmp_speed/1000, (tmp_speed%1000)/10);
		}
		//sprintf(str, "20.56");
		//sprintf(str, "%d/%d/%d/%d/%d", b_bs,o_bs, g_i,g_i2,g_zt);
	}	
	else if(3==id)//表温:
	{
		char battery_new;
		maibu_get_battery_percent(&battery_new);        //获取电量数据
		//sprintf(str, "%d%%", battery_new);
		float tmp1 = 0;
		maibu_get_temperature(&tmp1);
		sprintf(str, "%d%% %.1f", battery_new, tmp1);
//		sprintf(str, "");
	}	
	else if(4==id)//时间
	{
		/*如果时间大于1个小时，则只显示小时和分钟*/
		if (g_bicycle_data.time >= 3600)
		{
			sprintf(str, "%d:%02d:%02d", g_bicycle_data.time/3600, (g_bicycle_data.time%3600)/60, (g_bicycle_data.time%60));
		}
		else
		{
			sprintf(str, "0:%02d:%02d", (g_bicycle_data.time%3600)/60, (g_bicycle_data.time%60));
		}
	}	
	else if(5==id)//最大速度:
	{
		int32_t tmp_speed = (g_bicycle_data.max_speed * 3600)/100;
		sprintf(str, "%d.%02d", tmp_speed/1000, (tmp_speed%1000)/10);
	}	
	else if(6==id)//平均速度:
	{
		int32_t tmp_speed = (g_bicycle_data.average_speed * 3600)/100;
		sprintf(str, "%d.%02d", tmp_speed/1000, (tmp_speed%1000)/10);
	}	
	else if(7==id)//距离:
	{
		sprintf(str, "%d.%02d", g_bicycle_data.distance/1000, (g_bicycle_data.distance%1000)/10);
	}	
	else if(8==id)//海拔:
	{
		//g_bicycle_data.real_time_altitude=1912;
		int32_t tmp_altitude=(g_bicycle_data.real_time_altitude - 20000);
		if (tmp_altitude>=100000||tmp_altitude<0)
		{
			sprintf(str, "%d", tmp_altitude/100);
		}else
		{
			sprintf(str, "%01d.%02d", tmp_altitude/100, tmp_altitude%100);
		}
	}	
	else if(9==id)//心率:
	{
			sprintf(str, "%d", g_bicycle_data.real_time_heart);
	}	
	else if(10==id)//踏频
	{
		sprintf(str, "%d", g_bicycle_data.real_time_tread) ;
/*
		if (3==g_bicycle_data.sport_type)
		{
			sprintf(str, "%d", g_bicycle_data.real_time_tread) ;

			uint32_t tmp_time;
			//uint16_t js=0;
			tmp_time=maibu_get_system_tick();
			SportData data;
			maibu_get_sport_data(&data, 0);
			//sprintf(str, "%d", data.step);
			if (tmp_time-o_time<5000)
			{
				//js=(int)(data.step-o_bs)*(60/(tmp_time-o_time))/1000;

			}else if (tmp_time-o_time>60000)
			{
				o_bp=0;
				o_bs=data.step;
				o_time=tmp_time;
			}else{
				o_bp=(int)(data.step-o_bs)*(60/(tmp_time-o_time))/1000;
				o_bs=data.step;
				o_time=tmp_time;
			}

		}
		else{
			sprintf(str, "%d", o_bp);
		}
*/
	}	

}

void setSportInfo(P_Window p_window)
{
	int i;
	char str[40] = "";
	P_Layer layer_sport;
		for(i=0; i<LAYER_NUM; i++)
		{
			layer_sport = app_window_get_layer_by_id(p_window, g_app_mwd_layer_step_id[i]);
			if (NULL == layer_sport)
				continue;
			memset(str,0,sizeof(str));
			getSportStr(i, str);	
			app_layer_set_text_text(layer_sport, str);	
		}
}

void setInfoLayer(P_Window p_window, int id)
{
	char sta[LAYER_NUM] = {1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1};
	char str[40] = "";
	getSportStr(id, str);

	LayerText temp_LayerText = {0};
	temp_LayerText.text = str;
	temp_LayerText.frame = g_txt_frame[id];
	temp_LayerText.alignment = g_txt_ale[id];
	temp_LayerText.font_type = g_txt_f[id];
	temp_LayerText.bound_width = 0;
	if(sta[id]==0)
		temp_LayerText.foregroundColor = GColorWhite;
	else
		temp_LayerText.foregroundColor = GColorBlack;
	
	P_Layer layer_info = app_layer_create_text(&temp_LayerText);
	
	if(layer_info != NULL)
	{
		//设置文本图层背景颜色
		if(sta[id]==0)
			app_layer_set_bg_color(layer_info, GColorBlack);
		else
			app_layer_set_bg_color(layer_info, GColorWhite);
			
		//添加图层到窗口中
		g_app_mwd_layer_step_id[id] = app_window_add_layer(p_window, layer_info);
	}
}
void upData()
{
	P_Window p_window = app_window_stack_get_window_by_id(g_app_window_id);	
	if (NULL == p_window)
	{
		return;
	}
	setSportInfo(p_window);
	app_window_update(p_window);

}

/*创建并显示图片图层，需要坐标值，得到icon_key的数组，数组需要的参数值，P_Window*/
int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,enum GAlign how_to_align,enum GColor black_or_white,int32_t bmp_array_name[],int bmp_id_number)
{	

	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_array_name[bmp_id_number], &bmp_point);
	LayerBitmap layer_bitmap_struct_l = {bmp_point, *temp_p_frame, how_to_align};
 	temp_P_Layer = app_layer_create_bitmap(&layer_bitmap_struct_l);
	
	if(temp_P_Layer != NULL)
	{
		app_layer_set_bg_color(temp_P_Layer, black_or_white);
		return app_window_add_layer(p_window, temp_P_Layer);
	}

	return 0;
}


void time_change(enum SysEventType type, void *context)
{
	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		upData();
	}
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
		if (res != 0&&value!=0)
		{
			g_bicycle_data.distance = value;
		}

		res = app_bicycle_get_value_by_id(data_ptr, data_size,  BICYCLE_KEY_TIME_4, &value);
		if (res != 0)
		{
			g_bicycle_data.time = value;
		}

		res = app_bicycle_get_value_by_id(data_ptr, data_size,  BICYCLE_KEY_AVERAGE_SPEED_2, &value);
		if (res != 0&&value!=0)
		{
			g_bicycle_data.average_speed = value;
		}
		res = app_bicycle_get_value_by_id(data_ptr, data_size,  BICYCLE_KEY_MAX_SPEED_2, &value);
		if (res != 0&&value!=0)
		{
			g_bicycle_data.max_speed = value;
		}
		res = app_bicycle_get_value_by_id(data_ptr, data_size,  BICYCLE_KEY_REAL_TIME_SPEED_2, &value);
		if (res != 0)
		{
			g_bicycle_data.real_time_speed = value;
		}

		res = app_bicycle_get_value_by_id(data_ptr, data_size,  BICYCLE_KEY_REAL_TIME_ALTITUDE_2, &value);
		if (res != 0&&value!=20000)
		{
			g_bicycle_data.real_time_altitude = value;	//cm,偏移20000,如15000对应负50米，25000对应50米	
		}
	
		
		res = app_bicycle_get_value_by_id(data_ptr, data_size,  BICYCLE_KEY_REAL_TIME_TREAD_2, &value);
		if (res != 0&&value!=0)
		{
			g_bicycle_data.real_time_tread = value;	//
		}

		res = app_bicycle_get_value_by_id(data_ptr, data_size,  BICYCLE_KEY_REAL_TIME_HEART_1, &value);
		if (res != 0&&value!=0)
		{
			g_bicycle_data.real_time_heart = value;	//
		}

		res = app_bicycle_get_value_by_id(data_ptr, data_size,  BICYCLE_KEY_GPS_1, &value);
		if (res != 0)
		{
			g_bicycle_data.gps = value;	//
		}

		res = app_bicycle_get_value_by_id(data_ptr, data_size,  BICYCLE_KEY_SPORT_STATUS_1, &value);
		if (res != 0)
		{
			g_bicycle_data.sport_status = value;	//
		}

		res = app_bicycle_get_value_by_id(data_ptr, data_size,  BICYCLE_KEY_SPORT_TYPE_1, &value);
		if (res != 0)
		{
			g_bicycle_data.sport_type = value;	//
		}

		//o_time=maibu_get_system_tick();

		/*更新窗口*/	
		upData();

}



P_Window init_window()
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	/*添加完成图片图层*/
	GRect frame_bg = {{0, 0}, {176, 176}};
	display_target_layer(p_window,&frame_bg,GAlignCenter,GColorBlack,get_icon_key,0);
	//setInfoLayer(p_window, 6);
	int i;
	for(i=0;i<LAYER_NUM;i++) {
		setInfoLayer(p_window, i);
	}

	return (p_window);

}

int main()
{
//simulator_init();

	P_Window p_window = init_window();
	if (p_window != NULL)
	{
		g_app_window_id = app_window_stack_push(p_window);
		/*注册接受数据回调函数*/
		maibu_comm_register_msg_callback(app_bicycle_recv_callback);

		/*注册一个事件通知回调，当有时间改变是，立即更新时间*/
		maibu_service_sys_event_subscribe(time_change);

	}	


//simulator_wait();
	return 0;
}
