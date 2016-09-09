/*
 *  应用名：股票助手
 *
 *  优化说明：(减小代码体积)
 *      (1) 部分sprintf改用memcpy替代
 *
 */
#ifdef QT_PLATFORM

#include "../res/app/app_res.h"
#include "../res/os/os_res.h"
#include "../os_kernel/os_printk.h"
#include "../os_kernel/os_timer.h"
#include "../maibu_sdk/maibu_sdk.h"
#include "../res/app/app_res.h"
#include "../os_kernel/os_printk.h"
#include "../utility/json_checker.h"
#include "../module/md_ble_comm.h"

#define  LOG_INFO  os_printk

#else

#include "maibu_sdk.h"
#include "maibu_res.h"

#define  LOG_INFO(...)

#endif

/* 列表框架 */
#define MENU_FRAME_POS_X                     0
#define MENU_FRAME_POS_Y                     32
#define MENU_FRAME_SIZE_W                    128
#define MENU_FRAME_SIZE_H                    96

/* 列表项框架 */
#define MENU_ITEM_POS_X                      0
#define MENU_ITEM_POS_Y                      32
#define MENU_ITEM_SIZE_W                     128
#define MENU_ITEM_SIZE_H                     24

/* 时间项 */
#define DATE_TEXT_POS_X                      2
#define DATE_TEXT_POS_Y                      6
#define DATE_TEXT_SIZE_W                     70
#define DATE_TEXT_SIZE_H                     14

#define TIME_TEXT_POS_X                      90
#define TIME_TEXT_POS_Y                      6
#define TIME_TEXT_SIZE_W                     36
#define TIME_TEXT_SIZE_H                     14

/* 股票项标注1: 价格字符>=7 */
#define OPT1_STOCK_NAME_POS_X                2
#define OPT1_STOCK_NAME_POS_Y                6
#define OPT1_STOCK_NAME_SIZE_W               28
#define OPT1_STOCK_NAME_SIZE_H               12

#define OPT1_STOCK_DATA_POS_X                32
#define OPT1_STOCK_DATA_POS_Y                6
#define OPT1_STOCK_DATA_SIZE_W               95
#define OPT1_STOCK_DATA_SIZE_H               12

/* 股票项标注2: 价格字符=6 */
#define OPT2_STOCK_NAME_POS_X                2
#define OPT2_STOCK_NAME_POS_Y                6
#define OPT2_STOCK_NAME_SIZE_W               42
#define OPT2_STOCK_NAME_SIZE_H               12

#define OPT2_STOCK_DATA_POS_X                46
#define OPT2_STOCK_DATA_POS_Y                6
#define OPT2_STOCK_DATA_SIZE_W               81
#define OPT2_STOCK_DATA_SIZE_H               12

/* 股票项标注3: 价格字符<=5 */
#define OPT3_STOCK_NAME_POS_X                2
#define OPT3_STOCK_NAME_POS_Y                6
#define OPT3_STOCK_NAME_SIZE_W               49
#define OPT3_STOCK_NAME_SIZE_H               12

#define OPT3_STOCK_DATA_POS_X                53
#define OPT3_STOCK_DATA_POS_Y                6
#define OPT3_STOCK_DATA_SIZE_W               74
#define OPT3_STOCK_DATA_SIZE_H               12

/* 提示添加股票栏 */
#define PROMPT_TEXT_POS_X                    0
#define PROMPT_TEXT_POS_Y                    6
#define PROMPT_TEXT_SIZE_W                   128
#define PROMPT_TEXT_SIZE_H                   12

#define WEB_STOCK_DATA_LINK                  "http://web.juhe.cn:8080/finance/stock/hs?gid=%s&key=这里替换成你申请的Key"

#define MAX_STOCK_ITEM_NUM                   4

#define STOCK_FILE_KEY                       1

typedef enum{
    DispNone = 0,     
    DispShortData = 0xA,
    DispMiddleData,
    DispLongData
}EDispTextType;


