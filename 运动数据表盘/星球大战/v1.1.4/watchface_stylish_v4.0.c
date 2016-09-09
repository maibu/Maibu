/*

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

/*set background picture x\y\h\w*/
#define STYLISH_BG_ORIGIN_X     0
#define STYLISH_BG_ORIGIN_Y     0
#define STYLISH_BG_SIZE_H       128
#define STYLISH_BG_SIZE_W       128
/*set star wars picture x\y\h\w**/
#define STYLISH_SW_ORIGIN_X     12
#define STYLISH_SW_ORIGIN_Y     8
#define STYLISH_SW_SIZE_H       83
#define STYLISH_SW_SIZE_W       51
/*set bluetooth picture x\y\h\w*/
#define STYLISH_BT_ORIGIN_X     14
#define STYLISH_BT_ORIGIN_Y     68
#define STYLISH_BT_SIZE_H       23
#define STYLISH_BT_SIZE_W       46

/*set battery\temperature\altitude\floor TEXT x,y,h,w*/
#define STYLISH_TEXT_SIZE_H    14     /*set battery\temperature\altitude\floor text high*/
#define STYLISH_TEXT_SIZE_W    35     /*set battery\temperature\altitude\floor text wide*/
#define STYLISH_TEXT_ORIGIN_X  66     /*set battery\temperature\altitude\floor origin x*/
#define BATT_TEXT_ORIGIN_Y     11     /*set battery origin y*/
#define TEMP_TEXT_ORIGIN_Y     28     /*set temperature origin y*/
#define ALTI_TEXT_ORIGIN_Y     45     /*set altitude origin y*/
#define FLOO_TEXT_ORIGIN_Y     62     /*set floor origin y*/

/*set STEPS_TEXT x,y,h,w*/
#define STEPS_TEXT_ORIGIN_X      66
#define STEPS_TEXT_ORIGIN_Y      79
#define STEPS_TEXT_SIZE_H        14
#define STEPS_TEXT_SIZE_W        43

/*set WEEKDAY_TEXT x,y,h,w*/
#define WDAY_TEXT_ORIGIN_X      4
#define WDAY_TEXT_ORIGIN_Y      99
#define WDAY_TEXT_SIZE_H        12
#define WDAY_TEXT_SIZE_W        40

/*SET MONTH AND DAY TEXT X,Y,H,W **/
#define MDAY_TEXT_ORIGIN_X      3
#define MDAY_TEXT_ORIGIN_Y      112
#define MDAY_TEXT_SIZE_H        14
#define MDAY_TEXT_SIZE_W        40

/*SET TIME TEXT X,Y,H,W **/
#define TIME_TEXT_ORIGIN_X      46
#define TIME_TEXT_ORIGIN_Y      97
#define TIME_TEXT_SIZE_H        30
#define TIME_TEXT_SIZE_W        80

#define CREAT_LAYER     true
#define UPDATE_LAYER    false

static int32_t stylish_window_id = -1;        //窗口ID
static int8_t alti_text_layer_id = -1;        //海拔文本图层ID
static int altitude_old;                    //用来记录上一次的海拔数据

/**将float转换为int 
 *@function : change float to int
 *@parameter: float 
 *@return : int
 */
