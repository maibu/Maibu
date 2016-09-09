
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "maibu_sdk.h"
#include "maibu_res.h"

/*背景图层位置*/
#define MEMORIAL_DAY_BG_ORIGIN_X			0	
#define MEMORIAL_DAY_BG_ORIGIN_Y			0
#define MEMORIAL_DAY_BG_SIZE_H				128	
#define MEMORIAL_DAY_BG_SIZE_W				128	

/*时间文本图层位置*/
#define MEMORIAL_DAY_TIME_ORIGIN_X			86	
#define MEMORIAL_DAY_TIME_ORIGIN_Y			1
#define MEMORIAL_DAY_TIME_SIZE_H			12	
#define MEMORIAL_DAY_TIME_SIZE_W			40	

/*月日文本图层位置*/
#define MEMORIAL_DAY_MONTHDAY_ORIGIN_X		1	
#define MEMORIAL_DAY_MONTHDAY_ORIGIN_Y		1
#define MEMORIAL_DAY_MONTHDAY_SIZE_H		12
#define MEMORIAL_DAY_MONTHDAY_SIZE_W		40

/*主倒计日文本图层位置*/
#define MEMORIAL_DAY_MAIN_STR_ORIGIN_X		2	
#define MEMORIAL_DAY_MAIN_STR_ORIGIN_Y		22
#define MEMORIAL_DAY_MAIN_STR_SIZE_H		12
#define MEMORIAL_DAY_MAIN_STR_SIZE_W		124

/*主倒计日天数图层起始位置*/
#define MEMORIAL_DAY_DAYSNUMBER_ORIGIN_X	2	
#define MEMORIAL_DAY_DAYSNUMBER_ORIGIN_Y	38
#define MEMORIAL_DAY_DAYSNUMBER_SIZE_H		30
#define MEMORIAL_DAY_DAYSNUMBER_SIZE_W		NULL 

/*主倒计日天数文本"天"图层起始位置*/
#define MEMORIAL_DAY_DAYSTIAN_ORIGIN_X		NULL	
#define MEMORIAL_DAY_DAYSTIAN_ORIGIN_Y		51
#define MEMORIAL_DAY_DAYSTIAN_SIZE_H		12
#define MEMORIAL_DAY_DAYSTIAN_SIZE_W		16

/*次倒计日文本图层位置*/
#define MEMORIAL_DAY_SECONDARY_STR_ORIGIN_X			2	
#define MEMORIAL_DAY_SECONDARY_STR_ORIGIN_Y			78
#define MEMORIAL_DAY_SECONDARY_STR_SIZE_H			12
#define MEMORIAL_DAY_SECONDARY_STR_SIZE_W			124

/*次倒计日天数图层起始位置*/
#define MEMORIAL_DAY_SECONDARY_DAYSNUMBER_ORIGIN_X	2	
#define MEMORIAL_DAY_SECONDARY_DAYSNUMBER_ORIGIN_Y	94
#define MEMORIAL_DAY_SECONDARY_DAYSNUMBER_SIZE_H	30
#define MEMORIAL_DAY_SECONDARY_DAYSNUMBER_SIZE_W	NULL 

/*次倒计日天数文本"天"图层起始位置*/
#define MEMORIAL_DAY_SECONDARY_DAYSTIAN_ORIGIN_X	NULL	
#define MEMORIAL_DAY_SECONDARY_DAYSTIAN_ORIGIN_Y	107
#define MEMORIAL_DAY_SECONDARY_DAYSTIAN_SIZE_H		12
#define MEMORIAL_DAY_SECONDARY_DAYSTIAN_SIZE_W		16

//标志位

//存储块key
static uint32_t MEMORIAL_DAY_PRESIST_DATA_KEY  = 0x2003;

//窗口ID
static int32_t g_window_id = -1;
//表盘的记录的当天日期，用于比较日期是否变为第二天
static uint8_t g_today_num = -1;
//是否刚装上表盘
static uint8_t g_first_start = 1;


//初始数据
typedef struct str_day_info_pack
{
	char show_str[31];
	char days_value[6];
	uint32_t g_days_tian_width;
}str_day_info;

