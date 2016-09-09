/*
 * =====================================================================================
 * 
 *    Corporation:  Shenzhen Maibu Technology Co., Ltd. All Rights Reserved.
 *       Filename:  SpongeBob.c
 *         Author:  gliu , gliu@maibu.cc
 *        Created:  2016年02月20日 15时19分59秒
 * 
 *    Description:  热血高校
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

/*set background picture x\y\h\w居中*/
#define WINDOW_BG_ORIGIN_X     0
#define WINDOW_BG_ORIGIN_Y     0
#define WINDOW_BG_SIZE_H       128
#define WINDOW_BG_SIZE_W       128

/*设置kick图标x\y\h\w\*/
#define KICK_ICON_ORIGIN_X     14
#define KICK_ICON_ORIGIN_Y     84
#define KICK_ICON_SIZE_H       40
#define KICK_ICON_SIZE_W       72

/*设置 time 图层x\y\h\w居中*/
const uint8_t time_pos[] = {89,97,109,117};
#define TIME_ICON_ORIGIN_Y      4
#define TIME_ICON_SIZE_H        6
#define TIME_ICON_SIZE_W        8

/*设置电量 num 图层x\y\h\w*/
const uint8_t batt_pos[] = {50,58,66};
#define BAT_NUM_ICON_ORIGIN_Y      4
#define BAT_NUM_ICON_SIZE_H        6
#define BAT_NUM_ICON_SIZE_W        8

/*设置电量 icon 图层x\y\h\w*/
#define BATTERY_ICON_ORIGIN_X   30
#define BATTERY_ICON_ORIGIN_Y   3
#define BATTERY_ICON_SIZE_H     7
#define BATTERY_ICON_SIZE_W     19

#define CREAT_LAYER         true        //创建图层
#define UPDATE_LAYER        false       //更新图层

static int32_t main_window_id = -1;     //window id
static uint8_t time_icon_layer_id[] = {-1, -1, -1, -1};
static uint8_t bat_num_layer_id[] = {-1, -1, -1};
static uint8_t bat_icon_layer_id = -1;
static uint8_t kick_icon_layer_id = -1;

/*****************0~9数字对应的 digit icon***************************/
const uint32_t get_digiticon_by_num[11] = {NUMBER_0_BMP, NUMBER_1_BMP, NUMBER_2_BMP, NUMBER_3_BMP, NUMBER_4_BMP, \
                                    NUMBER_5_BMP, NUMBER_6_BMP, NUMBER_7_BMP, NUMBER_8_BMP, NUMBER_9_BMP, BLACK_BG_BMP};
                                    
const uint32_t get_kick_by_num[2] = {WHITE_BG_BMP, KICK_ICON};                                    

/**电量图标数组*/
const uint32_t get_bat_icon[] = {BATTEARY_10_ICON, BATTEARY_20_ICON, BATTEARY_40_ICON, \
                                    BATTEARY_60_ICON, BATTEARY_80_ICON, BATTEARY_100_ICON};
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
 *creat background icon layer
 */
static void creat_background_layer(P_Window p_window)
{
	GRect frame = {{WINDOW_BG_ORIGIN_X, WINDOW_BG_ORIGIN_Y},{WINDOW_BG_SIZE_H, WINDOW_BG_SIZE_W}}; 
    P_Layer layer = creat_icon_layer(frame, GAlignCenter, GColorWhite, BACK_GROUND_ICON);        //创建背景图层
    app_window_add_layer(p_window, layer);        //将图层添加到窗口中
}

/**
 *根据电量获取对应的电量图标
 */                                    
static void creat_update_battery_icon_layer(P_Window p_window, bool creat_update, char battery)
{
    uint32_t pic_key;
    
    if(battery <= 10)
        pic_key = 0;
    else
        pic_key = (battery+19)/20;
        
    if(creat_update == CREAT_LAYER)
    {
        GRect frame = {{BATTERY_ICON_ORIGIN_X, BATTERY_ICON_ORIGIN_Y},{BATTERY_ICON_SIZE_H, BATTERY_ICON_SIZE_W}};
        P_Layer new_layer = creat_icon_layer(frame, GAlignCenter, GColorWhite, get_bat_icon[pic_key]);//创建背景图层
        bat_icon_layer_id = app_window_add_layer(p_window, new_layer);        //将图层添加到窗口中
    }
    else
    {
        replace_old_layer(p_window, bat_icon_layer_id, get_bat_icon[pic_key]);       
    }
}

/**
 *creat update battery icon and number icon layer
 */