static int float_to_int(float src)
{
    char buf[10];
    memset(buf, 0, sizeof(buf));
    sprintf(buf,"%0.1f", src);
    
    return atoi(buf);
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
static int8_t add_pic_to_window(P_Window window, GRect frame, enum GAlign alignment, enum GColor color, uint32_t pic_key)
{
    GBitmap bitmap;
    res_get_user_bitmap(pic_key,  &bitmap);
    LayerBitmap lb = {bitmap, frame, alignment};
    P_Layer layer = app_layer_create_bitmap(&lb);   //创建图片图层
    app_layer_set_bg_color(layer, color);           //设置图层颜色
    
    return app_window_add_layer(window, layer);     //将图层添加到窗口中，并返回图层ID
}

/**
 *--------------------------------------------------------------------------------------
 * @function: 将文本图层添加到当前窗口
 * @parameter: @Window--窗口地址
               @frame--文本图层框架
               @GAlign--文字在图层的显示方式
               @GColor--文字背景颜色
               @ARial-- 文字大小
               @buf---文字内容
 * @return: layer id
 *--------------------------------------------------------------------------------------
 */
static int8_t add_text_to_window(P_Window window, GRect frame, enum GAlign alignment, enum GColor color, uint8_t font, char *buf)
{
	LayerText text = {buf, frame, alignment, font};
	P_Layer layer = app_layer_create_text(&text);       //创建文本图层
	app_layer_set_bg_color(layer, color);               //设置图层颜色
    
    return app_window_add_layer(window, layer);         //将图层添加到窗口中，并返回图层ID
}
/**
 *--------------------------------------------------------------------------------------
* @function:创建\更新 电量文本图层
* @parameter: context--window pointer
*           creat_update -- true 创建图层、 
                            false 更新图层
* @return: none
 *--------------------------------------------------------------------------------------
 */
 static void creat_update_battery_layer(void *context, bool creat_update)
 {
    P_Window p_window = (P_Window) context;
    char battery_new;
    static char battery_old;
    static int8_t batt_text_layer_id = -1;        //电量文本图层ID
    char buf[6];
    
    memset(buf, 0, sizeof(buf));    
    maibu_get_battery_percent(&battery_new);        //获取电量数据
    sprintf(buf, "%d", battery_new);
    
    if(creat_update){
        GRect frame = {{STYLISH_TEXT_ORIGIN_X, BATT_TEXT_ORIGIN_Y},{STYLISH_TEXT_SIZE_H, STYLISH_TEXT_SIZE_W}};
        batt_text_layer_id = add_text_to_window(p_window, frame, GAlignRight, GColorBlack, U_ASCII_ARIAL_14, buf);      //创建电量文本图层
    }else if(battery_old != battery_new){
        P_Layer text_layer = app_window_get_layer_by_id(p_window, batt_text_layer_id);      //更具图层id 获取图层链接
        app_layer_set_text_text(text_layer, buf);               //更新电量文本图层
    }
    
    battery_old = battery_new;
 }
 /**
 *--------------------------------------------------------------------------------------
* @function:创建\更新 温度文本图层
* @parameter: context--window pointer
*           creat_update -- true 创建图层、 
                            false 更新图层
* @return: none
 *--------------------------------------------------------------------------------------
 */
 static void creat_update_temperature_layer(void *context, bool creat_update)
 {
    P_Window p_window = (P_Window) context;
    static char temperature_old;
    static int8_t temp_text_layer_id = -1;        //温度文本图层ID
    float temperature_new;
    char buf[6];
    
    memset(buf, 0, sizeof(buf));
    maibu_get_temperature(&temperature_new);        //获取温度值
    int temp = float_to_int(temperature_new);       //将浮点型数据转变成整形数据
    sprintf(buf, "%d", float_to_int(temperature_new));        
    
    if(creat_update){                                                                                                   //判断是否需要创建图层
        GRect frame = {{STYLISH_TEXT_ORIGIN_X, TEMP_TEXT_ORIGIN_Y},{STYLISH_TEXT_SIZE_H, STYLISH_TEXT_SIZE_W}};
        temp_text_layer_id = add_text_to_window(p_window, frame, GAlignRight, GColorBlack, U_ASCII_ARIAL_14, buf);     //创建文本图层
    }else if(temperature_old != temp){                                                                                  //判断当前温度是否发生变化
        P_Layer text_layer = app_window_get_layer_by_id(p_window, temp_text_layer_id);                                  //通过文本图层ID获得图层链接
        app_layer_set_text_text(text_layer, buf);                                                                       //更新文本图层
    }
    
    temperature_old = temp;     //保存当前温度数值
 }
 /**
 *--------------------------------------------------------------------------------------
* @function:创建\更新 海拔文本图层
* @parameter: context--window pointer
*           creat_update -- true 创建图层、 
                            false 更新图层
* @return: none
 *--------------------------------------------------------------------------------------
 */
 static void creat_update_altitude_layer(void *context, bool creat_update)
 {
    P_Window p_window = (P_Window) context;
    char buf[6];
    memset(buf, 0, sizeof(buf));
    float altitude, accuracy;
    
    maibu_get_altitude(&altitude, &accuracy);   //获取海拔值
    int temp = float_to_int(altitude);          //将浮点型数据转变成整形数据
    sprintf(buf, "%d", temp);    
    
    if(creat_update){                                                                                                   //判断是否需要创建图层
        altitude_old = temp;        //保存当前海拔数值
        GRect frame = {{STYLISH_TEXT_ORIGIN_X, ALTI_TEXT_ORIGIN_Y},{STYLISH_TEXT_SIZE_H, STYLISH_TEXT_SIZE_W}};   
        alti_text_layer_id = add_text_to_window(p_window, frame, GAlignRight, GColorBlack, U_ASCII_ARIAL_14, buf);     //创建文本图层
    }else if((altitude_old < temp - 2) || (altitude_old > temp + 2)){     //判断当前海拔是否变化超过2m，防止静置不动时海拔数据经常变化
        altitude_old = temp;        //保存当前海拔数值
        P_Layer text_layer = app_window_get_layer_by_id(p_window, alti_text_layer_id);                                 //通过文本图层ID获得图层链接
        app_layer_set_text_text(text_layer, buf);                                                                      //更新文本图层
    }
}
/**
*--------------------------------------------------------------------------------------
* @function:创建\更新 楼层、步数文本图层
* @parameter: context--window pointer
*           creat_update -- true 创建图层、 
                            false 更新图层
* @return: none
*--------------------------------------------------------------------------------------
*/
static void creat_update_FloorStep_layer(void *context, bool creat_update)
{
    P_Window p_window = (P_Window) context;
    static uint32_t floor_old, step_old;
    static int8_t floor_text_layer_id = -1;        //楼层 图层ID
    static int8_t step_text_layer_id = -1;        //步数 图层ID
    char buf[8];
    P_Layer text_layer;
    SportData data;
    
    maibu_get_sport_data(&data, 0);         //获取运动数据
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%d", data.floor);

    if(creat_update){
        GRect frame = {{STYLISH_TEXT_ORIGIN_X, FLOO_TEXT_ORIGIN_Y},{STYLISH_TEXT_SIZE_H, STYLISH_TEXT_SIZE_W}};
        floor_text_layer_id = add_text_to_window(p_window, frame, GAlignRight, GColorBlack, U_ASCII_ARIAL_14, buf);     //创建楼层文本图层
    }else if(floor_old != data.floor){                                                                                  //判断楼层是否发生变化
        text_layer = app_window_get_layer_by_id(p_window, floor_text_layer_id);                                         //通过文本图层ID获得图层链接
        app_layer_set_text_text(text_layer, buf);                                                                       //更新文本图层
    }
    floor_old = data.floor;        //保存当前楼层数值

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%d", data.step);
    if(creat_update){
        GRect frame = {{STEPS_TEXT_ORIGIN_X, STEPS_TEXT_ORIGIN_Y},{STEPS_TEXT_SIZE_H, STEPS_TEXT_SIZE_W}};
        step_text_layer_id = add_text_to_window(p_window, frame, GAlignRight, GColorBlack, U_ASCII_ARIAL_14, buf);      //创建步数文本图层
    }else if(step_old != data.step){                                                                                    //判断步数是否发生变化
        text_layer = app_window_get_layer_by_id(p_window, step_text_layer_id);                                          //通过文本图层ID获得图层链接
        app_layer_set_text_text(text_layer, buf);                                                                       //更新文本图层
        
        float altitude, accuracy;
        maibu_get_altitude(&altitude, &accuracy);   //获取海拔值
        int temp = float_to_int(altitude);          //将浮点型数据转变成整形数据
        if(temp != altitude_old)                    //当步数变化时&海拔数值发生变化,立马更新海拔值
        {
            altitude_old = temp;        //保存当前海拔数值
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "%d", temp); 
            P_Layer text_layer = app_window_get_layer_by_id(p_window, alti_text_layer_id);                                 //通过文本图层ID获得图层链接
            app_layer_set_text_text(text_layer, buf);                                                                      //更新文本图层
        }
    }
    step_old = data.step;        //保存当前步数数值
}
/**
*--------------------------------------------------------------------------------------
* @function:创建\更新 时间文本图层
* @parameter: context--window pointer
*           creat_update -- true 创建图层、 
                            false 更新图层
* @return: none
*--------------------------------------------------------------------------------------
*/
static void creat_update_time_layer(void *context, bool creat_update)
{
    const char Weekday[7][10] = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};
    P_Window p_window = (P_Window) context;
    
    char buf[10];
    GRect frame;
    P_Layer text_layer;
    struct date_time datetime;
    static struct date_time datetime_old;
    static int8_t week_text_layer_id = -1;        //weekday text layer id
    static int8_t date_text_layer_id = -1;        //date text layer id
    static int8_t time_text_layer_id = -1;        //time text layer id
    
    app_service_get_datetime(&datetime);        //获取系统时间
    
    /*创建、更新 星期文本图层*/
    memset(buf, 0, sizeof(buf)); 
    memcpy(buf, Weekday[datetime.wday], 10);    
    if(creat_update){
        frame = (GRect){{WDAY_TEXT_ORIGIN_X, WDAY_TEXT_ORIGIN_Y},{WDAY_TEXT_SIZE_H, WDAY_TEXT_SIZE_W}};
        week_text_layer_id = add_text_to_window(p_window, frame, GAlignLeft, GColorWhite, U_ASCII_ARIAL_12, buf);       //创建星期文本图层
    } else if(datetime_old.wday != datetime.wday){                                              //判断星期数据是否发生变化
        text_layer = app_window_get_layer_by_id(p_window, week_text_layer_id);                  //根据图层id 获取图层链接
        app_layer_set_text_text(text_layer, buf);                                               //更新图层
    }
    
    /*创建、更新 日期文本图层*/
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%02d-%02d", datetime.mon, datetime.mday);
    if(creat_update){
        frame = (GRect){{MDAY_TEXT_ORIGIN_X, MDAY_TEXT_ORIGIN_Y},{MDAY_TEXT_SIZE_H, MDAY_TEXT_SIZE_W}}; //set text frame
        date_text_layer_id = add_text_to_window(p_window, frame, GAlignLeft, GColorWhite, U_ASCII_ARIAL_12, buf);
    }else if(datetime_old.mday != datetime.mday){
        text_layer = app_window_get_layer_by_id(p_window, date_text_layer_id);
        app_layer_set_text_text(text_layer, buf);
    }
    
    /*创建、更新 时钟文本图层*/
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%02d:%02d", datetime.hour, datetime.min);
    if(creat_update){
        frame = (GRect){{TIME_TEXT_ORIGIN_X, TIME_TEXT_ORIGIN_Y},{TIME_TEXT_SIZE_H, TIME_TEXT_SIZE_W}};
        time_text_layer_id = add_text_to_window(p_window, frame, GAlignRight, GColorWhite, U_ASCII_ARIAL_30, buf);
    } else if(datetime_old.min != datetime.min){
        text_layer = app_window_get_layer_by_id(p_window, time_text_layer_id);
        app_layer_set_text_text(text_layer, buf);
    }
    
    memcpy(&datetime_old, &datetime, sizeof(struct date_time));             //保存当前时间数据
}
 