static str_day_info g_info_pack[2]={{"",0,{0},1},{"",0,{0},1}}; 
//默认字符串和长度
static char init_str[2][30] = {"人类登陆火星 2023-05-01","香港回归 1997-07-01"};
static uint16_t init_str_count[2] = {30,24};


//记录主倒计日的字符串和目标时间
static char g_main_show_str[2][22] = {0};
static char g_main_time_str[2][11] = {0};

static char g_tian_str[4] = {"天"};

P_Window init_window(void);


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
int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,enum GAlign how_to_align,enum GColor black_or_white,int32_t bmp_array_key)
{	

	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_array_key, &bmp_point);
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

	//添加背景
	GRect temp_frame ={{MEMORIAL_DAY_BG_ORIGIN_X,MEMORIAL_DAY_BG_ORIGIN_Y},
						{MEMORIAL_DAY_BG_SIZE_H,MEMORIAL_DAY_BG_SIZE_H}};
	
	display_target_layer(p_window,&temp_frame,GAlignLeft,GColorWhite,RES_BITMAP_WATCHAPP_MEMORIAL_DAY_BG);

	//添加时间
	struct date_time datetime;
	app_service_get_datetime(&datetime);
	char buf[8] = "";
	
	temp_frame.origin.x = MEMORIAL_DAY_TIME_ORIGIN_X;
	temp_frame.origin.y = MEMORIAL_DAY_TIME_ORIGIN_Y;
	temp_frame.size.h = MEMORIAL_DAY_TIME_SIZE_H;
	temp_frame.size.w = MEMORIAL_DAY_TIME_SIZE_W;
	
	sprintf(buf, "%02d:%02d", datetime.hour, datetime.min);

	display_target_layerText(p_window,&temp_frame,GAlignRight,GColorBlack,buf,U_ASCII_ARIAL_12);
	
	//添加月日
	temp_frame.origin.x = MEMORIAL_DAY_MONTHDAY_ORIGIN_X;
	temp_frame.origin.y = MEMORIAL_DAY_MONTHDAY_ORIGIN_Y;
	temp_frame.size.h = MEMORIAL_DAY_MONTHDAY_SIZE_H;
	temp_frame.size.w = MEMORIAL_DAY_MONTHDAY_SIZE_W;
	
	sprintf(buf, "%02d-%02d",datetime.mon,datetime.mday);
	
	display_target_layerText(p_window,&temp_frame,GAlignLeft,GColorBlack,buf,U_ASCII_ARIAL_12);

	//添加主倒数日文本
	temp_frame.origin.x = MEMORIAL_DAY_MAIN_STR_ORIGIN_X;
	temp_frame.origin.y = MEMORIAL_DAY_MAIN_STR_ORIGIN_Y;
	temp_frame.size.h = MEMORIAL_DAY_MAIN_STR_SIZE_H;
	temp_frame.size.w = MEMORIAL_DAY_MAIN_STR_SIZE_W;

	display_target_layerText(p_window,&temp_frame,GAlignLeft,GColorBlack,g_info_pack[0].show_str,U_ASCII_ARIAL_12);
	
	//添加主倒数日天数和"天"
	temp_frame.origin.x = MEMORIAL_DAY_DAYSNUMBER_ORIGIN_X;
	temp_frame.origin.y = MEMORIAL_DAY_DAYSNUMBER_ORIGIN_Y;
	temp_frame.size.h = MEMORIAL_DAY_DAYSNUMBER_SIZE_H;
	temp_frame.size.w = g_info_pack[0].g_days_tian_width;

	display_target_layerText(p_window,&temp_frame,GAlignLeft,GColorBlack,g_info_pack[0].days_value,U_ASCII_ARIALBD_30);

	temp_frame.origin.x = MEMORIAL_DAY_DAYSNUMBER_ORIGIN_X+g_info_pack[0].g_days_tian_width+1;
	temp_frame.origin.y = MEMORIAL_DAY_DAYSTIAN_ORIGIN_Y;
	temp_frame.size.h = MEMORIAL_DAY_DAYSTIAN_SIZE_H;
	temp_frame.size.w = MEMORIAL_DAY_DAYSTIAN_SIZE_W;

	display_target_layerText(p_window,&temp_frame,GAlignLeft,GColorBlack,g_tian_str,U_ASCII_ARIAL_12);

	//添加次倒数日文本
	temp_frame.origin.x = MEMORIAL_DAY_SECONDARY_STR_ORIGIN_X;
	temp_frame.origin.y = MEMORIAL_DAY_SECONDARY_STR_ORIGIN_Y;
	temp_frame.size.h = MEMORIAL_DAY_SECONDARY_STR_SIZE_H;
	temp_frame.size.w = MEMORIAL_DAY_SECONDARY_STR_SIZE_W;

	display_target_layerText(p_window,&temp_frame,GAlignLeft,GColorWhite,g_info_pack[1].show_str,U_ASCII_ARIAL_12);
	
	//添加次倒数日天数和"天"
	temp_frame.origin.x = MEMORIAL_DAY_SECONDARY_DAYSNUMBER_ORIGIN_X;
	temp_frame.origin.y = MEMORIAL_DAY_SECONDARY_DAYSNUMBER_ORIGIN_Y;
	temp_frame.size.h = MEMORIAL_DAY_SECONDARY_DAYSNUMBER_SIZE_H;
	temp_frame.size.w = g_info_pack[1].g_days_tian_width;

	display_target_layerText(p_window,&temp_frame,GAlignLeft,GColorWhite,g_info_pack[1].days_value,U_ASCII_ARIALBD_30);

	temp_frame.origin.x = MEMORIAL_DAY_SECONDARY_DAYSNUMBER_ORIGIN_X+g_info_pack[1].g_days_tian_width+1;
	temp_frame.origin.y = MEMORIAL_DAY_SECONDARY_DAYSTIAN_ORIGIN_Y;
	temp_frame.size.h = MEMORIAL_DAY_SECONDARY_DAYSTIAN_SIZE_H;
	temp_frame.size.w = MEMORIAL_DAY_SECONDARY_DAYSTIAN_SIZE_W;

	display_target_layerText(p_window,&temp_frame,GAlignLeft,GColorWhite,g_tian_str,U_ASCII_ARIAL_12);



	return p_window;

}

