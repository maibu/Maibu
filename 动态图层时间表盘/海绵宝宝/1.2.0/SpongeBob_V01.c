/*
 * =====================================================================================
 * 
 *    Corporation:  Shenzhen Maibu Technology Co., Ltd. All Rights Reserved.
 *       Filename:  SpongeBob.c
 *         Author:  gliu , gliu@maibu.cc
 *        Created:  2016年02月20日 15时19分59秒
 * 
 *    Description:  海绵宝宝
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

/*set spongebob picture x\y\h\w居中*/
#define SPONGE_BOB_ORIGIN_X     11
#define SPONGE_BOB_ORIGIN_Y     26
#define SPONGE_BOB_SIZE_H       100
#define SPONGE_BOB_SIZE_W       106

/*设置时间图标x\y\h\w居中*/
const uint8_t time_pos[] = {23,42,68,87};
#define TIME_ICON_ORIGIN_Y      6
#define TIME_ICON_SIZE_H        18
#define TIME_ICON_SIZE_W        19

/*设置日期图标x\y\h\w\*/
#define COLON_ICON_ORIGIN_X     61
#define COLON_ICON_ORIGIN_Y     6
#define COLON_ICON_SIZE_H       18
#define COLON_ICON_SIZE_W       7

#define CREAT_LAYER         true
#define UPDATE_LAYER        false

#define SPONG_NUM       16
//#define RANDOM() (rand()%SPONG_NUM)       //creat 0~16 random digit

static int32_t main_window_id = -1;          //window id
static uint8_t time_icon_layer_id[] = {-1, -1, -1, -1};
static uint8_t spongbob_icon_layer_id = -1;

static uint8_t time_old[] = {0, 0, 0, 0};     //时间

/*****************0~9数字对应的 digit icon***************************/
const uint32_t get_digiticon_by_num[10] = {NUMBER_0_BMP, NUMBER_1_BMP, NUMBER_2_BMP, NUMBER_3_BMP, NUMBER_4_BMP, \
                                    NUMBER_5_BMP, NUMBER_6_BMP, NUMBER_7_BMP, NUMBER_8_BMP, NUMBER_9_BMP,};

/*****************0~9数字对应的 spongbob icon***************************/
const uint32_t get_spongbob_by_num[SPONG_NUM] = {SPONGEBOB_01_ICON, SPONGEBOB_02_ICON, SPONGEBOB_03_ICON, SPONGEBOB_04_ICON, \
                                    SPONGEBOB_05_ICON, SPONGEBOB_06_ICON, SPONGEBOB_07_ICON, SPONGEBOB_08_ICON, \
                                    SPONGEBOB_09_ICON, SPONGEBOB_10_ICON, SPONGEBOB_11_ICON, SPONGEBOB_12_ICON, \
                                    SPONGEBOB_13_ICON, SPONGEBOB_14_ICON, SPONGEBOB_15_ICON, SPONGEBOB_16_ICON};  

static uint16_t random_number(void)
{
    uint16_t x, y, z;
    struct date_time datetime;
    uint16_t new_num;
    
    app_service_get_datetime(&datetime);        //获取当前系统时间
    maibu_get_accel_data(&x, &y, &z);           //获取三轴数据
    new_num = (x+y+z+datetime.sec)%SPONG_NUM;   //根据三轴数据与系统时间计算随机数
    return new_num;
} 
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
 *creat  or update spongebob icon layer by random
 */
static void creat_update_spongebob_layer(P_Window p_window, bool creat_update)
{
    char rand_num = random_number();
    
    if(creat_update == CREAT_LAYER)
    {
        GRect frame = {{SPONGE_BOB_ORIGIN_X, SPONGE_BOB_ORIGIN_Y},{SPONGE_BOB_SIZE_H, SPONGE_BOB_SIZE_W}}; 
        P_Layer new_layer = creat_icon_layer(frame, GAlignCenter, GColorWhite, get_spongbob_by_num[rand_num]);        //创建背景图层
        spongbob_icon_layer_id = app_window_add_layer(p_window, new_layer);        //将图层添加到窗口中
    }
    else
    {
       replace_old_layer(p_window, spongbob_icon_layer_id, get_spongbob_by_num[rand_num]);
    }
}

/**
 *creat colon icon
 */
 static void creat_colon_layer(P_Window p_window)
 {
    GRect frame = {{COLON_ICON_ORIGIN_X, COLON_ICON_ORIGIN_Y},{COLON_ICON_SIZE_H, COLON_ICON_SIZE_W}};
    P_Layer layer = creat_icon_layer(frame, GAlignCenter, GColorWhite, COLON_ICON);        //创建colon图层
    app_window_add_layer(p_window, layer);        //将图层添加到窗口中
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

    creat_update_spongebob_layer(p_window, creat_update);   //spongebob change with time
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
    creat_colon_layer(p_window);
    creat_update_time_layer(p_window, CREAT_LAYER);     //creat hour/minute/month/mdaylayer
    
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

//  simulator_wait();    
}