/**
 * @function: 判断显示Bluetooth disconnect图标，还是显示一个小黑点
 * @parameter: context -- window point
 * @return: none
 *
**/
static void bluetooth_layer_display_which_icon(void *context, bool Creat_update)
{
    P_Window p_window = (P_Window)context;    
    GRect frame;
    LayerBitmap lb;
    GBitmap bitmap;
    P_Layer layer, old_layer;
    static bool ble_icon_display = false;
    static int8_t ble_icon_layer_id = -1;         //bluetooth picture layer id
    enum BLEStatus ble_status = maibu_get_ble_status();
    
    if(Creat_update)                                                            //如果需要创建图层
    {
        if(ble_status != BLEStatusConnected && ble_status != BLEStatusUsing)    //如果蓝牙处于断开状态，创建Bluetooth disconnect 图层
        {   
            ble_icon_display = true;
            frame = (GRect){{STYLISH_BT_ORIGIN_X, STYLISH_BT_ORIGIN_Y},{STYLISH_BT_SIZE_H, STYLISH_BT_SIZE_W}};
            res_get_user_bitmap(STYLISH_BLE_FAILED_BMP,  &bitmap);
            lb = (LayerBitmap){bitmap, frame, GAlignCenter};
            layer = app_layer_create_bitmap(&lb);
            app_layer_set_bg_color(layer, GColorWhite);
            ble_icon_layer_id = app_window_add_layer(p_window, layer);      //创建Bluetooth disconnect图层
        }
        else                                                                //如果蓝牙处于连接状态， 创建一个点图层
        {
            ble_icon_display = false;
            LayerGeometry layer_geometry;	//几何图层结构体
            Line l = {{14,68}, {15,68}};	//直线的起始点坐标，当蓝牙连接时用一个点代替Bluetooth disconnect图标。
            Geometry *lg = &(Geometry){GeometryTypeLine, FillOutline, GColorBlack, (void*)&l};        
            layer_geometry.num = 1;
            layer_geometry.p_g = &lg;
            layer = app_layer_create_geometry(&layer_geometry);        
            ble_icon_layer_id = app_window_add_layer(p_window, layer);      //创建小黑点图层
        }
    }
    else                                                                //如果需要UPDATE图层
    {
        if(ble_status != BLEStatusConnected && ble_status != BLEStatusUsing && !ble_icon_display)    //蓝牙未连接状态， 且Bluetooth disconnect图标未显示
        {   
            ble_icon_display = true;
            frame = (GRect){{STYLISH_BT_ORIGIN_X, STYLISH_BT_ORIGIN_Y},{STYLISH_BT_SIZE_H, STYLISH_BT_SIZE_W}};
            res_get_user_bitmap(STYLISH_BLE_FAILED_BMP,  &bitmap);
            lb = (LayerBitmap){bitmap, frame, GAlignCenter};            
            layer = app_layer_create_bitmap(&lb);
            app_layer_set_bg_color(layer, GColorWhite);
            old_layer = app_window_get_layer_by_id(p_window, ble_icon_layer_id);
            ble_icon_layer_id = app_window_replace_layer(p_window, old_layer, layer);                   //用Bluetooth disconnect图标 代替小黑点
        }
        else if(ble_icon_display && (ble_status == BLEStatusConnected || ble_status == BLEStatusUsing)) //蓝牙处于连接状态， 且Bluetooth disconnect图标 处于显示状态
        {
            ble_icon_display = false;
            LayerGeometry layer_geometry;	//几何图层结构体
            Line l = {{14,68}, {15,68}};	//直线的起始点坐标
            Geometry *lg = &(Geometry){GeometryTypeLine, FillOutline, GColorBlack, (void*)&l};
            
            layer_geometry.num = 1;
            layer_geometry.p_g = &lg;

            layer = app_layer_create_geometry(&layer_geometry);
            old_layer = app_window_get_layer_by_id(p_window, ble_icon_layer_id);            
            ble_icon_layer_id = app_window_replace_layer(p_window, old_layer, layer);      //用小黑点代替Bluetooth disconnect图标
        }
    }
}
/**
 *--------------------------------------------------------------------------------------
 * @function: 初始化窗口
 * @parameter: none
 * @return: window pointer
 *--------------------------------------------------------------------------------------
 */