uint32_t get_week_day_num(char *week_char_str_p)
{
	uint32_t week_day_num = -1;
	if(NULL != strstr(week_char_str_p,"一"))
	{
		week_day_num = 1;
	}
	else if(NULL != strstr(week_char_str_p,"二"))
	{
		week_day_num = 2;
	}
	else if(NULL != strstr(week_char_str_p,"三"))
	{
		week_day_num = 3;
	}
	else if(NULL != strstr(week_char_str_p,"四"))
	{
		week_day_num = 4;
	}
	else if(NULL != strstr(week_char_str_p,"五"))
	{
		week_day_num = 5;
	}
	else if(NULL != strstr(week_char_str_p,"六"))
	{
		week_day_num = 6;
	}
	else if((NULL != strstr(week_char_str_p,"天"))||(NULL != strstr(week_char_str_p,"日")))
	{
		week_day_num = 0;
	}

	return week_day_num;
	
}
//重复代码
void data_time_handler_repeate(char *show_str_p)
{
	char show_str_temp_swap[31] = {0};

	sprintf(show_str_temp_swap,"距%s还有",show_str_p);
	sprintf(show_str_p,"%s",show_str_temp_swap);

}


//处理时间数据函数
void data_time_handler(char *p,str_day_info *temp_info_pack_p)
{
	
	int32_t num_j = 0;
	struct date_time datetime_now;
	app_service_get_datetime(&datetime_now);

	uint32_t memorial_day_sec = 0;
	uint32_t datetime_now_sec = 0;
	int32_t days_value_sec = 0;

	struct date_time datetime_memorial_day = {0};

	datetime_memorial_day.hour = 23;
	datetime_memorial_day.min = 59;
	datetime_memorial_day.sec = 59;
	
	char temp_char_str[3][9] = {0};
	
	char *q = p;
	
	while(*p != '\0')
	{
		if((*p == '-'))
		{
			memcpy(temp_char_str[num_j],q,(p-q));
			num_j++;
			q = p+1;
		}
		p++;
	}
	if(*p == '\0')
	{
		memcpy(temp_char_str[num_j],q,(p-q));
		num_j++;
	}


	g_today_num = datetime_now.mday;
	datetime_now_sec = app_get_time(&datetime_now);

	if(num_j == 3)
	{
		datetime_memorial_day.year = atoi(temp_char_str[0]);
		datetime_memorial_day.mon = atoi(temp_char_str[1]);
		datetime_memorial_day.mday = atoi(temp_char_str[2]);

		if((datetime_memorial_day.year < 1970)||(datetime_memorial_day.year > 2080))
		{

			sprintf(temp_info_pack_p->days_value,"%s","--");
	
			temp_info_pack_p ->g_days_tian_width = strlen(temp_info_pack_p->days_value)*17;
			
			return;

		}
	
		memorial_day_sec = app_get_time(&datetime_memorial_day);

		days_value_sec = memorial_day_sec - datetime_now_sec;
		if(days_value_sec >= 0)
		{
			data_time_handler_repeate((char *)temp_info_pack_p->show_str);
		}
		else
		{
			days_value_sec = datetime_now_sec - memorial_day_sec + 86400; 
			
			sprintf(temp_info_pack_p->show_str,"%s已经",temp_info_pack_p->show_str);
		}
			
	}
	else if(num_j == 2)
	{
		datetime_memorial_day.year = datetime_now.year;
		datetime_memorial_day.mon = atoi(temp_char_str[0]);
		datetime_memorial_day.mday = atoi(temp_char_str[1]);

		data_time_handler_repeate((char *)temp_info_pack_p->show_str);
		
		memorial_day_sec = app_get_time(&datetime_memorial_day);

		days_value_sec = memorial_day_sec - datetime_now_sec;
		
		if(days_value_sec < 0)
		{
			datetime_memorial_day.year = datetime_now.year + 1;
			memorial_day_sec = app_get_time(&datetime_memorial_day);
			days_value_sec = memorial_day_sec - datetime_now_sec;
		}

	}
	else if(num_j == 1)
	{
		if((p-q) <= 3)
		{
			datetime_memorial_day.year = datetime_now.year;
			datetime_memorial_day.mon = datetime_now.mon;
			datetime_memorial_day.mday = atoi(temp_char_str[0]);

			data_time_handler_repeate((char *)temp_info_pack_p->show_str);
			
			memorial_day_sec = app_get_time(&datetime_memorial_day);

			days_value_sec = memorial_day_sec - datetime_now_sec;
			
			if(days_value_sec < 0)
			{
				datetime_memorial_day.mon =  (datetime_now.mon + 1)%12;
				datetime_memorial_day.year = datetime_now.year + ((datetime_now.mon + 1)/12);

				memorial_day_sec = app_get_time(&datetime_memorial_day);
				days_value_sec = memorial_day_sec - datetime_now_sec;
			}
			
		}			
		else
		{
			data_time_handler_repeate((char *)temp_info_pack_p->show_str);
						
			datetime_memorial_day.wday = get_week_day_num((char *)&temp_char_str[0]);
		
			days_value_sec = ((datetime_memorial_day.wday - datetime_now.wday + 7)%7)*86400;
			
		}
		
	}

	sprintf(temp_info_pack_p->days_value,"%d",days_value_sec/86400);
	
	temp_info_pack_p->g_days_tian_width = strlen(temp_info_pack_p->days_value)*17;
	
}