typedef struct{
    char              id[9];
    char              name[20];
    char              data[20];
    EDispTextType     disp_type;
}SStockData;

typedef struct{
    bool           exist;
    char           id[MAX_STOCK_ITEM_NUM][9];
    int            stock_type[MAX_STOCK_ITEM_NUM];
}SStockCfgInfo;

static const char weekday[7][11] =
{
    {"周日"},
    {"周一"},
    {"周二"},
    {"周三"},
    {"周四"},
    {"周五"},
    {"周六"}
};

static int32_t g_window_id       = -1;
static int32_t g_layer_id_date   = -1;
static int32_t g_layer_id_time   = -1;
static int32_t g_layer_id_stock_name[MAX_STOCK_ITEM_NUM] = {-1, -1, -1, -1};
static int32_t g_layer_id_stock_data[MAX_STOCK_ITEM_NUM] = {-1, -1, -1, -1};

static uint32_t g_comm_id_web = 0xffffffff;

static SStockData g_stock_item[MAX_STOCK_ITEM_NUM];
static SStockCfgInfo g_stock_cfg_info;;
static uint32_t g_stock_update_index = 0;

static int cache_data_available = false;

static const char default_stock_id[3][9] = {"sh000001", "sz399001", "sz399006"};

static void refresh_stock_data(P_Window p_window, int update);
static void web_request_stock_data(char *str_stock_id);


static char* my_strcat ( char * dst , const char * src )
{
    char * cp = dst;
    while( *cp )
        cp++;                        /*find end of dst */
    while( *cp++ = *src++ ) ;       /* Copy src to end of dst */
    return( dst );                  /* return dst */
}

void add_text_layer(P_Window p_window, int32_t *p_layer_id, char *p_str, GRect *p_frame, enum GAlign align, int8_t font, enum GColor color)
{
    LayerText text_cfg = {p_str, *p_frame, align, font, 0};
    P_Layer layer = app_layer_create_text(&text_cfg);
    app_layer_set_bg_color(layer, color);

    P_Layer old_layer = app_window_get_layer_by_id(p_window, *p_layer_id);
    if(old_layer)
    {
        *p_layer_id = app_window_replace_layer(p_window, old_layer, layer);
    }
    else
    {
        *p_layer_id = app_window_add_layer(p_window, layer);
    }
}

static void save_cfg_info(void)
{
    int i;
    g_stock_cfg_info.exist = true;
    for(i=0; i<MAX_STOCK_ITEM_NUM; i++)
    {
        sprintf(&g_stock_cfg_info.id[i][0], "%s", g_stock_item[i].id);
    }

    app_persist_write_data_extend(STOCK_FILE_KEY, &g_stock_cfg_info, sizeof(SStockCfgInfo));
}

void arrange_stock_data(char *p_gid, char *p_stock_name, char *p_stock_now_pri, char *p_stock_incre_per)
{
    //查找对应股票代码
    SStockData *p_stock_data = NULL;
    int i, index = -1;
    for(i=0; i< MAX_STOCK_ITEM_NUM; i++)
    {
        if( strcmp(p_gid, g_stock_item[i].id) == 0 )
        {
            p_stock_data = &g_stock_item[i];
            if(p_stock_data == NULL)
            {
                break;
            }

            char stock_data[20]; //NOTE:为节约代码，不初始化

            sprintf(stock_data, "%s", p_stock_now_pri);

            //产品需求：根据价格字符数确定要展示的对应文本框架类型
            int now_pri_str_len = strlen(p_stock_now_pri);
            if(now_pri_str_len >= 7)
            {
                p_stock_data->disp_type = DispShortData;
            }
            else if(now_pri_str_len == 6)
            {
                p_stock_data->disp_type = DispMiddleData;
            }
            else
            {
                p_stock_data->disp_type = DispLongData;
            }

            //LOG_INFO("Type: %d", p_stock_data->disp_type);

            int is_negative = 0;
            if(p_stock_incre_per[0] == '-')
            {
                my_strcat(stock_data, " "); //产品需求：由于'-'比'+'窄，补空格
                is_negative = 1;
            }
            else
            {
                my_strcat(stock_data, "+");
            }

            if(strlen(p_stock_incre_per) >= 5+is_negative) //产品需求：价格显示不超过4个字符
            {
                p_stock_incre_per[4+is_negative] = '\0';
            }

            my_strcat(stock_data, p_stock_incre_per);
            my_strcat(stock_data, "%");

            sprintf(p_stock_data->name, "%s", p_stock_name);
            sprintf(p_stock_data->data, "%s", stock_data);
        }
    }

    //更新股票窗口信息
    P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
    if (p_window == NULL)
    {
        return;
    }

    refresh_stock_data(p_window, true);
}