static P_Window stylish_init_window(void)
{
    P_Window p_window = app_window_create();
    
    /*Add a background image layer*/
	GRect frame = {{STYLISH_BG_ORIGIN_X, STYLISH_BG_ORIGIN_Y},{STYLISH_BG_SIZE_H, STYLISH_BG_SIZE_W}};
	add_pic_to_window(p_window, frame, GAlignCenter, GColorWhite, STYLISH_WATCHFACE_BMP);           //设置背景图，
    
   /*Add a star wars image layer*/
    frame = (GRect){{STYLISH_SW_ORIGIN_X, STYLISH_SW_ORIGIN_Y},{STYLISH_SW_SIZE_H, STYLISH_SW_SIZE_W}};
	add_pic_to_window(p_window, frame, GAlignCenter, GColorWhite, STYLISH_STYLISH_BMP);         //设置战士 图片
        
    bluetooth_layer_display_which_icon(p_window, CREAT_LAYER);       //创建蓝牙图层    
    creat_update_battery_layer(p_window, CREAT_LAYER);               //创建电量文本图层
    creat_update_temperature_layer(p_window, CREAT_LAYER);           //创建温度文本图层
    creat_update_altitude_layer(p_window, CREAT_LAYER);              // 创建海拔文本图层
    creat_update_FloorStep_layer(p_window, CREAT_LAYER);             //创建楼层、步数文本图层
    creat_update_time_layer(p_window, CREAT_LAYER);                  //创建多个时间文本图层

    return (p_window);
}
/**
 *--------------------------------------------------------------------------------------
 * @function: 定时查看 temperature\battery\sport date and bluetooth statue 数据是否需要更新
 * @parameter: 
 * @return: none
 *--------------------------------------------------------------------------------------
 */
