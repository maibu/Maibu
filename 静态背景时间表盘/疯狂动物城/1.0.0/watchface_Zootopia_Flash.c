#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "maibu_sdk.h"
#include "maibu_res.h"

uint32_t g_window_id = -1;

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
	RES_BITMAP_WATCHFACE_ZOOTOPIA_FLASH_BG
};

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
int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,enum GAlign how_to_align,int32_t bmp_array_name[],int bmp_id_number)
{	
	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_array_name[bmp_id_number], &bmp_point);
	LayerBitmap layer_bitmap_struct_l = {bmp_point, *temp_p_frame, how_to_align};
 	temp_P_Layer = app_layer_create_bitmap(&layer_bitmap_struct_l);
	
	if(temp_P_Layer != NULL)
	{
		return app_window_add_layer(p_window, temp_P_Layer);
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

	/*添加背景图片图层*/
	GRect temp_frame = {{0, 0}, {128, 128}};
	display_target_layer(p_window,&temp_frame,GAlignCenter,bmp_array_name,10);


	struct date_time datetime;
	app_service_get_datetime(&datetime);
		
//hour
	temp_frame.origin.x = 2;
	temp_frame.origin.y = 83;
	temp_frame.size.h = 12;
	temp_frame.size.w = 11;
	
	display_target_layer(p_window,&temp_frame,GAlignCenter,bmp_array_name,(datetime.hour / 10));
	
	temp_frame.origin.x = 14;
	
	display_target_layer(p_window,&temp_frame,GAlignCenter,bmp_array_name,(datetime.hour % 10));

	
//min
	temp_frame.origin.x = 33;
	display_target_layer(p_window,&temp_frame,GAlignCenter,bmp_array_name,(datetime.min / 10));
	
	temp_frame.origin.x = 45;
	display_target_layer(p_window,&temp_frame,GAlignCenter,bmp_array_name,(datetime.min % 10));

	return p_window;
	
}

void  time_change (enum SysEventType type, void *context)
{
	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		window_reloading();
	}
	
}

int main(void)
{

	P_Window p_window = init_window(); 
	
	/*放入窗口栈显示*/
	g_window_id = app_window_stack_push(p_window);
	/*注册一个事件通知回调，当有改变时，改变表盘显示数据*/
    maibu_service_sys_event_subscribe(time_change);

	return 0;
}

