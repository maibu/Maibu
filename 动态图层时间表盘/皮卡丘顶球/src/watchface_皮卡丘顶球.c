#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "maibu_sdk.h"
#include "maibu_res.h"

//<静态变量声明>

//屏幕坐标与尺寸相关
#define WATCHFACE_BG_ORIGIN_X 	0
#define WATCHFACE_BG_ORIGIN_Y 	0
#define WATCHFACE_BG_SIZE_H	128
#define WATCHFACE_BG_SIZE_W 	128

#define WATCHFACE_ANI_ORIGIN_X 	5
#define WATCHFACE_ANI_ORIGIN_Y 	35
#define WATCHFACE_ANI_SIZE_H	84
#define WATCHFACE_ANI_SIZE_W 	53

#define WATCHFACE_TIME_HOUR1_ORIGIN_X   60
#define WATCHFACE_TIME_HOUR2_ORIGIN_X   73

#define WATCHFACE_TIME_HOUR_ORIGIN_Y   14
#define WATCHFACE_TIME_HOUR_SIZE_H  25
#define WATCHFACE_TIME_HOUR_SIZE_W  60

#define WATCHFACE_TIME_MIN1_ORIGIN_X    95
#define WATCHFACE_TIME_MIN2_ORIGIN_X    110

#define WATCHFACE_TIME_MIN_ORIGIN_Y 16
#define WATCHFACE_TIME_MIN_SIZE_H   31
#define WATCHFACE_TIME_MIN_SIZE_W   15

static GRect bmp_time_rect[] = {
    {
        {WATCHFACE_TIME_HOUR1_ORIGIN_X,WATCHFACE_TIME_HOUR_ORIGIN_Y},
        {WATCHFACE_TIME_HOUR_SIZE_H,WATCHFACE_TIME_HOUR_SIZE_W}
    },
    {
        {WATCHFACE_TIME_HOUR2_ORIGIN_X,WATCHFACE_TIME_HOUR_ORIGIN_Y},
        {WATCHFACE_TIME_HOUR_SIZE_H,WATCHFACE_TIME_HOUR_SIZE_W}
    },
    {
        {WATCHFACE_TIME_MIN1_ORIGIN_X,WATCHFACE_TIME_MIN_ORIGIN_Y},
        {WATCHFACE_TIME_MIN_SIZE_H,WATCHFACE_TIME_MIN_SIZE_W}
    },
    {
        {WATCHFACE_TIME_MIN2_ORIGIN_X,WATCHFACE_TIME_MIN_ORIGIN_Y},
        {WATCHFACE_TIME_MIN_SIZE_H,WATCHFACE_TIME_MIN_SIZE_W}
    }
};

static char wday_string_arr[7][15] = {
    "星期日",
    "星期一",
    "星期二",
    "星期三",
    "星期四",
    "星期五",
    "星期六"
};

static GRect bmp_ani_rect = {
	{WATCHFACE_ANI_ORIGIN_X,WATCHFACE_ANI_ORIGIN_Y},
	{WATCHFACE_ANI_SIZE_H,WATCHFACE_ANI_SIZE_W}
};

static uint32_t bmp_bg[]=
{
	RES_BITMAP_WATCHFACE_BG
};

//动画帧数组
static uint32_t bmp_ani_frames[] =
{
	RES_BITMAP_WATCHFACE_ANI_1,
	RES_BITMAP_WATCHFACE_ANI_3,
	RES_BITMAP_WATCHFACE_ANI_4,
	RES_BITMAP_WATCHFACE_ANI_5
};

//背景颜色
enum GColor g_backgroundColor = GColorBlack;

static uint32_t APP_SET_PRESIST_DATA_KEY = 0x2003;
static char app_set_text[20] = {0};

/*动画图层ID*/
static int8_t g_layer_ani_id = -1;
static int8_t g_layer_sec_id = -1;

/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_window_id = -1;

static int8_t g_ani_index= 0;
static int8_t g_ani_frame_count= sizeof(bmp_ani_frames)/sizeof(bmp_ani_frames[0]);

/*表盘应显示数据*/
static uint8_t watch_data[11] = {0};

/*文字缓存*/
char text_buf[20]= "";
/*文字矩形区域*/
GRect frame_bg = {{0,0}, {128,128}};
GRect frame_date = {{12, 10}, {16, 44}};
GRect frame_wday = {{72, 6}, {25, 44}};
GRect frame_hm = {{63, 42}, {25, 60}};
GRect frame_sec = {{70, 75}, {40, 50}};

