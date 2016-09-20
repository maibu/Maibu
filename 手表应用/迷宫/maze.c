/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  demo_text.c
 *
 *    Description:  
 *    Corporation:
 * 
 *         Author:  gliu (), gliu@damaijiankang.com
 *        Created:  2014年12月11日 17时06分32秒
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

#define CELL_UP_MASK         0x8
#define CELL_RIGHT_MASK      0x4
#define CELL_DOWN_MASK       0x2
#define CELL_LEFT_MASK       0x1

static int8_t    ball_layer_id = -1; 
static P_Window  g_window_id = NULL;
static uint8_t   ball_pos_x = 3, ball_pos_y = 3;

static const uint8_t level_1_map[16][8] = {
	{0xDB, 0xA8, 0xC9, 0xAA, 0xCB, 0xCD, 0xB8, 0xED},
	{0x3A, 0xC5, 0x36, 0x9C, 0x7B, 0x65, 0x96, 0x96},
	{0x9A, 0x65, 0xD9, 0x63, 0xAE, 0xB6, 0x1A, 0x0A},
	{0x5B, 0xA2, 0x37, 0x9A, 0xEB, 0xC9, 0x6D, 0x5D},
	{0x3A, 0x8A, 0xCD, 0x1A, 0x1E, 0x55, 0xB4, 0x36},
	{0x9E, 0x1E, 0x12, 0x4D, 0x5D, 0x53, 0xC3, 0xC9},
	{0x3A, 0x69, 0x4B, 0x26, 0x32, 0x2E, 0x3C, 0x34},
	{0xDB, 0xC7, 0x3A, 0x8E, 0xBC, 0xD9, 0xA6, 0xD5},	
	{0x1E, 0x59, 0xAE, 0x5B, 0xA0, 0x65, 0xDB, 0x65},
	{0x5B, 0x24, 0x98, 0x6D, 0xB2, 0xA6, 0x38, 0xE5},	
	{0x79, 0xC5, 0x55, 0xDE, 0xC9, 0xA8, 0xE5, 0x96},
	{0xD7, 0x77, 0x53, 0x45, 0x75, 0xD5, 0x96, 0x5D},
	{0x5D, 0xDD, 0x3C, 0x57, 0x96, 0x55, 0x2A, 0x24},
	{0x30, 0x65, 0xD5, 0x79, 0x6B, 0x43, 0xAA, 0xC7},
	{0xD5, 0x94, 0x53, 0xA6, 0x9A, 0x0A, 0xAC, 0x3C},
	{0x36, 0x77, 0x3A, 0xEB, 0x6B, 0x2E, 0xB2, 0xE7}
};

static void ball_pos_update(uint8_t x, uint8_t y)
{
	P_Window p_old_window = app_window_stack_get_window_by_id(g_window_id);
	
	if(p_old_window == NULL)
	{
		return;
	}
	
	/* 创建小球位图图层 */
	GRect ball_frame = {{0, 0}, {7, 7}}; //创建小球位图图层显示框架范围，起始点{x=0,y=0},屏幕左上角为{0,0},高度宽度{h=7,w=7}
	GBitmap ball_bitmap;
	
	//更新为当前小球坐标
	ball_frame.origin.x = ball_pos_x - 3;
	ball_frame.origin.y = ball_pos_y - 3;
	
	//获取位图资源, 宏RES_BITMAP_WATCHFACE_WEATHER_CLOUDY由用户在appinfo.json中配置
	res_get_user_bitmap(BALL, &ball_bitmap); 
	
	//生成位图结构体, 依次为位图资源、显示位置、对齐方式
	LayerBitmap ball_layer_bitmap = {ball_bitmap, ball_frame, GAlignCenter}; 
	
	//创建图层 
	P_Layer p_new_ball_layer = app_layer_create_bitmap(&ball_layer_bitmap); 
	

	Layer * p_old_ball_layer = app_window_get_layer_by_id(p_old_window, ball_layer_id);
	if(p_old_ball_layer)
	{
		ball_layer_id = app_window_replace_layer(p_old_window, p_old_ball_layer, p_new_ball_layer);
		app_window_update(p_old_window);

	}
}

static void on_button_back_clicked(void *p_context)
{
	ball_pos_x = 3;
	ball_pos_y = 3;
	
	P_Window p_window = (P_Window)p_context;
	if(p_window)
	{
		app_window_stack_pop(p_window);
	}
}

