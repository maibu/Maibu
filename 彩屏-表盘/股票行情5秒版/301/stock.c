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
#define MENU_FRAME_POS_Y                     44
#define MENU_FRAME_SIZE_W                    176
#define MENU_FRAME_SIZE_H                    132

/* 列表项框架 */
#define MENU_ITEM_POS_X                      0
#define MENU_ITEM_POS_Y                      44
#define MENU_ITEM_SIZE_W                     176
#define MENU_ITEM_SIZE_H                     32

/* 时间项 */
#define DATE_TEXT_POS_X                      3
#define DATE_TEXT_POS_Y                      8
#define DATE_TEXT_SIZE_W                     100
#define DATE_TEXT_SIZE_H                     20

#define TIME_TEXT_POS_X                      120
#define TIME_TEXT_POS_Y                      8
#define TIME_TEXT_SIZE_W                     54
#define TIME_TEXT_SIZE_H                     20

/* 股票项标注1: 价格字符>=7 */
#define OPT1_STOCK_NAME_POS_X                3
#define OPT1_STOCK_NAME_POS_Y                9
#define OPT1_STOCK_NAME_SIZE_W               39
#define OPT1_STOCK_NAME_SIZE_H               16

#define OPT1_STOCK_DATA_POS_X                44
#define OPT1_STOCK_DATA_POS_Y                9
#define OPT1_STOCK_DATA_SIZE_W               131
#define OPT1_STOCK_DATA_SIZE_H               16

/* 股票项标注2: 价格字符=6 */
#define OPT2_STOCK_NAME_POS_X                3
#define OPT2_STOCK_NAME_POS_Y                9
#define OPT2_STOCK_NAME_SIZE_W               58
#define OPT2_STOCK_NAME_SIZE_H               16

#define OPT2_STOCK_DATA_POS_X                63
#define OPT2_STOCK_DATA_POS_Y                9
#define OPT2_STOCK_DATA_SIZE_W               112
#define OPT2_STOCK_DATA_SIZE_H               16

/* 股票项标注3: 价格字符<=5 */
#define OPT3_STOCK_NAME_POS_X                3
#define OPT3_STOCK_NAME_POS_Y                9
#define OPT3_STOCK_NAME_SIZE_W               68
#define OPT3_STOCK_NAME_SIZE_H               16

#define OPT3_STOCK_DATA_POS_X                73
#define OPT3_STOCK_DATA_POS_Y                9
#define OPT3_STOCK_DATA_SIZE_W               102
#define OPT3_STOCK_DATA_SIZE_H               16

/* 提示添加股票栏 */
#define PROMPT_TEXT_POS_X                    0
#define PROMPT_TEXT_POS_Y                    9
#define PROMPT_TEXT_SIZE_W                   176
#define PROMPT_TEXT_SIZE_H                   16

#define WEB_STOCK_DATA_LINK                  "http://tianxhq.cindasc.com:9611/market/json?funcno=20000&version=1&field=23:24:22:2:1&stock_list=%s"

#define MAX_STOCK_ITEM_NUM                   4

#define STOCK_FILE_KEY                       1

typedef enum{
    DispNone = 0,     
    DispShortData = 0xA,
    DispMiddleData,
    DispLongData
}EDispTextType;


typedef struct{
    char              id[12];
    char              name[20];
    char              data[20];
    EDispTextType     disp_type;
	char 			  alarm_per[8];
	char 			  flag;	//1，red , 2, green 3, white
}SStockData;

