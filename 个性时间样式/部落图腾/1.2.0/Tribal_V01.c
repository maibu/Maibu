/*
 * =====================================================================================
 * 
 *    Corporation:  Shenzhen Maibu Technology Co., Ltd. All Rights Reserved.
 *       Filename:  Tribal.c
 *         Author:  gliu , gliu@maibu.cc
 *        Created:  2016年02月20日 15时19分59秒
 * 
 *    Description:  部落图腾
 *
 * =====================================================================================
 *  version v1.0
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

/*设置小时、分钟图标x\y\h\w\*/
const uint8_t time_xpos[] = {14, 69, 14, 69};
const uint8_t time_ypos[] = {2, 2, 53, 53};
#define TIME_ICON_SIZE_H       45
#define TIME_ICON_SIZE_W       45

/*设置月份日期x\y\h\w*/
const uint8_t date_xpos[] = {14,39,69, 94};
#define DATE_NUM_ICON_ORIGIN_Y      103
#define DATE_NUM_ICON_SIZE_H        20
#define DATE_NUM_ICON_SIZE_W        20

#define CREAT_LAYER         true
#define UPDATE_LAYER        false

static int32_t main_window_id = -1;          //window id
static uint8_t time_icon_layer_id[] = {-1, -1, -1, -1};
static uint8_t date_icon_layer_id[] = {-1, -1, -1, -1};

static uint8_t time_old[] = {0, 0, 0, 0};     //时间
static uint8_t date_old[] = {0, 0, 0, 0};       //日期
/*****************0~9数字对应的 digit icon***************************/
const uint32_t get_bigicon_by_num[11] = {NUMBER_0B_BMP, NUMBER_1B_BMP, NUMBER_2B_BMP, NUMBER_3B_BMP, NUMBER_4B_BMP, \
                                    NUMBER_5B_BMP, NUMBER_6B_BMP, NUMBER_7B_BMP, NUMBER_8B_BMP, NUMBER_9B_BMP,NUMBER_10B_BMP};
const uint32_t get_liticon_by_num[11] = {NUMBER_0_BMP, NUMBER_1_BMP, NUMBER_2_BMP, NUMBER_3_BMP, NUMBER_4_BMP, \
                                    NUMBER_5_BMP, NUMBER_6_BMP, NUMBER_7_BMP, NUMBER_8_BMP, NUMBER_9_BMP,NUMBER_10_BMP};
 
/**
 *--------------------------------------------------------------------------------------
 * @function: 添加一张图片到窗口
 * @parameter: @Window--窗口地址
               @frame--图片的框架
               @GAlign--图片显示方式
               @GColor-- 图片背景颜色
               @pic_key--图片的ID
 * @return: layer id
 *--------------------------------------------------------------------------------------
 */
static P_Layer creat_icon_layer(GRect frame, enum GAlign alignment, enum GColor color, uint32_t pic_key)
{
    GBitmap bitmap;
    res_get_user_bitmap(pic_key,  &bitmap);
    LayerBitmap lb = {bitmap, frame, alignment};
    P_Layer layer = app_layer_create_bitmap(&lb);   //创建图片图层
    app_layer_set_bg_color(layer, color);           //设置图层颜色
    
    return layer;      //将图层添加到窗口中，并返回图层ID
}

/**
 *用相同大小的图片替换旧的图片
 */
static void replace_old_layer(P_Window p_window, uint8_t old_layer_id, uint32_t old_pic_key)
{
    GBitmap bitmap;
    res_get_user_bitmap(old_pic_key, &bitmap);
    P_Layer old_layer = app_window_get_layer_by_id(p_window, old_layer_id);        //get old layer by id
    app_layer_set_bitmap_bitmap(old_layer, &bitmap);
}

/**
 *创建背景图层
 */
static uint8_t creat_background_layer(P_Window p_window)
{
    GRect frame = {{0, 0},{128, 128}};
    P_Layer new_layer = creat_icon_layer(frame, GAlignCenter, GColorWhite, FULL_BLACK_BMP);  //创建background图层
    app_window_add_layer(p_window, new_layer);        //将图层添加到窗口中
}
/**
 *get time and date
 */