void data_handler(str_day_info *temp_info_pack_p,uint8_t *temp_context,uint16_t temp_context_size,uint8_t g_info_pack_number)
{
	uint8_t *p = temp_context;
	uint8_t *q = temp_context;
	int32_t num_i = 0;
	int32_t num_i_first_data = 0;
	char time_str[10] = {0};

	//取第一个数据
	while((*p != ' ')&&(num_i < temp_context_size)&&(num_i < 21))
	{
		p++;
		num_i++;
	}
	
	memcpy(temp_info_pack_p->show_str,q,(p-q));
	temp_info_pack_p->show_str[p-q] = '\0';

	memset(g_main_show_str[g_info_pack_number],0,sizeof(g_main_show_str[g_info_pack_number]));
	strcpy(g_main_show_str[g_info_pack_number],temp_info_pack_p->show_str);
	
	//如果文字多了，去掉多余文字
	while((*p != ' ')&&(num_i < temp_context_size))
	{
		p++;
		num_i++;
	}

	//去掉空格
	while((*p == ' ')&&(num_i < temp_context_size))
	{
		p++;
		num_i++;
	}
	
	//取第二个数据
	num_i_first_data = num_i;
	q = p;
	while((num_i < temp_context_size)&&(num_i-num_i_first_data < 10))
	{
		p++;
		num_i++;
	}
		
	memcpy(time_str,q,(p-q));
	time_str[p-q] = '\0';
	strcpy(g_main_time_str[g_info_pack_number],time_str);

	//处理时间数据
	data_time_handler(time_str,temp_info_pack_p);

	
}