typedef struct{
    bool           exist;
    char           id[MAX_STOCK_ITEM_NUM][12];
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
static SStockCfgInfo g_stock_cfg_info;
static int8_t  g_stock_is_alarmed_flag[MAX_STOCK_ITEM_NUM] = {0};


static uint32_t g_stock_update_index = 0;
static uint32_t g_stock_update_index_start = 2;
static uint32_t g_stock_update_count = 0;

static int cache_data_available = false;

static const char default_stock_id[3][12] = {"SH:000001", "SZ:399001", "SZ:399006"};

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


void add_text_layer(P_Window p_window, int32_t *p_layer_id, char *p_str, GRect *p_frame, enum GAlign align, int8_t font, enum GColor color, enum GColor fcolor)
{
    LayerText text_cfg = {p_str, *p_frame, align, fcolor, font, 0};
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

void alarm_notify(const char is_zhang,const char* stock_name,const char *per_str)
{	
	//os_printk("stock_name:%s is_zhang:%d per_str:%s\n",stock_name,is_zhang,per_str);
	
	GBitmap bitmap;
	res_get_user_bitmap(NOTICE_BITMAP, &bitmap);

	NotifyParam notic_ad = {	\
		.bmp = bitmap,	\
		.main_title = {0},	\
		.sub_title = {0},	\
		.pulse_type = VibesPulseTypeShort,		\
		.pulse_time = 3,		\
		.context = NULL,	\
	};
	
	if(is_zhang == 1)
	{
		memcpy(notic_ad.main_title,"上涨提醒",strlen("上涨提醒"));
	}
	else if(is_zhang == 0)
	{
		memcpy(notic_ad.main_title,"下跌提醒",strlen("下跌提醒"));
	}

	sprintf(notic_ad.sub_title,"%s %s%%",stock_name,per_str);

	maibu_service_sys_notify(&notic_ad);

}

int my_strchr(const char * Str, int Val)
{
    const char * p = Str;
	//如果*p==_Val或者*p为'\0',退出循环
	while(*p!=Val && *p)
		p++;
	return p-Str;
}

	
void is_alarm_incre_per(const char* alarm_line,const char * now_per,const char* stock_name,char index)
{
	char temp_now_per[16] = {0};
	char is_zhang = 1;
	//os_printk("stock_name:%s alarm_line:%s now_per:%s\n",stock_name,alarm_line,now_per);
	
	if(strlen(alarm_line) == 0)
	{
		return;
	}
	
	if(now_per[0]=='-')
	{
		memcpy(temp_now_per,&now_per[1],strlen(now_per-1));
		is_zhang = 0;
	}
	else
	{
		memcpy(temp_now_per,now_per,strlen(now_per));
	}
	
	//os_printk("line:%d now:%d\n",atoi(alarm_line),atoi(temp_now_per));
	if(atoi(alarm_line) > atoi(temp_now_per))
	{
		return;
	}
	else if(atoi(alarm_line) == atoi(temp_now_per))
	{
		char num1 = my_strchr(alarm_line,'.');
		char num2 = my_strchr(temp_now_per,'.');
		//os_printk("alarm_line[%d]:%d temp_now_per[%d]:%d\n",num1+1,atoi(&alarm_line[num1+1]),num2+1,atoi(&temp_now_per[num2+1]));
		if(atoi(&alarm_line[num1+1])>atoi(&temp_now_per[num2+1]))
		{
			return;
		}
	}
	else 
	{

	}


	g_stock_is_alarmed_flag[index] = 1;

	//提醒函数		
	alarm_notify(is_zhang,stock_name,now_per);	
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
				p_stock_data->flag = 2;   //绿色
            }
            else if(p_stock_incre_per[0] == '0' && p_stock_incre_per[2] == '0' && p_stock_incre_per[3] == '0')
			{
				my_strcat(stock_data, "  ");  //补两个空格
				p_stock_data->flag = 3; //白色
			}
			else
            {
                my_strcat(stock_data, "+");
				p_stock_data->flag = 1;	 //红色
            }

            if(strlen(p_stock_incre_per) >= 5+is_negative) //产品需求：价格显示不超过4个字符
            {
                p_stock_incre_per[4+is_negative] = '\0';
            }

            my_strcat(stock_data, p_stock_incre_per);
            my_strcat(stock_data, "%");
		
            sprintf(p_stock_data->name, "%s", p_stock_name);
            sprintf(p_stock_data->data, "%s", stock_data);

			//os_printk("\ni:%d Incre Per: %s\n", i,p_stock_incre_per);
			if(g_stock_is_alarmed_flag[i] == 0)
			{
				is_alarm_incre_per(p_stock_data->alarm_per,p_stock_incre_per,p_stock_name,i);
			}
			
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
                    my_strcat(g_stock_item[stock_num].id, "SH:");
                }
                else
                {
                    my_strcat(g_stock_item[stock_num].id, "SZ:");
                }				
				
                memcpy(&g_stock_item[stock_num].id[3], &buf[i], 6);
                g_stock_item[stock_num].disp_type = DispLongData;

                
                i += 6;
				if(buf[i] == ' ')
				{
					int k = 1;
					while(i+k+1<=len)
					{							
						if(buf[i+k] == '%')
						{
							memcpy(&g_stock_item[stock_num].alarm_per, &buf[i+1], k-1);
							i = i + k;
							break;
						}
						k++;
						//os_printk("i: %d + %d = %d\n",i,k,i+k);

						if(((buf[i+k] == '\r')||(buf[i+k] == '\n'))&&(i+k<=len))
						{
							i = i + k;
							break;
						}
						//os_printk("i: %d + %d = %d\n",i,k,i+k);
					}
					
					
					
				}
				//os_printk("id:%s alarm_per:%s\n",g_stock_item[stock_num].id,g_stock_item[stock_num].alarm_per);
				stock_num++;
            }
            else
            {
                break;
            }
        }

        //os_printk("stock num = %d", stock_num);
        for(i=stock_num, j=0; i<MAX_STOCK_ITEM_NUM; i++, j++)
        {
            //g_stock_item[i] = g_default_stock_item[j];
            memset(&g_stock_item[i], 0, sizeof(SStockData));
            memcpy(g_stock_item[i].id, &default_stock_id[j][0], 12);
            g_stock_item[i].disp_type = DispLongData;
        }

		memset(g_stock_is_alarmed_flag,0,sizeof(g_stock_is_alarmed_flag));
		
        //保存配置信息并更新窗口
        save_cfg_info();
