/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  watchface_double_time_zone.c
 *
 *    Description:  双时区表盘
 *    Corporation:
 * 
 *         Author:  gliu (), gliu@maibu.cc
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

#include "maibu_sdk.h"
#include "maibu_res.h"



typedef struct tag_DoubleTimeZone
{
	int8_t flag;		/*是否使用标志*/
	int8_t show_second_flag; /*显示第二个表盘标志*/
	char zone1_addr[30];	/*地域名1*/
	char zone2_addr[30];	/*地域名2*/
	int8_t  zone1_offset;	/*地域1时区与手机所在时区差别,单位小时*/	
	int8_t  zone2_offset;	/*地域1时区与手机所在时区差别,单位小时*/
}DoubleTimeZone;


/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_double_timezone_window_id = -1;

/*图层ID，通过该图层ID获取图层句柄*/

/*定义各个图层的位置*/

/*背景图层*/
#define TIMEZONE_BG_ORIGIN_X			0
#define TIMEZONE_BG_ORIGIN_Y			0
#define TIMEZONE_BG_SIZE_H			128
#define TIMEZONE_BG_SIZE_W			128


/*地域1图层*/
#define TIMEZONE_ADDR1_ORIGIN_X			4
#define TIMEZONE_ADDR1_ORIGIN_Y			8
#define TIMEZONE_ADDR1_SIZE_H			12
#define TIMEZONE_ADDR1_SIZE_W			88


/*地域2图层*/
#define TIMEZONE_ADDR2_ORIGIN_X		4
#define TIMEZONE_ADDR2_ORIGIN_Y		65
#define TIMEZONE_ADDR2_SIZE_H		12
#define TIMEZONE_ADDR2_SIZE_W		88


/*差距图层*/
#define TIMEZONE_OFFSET_ORIGIN_X		4
#define TIMEZONE_OFFSET_ORIGIN_Y		114	
#define TIMEZONE_OFFSET_SIZE_H		12
#define TIMEZONE_OFFSET_SIZE_W		120

/*地域1时间图层*/
#define TIMEZONE_TIME1_ORIGIN_X		4
#define TIMEZONE_TIME1_ORIGIN_Y		26
#define TIMEZONE_TIME1_SIZE_H		23
#define TIMEZONE_TIME1_SIZE_W		19

/*地域2时间图层*/
#define TIMEZONE_TIME2_ORIGIN_X		4
#define TIMEZONE_TIME2_ORIGIN_Y		83
#define TIMEZONE_TIME2_SIZE_H		23
#define TIMEZONE_TIME2_SIZE_W		19



/*保存配置信息文件KEY*/
#define DOUBLE_TIMEZONE_KEY	1

#define ZONE1_ADDR_NAME_INIT		"北京"
#define ZONE2_ADDR_NAME_INIT		"莫斯科"
#define ZONE1_BEIJING_OFFSET_INIT	0
#define ZONE2_BEIJING_OFFSET_INIT	-5


/*全局变量保存数据*/
static DoubleTimeZone  g_double_timezone = {0,0,"","",0,0};

int32_t bmp_id[13] = {RES_BITMAP_WATCHFACE_TIMEZONE_0, RES_BITMAP_WATCHFACE_TIMEZONE_1,RES_BITMAP_WATCHFACE_TIMEZONE_2, RES_BITMAP_WATCHFACE_TIMEZONE_3,
	RES_BITMAP_WATCHFACE_TIMEZONE_4, RES_BITMAP_WATCHFACE_TIMEZONE_5, RES_BITMAP_WATCHFACE_TIMEZONE_6, RES_BITMAP_WATCHFACE_TIMEZONE_7,
	RES_BITMAP_WATCHFACE_TIMEZONE_8, RES_BITMAP_WATCHFACE_TIMEZONE_9, RES_BITMAP_WATCHFACE_TIMEZONE_BG};


/*创建并显示图层，需要坐标值，得到icon_key的数组，数组需要的参数值，P_Window*/
int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,enum GAlign how_to_align,enum GColor black_or_white,int32_t bmp_array_name[],int bmp_id_number)
{	
	int32_t g_layer_id_temporary;

	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_array_name[bmp_id_number], &bmp_point);
	LayerBitmap layer_bitmap_struct_l = {bmp_point, *temp_p_frame, how_to_align};
 	temp_P_Layer = app_layer_create_bitmap(&layer_bitmap_struct_l);
	
	if(temp_P_Layer != NULL)
	{
		app_layer_set_bg_color(temp_P_Layer, black_or_white);
		g_layer_id_temporary= app_window_add_layer(p_window, temp_P_Layer);
	}

	return g_layer_id_temporary;
}

int32_t display_target_layerText (P_Window p_window,GRect temp_text_frame,char *text_str,enum GAlign how_to_align,enum GColor black_or_white)
{
	int32_t g_layer_id_temporary;
	GBitmap temp_GBitmap = {0};
	
	LayerText layer_text_struct_lg = {text_str, temp_text_frame, how_to_align, U_GBK_SIMSUN_12};
	P_Layer temp_P_LayerText_lg = app_layer_create_text(&layer_text_struct_lg);
	
	if(temp_P_LayerText_lg != NULL)
	{	
		app_layer_set_bg_color(temp_P_LayerText_lg, black_or_white);
		g_layer_id_temporary = app_window_add_layer(p_window, temp_P_LayerText_lg);
	}
	return g_layer_id_temporary;
}