//</静态变量声明>

//<静态函数声明>
static P_Window init_window(void);
//</静态函数声明>


/*
 *--------------------------------------------------------------------------------------
 *     function:  get_watch_data
 *    parameter:  0为watch_data所有元素值，1仅为watch_data中的秒
 *       return:
 *  description:  将数组参数赋值为当前表盘应显示值
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
static void get_watch_data(uint8_t sec_refresh)
{
	struct date_time datetime;
	app_service_get_datetime(&datetime);

	watch_data[4] = datetime.sec/10;
	watch_data[5] = datetime.sec%10;
	if(sec_refresh == 1)
	{
		return;
	}
	watch_data[0] = datetime.hour/10;
	watch_data[1] = datetime.hour%10;
	watch_data[2] = datetime.min/10;
	watch_data[3] = datetime.min%10;

	watch_data[6] = datetime.mon/10;
	watch_data[7] = datetime.mon%10;
	watch_data[8] = datetime.mday/10;
	watch_data[9] = datetime.mday%10;

	watch_data[10] = datetime.wday;

}

/*创建并显示图片图层，需要坐标值，得到icon_key的数组，数组需要的参数值，P_Window*/
int32_t display_target_layer(P_Window p_window,GRect *temp_p_frame,enum GAlign how_to_align,enum GColor black_or_white,int32_t bmp_array_name[],int bmp_id_number)
{
	GBitmap bmp_point;
	P_Layer temp_P_Layer = NULL;

	res_get_user_bitmap(bmp_array_name[bmp_id_number], &bmp_point);
	LayerBitmap layer_bitmap_struct_l = {bmp_point, *temp_p_frame, how_to_align};
 	temp_P_Layer = app_layer_create_bitmap(&layer_bitmap_struct_l);

	if(temp_P_Layer != NULL)
	{
		app_layer_set_bg_color(temp_P_Layer, black_or_white);
		return app_window_add_layer(p_window, temp_P_Layer);
	}

	return 0;
}

int32_t display_text_layer(P_Window p_window, GRect *temp_p_frame, enum GAlign how_to_align,enum GColor black_or_white, char* text_buf, uint8_t font_type)
{
    /*生成文本结构体*/
    LayerText text = {text_buf, *temp_p_frame, how_to_align, font_type};
    /*创建文本图层*/
    P_Layer temp_P_Layer = app_layer_create_text(&text);

    if(temp_P_Layer != NULL)
    {
        app_layer_set_bg_color(temp_P_Layer, black_or_white);
        return app_window_add_layer(p_window, temp_P_Layer);
    }

    return 0;
}

static void sec_callback()
{
    P_Window p_window = NULL;
    P_Layer p_layer = NULL;
    GBitmap bitmap = {0};
    uint8_t i;

    /*根据窗口ID获取窗口句柄*/
    p_window = app_window_stack_get_window_by_id(g_window_id);
    if (p_window == NULL)
    {
        return;
    }

    /*获取数据图层句柄*/
    p_layer = app_window_get_layer_by_id(p_window, g_layer_sec_id);
    if (p_layer != NULL)
    {
        sprintf(text_buf, "%d%d", watch_data[4], watch_data[5]);
        app_layer_set_text_text(p_layer, text_buf);
    }

    /*窗口显示*/
    app_window_update(p_window);
}

static void ani_index_update()
 {
    g_ani_index++;
    if(g_ani_index>=g_ani_frame_count)
    {
        g_ani_index= 0;
    }
 }

static void ani_callback(date_time_t tick_time, uint32_t millis,void *context)
{
  	P_Window p_window = NULL;
	P_Layer p_layer = NULL;
	GBitmap bitmap = {0};
	uint8_t i;

	/*根据窗口ID获取窗口句柄*/
	p_window = app_window_stack_get_window_by_id(g_window_id);
	if (p_window == NULL)
	{
		return;
	}
    ani_index_update();

    uint8_t oldSec2= watch_data[5];
    get_watch_data(1);
    if(oldSec2!= watch_data[5])
    {
        sec_callback();
    }

	/*获取数据图层句柄*/
    p_layer = app_window_get_layer_by_id(p_window, g_layer_ani_id);
    if (p_layer != NULL)
    {
        /*更新数据图层图片*/
        res_get_user_bitmap(bmp_ani_frames[g_ani_index], &bitmap);
        app_layer_set_bitmap_bitmap(p_layer, &bitmap);
    }

	/*窗口显示*/
	app_window_update(p_window);
}