#if 1
		//增加提醒功能后，后台要更新
        P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
        if (p_window != NULL)
        {
            //更新为默认界面
        	refresh_stock_data(p_window, true);
        }
        
#endif

        //立刻请求一次数据
        g_stock_update_count = 0;
        g_stock_update_index = 0;
        web_request_stock_data(g_stock_item[g_stock_update_index].id);
    }
}

static void web_request_stock_data(char *str_stock_id)
{
	
    if(str_stock_id[0] != 'S')
   {
	   if(g_stock_update_count < 4)
	   {
	       g_stock_update_index = (g_stock_update_index + 1)%4;
	       web_request_stock_data(g_stock_item[g_stock_update_index].id);
	   }
       //LOG_INFO("Stock NOT exist.");
       return ;
   }
	
    /*拼接url请求地址, 注意url的缓存大小*/
    char url[200];
    sprintf(url, WEB_STOCK_DATA_LINK, str_stock_id);

	
    /*拼接过滤参数，即只接受和过滤参数匹配的返回值*/
    //os_printk("\nUrl: %s\n", url);

    g_comm_id_web = maibu_comm_request_web(url,"results", 0);
}

static void shorten_price(char *price)
{
    while(*price)
    {
        if(*price == '.')
        {
            *(price+3) = '\0';
			if(*(price+2) == '\0')
			{
				*(price+2) = '0';
			}
			
            return;
        }
        price++;
    }
}