P_Window double_timezone_create_window_handle(P_Window p_window,int addr_origin_y,int time_origin_y,int flag_sec)
{
	
	struct date_time t;
	struct date_time t1;
	uint32_t whole_seconds;
	uint32_t whole_seconds1;
	app_service_get_datetime(&t);
	whole_seconds = app_get_time(&t);

	char zone_addr_temp[30];
	char zone_mon_day_temp[10];
	
	enum GColor time_color = GColorBlack;
	enum GColor addr_color = GColorWhite;
	strcpy(zone_addr_temp,g_double_timezone.zone1_addr);
	int8_t zone_offset_temp = g_double_timezone.zone1_offset;

	if(flag_sec == 1)
	{
		time_color = GColorWhite;
		addr_color = GColorBlack;
		strcpy(zone_addr_temp,g_double_timezone.zone2_addr);
		zone_offset_temp = g_double_timezone.zone2_offset;
	}

	whole_seconds1 = whole_seconds + zone_offset_temp*3600;
	app_get_dt_by_num(whole_seconds1,&t1);

	sprintf(zone_mon_day_temp,"%02d-%02d",t1.mon,t1.mday);

		
	/*创建时区1地址文本图层*/
	GRect frame_addr1 = {{TIMEZONE_ADDR1_ORIGIN_X, addr_origin_y}, {TIMEZONE_ADDR1_SIZE_H, TIMEZONE_ADDR1_SIZE_W}};
	display_target_layerText(p_window,frame_addr1,zone_addr_temp,GAlignLeft,addr_color);
	/*创建时区1日期文本图层*/
	frame_addr1.origin.x = 94;
	frame_addr1.size.w = 33;
	display_target_layerText(p_window,frame_addr1,zone_mon_day_temp,GAlignLeft,addr_color);
	
	/*创建时1图层*/
	GRect frame_zone1_h1 = {{TIMEZONE_TIME1_ORIGIN_X, time_origin_y}, {TIMEZONE_TIME1_SIZE_H,TIMEZONE_TIME1_SIZE_W}};

	int iii;
	int iii_array[4] = {t1.hour/10,t1.hour%10,t1.min/10,t1.min%10};
	int iii_offset[4] = {0,19,46,65};
	for(iii = 0;iii <= 3;iii++)
	{	
		frame_zone1_h1.origin.x = TIMEZONE_TIME1_ORIGIN_X+iii_offset[iii];
		display_target_layer(p_window,&frame_zone1_h1,GAlignCenter,time_color,bmp_id,iii_array[iii]);
	}

}


P_Window double_timezone_create_window()
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	/*创建背景图层*/
	GRect frame_bg = {{TIMEZONE_BG_ORIGIN_X, TIMEZONE_BG_ORIGIN_Y}, {TIMEZONE_BG_SIZE_H, TIMEZONE_BG_SIZE_W}};
	display_target_layer(p_window,&frame_bg,GAlignCenter,GColorWhite,bmp_id,10);
	
	double_timezone_create_window_handle(p_window,TIMEZONE_ADDR1_ORIGIN_Y,TIMEZONE_TIME1_ORIGIN_Y,0);
	
	if(g_double_timezone.show_second_flag == 1)
	{
		double_timezone_create_window_handle(p_window,TIMEZONE_ADDR2_ORIGIN_Y,TIMEZONE_TIME2_ORIGIN_Y,1);
				
		/*创建时差图层*/
		if(g_double_timezone.zone1_offset != g_double_timezone.zone2_offset)
		{
			GRect frame_offset = {{TIMEZONE_OFFSET_ORIGIN_X, TIMEZONE_OFFSET_ORIGIN_Y}, {TIMEZONE_OFFSET_SIZE_H, TIMEZONE_OFFSET_SIZE_W}};
			char str_tmp[20] = "";
			if (g_double_timezone.zone1_offset > g_double_timezone.zone2_offset)
			{
				sprintf(str_tmp," 晚%d小时", g_double_timezone.zone1_offset - g_double_timezone.zone2_offset);	
			}
			else
			{
				sprintf(str_tmp," 早%d小时", g_double_timezone.zone2_offset - g_double_timezone.zone1_offset);
			}
			display_target_layerText(p_window,frame_offset,str_tmp,GAlignLeft,GColorBlack);
		}		
	}
	
	return (p_window);
}



void app_double_timezone_watch_time_change(enum SysEventType type, void *context)
{
	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		/*根据窗口ID获取窗口句柄*/
		P_Window p_old_window = app_window_stack_get_window_by_id(g_double_timezone_window_id);	
		if (NULL == p_old_window)
		{
			return;	
		}

		P_Window p_new_window = double_timezone_create_window();
		if (NULL != p_new_window)
		{
			g_double_timezone_window_id = app_window_stack_replace_window(p_old_window, p_new_window);
		}	

	}

	return;
}