void watchapp_comm_callback(enum ESyncWatchApp type, uint8_t *buf, uint16_t len)
{
    //6位股票代码
    if(len < 6)
    {
        return;
    }

    int i, j;
    int stock_num = 0;

    if(type == ESyncWatchAppUpdateParam)
    {
        for(i=0; i<len; )
        {
            if( (buf[i]<'0') || (buf[i]>'9') ) //检查符号位
            {
                i++;
                continue;
            }
            else if(i+6 <= len)
            {
                memset(&g_stock_item[stock_num], 0, sizeof(SStockData));

                if( (buf[i] == '5') || (buf[i] == '6') || (buf[i] == '7') || (buf[i] == '9') )
                {
                    my_strcat(g_stock_item[stock_num].id, "sh");
                }
                else
                {
                    my_strcat(g_stock_item[stock_num].id, "sz");
                }

                memcpy(&g_stock_item[stock_num].id[2], &buf[i], 6);
                g_stock_item[stock_num].disp_type = DispLongData;

                stock_num++;
                i += 6;
            }
            else
            {
                break;
            }
        }

        LOG_INFO("stock num = %d", stock_num);
        for(i=stock_num, j=0; i<MAX_STOCK_ITEM_NUM; i++, j++)
        {
            //g_stock_item[i] = g_default_stock_item[j];
            memset(&g_stock_item[i], 0, sizeof(SStockData));
            memcpy(g_stock_item[i].id, &default_stock_id[j][0], 9);
            g_stock_item[i].disp_type = DispLongData;
        }

        //保存配置信息并更新窗口
        save_cfg_info();

        P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
        if (p_window == NULL)
        {
            return;
        }

        //更新为默认界面
        refresh_stock_data(p_window, true);

        //立刻请求一次数据
        g_stock_update_index = 0;
        web_request_stock_data(g_stock_item[g_stock_update_index].id);
    }
}

static void web_request_stock_data(char *str_stock_id)
{
    if(str_stock_id[0] != 's')
    {
        //LOG_INFO("Stock NOT exist.");
        return ;
    }

    /*拼接url请求地址, 注意url的缓存大小*/
    char url[200];
    sprintf(url, WEB_STOCK_DATA_LINK, str_stock_id);

    /*拼接过滤参数，即只接受和过滤参数匹配的返回值*/
    //LOG_INFO("Url: %s", url);

    g_comm_id_web = maibu_comm_request_web(url, "gid,name,nowPri,increPer",  0);
}

static void shorten_price(char *price)
{
    while(*price)
    {
        if(*price == '.')
        {
            *(price+3) = '\0';
            return;
        }
        price++;
    }
}