static void shorten_incre_per(char *incre_per)
{
	char *pp = incre_per;
	char *qq; 
	char cmp_array[4] ="0.0";
	
	if(memcmp(incre_per,cmp_array,4) == 0)
	{
		*(incre_per+3) = '0';
		*(incre_per+4) = '\0';
		return;
	}

	
	pp = incre_per;
	while(*pp++ != '.');
	*(pp-1) = *pp;
	*pp = *(pp+1);
	*(pp+1) = '.';

	pp = incre_per;
	qq = pp;
	

	while(*(pp+1) != '.')
	{
		if(*pp == '0')
		{
			pp++;
			continue;
		}
		*qq = *pp;
		qq++;
		pp++;
	}
	
	while(*pp != NULL)
	{
		if(*pp == 'E')
		{
			
			if(*(pp+2) == '5')
			{
				pp = incre_per;
				memcpy(pp,"0.00",4);
				*(pp + 4) = '\0';
				return;
			}
			if(*(pp+2) == '4')
			{
				char temp_char;
				pp = incre_per;
				if(*pp == '-')
				{
					pp++;
				}
				temp_char = *pp;
				memcpy(pp,"0.0",3);
				
				*(pp + 3) = temp_char;
				*(pp + 4) = '\0';
				return;
			}
		}
		
		*qq = *pp;
		qq++;
		pp++;
	}
	

}

uint8_t get_data_from_web_back(const char *src,char *dst)
{
	uint8_t count = 0;
	char *end = src;
	char *start;
			
	while((*end != ',')&&(*end != '['))
	{
		end++;
	}
	
	end++;
	start = end;
	
	while((*end != ',')&&(*end != ']'))
	{
		end++;
	}

	count = end - src;
		
	while(start < end)
	{
		if(*start == '\"')
		{
			start++;
			continue;
		}
		*dst = *start;
		dst++;
		start++;
	}
	*dst = '\0';
	return count;

}

