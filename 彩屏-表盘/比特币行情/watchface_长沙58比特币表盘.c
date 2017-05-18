#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "maibu_sdk.h"
#include "maibu_res.h"

#define WATCHFACE_GYD_BG_ORIGIN_X 	0
#define WATCHFACE_GYD_BG_ORIGIN_Y 	0
#define WATCHFACE_GYD_BG_WIZE_H		176
#define WATCHFACE_GYD_BG_SIZE_W 	176


#define WATCHFACE_GYD_TIME_HOUR1_ORIGIN_X 	8
#define WATCHFACE_GYD_TIME_HOUR1_ORIGIN_Y 	71
#define WATCHFACE_GYD_TIME_HOUR1_WIZE_H		35
#define WATCHFACE_GYD_TIME_HOUR1_SIZE_W 	36


#define WATCHFACE_GYD_TIME_HOUR2_ORIGIN_X 	44
#define WATCHFACE_GYD_TIME_HOUR2_ORIGIN_Y 	71
#define WATCHFACE_GYD_TIME_HOUR2_WIZE_H		35
#define WATCHFACE_GYD_TIME_HOUR2_SIZE_W 	36

#define WATCHFACE_GYD_TIME_MIN1_ORIGIN_X 	96
#define WATCHFACE_GYD_TIME_MIN1_ORIGIN_Y 	71
#define WATCHFACE_GYD_TIME_MIN1_WIZE_H		35
#define WATCHFACE_GYD_TIME_MIN1_SIZE_W 		36

#define WATCHFACE_GYD_TIME_MIN2_ORIGIN_X 	132
#define WATCHFACE_GYD_TIME_MIN2_ORIGIN_Y 	71	
#define WATCHFACE_GYD_TIME_MIN2_WIZE_H		35
#define WATCHFACE_GYD_TIME_MIN2_SIZE_W 		36

#define WATCHFACE_GYD_WEEK_ORIGIN_X 	108
#define WATCHFACE_GYD_WEEK_ORIGIN_Y 	20
#define WATCHFACE_GYD_WEEK_WIZE_H		16
#define WATCHFACE_GYD_WEEK_SIZE_W 		66

#define WATCHFACE_GYD_NAME_ORIGIN_X 	7
#define WATCHFACE_GYD_NAME_ORIGIN_Y 	2
#define WATCHFACE_GYD_NAME_WIZE_H		16
#define WATCHFACE_GYD_NAME_SIZE_W 		110


#define WATCHFACE_GYD_DAY2_ORIGIN_X 	124
#define WATCHFACE_GYD_DAY2_ORIGIN_Y 	2
#define WATCHFACE_GYD_DAY2_WIZE_H		16
#define WATCHFACE_GYD_DAY2_SIZE_W 		50


#define WATCHFACE_GYD_STEP_ORIGIN_X  	0
#define WATCHFACE_GYD_STEP_ORIGIN_Y 	128
#define WATCHFACE_GYD_STEP_WIZE_H		36
#define WATCHFACE_GYD_STEP_SIZE_W 		19


const static char wday_str[7][8]={"周日","周一","周二","周三","周四","周五","周六"}; 

#define KEY_WORDS	"cName,last,type,dollar"
#define REQUEST_URL "https://www.btc123.com/api/getTicker?symbol="

const static GRect bmp_origin_size_bg = {
	{WATCHFACE_GYD_BG_ORIGIN_X,WATCHFACE_GYD_BG_ORIGIN_Y},
	{WATCHFACE_GYD_BG_WIZE_H,WATCHFACE_GYD_BG_SIZE_W}
};