static P_Window init_window(void)
{
	P_Window p_window = NULL;

	/*创建一个窗口*/
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

    app_persist_read_data(APP_SET_PRESIST_DATA_KEY, 0, app_set_text, sizeof(app_set_text));


    if(!strcmp(app_set_text, "white"))
    {
        g_backgroundColor = GColorWhite;
    }
    else
    {
        g_backgroundColor = GColorBlack;
    }

    /*创建背景层*/
    display_target_layer(p_window,&frame_bg,GAlignCenter,g_backgroundColor,bmp_bg,0);

    /*创建月日图层*/
    sprintf(text_buf, "%d%d-%d%d", watch_data[6], watch_data[7], watch_data[8], watch_data[9]);
    display_text_layer(p_window,&frame_date,GAlignCenter,g_backgroundColor,text_buf,U_ASCII_ARIAL_16);

     /*创建星期图层*/
    sprintf(text_buf, "%s", wday_string_arr[watch_data[10]]);
    display_text_layer(p_window,&frame_wday,GAlignCenter,g_backgroundColor,text_buf,U_GBK_SIMSUNBD_14);

    /*创建时分图层*/
    sprintf(text_buf, "%d%d:%d%d", watch_data[0], watch_data[1], watch_data[2], watch_data[3]);
    display_text_layer(p_window,&frame_hm,GAlignLeft,g_backgroundColor,text_buf,U_ASCII_ARIAL_24);

    /*创建秒图层*/
    sprintf(text_buf, "%d%d", watch_data[4], watch_data[5]);
    g_layer_sec_id= display_text_layer(p_window,&frame_sec,GAlignCenter,g_backgroundColor,text_buf,U_ASCII_ARIAL_42);

	/*创建动画图层*/
	g_layer_ani_id = display_target_layer(p_window,&bmp_ani_rect,GAlignLeft,g_backgroundColor,bmp_ani_frames,g_ani_index);

    /*定义窗口定时器，用于动作刷新*/
    app_window_timer_subscribe(p_window, 1000, ani_callback, NULL);

	return p_window;
}

//重新载入并刷新窗口所有图层
void window_reloading(void)
{
	/*根据窗口ID获取窗口句柄*/
	P_Window p_old_window = app_window_stack_get_window_by_id(g_window_id);

	if (NULL != p_old_window)
	{
	    ani_index_update();
		P_Window p_window = init_window();
		if (NULL != p_window)
		{
			g_window_id = app_window_stack_replace_window(p_old_window, p_window);
		}
	}

}

void watchapp_comm_callback(enum ESyncWatchApp type, uint8_t *context, uint16_t context_len)
{
    if(type == ESyncWatchAppUpdateParam)
    {
        if(context_len > 0&& context_len < sizeof(app_set_text))
        {
            memcpy(app_set_text,context,context_len);
            app_set_text[context_len] = '\0';

           
            app_persist_write_data_extend(APP_SET_PRESIST_DATA_KEY,app_set_text,sizeof(app_set_text));
        }
        else
        {
            memset(app_set_text,0,sizeof(app_set_text));
        }


        window_reloading(); 
    }
}

/*
 *--------------------------------------------------------------------------------------
 *     function:  app_watch_time_change
 *    parameter:
 *       return:
 *  description:  系统时间有变化时，更新时间图层
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
static void app_watch_time_change(enum SysEventType type, void *context)
{

	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		get_watch_data(0);
		window_reloading();
	}
}

int main()
{
//simulator_init();

    //创建配置存储
    app_persist_create(APP_SET_PRESIST_DATA_KEY, sizeof(app_set_text));

    get_watch_data(0);
	/*创建显示表盘窗口*/
	P_Window p_window = init_window();
	if (p_window != NULL)
	{
		/*放入窗口栈显示*/
		g_window_id = app_window_stack_push(p_window);

		/*注册一个事件通知回调，当有时间改变时，立即更新时间*/
        maibu_service_sys_event_subscribe(app_watch_time_change);

        //注册手机设置回调函数
        maibu_comm_register_watchapp_callback(watchapp_comm_callback);
	}
//simulator_wait();
	return 0;
}