
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "maibu_sdk.h"
#include "maibu_res.h"

static int32_t g_window_id             = -1;

static char wday[7][8]={"周日","周一","周二","周三","周四","周五","周六"}; 


static uint8_t g_request_timer		   = 0;

static uint32_t g_steps				   = 0;
static uint32_t g_calorie			   = 0;
static uint32_t g_distance			   = 0;



static P_Window init_window(void);


//小号数字数组
const static uint32_t get_LIT_icon_key[] =
{
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_LIT_0,
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_LIT_1,
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_LIT_2,
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_LIT_3,
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_LIT_4,
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_LIT_5,
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_LIT_6,
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_LIT_7,
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_LIT_8,
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_LIT_9,

};
//大号数字数组
const static uint32_t get_BIG_icon_key[] =
{
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_BIG_0,  
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_BIG_1, 
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_BIG_2,  
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_BIG_3,  
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_BIG_4,  
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_BIG_5,  
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_BIG_6,  
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_BIG_7, 
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_BIG_8,  
	RES_BITMAP_WATCHFACE_MAIBU_SPORTS_NUMBER_BIG_9
};

static void window_reloading(void)
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


static void time_change(enum SysEventType type, void *context)
{
	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		window_reloading();
	}
}

static int32_t set_temp_layerText(LayerText* temp_LayerText,char * str,GRect rect,enum GAlign alignment, uint8_t font_type,P_Window p_window,P_Layer p_layer,enum GColor color)
{
	int32_t g_layer_id_temporary = 0;
	
	temp_LayerText->text = str;
	temp_LayerText->frame = rect;
	temp_LayerText->alignment = alignment;
	temp_LayerText->font_type = font_type;
	
	p_layer = app_layer_create_text(temp_LayerText);
	
	if(p_layer != NULL)
	{
		app_layer_set_bg_color(p_layer, color);
		g_layer_id_temporary = app_window_add_layer(p_window, p_layer);
	}
	return g_layer_id_temporary;
}

/*创建并显示图层，需要坐标值，得到icon_key的数组，数组需要的参数值，P_Window*/
static int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,int32_t bmp_array_name_key)
{	
	int32_t g_layer_id_temporary;

	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_array_name_key, &bmp_point);
	LayerBitmap layer_bitmap_struct_l = {bmp_point, *temp_p_frame, GAlignLeft};
 	temp_P_Layer = app_layer_create_bitmap(&layer_bitmap_struct_l);
	
	if(temp_P_Layer != NULL)
	{
		app_layer_set_bg_color(temp_P_Layer, GColorWhite);
		g_layer_id_temporary= app_window_add_layer(p_window, temp_P_Layer);
	}

	return g_layer_id_temporary;
}

//显示用图片表示紧挨着的数字
static void display_same_bmp_one_by_one_layer(P_Window p_window,GRect *temp_p_frame,uint32_t numbers_bmp_show,uint8_t max_num_long,const uint32_t get_bmp_icon_key_from_array[],enum GAlign alignment)
{
	if(max_num_long > 10)
	{
		max_num_long = 10;
	}
	
	int8_t loop_count;
	uint8_t flag = 0;
	uint32_t temp_divisor = 1;
		
	for(loop_count = 1;loop_count < max_num_long;loop_count++)
	{
		temp_divisor = temp_divisor*10;
	}
	
	
	for(loop_count = max_num_long-1;loop_count >= 0;loop_count--)
	{
	
		if((numbers_bmp_show/temp_divisor%10 != 0)||(flag == 1)||(loop_count == 0))
		{	
			if(alignment == GAlignCenter)
			{
				if(flag == 0)
				{
					temp_p_frame->origin.x = (128-(loop_count+1)*(temp_p_frame->size.w))/2;
				}
			}
			
			flag = 1;
			
			display_target_layer(p_window,temp_p_frame,get_bmp_icon_key_from_array[numbers_bmp_show/temp_divisor%10]);
			
			if((alignment == GAlignCenter)||(alignment == GAlignLeft))
			{
				temp_p_frame->origin.x = (temp_p_frame->origin.x) + (temp_p_frame->size.w);
			}
		}
		
		if(alignment == GAlignRight)
		{
			
			temp_p_frame->origin.x = (temp_p_frame->origin.x) + (temp_p_frame->size.w);
		}
			
		temp_divisor = temp_divisor/10;
	}
}

