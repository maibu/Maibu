/*
 * =====================================================================================
 * 
 *    Corporation:  Shenzhen Maibu Technology Co., Ltd. All Rights Reserved.
 *       Filename:  super mario.c
 *         Author:  gliu , gliu@maibu.cc
 *        Created:  2016年02月18日 15时19分59秒
 * 
 *    Description:  超级玛丽
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

/*set background picture x\y\h\w*/
#define WINDOW_BG_ORIGIN_X     0
#define WINDOW_BG_ORIGIN_Y     0
#define WINDOW_BG_SIZE_H       128
#define WINDOW_BG_SIZE_W       128

/*设置时间图标x\y\h\w*/
const time_pos[] = {24,42,68,86};
#define TIME_ICON_ORIGIN_Y      101
#define TIME_ICON_SIZE_H        24
#define TIME_ICON_SIZE_W        18

/*设置日期图标x\y\h\w\*/
const mday_pos[] = {94,102,110,118};
#define MDAY_ICON_ORIGIN_Y      3
#define MDAY_ICON_SIZE_H        7
#define MDAY_ICON_SIZE_W        8

/*设置步数图标x\y\h\w*/
const step_pos[] = {16,24,32,40,48,56};
#define MDAY_ICON_ORIGIN_Y      3
#define MDAY_ICON_SIZE_H        7
#define MDAY_ICON_SIZE_W        8

#define CREAT_LAYER         true
#define UPDATE_LAYER        false

static int32_t main_window_id = -1;          //window id
static uint8_t time_icon_layer_id[] = {0, 0, 0, 0};
static uint8_t mday_icon_layer_id[] = {0, 0, 0, 0};
static uint8_t step_icon_layer_id[] = {0, 0, 0, 0, 0, 0};

static uint8_t data_old[] = {0, 0, 0, 0};    //日期
static uint8_t step_old[] = {0, 0, 0, 0, 0, 0};     //步数

static bool creat_update = CREAT_LAYER;
/*****************0~9数字对应的 大图片***************************/
const uint32_t get_bigicon_by_num[10] = {BIG_NUM_0_BMP, BIG_NUM_1_BMP, BIG_NUM_2_BMP, BIG_NUM_3_BMP, BIG_NUM_4_BMP, \
                                    BIG_NUM_5_BMP, BIG_NUM_6_BMP, BIG_NUM_7_BMP, BIG_NUM_8_BMP, BIG_NUM_9_BMP,};

/*****************0~9数字对应的 小图片***************************/
const uint32_t get_smallicon_by_num[11] = {SMALL_NUM_0_BMP, SMALL_NUM_1_BMP, SMALL_NUM_2_BMP, SMALL_NUM_3_BMP, SMALL_NUM_4_BMP, \
                                    SMALL_NUM_5_BMP, SMALL_NUM_6_BMP, SMALL_NUM_7_BMP, SMALL_NUM_8_BMP, SMALL_NUM_9_BMP, WHITE_BG_BMP};                                  
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
static creat_background_layer(P_Window p_window)
{
	GRect frame = {{WINDOW_BG_ORIGIN_X, WINDOW_BG_ORIGIN_Y},{WINDOW_BG_SIZE_H, WINDOW_BG_SIZE_W}}; 
    P_Layer layer = creat_icon_layer(frame, GAlignCenter, GColorWhite, SUPER_MARIO_BG);        //创建背景图层
    app_window_add_layer(p_window, layer);        //将图层添加到窗口中
}

/*
 *将步数数据转换为 字符串
 */
static void get_step_data_string(uint8_t* buf)
{
    SportData data;
    maibu_get_sport_data(&data, 0);     //get sport data
  
    sprintf(buf,"%d", data.step);
}
/**
 *creat or update step layer
 */