static void web_recv_callback(const uint8_t *buff, uint16_t size)
{
	
	//os_printk("web_recv_callback_buff: %s\n", buff);
   	if(g_stock_update_count > 4)
    {
    	return;
    }
    else if(g_stock_update_count < 4)
    {
		g_stock_update_count++;
        g_stock_update_index = (g_stock_update_index + 1)%4;
        web_request_stock_data(g_stock_item[g_stock_update_index].id);
    }

	char temp_buff[72] = "";
	maibu_get_json_str(buff, "results", temp_buff, 72);
	char *p = temp_buff;
	char *q = p;

	while(*p != NULL)
	{
		if(*p == '\\')
			p++;	

		*q = *p;
		p++;
		q++;
	}
	
	if(strlen(temp_buff) <= 30)
	{
		return;
	}
	

	//更新对应的数据  //注：为优化代码这里不初始化，节约200字节
	char stock_gid[10];
	char stock_name[20];
	char stock_now_pri[20];
  	char stock_incre_per[16];
	uint8_t ret_num;

	uint8_t num = 1;
	
	ret_num = get_data_from_web_back(&temp_buff[num],stock_gid);
	
	stock_gid[2] = ':';

	num = ret_num + num;
	ret_num = get_data_from_web_back(&temp_buff[num],&stock_gid[3]);
	

	num = ret_num + num;
	ret_num = get_data_from_web_back(&temp_buff[num],stock_name);
	

	num = ret_num + num;
	ret_num = get_data_from_web_back(&temp_buff[num],stock_now_pri);
		

	num = ret_num + num;
	ret_num = get_data_from_web_back(&temp_buff[num],stock_incre_per);

	
    shorten_price(stock_now_pri);
	
	shorten_incre_per(stock_incre_per);
	
    //os_printk("Stock ID: %s\n", stock_gid);
    //os_printk("Stock Name: i %d %s\n", g_stock_update_count-1, stock_name);
    //os_printk("Stock Now Pri: %s\n", stock_now_pri);
    //os_printk("Incre Per: %s\n", stock_incre_per);

	
	

    arrange_stock_data(stock_gid,  stock_name, stock_now_pri, stock_incre_per);

   
    
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

	if(min_time <= 550)
	{
		memset(g_stock_is_alarmed_flag,0,sizeof(g_stock_is_alarmed_flag));
	}

	//后台不刷新,增加提醒功能后，后台要刷新
#if 0
	//增加提醒功能后，后台要更新
			P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
			if (p_window == NULL)
			{
				return;
			}
#endif

	
    if( ((min_time>=550) && (min_time<=695)) || ((min_time>=775) && (min_time<=905)) )//节约24字节
    //if( (min_time>=550) &&  (min_time<=905) )
    {
        //每隔1分钟请求一轮数据(全部股票)
        g_stock_update_count = 0;
		g_stock_update_index_start = (g_stock_update_index_start + 1)%4;
		g_stock_update_index = g_stock_update_index_start;
		
		web_request_stock_data(g_stock_item[g_stock_update_index].id);
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

        add_text_layer(p_window, &g_layer_id_date, (char*)text_buf, &frame, GAlignLeft, U_ASCII_ARIAL_20, GColorWhite, GColorBlack);

        frame.origin.x = TIME_TEXT_POS_X;
        frame.origin.y = TIME_TEXT_POS_Y;
        frame.size.h   = TIME_TEXT_SIZE_H;
        frame.size.w   = TIME_TEXT_SIZE_W;

        memset(text_buf, 0, sizeof(text_buf));
        sprintf(text_buf, "%02d:%02d", t.hour, t.min);

        add_text_layer(p_window, &g_layer_id_time, (char*)text_buf, &frame, GAlignLeft, U_ASCII_ARIAL_20, GColorWhite, GColorBlack);

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

    add_text_layer(p_window, &g_layer_id_date, (char*)text_buf, &frame, GAlignLeft, U_ASCII_ARIAL_20, GColorWhite, GColorBlack);

    frame.origin.x = TIME_TEXT_POS_X;
    frame.origin.y = TIME_TEXT_POS_Y;
    frame.size.h   = TIME_TEXT_SIZE_H;
    frame.size.w   = TIME_TEXT_SIZE_W;

    memset(text_buf, 0, sizeof(text_buf));
    sprintf(text_buf, "%02d:%02d", t.hour, t.min);

    add_text_layer(p_window, &g_layer_id_time, (char*)text_buf, &frame, GAlignLeft, U_ASCII_ARIAL_20, GColorWhite, GColorBlack);
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

		add_text_layer(p_window, &g_layer_id_stock_name[i], g_stock_item[i].name, &text1_frame, GAlignLeft, U_ASCII_ARIAL_16, GColorBlack, GColorWhite);
		
		if (g_stock_item[i].flag == 1)
			add_text_layer(p_window, &g_layer_id_stock_data[i], g_stock_item[i].data, &text2_frame, GAlignRight, U_ASCII_ARIALBD_16, GColorBlack, GColorRed);
		else if (g_stock_item[i].flag == 2)
			add_text_layer(p_window, &g_layer_id_stock_data[i], g_stock_item[i].data, &text2_frame, GAlignRight, U_ASCII_ARIALBD_16, GColorBlack, GColorGreen);
		else
			add_text_layer(p_window, &g_layer_id_stock_data[i], g_stock_item[i].data, &text2_frame, GAlignRight, U_ASCII_ARIALBD_16, GColorBlack, GColorWhite);
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
    GRect frame = {{0, 0}, {176, 176}};
    GBitmap bitmap;

    res_get_user_bitmap(BMP_STOCK_BG, &bitmap);
    LayerBitmap layer_bitmap = {bitmap, frame, GAlignCenter};
    P_Layer layer = app_layer_create_bitmap(&layer_bitmap);
	app_layer_set_bg_color(layer, GColorBlack);
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
        if( (g_stock_cfg_info.exist == true) && (g_stock_cfg_info.id[0][0] == 'S') )
        {

            //还原保存的股票
            for(i=0; i<MAX_STOCK_ITEM_NUM; i++)
            {
                memcpy(g_stock_item[i].id,   &g_stock_cfg_info.id[i][0], 12);
                g_stock_item[i].disp_type = DispLongData;
            }
        }
        else
        {
            //使用默认三支大盘股票
            //memcpy(g_stock_item, g_default_stock_item, sizeof(g_default_stock_item));
            for(i=0; i<3; i++)
            {
                memcpy(g_stock_item[i].id, &default_stock_id[i][0],12);
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
    app_window_timer_subscribe(p_window, 5*1000, stock_timer_callback, (void *)p_window);

    //立刻请求一次数据
	g_stock_update_count = 0;
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