P_Window double_timezone_init()
{
	
		/*创建一个可读可写的保存构体的文件key*/
		app_persist_create(DOUBLE_TIMEZONE_KEY, sizeof(DoubleTimeZone));
		
		/*读取结构信息*/
		app_persist_read_data(DOUBLE_TIMEZONE_KEY, 0, (unsigned char *)&g_double_timezone, sizeof(DoubleTimeZone));
		
		if (g_double_timezone.flag == 0)
		{
			strcpy(g_double_timezone.zone1_addr, ZONE1_ADDR_NAME_INIT);
			strcpy(g_double_timezone.zone2_addr, ZONE2_ADDR_NAME_INIT);
			g_double_timezone.zone1_offset = ZONE1_BEIJING_OFFSET_INIT;
			g_double_timezone.zone2_offset = ZONE2_BEIJING_OFFSET_INIT;	
			g_double_timezone.flag = 1;
			g_double_timezone.show_second_flag = 1;
			
			/*保存数据*/
			app_persist_write_data_extend(DOUBLE_TIMEZONE_KEY, (unsigned char *)&g_double_timezone, sizeof(DoubleTimeZone));
			
		}	
	
	P_Window p_window = double_timezone_create_window();

	/*注册一个事件通知回调，当有时间改变是，立即更新时间*/
	maibu_service_sys_event_subscribe(app_double_timezone_watch_time_change);
	return p_window;

}


int32_t timezone_lg_handle(int32_t num_i,uint8_t *context_head, uint16_t context_max_size,int8_t *cpy_str_point)
{
	uint8_t *p = context_head;
	uint8_t *q = 0;

	while(num_i < context_max_size)
	{
		if((*p != ' ')&&(*p != '\n')&&(*p != '\r'))
		{	
			q = p;
				
			while((*p != ' ')&&(*p != '\n')&&(*p != '\r')&&(num_i < context_max_size))
			{
				p++;
				num_i++;
			}
			if((num_i-1) < context_max_size)
			{
				if((p-q)<=21)
					memcpy(cpy_str_point,q,(p-q));
				else
					memcpy(cpy_str_point,q,21);
				return num_i;	
			}	
		}
		
		if(((*p == ' ')||(*p == '\n')||(*p == '\r'))&&(num_i < context_max_size))
		{
			p++;
			num_i++;
		}
	}
	return num_i;			
}

void double_timezone_param_callback(enum ESyncWatchApp type, uint8_t *context, uint16_t context_size)
{	
	if(type == ESyncWatchAppUpdateParam)
	{
		/*如果配置参数不为空，则是手机客户端下发配置信息，需要保存*/
		if (context != NULL)
		{
			int32_t i = 0;
			char zone_l_offset[5] = {0};
			DoubleTimeZone data_temporary = {1,0,"系统时间","",0,0};
					
			if((context_size < 7)||(context_size > 100))
        	{	
        		if((context[1]='c')&&(context[2]='l')&&(context[3]='c'))
					app_persist_delete_data(DOUBLE_TIMEZONE_KEY);
			}

			else
			{	
				memset(data_temporary.zone1_addr,0,30);
				i = timezone_lg_handle(i,&context[i],context_size,data_temporary.zone1_addr);
				i = timezone_lg_handle(i,&context[i],context_size,zone_l_offset);
				data_temporary.zone1_offset = atoi(zone_l_offset) -8;
				data_temporary.show_second_flag = 0;
				memset(zone_l_offset,0,sizeof(zone_l_offset));

				while(((context[i] == ' ')||(context[i] == '\n')||(context[i] == '\r'))&&(i < context_size)) //去掉后端空格和换行
				{
					i++;
				}
				
				if(i < context_size)
				{
					i = timezone_lg_handle(i,&context[i],context_size,data_temporary.zone2_addr);
					timezone_lg_handle(i,&context[i],context_size,zone_l_offset);
					data_temporary.zone2_offset = atoi(zone_l_offset) -8;
					data_temporary.show_second_flag = 1;	
				}
						
			}
				
			g_double_timezone = data_temporary;
			
			app_persist_write_data_extend( DOUBLE_TIMEZONE_KEY, (unsigned char *)&g_double_timezone, sizeof(DoubleTimeZone));

			P_Window p_window_old = app_window_stack_get_window_by_id(g_double_timezone_window_id);
	        if (p_window_old == NULL)
	        {
	            return;
	        }
						
	        //更新界面
	        P_Window p_window = double_timezone_create_window();
			g_double_timezone_window_id = app_window_stack_replace_window(p_window_old,p_window);
		}
	}
}


int main()
{
	//	simulator_init();


	/*创建显示汉字表盘窗口*/
	P_Window p_window = double_timezone_init(); 
	if(p_window != NULL)
	{
		/*放入窗口栈显示*/
		g_double_timezone_window_id = app_window_stack_push(p_window);

		/*创建接受配置信息回调*/
		maibu_comm_register_watchapp_callback(double_timezone_param_callback);

	}


	//	simulator_wait();

	return 0;

}