static void creat_update_battery_num_layer(P_Window p_window, bool creat_update)
{
    char battery, pos, buf[3];
    
    maibu_get_battery_percent(&battery);        //获取电量数据
    
    memset(buf, 0, sizeof(buf));
    if(battery >= 100)
        sprintf(buf, "%d", battery);
    else
    {
        sprintf(buf, "%02d",battery);
        buf[2] = '9' + 1;
    }
    for(pos = 0; pos < 3; pos++)
    {
        if(creat_update == CREAT_LAYER)
        {
            GRect frame = {{batt_pos[pos], BAT_NUM_ICON_ORIGIN_Y},{BAT_NUM_ICON_SIZE_H, BAT_NUM_ICON_SIZE_W}};        
            P_Layer new_layer = creat_icon_layer(frame, GAlignCenter, GColorWhite, get_digiticon_by_num[buf[pos]-'0']);//创建背景图层
            bat_num_layer_id[pos] = app_window_add_layer(p_window, new_layer);        //将电量 数字 图层添加到窗口中
        }
        else
        {
            replace_old_layer(p_window, bat_num_layer_id[pos], get_digiticon_by_num[buf[pos]-'0']);
        }
    }
    
    creat_update_battery_icon_layer(p_window, creat_update, battery); //创建电量图片 图层
}

/**
 *creat KICK icon
 */
 static void creat_update_kick_layer(P_Window p_window, bool creat_update)
 {
   static char kick_num = 0;
    
    if(creat_update == CREAT_LAYER)
    {
        GRect frame = {{KICK_ICON_ORIGIN_X, KICK_ICON_ORIGIN_Y},{KICK_ICON_SIZE_H, KICK_ICON_SIZE_W}};
        P_Layer new_layer = creat_icon_layer(frame, GAlignCenter, GColorWhite, get_kick_by_num[kick_num++%2]);        //创建KICK图层WHITE_BG_BMP
        kick_icon_layer_id = app_window_add_layer(p_window, new_layer);        //将图层添加到窗口中
    }
    else
    {
        replace_old_layer(p_window, kick_icon_layer_id, get_kick_by_num[kick_num++%2]);
    }  
 }
 
/**
 *creat update time layer
 */
static void creat_update_time_layer(P_Window p_window, bool creat_update)
{
    uint8_t pos;
    
    struct date_time datetime;
    
    app_service_get_datetime(&datetime);        //获取当前系统时间
    
    uint8_t cur_time[4] = {datetime.hour/10, datetime.hour%10, datetime.min/10, datetime.min%10};
    
    for(pos = 0; pos < 4; pos++)
    {
        if(creat_update == CREAT_LAYER)
        {
            GRect frame = {{time_pos[pos], TIME_ICON_ORIGIN_Y},{TIME_ICON_SIZE_H, TIME_ICON_SIZE_W}}; 
            P_Layer new_layer = creat_icon_layer(frame, GAlignCenter, GColorWhite, get_digiticon_by_num[cur_time[pos]]);  //创建时间图层
            time_icon_layer_id[pos] = app_window_add_layer(p_window, new_layer);        //将图层添加到窗口中
        }
        else if(creat_update == UPDATE_LAYER)
        {
            replace_old_layer(p_window, time_icon_layer_id[pos], get_digiticon_by_num[cur_time[pos]]);          
        }
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
    creat_update_kick_layer(p_window, CREAT_LAYER);
    creat_update_battery_num_layer(p_window, CREAT_LAYER);    //creat battery layer
    creat_update_time_layer(p_window, CREAT_LAYER);     //creat hour/minute/month/mdaylayer
    
    return (p_window);
}

/**
 *--------------------------------------------------------------------------------------
 * @function: 定时查看 temperature\battery\sport date and bluetooth statue 数据是否需要更新
 * @parameter: 
 * @return: none
 *--------------------------------------------------------------------------------------
 */
static void timing_layer_callback(date_time_t tick_time, uint32_t millis,void *context)
{
    P_Window p_window = app_window_stack_get_window_by_id(main_window_id);   //根据窗口ID 获取窗口
    
    if(p_window != NULL)
    {
        creat_update_kick_layer(p_window, UPDATE_LAYER);           //定时刷新 kick
        app_window_update(p_window);                        //刷新窗口
    }
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
            creat_update_battery_num_layer(p_window, UPDATE_LAYER);     //定时查询电量数字 图层是否需要刷新
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
    
    app_window_timer_subscribe(p_window, 1000, timing_layer_callback, NULL);    
    maibu_service_sys_event_subscribe(sys_time_change_callback);

 // simulator_wait();    
}