static void creat_update_step_layer(P_Window p_window, bool creat_update)
{
    uint8_t cur_step[6], pos;
    
    memset(cur_step, 0, sizeof(cur_step));
    get_step_data_string(cur_step);
    
    for(pos = 0; pos < 6; pos++)
    {
        if(cur_step[pos] < '0' || cur_step[pos] > '9')
            cur_step[pos] = '9' + 1;

        if(creat_update == CREAT_LAYER)
        {
            GRect frame = {{step_pos[pos], MDAY_ICON_ORIGIN_Y},{MDAY_ICON_SIZE_H, MDAY_ICON_SIZE_W}};
            P_Layer new_layer = creat_icon_layer(frame, GAlignCenter, GColorWhite, get_smallicon_by_num[cur_step[pos]-'0']); //创建步数
            step_icon_layer_id[pos] = app_window_add_layer(p_window, new_layer);        //将图层添加到窗口中
        }
        else if(cur_step[pos] != step_old[pos])
        {
            replace_old_layer(p_window,step_icon_layer_id[pos], get_smallicon_by_num[cur_step[pos]-'0']);         
        }
        step_old[pos] = cur_step[pos];
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
            P_Layer new_layer = creat_icon_layer(frame, GAlignCenter, GColorWhite, get_bigicon_by_num[cur_time[pos]]);  //创建时间图层
            time_icon_layer_id[pos] = app_window_add_layer(p_window, new_layer);        //将图层添加到窗口中
        }
        else
        {
            replace_old_layer(p_window,time_icon_layer_id[pos], get_bigicon_by_num[cur_time[pos]]);
        }
    }
}

/**
 *creat update data layer
 */
static void creat_update_data_layer(P_Window p_window, bool creat_update)
{
    uint8_t pos;
    struct date_time datetime;
    
    app_service_get_datetime(&datetime);        //获取当前系统时间
    uint8_t cur_data[4] = {datetime.mon/10, datetime.mon%10, datetime.mday/10, datetime.mday%10};
    
    for(pos = 0; pos < 4; pos++)
    {
        if(creat_update == CREAT_LAYER)
        {
            GRect frame = {{mday_pos[pos], MDAY_ICON_ORIGIN_Y},{MDAY_ICON_SIZE_H, MDAY_ICON_SIZE_W}};
            P_Layer new_layer = creat_icon_layer(frame, GAlignCenter, GColorWhite, get_smallicon_by_num[cur_data[pos]]);  //创建时间图层
            mday_icon_layer_id[pos] = app_window_add_layer(p_window, new_layer);        //将图层添加到窗口中
        }
        else if(cur_data[pos] != data_old[pos])
        {
            replace_old_layer(p_window,mday_icon_layer_id[pos], get_smallicon_by_num[cur_data[pos]]);
        }
        
        data_old[pos] = cur_data[pos];
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
    
    creat_background_layer(p_window);      //creat background layer
    creat_update_time_layer(p_window, CREAT_LAYER);     //creat hour/minute/month/mdaylayer
    creat_update_data_layer(p_window, CREAT_LAYER);     //creat month and mday layer
    creat_update_step_layer(p_window, CREAT_LAYER);     //creat step layer    
    
    return (p_window);
}

/**
 *--------------------------------------------------------------------------------------
 * @function: timing update temperature\battery\sport date and bluetooth statue
 * @parameter: 
 * @return: none
 *--------------------------------------------------------------------------------------
 */
static void timing_update_layer_callback(date_time_t tick_time, uint32_t millis,void *context)
{
    P_Window p_window = app_window_stack_get_window_by_id(main_window_id);	
    if(p_window != NULL)
    {
        creat_update_step_layer(p_window, UPDATE_LAYER);    //update step        
        app_window_update(p_window);                        //update window
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
            creat_update_time_layer(p_window, UPDATE_LAYER);    //update hour/minute/month/mdaylayer
            creat_update_data_layer(p_window, UPDATE_LAYER);    //update month and mday
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
        
    app_window_timer_subscribe(p_window, 600, timing_update_layer_callback, NULL);
    maibu_service_sys_event_subscribe(sys_time_change_callback);

//  simulator_wait();    
}