const static GRect bmp_origin_size[] = {

	{
		{WATCHFACE_GYD_TIME_HOUR1_ORIGIN_X,WATCHFACE_GYD_TIME_HOUR1_ORIGIN_Y},
		{WATCHFACE_GYD_TIME_HOUR1_WIZE_H,WATCHFACE_GYD_TIME_HOUR1_SIZE_W}
	},
	{
		{WATCHFACE_GYD_TIME_HOUR2_ORIGIN_X,WATCHFACE_GYD_TIME_HOUR2_ORIGIN_Y},
		{WATCHFACE_GYD_TIME_HOUR2_WIZE_H,WATCHFACE_GYD_TIME_HOUR2_SIZE_W}
	},
	{
		{WATCHFACE_GYD_TIME_MIN1_ORIGIN_X,WATCHFACE_GYD_TIME_MIN1_ORIGIN_Y},
		{WATCHFACE_GYD_TIME_MIN1_WIZE_H,WATCHFACE_GYD_TIME_MIN1_SIZE_W}
	},	

	{
		{WATCHFACE_GYD_TIME_MIN2_ORIGIN_X,WATCHFACE_GYD_TIME_MIN2_ORIGIN_Y},
		{WATCHFACE_GYD_TIME_MIN2_WIZE_H,WATCHFACE_GYD_TIME_MIN2_SIZE_W}
	},

	{
		{WATCHFACE_GYD_DAY2_ORIGIN_X,WATCHFACE_GYD_DAY2_ORIGIN_Y},
		{WATCHFACE_GYD_DAY2_WIZE_H,WATCHFACE_GYD_DAY2_SIZE_W}
	},

	{
		{WATCHFACE_GYD_WEEK_ORIGIN_X,WATCHFACE_GYD_WEEK_ORIGIN_Y},
		{WATCHFACE_GYD_WEEK_WIZE_H,WATCHFACE_GYD_WEEK_SIZE_W}
	},
	
	
	{
		{WATCHFACE_GYD_STEP_ORIGIN_X,WATCHFACE_GYD_STEP_ORIGIN_Y},
		{WATCHFACE_GYD_STEP_WIZE_H,WATCHFACE_GYD_STEP_SIZE_W}
	},
	{
		{WATCHFACE_GYD_NAME_ORIGIN_X,WATCHFACE_GYD_NAME_ORIGIN_Y},
		{WATCHFACE_GYD_NAME_WIZE_H,WATCHFACE_GYD_NAME_SIZE_W}
	}
	
};


//小号数字图片数组
const static uint32_t get_LIT_icon_key[] =
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

//大号数字图片数组
const static uint32_t get_BIG_icon_key[] =
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

typedef struct name_api
{
	char name[24];
	char symbol[24];
}name_api;	

const static name_api CNameApi[] = {

	{
		.name = {"中国"},
		.symbol = {"btcchinabtccny"}
	},
	{
		.name = {"火币"},
		.symbol = {"huobibtccny"}
	},
	{
		.name = {"国际"},//okcoin国际
		.symbol = {"okcoinbtcusd"}
	},
	{
		.name = {"okcoin"},
		.symbol = {"okcoincnbtccny"}
	},
	{
		.name = {"chbtc"},
		.symbol = {"chbtcbtccny"}
	},
	{
		.name = {"coinbase"},
		.symbol = {"coinbasebtcusd"}
	},
	{
		.name = {"bitfinex"},
		.symbol = {"bitfinexbtcusd"}
	},
	{
		.name = {"bitstamp"},
		.symbol = {"bitstampbtcusd"}
	},
	
	{
		.name = {"比特儿"},
		.symbol = {"bterbtccny"}
	},
	{
		.name = {"时代"},
		.symbol = {"btc38btccny"}
	},
	{
		.name = {"btc"},
		.symbol = {"btcebtcusd"}
	},
	{
		.name = {"比特币交易网"},
		.symbol = {"btctradebtccny"}
	},
	{
		.name = {"云币"},
		.symbol = {"yunbibtccny"}
	}
	
};



/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_window_id = -1;


/*窗口ID, 通过该窗口ID获取窗口句柄*/
static char g_alarm_str[12] = {0};
static int8_t g_alarm_flag = -1;


static char g_CNY_str[12] = {0};
static char g_USD_str[12] = {0};

static char g_pre_CNY_str[12] = {0};
static char g_pre_USD_str[12] = {0};


static int8_t g_money_flag = 0;

//static int8_t g_is_alarmed_flag = 0;

static uint32_t g_comm_id_web_request =0;
static char g_symbol[24] = {"huobibtccny"};
static char g_cname[24] = {0};