static void get_time_date(uint8_t *time, uint8_t* date)
{
    struct date_time datetime;
    
    app_service_get_datetime(&datetime);        //获取当前系统时间
    
    time[0] = datetime.hour/10;
    time[1] = datetime.hour%10;
    time[2] = datetime.min/10;
    time[3] = datetime.min%10;
    
    date[0] = datetime.mon/10;
    date[1] = datetime.mon%10;
    date[2] = datetime.mday/10;
    date[3] = datetime.mday%10;
    
    if(datetime.hour/10 == 0)
        time[0] = 10;
        
    if(datetime.mon/10 == 0)
        date[0] = 10;
    
}
/**
 *creat update time layer
 */
static void creat_update_time_layer(P_Window p_window, bool creat_update)
{
    uint8_t pos;
    
    uint8_t cur_time[4], cur_date[4];
    
    get_time_date(cur_time, cur_date);
    
    for(pos = 0; pos < 4; pos++)
    {
        if(creat_update == CREAT_LAYER)
        {
            GRect frame = {{time_xpos[pos], time_ypos[pos]},{TIME_ICON_SIZE_H, TIME_ICON_SIZE_W}};
            P_Layer new_layer = creat_icon_layer(frame, GAlignCenter, GColorWhite, get_bigicon_by_num[cur_time[pos]]);  //创建time图层
            time_icon_layer_id[pos] = app_window_add_layer(p_window, new_layer);        //将图层添加到窗口中
            
            frame = (GRect){{date_xpos[pos], DATE_NUM_ICON_ORIGIN_Y},{DATE_NUM_ICON_SIZE_H, DATE_NUM_ICON_SIZE_W}};
            new_layer = creat_icon_layer(frame, GAlignCenter, GColorWhite, get_liticon_by_num[cur_date[pos]]);  //创建date图层
            date_icon_layer_id[pos] = app_window_add_layer(p_window, new_layer);        //将图层添加到窗口中
        }
        else if(creat_update == UPDATE_LAYER) 
        {
            if(cur_time[pos] != time_old[pos])
            {
                replace_old_layer(p_window, time_icon_layer_id[pos], get_bigicon_by_num[cur_time[pos]]);
            }
            
            if(cur_date[pos] != date_old[pos])
            {
                replace_old_layer(p_window, date_icon_layer_id[pos], get_liticon_by_num[cur_date[pos]]);
            }
        }
        
        time_old[pos] = cur_time[pos];
        date_old[pos] = cur_date[pos];
    }
}

/**
 *--------------------------------------------------------------------------------------
 * @function: creat init window
 * @parameter: none
 * @return: window pointer
 *--------------------------------------------------------------------------------------
 */
static P_Window init_main_window(void)
{
    P_Window p_window = app_window_create();

    creat_background_layer(p_window);
    creat_update_time_layer(p_window, CREAT_LAYER);     //creat hour/minute/month/mday layer
    
    return (p_window);
}

/**
 *--------------------------------------------------------------------------------------
 * @function: timing update time data
 * @parameter: 
 * @return: none
 *--------------------------------------------------------------------------------------
 */
static void sys_time_change_callback(enum SysEventType type, void *context)
{
    if (type == SysEventTypeTimeChange)
	{
		P_Window p_window = app_window_stack_get_window_by_id(main_window_id);	
		if (NULL != p_window)
		{
            creat_update_time_layer(p_window, UPDATE_LAYER);    //update hour/minute/spongbob
            app_window_update(p_window);                        //update window
		}
	}
}

int main(void)
{
//   simulator_init();
    P_Window p_window = init_main_window();
    
    if(p_window != NULL)
        main_window_id = app_window_stack_push(p_window);
    
    maibu_service_sys_event_subscribe(sys_time_change_callback);

 // simulator_wait();    
}