static void web_recv_callback(const uint8_t *buff, uint16_t size)
{
    if(g_stock_update_index == 4)
    {
        return;
    }
    else
    {
        g_stock_update_index++;
        web_request_stock_data(g_stock_item[g_stock_update_index].id);
    }

    //更新对应的数据  //注：为优化代码这里不初始化，节约200字节
    char stock_gid[10] ;
    char stock_name[20] ;
    char stock_now_pri[20];
    char stock_incre_per[10] ;

    maibu_get_json_str(buff, "gid", stock_gid, 10);
    maibu_get_json_str(buff, "name", stock_name, 20);
    maibu_get_json_str(buff, "nowPri", stock_now_pri, 20);
    maibu_get_json_str(buff, "increPer", stock_incre_per, 10);

    shorten_price(stock_now_pri);
//    LOG_INFO("Stock ID: %s", stock_gid);
//    LOG_INFO("Stock Name: i %d %s", index, stock_name);
//    LOG_INFO("Stock Now Pri: %s", stock_now_pri);
//    LOG_INFO("Incre Per: %s", stock_incre_per);

    arrange_stock_data(stock_gid,  stock_name, stock_now_pri, stock_incre_per);

    //LOG_INFO("web_recv_callback disp index: %d disp_len: %d", index, g_stock_item[index].disp_len );
}

static void stock_comm_result_callback(enum ECommResult result, uint32_t comm_id, void *context)
{
    /*如果上一次请求WEB通讯失败，并且通讯ID相同，则重新发送*/
    if ((result == ECommResultFail) && (comm_id == g_comm_id_web))
    {
        //os_printk("stock data request failed!!!\r\n");
        web_request_stock_data(g_stock_item[g_stock_update_index].id);
    }
}

static void stock_timer_callback(date_time_t tick_time, uint32_t millis, void *context)
{
    struct date_time t;
    app_service_get_datetime(&t);

    //周末不获取数据
    if( (t.wday==6) || (t.wday==0) )
    {
        return;
    }

    //上午时间段9:10~11:35, 转成分钟范围560~695；下午时间段12:55~15:05,转成分钟范围775~905
    uint32_t min_time = t.hour*60 + t.min;
    // if( ((min_time>=550) && (min_time<=695)) || ((min_time>=775) && (min_time<=905)) )节约24字节
    if( (min_time>=550) &&  (min_time<=905) )
    {
        //每隔1分钟请求一轮数据(全部股票)
		g_stock_update_index = 0;
        web_request_stock_data(g_stock_item[0].id);
    }
}

static void watch_time_change_callback(enum SysEventType type, void *context)
{
    P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
    if (p_window == NULL)
    {
       return;
    }

    /*时间更改，分变化*/
    if (type == SysEventTypeTimeChange)
    {
        uint8_t text_buf[40];
        struct date_time t;
        app_service_get_datetime(&t);

        memset(text_buf, 0, sizeof(text_buf));
        sprintf((char *)text_buf, "%s", (char *)&weekday[t.wday][0]);
        sprintf(&text_buf[6], "%02d-%02d", t.mon, t.mday);

        GRect frame;
        frame.origin.x = DATE_TEXT_POS_X;
        frame.origin.y = DATE_TEXT_POS_Y;
        frame.size.h   = DATE_TEXT_SIZE_H;
        frame.size.w   = DATE_TEXT_SIZE_W;

        add_text_layer(p_window, &g_layer_id_date, (char*)text_buf, &frame, GAlignLeft, U_ASCII_ARIAL_14, GColorWhite);

        frame.origin.x = TIME_TEXT_POS_X;
        frame.origin.y = TIME_TEXT_POS_Y;
        frame.size.h   = TIME_TEXT_SIZE_H;
        frame.size.w   = TIME_TEXT_SIZE_W;

        memset(text_buf, 0, sizeof(text_buf));
        sprintf(text_buf, "%02d:%02d", t.hour, t.min);

        add_text_layer(p_window, &g_layer_id_time, (char*)text_buf, &frame, GAlignLeft, U_ASCII_ARIAL_14, GColorWhite);

        app_window_update(p_window);
    }
}