static uint8_t g_blue_status = BLEStatusClose;		//蓝牙状态
static int32_t g_layer_id_ble = -1;    //蓝牙图层


static P_Window init_window(void);

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



/*
 *--------------------------------------------------------------------------------------
 *     function:  app_watch_time_change 
 *    parameter: 
 *       return:
 *  description:  系统时间有变化时，更新时间图层
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
static void app_watch_time_change(enum SysEventType type, void *context)
{

	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		window_reloading();
	}
}





/*
 *--------------------------------------------------------------------------------------
 *     function:  
 *    parameter: 
 *       return:
 *  description:  生成表盘窗口的各图层
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
 /*创建并显示文本图层*/
int32_t display_target_layerText(P_Window p_window,const GRect  *temp_p_frame,enum GAlign how_to_align,enum GColor color,char * str,uint8_t font_type,enum GColor fcolor)
{
	LayerText temp_LayerText = {0};
	temp_LayerText.text = str;
	temp_LayerText.frame = *temp_p_frame;
	temp_LayerText.alignment = how_to_align;
	temp_LayerText.font_type = font_type;
	temp_LayerText.foregroundColor = fcolor;
	
	P_Layer p_layer = app_layer_create_text(&temp_LayerText);
	
	if(p_layer != NULL)
	{
		app_layer_set_bg_color(p_layer, color);
		return app_window_add_layer(p_window, p_layer);
	}
	return 0;
}

/*创建并显示图片图层，需要坐标值，得到icon_key的数组，数组需要的参数值，P_Window*/
int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,enum GAlign how_to_align,enum GColor black_or_white,uint32_t bmp_key)
{	


	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_key, &bmp_point);
	LayerBitmap layer_bitmap_struct_l = {bmp_point, *temp_p_frame, how_to_align};
 	temp_P_Layer = app_layer_create_bitmap(&layer_bitmap_struct_l);
	
	if(temp_P_Layer != NULL)
	{
		app_layer_set_bg_color(temp_P_Layer, black_or_white);
		return app_window_add_layer(p_window, temp_P_Layer);
	}

	return 0;
}



/*定义后退按键事件*/
static void counter_select_back(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		g_money_flag = g_money_flag + 1 <= 1? g_money_flag + 1:0;
		window_reloading();
	}
}



static void timer_callback(date_time_t tick_time, uint32_t millis, void* context)
{
	

	P_Window p_window = app_window_stack_get_window_by_id(g_window_id); 
	
	if(p_window == NULL)
	{
		return;
	}

	uint8_t old_ble_status = g_blue_status;
	g_blue_status = maibu_get_ble_status();
	//os_printk("old_ble_status:%d g_blue_status:%d\n",old_ble_status,g_blue_status);
	if((old_ble_status == BLEStatusAdvertising)||(old_ble_status == BLEStatusClose))
	{
		if((g_blue_status == BLEStatusConnected)||(g_blue_status == BLEStatusUsing))
		{
			maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_layer_id_ble),false);
		}
	}
	else if((old_ble_status == BLEStatusConnected)||(old_ble_status == BLEStatusUsing))
	{
		if((g_blue_status == BLEStatusAdvertising)||(g_blue_status == BLEStatusClose))
		{
			maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_layer_id_ble),true);
		}
	}
	
	app_window_update(p_window);
}