static void sportsdate_bluetooth_callback(date_time_t tick_time, uint32_t millis,void *context)
{
    P_Window p_window = app_window_stack_get_window_by_id(stylish_window_id);   //根据窗口ID 获取窗口
    
    if(p_window != NULL)
    {
        creat_update_battery_layer(p_window, UPDATE_LAYER);               //更新电量
        creat_update_temperature_layer(p_window, UPDATE_LAYER);           //更新温度
        bluetooth_layer_display_which_icon(p_window, UPDATE_LAYER);       //更新蓝牙显示状态   
        creat_update_altitude_layer(p_window, UPDATE_LAYER);              //更新海拔
        creat_update_FloorStep_layer(p_window, UPDATE_LAYER);             //更新楼层、步数
        
        app_window_update(p_window);                        //刷新窗口
    }
}

/**
 *--------------------------------------------------------------------------------------
 * @function: 时间跟新函数，查看时间文本内数据是否需要更新
 * @parameter: 
 * @return: none
 *--------------------------------------------------------------------------------------
 */
static void stylish_time_change_callback(enum SysEventType type, void *context)
{
    if (type == SysEventTypeTimeChange)     //判断是否为系统时间变化事件
	{
		P_Window p_window = app_window_stack_get_window_by_id(stylish_window_id);	    //更具窗口ID获取窗口
		if (NULL != p_window)
		{
            creat_update_time_layer(p_window, UPDATE_LAYER);      //更新时间            
			app_window_update(p_window);                          //刷新窗口
		}
	}
}

int main()
{
 //   simulator_init();     //开模拟器的时候打开
    
	/*creat window*/
	P_Window p_window = stylish_init_window(); 
	/*push window to stack*/
	stylish_window_id = app_window_stack_push(p_window);
    
    app_window_timer_subscribe(p_window, 1000, sportsdate_bluetooth_callback, NULL);    //每秒调用一次sportsdate_bluetooth_callback函数，查看是否有数据更新
    maibu_service_sys_event_subscribe(stylish_time_change_callback);                    //当系统时间发生变化时，调用时间更新函数
    
 //  simulator_wait();      //开模拟器的时候打开
    
	return 0;
}