static void add_time_bar(P_Window p_window)
{
    /* 添加时间图层 */
    uint8_t text_buf[40];
    struct date_time t;
    app_service_get_datetime(&t);

    memset(text_buf, 0, sizeof(text_buf));
    sprintf((char *)text_buf, "%s", (char *)&weekday[t.wday][0]);
    sprintf(&text_buf[6], "%02d-%02d", t.mon, t.mday);

    GRect frame;
    frame.origin.x = DATE_TEXT_POS_X;
    frame.origin.y = DATE_TEXT_POS_Y;
    frame.size.h   = DATE_TEXT_SIZE_H;
    frame.size.w   = DATE_TEXT_SIZE_W;

    add_text_layer(p_window, &g_layer_id_date, (char*)text_buf, &frame, GAlignLeft, U_ASCII_ARIAL_14, GColorWhite);

    frame.origin.x = TIME_TEXT_POS_X;
    frame.origin.y = TIME_TEXT_POS_Y;
    frame.size.h   = TIME_TEXT_SIZE_H;
    frame.size.w   = TIME_TEXT_SIZE_W;

    memset(text_buf, 0, sizeof(text_buf));
    sprintf(text_buf, "%02d:%02d", t.hour, t.min);

    add_text_layer(p_window, &g_layer_id_time, (char*)text_buf, &frame, GAlignLeft, U_ASCII_ARIAL_14, GColorWhite);
}



/* 更新股票信息 */
void refresh_stock_data(P_Window p_window, int update)
{
    int i;
    GRect text1_frame, text2_frame;
    int is_prompt_text = false;

    for(i=0; i<MAX_STOCK_ITEM_NUM; i++)
    {
        switch(g_stock_item[i].disp_type)
        {
            case DispShortData:
                text1_frame.origin.x = OPT1_STOCK_NAME_POS_X + MENU_ITEM_POS_X;
                text1_frame.origin.y = OPT1_STOCK_NAME_POS_Y + MENU_ITEM_POS_Y;
                text1_frame.size.h   = OPT1_STOCK_NAME_SIZE_H;
                text1_frame.size.w   = OPT1_STOCK_NAME_SIZE_W;
                text2_frame.origin.x = OPT1_STOCK_DATA_POS_X + MENU_ITEM_POS_X;
                text2_frame.origin.y = OPT1_STOCK_DATA_POS_Y + MENU_ITEM_POS_Y;
                text2_frame.size.h   = OPT1_STOCK_DATA_SIZE_H;
                text2_frame.size.w   = OPT1_STOCK_DATA_SIZE_W;
                break;
            case DispMiddleData:
                text1_frame.origin.x = OPT2_STOCK_NAME_POS_X + MENU_ITEM_POS_X;
                text1_frame.origin.y = OPT2_STOCK_NAME_POS_Y + MENU_ITEM_POS_Y;
                text1_frame.size.h   = OPT2_STOCK_NAME_SIZE_H;
                text1_frame.size.w   = OPT2_STOCK_NAME_SIZE_W;
                text2_frame.origin.x = OPT2_STOCK_DATA_POS_X + MENU_ITEM_POS_X;
                text2_frame.origin.y = OPT2_STOCK_DATA_POS_Y + MENU_ITEM_POS_Y;
                text2_frame.size.h   = OPT2_STOCK_DATA_SIZE_H;
                text2_frame.size.w   = OPT2_STOCK_DATA_SIZE_W;
                break;
            case DispLongData:
                text1_frame.origin.x = OPT3_STOCK_NAME_POS_X + MENU_ITEM_POS_X;
                text1_frame.origin.y = OPT3_STOCK_NAME_POS_Y + MENU_ITEM_POS_Y;
                text1_frame.size.h   = OPT3_STOCK_NAME_SIZE_H;
                text1_frame.size.w   = OPT3_STOCK_NAME_SIZE_W;
                text2_frame.origin.x = OPT3_STOCK_DATA_POS_X + MENU_ITEM_POS_X;
                text2_frame.origin.y = OPT3_STOCK_DATA_POS_Y + MENU_ITEM_POS_Y;
                text2_frame.size.h   = OPT3_STOCK_DATA_SIZE_H;
                text2_frame.size.w   = OPT3_STOCK_DATA_SIZE_W;
                break;

            default:
                //第一个空行展示提示添加股票文本
                break;
        }

		text1_frame.origin.y += i * MENU_ITEM_SIZE_H;
		text2_frame.origin.y += i * MENU_ITEM_SIZE_H;

		add_text_layer(p_window, &g_layer_id_stock_name[i], g_stock_item[i].name, &text1_frame, GAlignLeft, U_ASCII_ARIAL_12, GColorBlack);
		add_text_layer(p_window, &g_layer_id_stock_data[i], g_stock_item[i].data, &text2_frame, GAlignRight, U_ASCII_ARIAL_12, GColorBlack);
    }

    if(update)
    {
        app_window_update(p_window);
    }

}