void  time_change (enum SysEventType type, void *context)
{
	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		int m_num = 0;
		int m_temp = 0;
		struct date_time datetime_now_change;
		app_service_get_datetime(&datetime_now_change);
	
		if(g_today_num != datetime_now_change.mday)
		{
			for(m_num = 0;m_num < 2;m_num++)
			{	
				memset(&(g_info_pack[m_num].show_str),0,sizeof(g_info_pack[m_num].show_str));
				strcpy((char *)&(g_info_pack[m_num].show_str),g_main_show_str[m_num]);
			
				memset(&(g_info_pack[m_num].days_value),0,sizeof(g_info_pack[m_num].days_value));
				data_time_handler(g_main_time_str[m_num],&g_info_pack[m_num]);
			}
	
		}

		window_reloading();
	}
	
}


void memorial_day_param_callback(enum ESyncWatchApp type, uint8_t *context, uint16_t context_size)
{	
	if(type == ESyncWatchAppUpdateParam)
	{
		memset(g_info_pack,0,sizeof(g_info_pack));
				
		uint8_t *tail_p = context;
		uint8_t *head_q = context;
		uint32_t count = 0;
		uint32_t count_total = 1;
		int32_t i;

		for(i = 0;i < 2;i++)
		{
		//去掉前置空格和上一次的后置回车+空格
			while((*tail_p == ' ')||(*tail_p == '\n'))
			{
				tail_p++;
				count_total++;
			}
			head_q = tail_p;
			
			while((*tail_p!= '\n')&&(*tail_p!= NULL)&&(count_total <= context_size))
			{
				tail_p++;
				count++;
				count_total++;
			}
			
			if(count > 0)
			{
				data_handler(&g_info_pack[i],head_q,count,i);
			}
			else if(count == 0)
			{
				data_handler(&g_info_pack[i],(char *)&init_str[i],init_str_count[i],i);
			}
			count = 0;
		}
		count_total = 1;

		window_reloading();		
		app_persist_write_data_extend(MEMORIAL_DAY_PRESIST_DATA_KEY, context, context_size);
	}

}


static unsigned char context_buff[244] = {0};
int main(void)
{
	//创建储存空间
	app_persist_create(MEMORIAL_DAY_PRESIST_DATA_KEY, 244);

	/*读取结构信息*/
	app_persist_read_data(MEMORIAL_DAY_PRESIST_DATA_KEY, 0,context_buff, sizeof(context_buff));

	/*创建接受配置信息回调*/
	maibu_comm_register_watchapp_callback(memorial_day_param_callback);

	if(g_first_start == 1)
	{
		memorial_day_param_callback(ESyncWatchAppUpdateParam,context_buff,(uint16_t)strlen(context_buff));
		g_first_start = 0;
	}
	else
	{
		time_change(SysEventTypeTimeChange,NULL);
	}


	P_Window p_window = init_window(); 

	/*放入窗口栈显示*/
	g_window_id = app_window_stack_push(p_window);

	/*注册一个事件通知回调，当有分钟改变时，检测天数是否改变*/
    maibu_service_sys_event_subscribe(time_change);


	return 0;

}