static P_Window init_window(void)
{
	P_Window p_window = NULL;

	/*创建一个窗口*/
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	/*创建背景图层*/
	display_target_layer(p_window,&bmp_origin_size_bg,GAlignLeft,GColorBlack,RES_BITMAP_WATCHFACE_WEATHER_BG);

	struct date_time datetime;
	app_service_get_datetime(&datetime);

	
	GRect frame = {{7,22},{17,17}};
	
	g_layer_id_ble = display_target_layer(p_window,&frame,GAlignCenter,GColorWhite,RES_BITMAP_BLUE_ICON);
	
	if((g_blue_status == BLEStatusConnected)||(g_blue_status == BLEStatusUsing))
	{
		maibu_layer_set_visible_status(app_window_get_layer_by_id(p_window,g_layer_id_ble),false);
	}
	

	uint8_t i = 0;
	/*创建时间图层*/
	uint32_t time_key_array[] = {datetime.hour/10,datetime.hour%10,datetime.min/10,datetime.min%10};

	for(i = 0;i<=3;i++)
	{
		display_target_layer(p_window,&bmp_origin_size[i],GAlignLeft,GColorBlack,get_BIG_icon_key[time_key_array[i]]);
	}
		
	/*创建星期图层*/
	int8_t temp_str[8] = {0};
	sprintf(temp_str, "%s",&wday_str[datetime.wday]);
	
	display_target_layerText(p_window,&bmp_origin_size[5],GAlignRight,GColorBlack,temp_str,U_ASCII_ARIAL_16,GColorWhite);
	/*创建月日图层*/

	sprintf(temp_str, "%d/%d",datetime.mon,datetime.mday);
	
	display_target_layerText(p_window,&bmp_origin_size[4],GAlignRight,GColorBlack,temp_str,U_ASCII_ARIAL_16,GColorWhite);

	/*创建名称图层*/
	display_target_layerText(p_window,&bmp_origin_size[7],GAlignLeft,GColorBlack,g_cname,U_ASCII_ARIAL_16,GColorWhite);

	
	/*创建步数图片图层*/
	//步数
	GRect temp_origin_size = {0};
	
	temp_origin_size = bmp_origin_size[6];

	char money_str[12] = {0};
	char money_name[4] = {0};
	
	if(g_money_flag == 0)
	{
		memcpy(money_str,g_CNY_str,sizeof(g_CNY_str));
		memcpy(money_name,"中",3);

	}
	else if(g_money_flag == 1)
	{
		memcpy(money_str,g_USD_str,sizeof(g_USD_str));
		memcpy(money_name,"美",3);
	}
	
	//os_printk("g_CNY_str:%s\n",g_CNY_str);
	//os_printk("g_USD_str:%s\n",g_USD_str);
		
	if(strlen(money_str) == 0)
	{
		return p_window;
	}
	
	temp_origin_size.origin.x = (178 - (strlen(money_str)*WATCHFACE_GYD_STEP_SIZE_W-(14-5)) - 17)/2;
	//os_printk("name x:%d\n",temp_origin_size.origin.x);
	
	temp_origin_size.size.h = 17;
	temp_origin_size.size.w = 17;
	
	display_target_layerText(p_window,&temp_origin_size,GAlignCenter,GColorWhite,money_name,U_ASCII_ARIAL_16,GColorBlack);

	temp_origin_size.origin.x = temp_origin_size.origin.x + temp_origin_size.size.w;
	//os_printk("num x:%d\n",temp_origin_size.origin.x);
	
	temp_origin_size.size.h = bmp_origin_size[6].size.h;
	temp_origin_size.size.w = bmp_origin_size[6].size.w;
	
	
	for(i = 0;i < strlen(money_str);i++)
	{	
		uint16_t bmp_key = 0;
		if((money_str[i]>='0')&&(money_str[i]<='9'))
		{
			bmp_key = get_LIT_icon_key[money_str[i]-'0'];
			temp_origin_size.size.w = 20;
		
		}
		else if(money_str[i] == '.')
		{
			bmp_key = RES_BITMAP_WATCHFACE_DOT;
			temp_origin_size.size.w = 8;
			
		}
		else 
		{
			break;
		}

		display_target_layer(p_window,&temp_origin_size,GAlignLeft,GColorBlack,bmp_key);
			
		temp_origin_size.origin.x = temp_origin_size.origin.x + temp_origin_size.size.w; 
		
	}
	
	app_window_click_subscribe(p_window, ButtonIdBack, counter_select_back);

	//蓝牙一秒更新一次
    app_window_timer_subscribe(p_window, 1000, timer_callback,NULL);

	return p_window;
}