/* 初始化股票应用串口 */
static void init_start_window(P_Window p_window)
{
    /* 添加表盘背景 */
    GRect frame = {{0, 0}, {128, 128}};
    GBitmap bitmap;

    res_get_user_bitmap(BMP_STOCK_BG, &bitmap);
    LayerBitmap layer_bitmap = {bitmap, frame, GAlignCenter};
    P_Layer layer = app_layer_create_bitmap(&layer_bitmap);
    app_window_add_layer(p_window, layer);

    /* 添加时间栏 */
    add_time_bar(p_window);

    /* 添加股票项目 */
    refresh_stock_data(p_window, false);

    /*把窗口放入窗口栈中显示*/
    g_window_id = app_window_stack_push(p_window);
}

void stock_app_init(void)
{
    int i;

    if(cache_data_available == false)
    {
        memset(g_stock_item, 0, sizeof(g_stock_item));
        memset(&g_stock_cfg_info, 0, sizeof(g_stock_cfg_info));

        //创建保存股票信息的结构体
        app_persist_create(STOCK_FILE_KEY, 4096);
        app_persist_read_data(STOCK_FILE_KEY, 0, &g_stock_cfg_info, sizeof(g_stock_cfg_info));
        if( (g_stock_cfg_info.exist == true) && (g_stock_cfg_info.id[0][0] == 's') )
        {

            //还原保存的股票
            for(i=0; i<MAX_STOCK_ITEM_NUM; i++)
            {
                memcpy(g_stock_item[i].id,   &g_stock_cfg_info.id[i][0], 9);
                g_stock_item[i].disp_type = DispLongData;
            }
        }
        else
        {
            //使用默认三支大盘股票
            //memcpy(g_stock_item, g_default_stock_item, sizeof(g_default_stock_item));
            for(i=0; i<3; i++)
            {
                memcpy(g_stock_item[i].id, &default_stock_id[i][0], 9);
                g_stock_item[i].disp_type = DispLongData;
            }
        }

        cache_data_available = true;
    }

    P_Window p_window = NULL;
    p_window = app_window_create();
    if (NULL == p_window)
    {
        return;
    }

    //初始化窗口
    init_start_window(p_window);

    //订阅时间改变事件
    maibu_service_sys_event_subscribe(watch_time_change_callback);

    //注册网络请求回调函数
    maibu_comm_register_web_callback(web_recv_callback);

    //注册请求手表app配置数据回调
    maibu_comm_register_watchapp_callback(watchapp_comm_callback);

    //注册通讯结果回调
    maibu_comm_register_result_callback(stock_comm_result_callback);

    //聚合数据源每隔2分钟更新数据
    app_window_timer_subscribe(p_window, 60000, stock_timer_callback, (void *)p_window);

    //立刻请求一次数据
    g_stock_update_index = 0;
    web_request_stock_data(g_stock_item[g_stock_update_index].id); 

#ifdef QT_PLATFORM
    //watchapp_comm_callback(ESyncWatchAppUpdateParam, "-sz399001-sh000001+sz000001+sz000001+sz000001-sz000001",54);
    //watchapp_comm_callback(ESyncWatchAppUpdateParam, "600130 601398",14);
#endif
}

#ifndef QT_PLATFORM

int main(void)
{
    stock_app_init();
}

#endif