static void ball_pos_calc(int delta_x, int delta_y)
{
    uint8_t curr_cell_pos_x, curr_cell_pos_y;

    curr_cell_pos_x = ball_pos_x / 8;
    curr_cell_pos_y = ball_pos_y / 8;

    if(delta_x>0)
    {
        if( !(level_1_map[curr_cell_pos_y][curr_cell_pos_x/2] & (CELL_RIGHT_MASK<<( (curr_cell_pos_x%2) ? 0 : 4)) ) )
        {
            //没有墙壁，并且y在中心点时才允许拐弯前进后退
            if((ball_pos_y-3)%8 == 0)
            {
                ball_pos_x++;
            }
        }
        else
        {
            //有墙壁，并且x不在中心点时允许前进后退
            if( ((ball_pos_y-3)%8 == 0) && ((ball_pos_x-3)%8 != 0) )
            {
                ball_pos_x++;
            }
        }
    }
    else if(delta_x<0)
    {
        if( !(level_1_map[curr_cell_pos_y][curr_cell_pos_x/2] & (CELL_LEFT_MASK<<( (curr_cell_pos_x%2) ? 0 : 4)) ) )
        {
            //没有墙壁，并且y在中心点时才允许拐弯前进后退
            if((ball_pos_y-3)%8 == 0)
            {
                ball_pos_x--;
            }
        }
        else
        {
            //有墙壁，并且x不在中心点时才允许前进后退
            if( ((ball_pos_y-3)%8 == 0) && ((ball_pos_x-3)%8 != 0) )
            {
                ball_pos_x--;
            }
        }
    }

    if(delta_y>0)
    {
        if( !(level_1_map[curr_cell_pos_y][curr_cell_pos_x/2] & (CELL_DOWN_MASK<<( (curr_cell_pos_x%2) ? 0 : 4)) ) )
        {
            //没有墙壁，并且y在中心点时才允许拐弯前进后退
            if((ball_pos_x-3)%8 == 0)
            {
                ball_pos_y++;
            }
        }
        else
        {
            //有墙壁，并且y不在中心点时才允许前进后退
            if( ((ball_pos_x-3)%8 == 0) && ((ball_pos_y-3)%8 != 0) )
            {
                ball_pos_y++;
            }
        }
    }
    else if(delta_y<0)
    {
        if( !(level_1_map[curr_cell_pos_y][curr_cell_pos_x/2] & (CELL_UP_MASK<<( (curr_cell_pos_x%2) ? 0 : 4)) ) )
        {
            //没有墙壁，并且y在中心点时才允许拐弯前进后退
            if((ball_pos_x-3)%8 == 0)
            {
                ball_pos_y--;
            }
        }
        else
        {
            //有墙壁，并且y不在中心点时才允许前进后退
            if( ((ball_pos_x-3)%8 == 0) && ((ball_pos_y-3)%8 != 0) )
            {
                ball_pos_y--;
            }
        }
    }
}

static void ball_thread(date_time_t tick_time, uint32_t millis, void* context)
{
	int16_t x = 0, y = 0, z = 0;
	
	maibu_get_accel_data(&x, &y, &z);
	
	if(x > 2048)
	{
		//ball_pos_y -= 1;
		ball_pos_calc(0, -2);
	}
	else
	{
		//ball_pos_y += 1;
		ball_pos_calc(0, 2);
	}
	
	if(y > 2048)
	{
		//ball_pos_x -= 1;
		ball_pos_calc(-2, 0);
	}
	else
	{
		//ball_pos_x += 1;
		ball_pos_calc(2, 0);
	}

	ball_pos_update(ball_pos_x, ball_pos_y);
}

int main()
{
	P_Window  p_window;
	
	/*创建窗口，窗口中可以添加唯一的基本元素图层*/
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return 0;	
	}

	/* 创建位图图层 */
	GRect map_frame = {{0, 0}, {128, 128}}; //创建位图图层显示框架范围，起始点{x=0,y=0},屏幕左上角为{0,0},高度宽度{h=128,w=128}
	GBitmap map_bitmap;	

	//获取位图资源, 宏RES_BITMAP_WATCHFACE_WEATHER_CLOUDY由用户在appinfo.json中配置
	res_get_user_bitmap(RES_BITMAP_LEVEL_1_1, &map_bitmap); 
	
	//生成位图结构体, 依次为位图资源、显示位置、对齐方式
	LayerBitmap map_layer_bitmap = {map_bitmap, map_frame, GAlignCenter}; 

	//创建图层 
	P_Layer map_layer = app_layer_create_bitmap(&map_layer_bitmap); 

	//添加图层到窗口中
	app_window_add_layer(p_window, map_layer);
	
	/* 创建小球位图图层 */
	GRect ball_frame = {{0, 0}, {7, 7}}; //创建小球位图图层显示框架范围，起始点{x=0,y=0},屏幕左上角为{0,0},高度宽度{h=7,w=7}
	GBitmap ball_bitmap;
	
	//获取位图资源, 宏RES_BITMAP_WATCHFACE_WEATHER_CLOUDY由用户在appinfo.json中配置
	res_get_user_bitmap(BALL, &ball_bitmap); 
	
	//生成位图结构体, 依次为位图资源、显示位置、对齐方式
	LayerBitmap ball_layer_bitmap = {ball_bitmap, ball_frame, GAlignCenter}; 
	
	//创建图层 
	P_Layer ball_layer = app_layer_create_bitmap(&ball_layer_bitmap); 
	
	//添加图层到窗口中
	ball_layer_id = app_window_add_layer(p_window, ball_layer);
	
	/* 注册按键回调事件 */
	app_window_click_subscribe(p_window, ButtonIdBack, on_button_back_clicked);
	
	/* 注册定时器回调事件 */
	app_service_timer_subscribe(50, ball_thread, NULL);
	
	/*把窗口放入窗口栈中显示*/
	g_window_id = app_window_stack_push(p_window);

	return 0;
}