//请求天气数据
static void request_web_info(char * symbol)
{	
			
	if((symbol != NULL)&&(strlen(symbol) >= 10))
	{
		char url[256] = {0};
		sprintf(url,"%s%s",REQUEST_URL,symbol);
		g_comm_id_web_request = maibu_comm_request_web(url,KEY_WORDS,5);
		
	}
	else 
	{
		
		return;
	}
		
	

}

int my_strchr(const char * Str, int Val)
{
    const char * p = Str;
	//如果*p==_Val或者*p为'\0',退出循环
	while(*p!=Val && *p)
		p++;
	return p-Str;
}


void alarm_notify(const char is_zhang,const char *per_str)
{	
	////os_printk("stock_name:%s is_zhang:%d per_str:%s\n",stock_name,is_zhang,per_str);
	
	GBitmap bitmap;
	res_get_user_bitmap(NOTICE_BITMAP, &bitmap);

	NotifyParam notic_ad = {	\
		.bmp = bitmap,	\
		.main_title = {0},	\
		.sub_title = {0},	\
		.pulse_type = VibesPulseTypeMiddle,		\
		.pulse_time = 6,		\
		.context = NULL,	\
	};
	
	if(is_zhang == 1)
	{
		memcpy(notic_ad.main_title,"上涨提醒",strlen("上涨提醒"));
	}
	else if(is_zhang == 0)
	{
		memcpy(notic_ad.main_title,"下跌提醒",strlen("下跌提醒"));
	}

	sprintf(notic_ad.sub_title,"现价:%s",per_str);

	maibu_service_sys_notify(&notic_ad);

}

int my_1_big(const char*str1,const char*str2)
{
	int32_t flag = 0;
	
	if(atoi(str1) > atoi(str2))
	{
		flag = 1;
	}
	else if(atoi(str1) == atoi(str2))
	{
		char num1 = my_strchr(str1,'.');
		char num2 = my_strchr(str2,'.');
		
		//os_printk("alarm_line[%d]:%d now[%d]:%d\n",num1+1,atoi(&str1[num1+1]),num2+1,atoi(&str2[num2+1]));

		if(atoi(&str1[num1+1])>atoi(&str2[num2+1]))
		{
			flag = 1;
		}
	}	
	
	return flag;
}

void is_alarm_money(const char* alarm_line,const char * now,const char *pre,const char *show_str)
{
	char is_zhang = 1;
	//os_printk("alarm_line:%s now_per:%s [0]:%c len:%d\n",alarm_line,now,now[0],strlen(now));
	
	if((strlen(alarm_line) == 0)||(strlen(now) == 0))
	{
		return;
	}
		
	//os_printk("line:%d %s,now:%d\n",atoi(alarm_line),now,atoi(now));
	if(my_1_big(alarm_line,now) == 1)
	{
		if(my_1_big(pre,alarm_line) == 1)
		{
			is_zhang = 0;
		}
		else
		{
			return;
		}
		
	}
	else if(my_1_big(alarm_line,now) == 0)
	{
		if(my_1_big(pre,alarm_line) == 0)
		{
			is_zhang = 1;	
		}
		else
		{
			return;
		}
	}	

	

	alarm_notify(is_zhang,show_str);


}


//请求网络数据回调
void web_request_callback(const uint8_t *buff,uint16_t size)
{
	char t_buff[24] = {0};	

	//os_printk("buff:%s\n",buff);
	
	int ret = maibu_get_json_str(buff, "cName", t_buff,sizeof(t_buff));

	if( ret != -1 )
	{
				
	}
	else
	{
		return;
	}

	char type_str[4] = {0};
	
	maibu_get_json_str(buff, "type", type_str,sizeof(type_str));
	//os_printk("type_str:%s\n",type_str);

	int32_t type = -1;
	type = atoi(type_str);

	//os_printk("type:%d\n",type);	
	
	if(type == 0)
	{
		memcpy(g_pre_CNY_str,g_CNY_str,sizeof(g_CNY_str));
		memcpy(g_pre_USD_str,g_USD_str,sizeof(g_USD_str));
	
		maibu_get_json_str(buff, "cName", g_cname,sizeof(g_cname));		
		
		maibu_get_json_str(buff, "last", g_CNY_str,sizeof(g_CNY_str));

		maibu_get_json_str(buff, "dollar", g_USD_str,sizeof(g_USD_str));

		window_reloading();

		char show_str[24] = {0};	
		if(g_alarm_flag == 0)
		{
			sprintf(show_str,"%s%s","中",g_CNY_str);
			is_alarm_money(g_alarm_str,g_CNY_str,g_pre_CNY_str,show_str);
		}
		else if(g_alarm_flag == 1)
		{
			sprintf(show_str,"%s%s","美",g_USD_str);
			is_alarm_money(g_alarm_str,g_USD_str,g_pre_USD_str,show_str);
		}
	}


}