static P_Window init_window(void)
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}
	
	LayerText temp_LayerText = {0};
	temp_LayerText.bound_width = 0;
	
	GBitmap temp_GBitmap = {0};
	P_Layer temp_P_Layer = NULL;

	/*添加背景图片图层*/
	GRect temp_frame = {{0, 0}, {128, 128}};
	
	display_target_layer(p_window,&temp_frame,RES_BITMAP_WATCHFACE_MAIBU_SPORTS_BG);
   	

	char buffer[16] = {0};	
	
	struct date_time t;
	app_service_get_datetime(&t);

	/*添加小时分钟图层*/
	temp_frame.origin.x = 6;
	temp_frame.origin.y = 3;
	temp_frame.size.w = 40;
	temp_frame.size.h = 12;
	
	
	sprintf(buffer, "%02d:%02d", t.hour, t.min);
	set_temp_layerText(&temp_LayerText,buffer, temp_frame, GAlignLeft, U_ASCII_ARIAL_12,p_window,temp_P_Layer,GColorBlack);


	/*添加星期月图层*/
	temp_frame.origin.x = 62;
	temp_frame.origin.y = 3;
	temp_frame.size.w = 60;
	temp_frame.size.h = 12;
	
	sprintf(buffer, "%02d-%02d %s", t.mon, t.mday, wday[t.wday]);
	set_temp_layerText(&temp_LayerText, buffer,temp_frame, GAlignLeft,U_ASCII_ARIAL_12,p_window,temp_P_Layer,GColorBlack);

	

	/*添加步数图层*/

	temp_frame.origin.x = 0;
	temp_frame.origin.y = 56;
	temp_frame.size.w = 16;
	temp_frame.size.h = 20;

	display_same_bmp_one_by_one_layer(p_window,&temp_frame,g_steps,8,get_BIG_icon_key,GAlignCenter);
			
	/*添加热量图层*/
	temp_frame.origin.x = 6;
	temp_frame.origin.y = 108;
	temp_frame.size.w = 13;
	temp_frame.size.h = 15;

	display_same_bmp_one_by_one_layer(p_window,&temp_frame,g_calorie,4,get_LIT_icon_key,GAlignLeft);
	

	/*添加距离图层*/
	temp_frame.origin.x = 67;
	temp_frame.origin.y = 108;
	temp_frame.size.w = 13;
	temp_frame.size.h = 15;

	display_same_bmp_one_by_one_layer(p_window,&temp_frame,g_distance/10,3,get_LIT_icon_key,GAlignRight);

	temp_frame.origin.x = 111;
	display_target_layer(p_window,&temp_frame,get_LIT_icon_key[g_distance%10]);

	return p_window;
}
 

static void app_sports_update_timer_callback(date_time_t tick_time, uint32_t millis, void *context)
{	
	
	SportData sport_datas;
	maibu_get_sport_data(&sport_datas,0);

	if((g_steps != sport_datas.step)||(g_calorie != sport_datas.calorie)||(g_distance != sport_datas.distance/10000))
	{
		g_steps = sport_datas.step;
		g_calorie = sport_datas.calorie;
		g_distance = sport_datas.distance/10000;

		window_reloading();
	}
}


static int main()
{
	/*创建显示窗口*/
	P_Window p_window = init_window(); 
	/*放入窗口栈显示*/
	g_window_id = app_window_stack_push(p_window);

	/*注册一个事件通知回调，当有时间改变时，立即更新时间*/
	maibu_service_sys_event_subscribe(time_change);
	
	//注册定时查询函数
	g_request_timer = app_service_timer_subscribe(500, app_sports_update_timer_callback, NULL);	
	

	return 0;

}