static void web_comm_result_callback(enum ECommResult result, uint32_t comm_id, void *context)
{
	/*如果上一次请求GPS通讯失败，并且通讯ID相同，则重新发送*/
	if (result == ECommResultFail) 
	{	
		/*如果上一次请求WEB通讯失败，并且通讯ID相同，则重新发送*/
		if (comm_id == g_comm_id_web_request)
		{
			request_web_info(g_symbol);		
		}
	}
	
}

int get_api_symbol(char *src,uint16_t src_size,char *dest)
{
	char src_temp[src_size];
	memcpy(src_temp,src,src_size);
	
	uint32_t loop;

	for(loop = 0;loop < src_size;loop++)
	{
		if((src_temp[loop] >= 'A') &&(src_temp[loop] <= 'Z'))
		{
			src_temp[loop] = src_temp[loop]+('a' - 'A');
		}
	}

	
	for(loop = 0;loop < sizeof(CNameApi)/sizeof(name_api);loop++)
	{
		if( strstr(src_temp,CNameApi[loop].name) != NULL )
		{
			memcpy(dest,CNameApi[loop].symbol,sizeof(CNameApi[loop].symbol));
			return 1;
		}
	}
	return 0;

}

void watchapp_comm_callback(enum ESyncWatchApp type, uint8_t *buf, uint16_t len)
{
	
	if(type == ESyncWatchAppUpdateParam)
    {		
		if(len >= 3)
		{	
			uint16_t loop;
			char cfg_name[24] = {0};
			for(loop = 0;loop<len;loop++)
			{
				if((buf[loop] == '\n')||(buf[loop] == ' '))
				{
					break;					
				}
			}

			uint8_t cpy_size = len>sizeof(cfg_name)?sizeof(cfg_name):len;
			
			memcpy(cfg_name,buf,cpy_size);
			
			int ret = -1;
			ret	= get_api_symbol(cfg_name,sizeof(cfg_name),g_symbol);
			if(ret == 0)
			{
				memset(g_symbol,0,sizeof(g_symbol));
			}

			g_alarm_flag = -1;
				
			if(buf[loop] == ' ')
			{
				uint16_t pre_loop = loop;
				for(;loop<len;loop++)
				{
					if(buf[loop] == '\n')
					{
						break;
					}
				}
				
				memcpy(g_alarm_str,&buf[pre_loop],loop-pre_loop);
				g_alarm_flag = g_money_flag;
				
				//os_printk("g_alarm_str:%s  %d\n",g_alarm_str,strlen(g_alarm_str));
			}
			
			request_web_info(g_symbol); 				
		}
	}
}


/*
 *--------------------------------------------------------------------------------------
 *     function:  main
 *    parameter: 
 *       return:
 *  description:  主程序
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
int main()
{
	
	/*创建显示表盘窗口*/
	P_Window p_window = init_window();
	if (p_window != NULL)
	{
		/*放入窗口栈显示*/
		g_window_id = app_window_stack_push(p_window);

		/*注册一个事件通知回调，当有时间改变时，立即更新时间*/
		maibu_service_sys_event_subscribe(app_watch_time_change);

		/*注册通讯结果回调*/
		maibu_comm_register_result_callback(web_comm_result_callback);

		maibu_comm_register_web_callback(web_request_callback);

		//注册手机设置回调函数
		maibu_comm_register_watchapp_callback(watchapp_comm_callback);

		
		request_web_info(g_symbol);	
		
	}
		
	return 0;
}
