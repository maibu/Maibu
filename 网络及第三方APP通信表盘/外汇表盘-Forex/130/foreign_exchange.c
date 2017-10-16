/*
 *  应用名：外汇表盘（黑白屏）
 *  版本: 1.3.0	 修改用户配置内容含义，设置价格落在上一次价格与最新一次价格之间，则到价提醒。
				外汇配置格式为：外汇对 提醒价格1 提醒价格2（不同的外汇项用换行的方式隔开，可以没有提醒价格或只有一个提醒价格）
				程序在前台时，15秒请求一次数据；
				程序在后台时，1分钟请求一次数据；
				3分钟之内没有获取到数据，价格颜色白底黑字
 *  版本：1.1.0，修改时间显示宽度
 *	版本：1.0.0，初始版本。
				价格数据5秒更新一次;
 */
 
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "maibu_sdk.h"
#include "maibu_res.h"


/* 列表框架 */
#define MENU_FRAME_POS_X                     0
#define MENU_FRAME_POS_Y                     26
#define MENU_FRAME_SIZE_W                    128
#define MENU_FRAME_SIZE_H                    102

/* 列表项框架 */
#define MENU_ITEM_POS_X                      0
#define MENU_ITEM_POS_Y                      26
#define MENU_ITEM_SIZE_W                     128
#define MENU_ITEM_SIZE_H                     20

/* 时间项 */
#define WEEK_TEXT_POS_X                      0
#define WEEK_TEXT_POS_Y                      5
#define WEEK_TEXT_SIZE_W                     36
#define WEEK_TEXT_SIZE_H                     16

#define DATE_TEXT_POS_X                      36
#define DATE_TEXT_POS_Y                      5
#define DATE_TEXT_SIZE_W                     48
#define DATE_TEXT_SIZE_H                     16

#define TIME_TEXT_POS_X                      84
#define TIME_TEXT_POS_Y                      5
#define TIME_TEXT_SIZE_W                     44
#define TIME_TEXT_SIZE_H                     16

/* 外汇货币对 */
#define EXCHANGE_SYMBOL_POS_X           1
#define EXCHANGE_SYMBOL_POS_Y           3
#define EXCHANGE_SYMBOL_SIZE_W          62
#define EXCHANGE_SYMBOL_SIZE_H          14

/* 外汇卖出价*/
#define EXCHANGE_BID_POS_X              65
#define EXCHANGE_BID_POS_Y              3
#define EXCHANGE_BID_SIZE_W             62
#define EXCHANGE_BID_SIZE_H             14


/* 外汇买入价
#define EXCHANGE_ASK_POS_X              54
#define EXCHANGE_ASK_POS_Y              16
#define EXCHANGE_ASK_SIZE_W             46
#define EXCHANGE_ASK_SIZE_H             12

/* 外汇买卖差价
#define EXCHANGE_SPREAD_POS_X           104
#define EXCHANGE_SPREAD_POS_Y           2
#define EXCHANGE_SPREAD_SIZE_W          24
#define EXCHANGE_SPREAD_SIZE_H          12
*/

#define WEB_EXCHANGE_DATA_LINK               "https://ratesjson.fxcm.com/DataDisplayerHK?symbols=%s&since=20170815072353&callback=jsonCallback20170815072353&_=1502781704902"

#define MAX_ITEM_NUM  		                 5

#define EXCHANGE_FILE_KEY                    1


typedef struct{
    char	symbol[10];  	//货币对
    char	bid[10];  		//卖出价格
    //char	ask[10];  		//买入价格
	//char	spread[6];	 	//买卖差价
	uint32_t time;			//获取数据的时间
	enum GColor fcolor;		//价格的前景颜色
	enum GColor bcolor;		//价格的背景颜色
}ExchangeData;

typedef struct{
	bool 	exist;				//配置是否存在
    char	symbol[10];
	char	alarm_value1[10];   //价格提醒值1
	char	alarm_value2[10];   //价格提醒值2
}ExchangeCfgInfo;


static int32_t g_window_id       = -1;
static int32_t g_layer_id_week   = -1;
static int32_t g_layer_id_date   = -1;
static int32_t g_layer_id_time   = -1;
static uint8_t exchange_alarm_timer_id = -1; 

static int32_t g_layer_id_exchange_symbol[MAX_ITEM_NUM] = {-1, -1, -1, -1, -1};
static int32_t g_layer_id_exchange_bid[MAX_ITEM_NUM] = {-1, -1, -1, -1, -1};
//static int32_t g_layer_id_exchange_ask[MAX_ITEM_NUM] = {-1, -1, -1, -1, -1,};
//static int32_t g_layer_id_exchange_spread[MAX_ITEM_NUM] = {-1, -1, -1, -1, -1,};

static uint32_t g_comm_id_web = 0xffffffff;

static ExchangeData g_exchange_item[MAX_ITEM_NUM];
static ExchangeData g_exchange_item_before[MAX_ITEM_NUM];
static ExchangeCfgInfo g_exchange_cfg_info[MAX_ITEM_NUM];

//两个价格的提醒时间
static uint32_t alarm_time1[MAX_ITEM_NUM] = {0};
static uint32_t alarm_time2[MAX_ITEM_NUM] = {0};

//价格提醒标志，为0表示未提醒过，为1表示已经提醒过
static int8_t  g_exchange_is_alarmed_flag1[MAX_ITEM_NUM] = {0};
static int8_t  g_exchange_is_alarmed_flag2[MAX_ITEM_NUM] = {0};

static uint32_t g_exchange_update_index = 0;
static uint32_t g_exchange_update_count = 0;

static uint8_t cache_data_available = false;

//默认价格更新周期（秒）
static uint32_t DEFAULT_FRONT_CYCLE = 15;
static uint32_t DEFAULT_BACK_CYCLE = 120;

static const char default_exchange_symbol[5][10] = {"EURUSD", "GBPUSD", "USDJPY", "XAUUSD", "USOil"};
//static const char default_exchange_symbol[4][10] = {"EURUSD", "", "", ""};

static const char weekday[7][5] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};

static void refresh_exchange_data(P_Window p_window, int update);
static void web_request_exchange_data(char *str_exchange_symbol);
   

//返回特定字符串的字符个数
int my_strchr(const char * Str, int Val)
{
    const char * p = Str;
	//如果*p==_Val或者*p为'\0',退出循环
	while(*p!=Val && *p)
		p++;
	return p-Str;
}


//判断字符串是否全部为数字（包括小数），如果是返回1，不是返回0
int8_t is_digital_string(char* str)
{
	uint8_t i;
	for(i=0;i<strlen(str);i++)
	{
		if((str[i] < '0' || str[i] > '9') && (str[i] != '.'))
		{
			return 0;
		}
	}
	
	return 1;
}


//提取字符串函数
void data_handler(uint8_t *temp_buf, uint16_t temp_buf_size, uint8_t item_number)
{
	//os_printk("temp_buf : %s; temp_buf_size : %d ; item_number : %d \n", temp_buf, temp_buf_size, item_number);
	
	uint8_t *p = temp_buf;
	uint8_t *q = temp_buf;
	int32_t num_i = 0;
	int32_t num_i_first_data = 0;
	int32_t num_i_second_data = 0;
	char temp_symbol[10] = {0};
	char temp_alarm_value1[10] = {0};
	char temp_alarm_value2[10] = {0};
	bool is_set = false;	//是否为设置行

	
	//取第一个数据:symbol或者"set"
	while((*p != '\n')&&(*p != ' ')&&(num_i < temp_buf_size)&&(num_i < 10))	//外汇对代码最多10个字符
	{
		p++;
		num_i++;
	}
	
	memcpy(temp_symbol, q, (p-q));
	temp_symbol[p-q] = '\0';

	strcpy(g_exchange_item[item_number].symbol, temp_symbol);
	//os_printk("g_exchange_item[%d].symbol: %s \n", item_number, g_exchange_item[item_number].symbol);
	
	strcpy(g_exchange_cfg_info[item_number].symbol, temp_symbol);
	//os_printk("g_exchange_cfg_info[%d].symbol: %s \n", item_number, temp_symbol);
	
	
	//如果字符多了，跳过多余字符
	while((*p != ' ')&&(num_i < temp_buf_size))
	{
		p++;
		num_i++;
	}

	//找到空格并去掉多余的空格
	while((*p == ' ')&&(num_i < temp_buf_size))
	{
		p++;
		num_i++;
	}
	
	
	//取第二个数据：alarm_value1或者前台请求数据周期设置
	num_i_first_data = num_i;
	q = p;
	while((*p != '\n')&&(*p != ' ')&&(num_i < temp_buf_size)&&(num_i-num_i_first_data < 10)) //配置价格最多10个字符
	{
		p++;
		num_i++;
	}
		
	memcpy(temp_alarm_value1,q,(p-q));
	temp_alarm_value1[p-q] = '\0';

	strcpy(g_exchange_cfg_info[item_number].alarm_value1,temp_alarm_value1);
	//os_printk("g_exchange_cfg_info[%d].alarm_value1: %s \n", item_number, g_exchange_cfg_info[item_number].alarm_value1);	
	
	
	//如果字符多了，跳过多余字符
	while((*p != ' ')&&(num_i < temp_buf_size))
	{
		p++;
		num_i++;
	}
	
	//找到空格并去掉多余的空格
	while((*p == ' ')&&(num_i < temp_buf_size))
	{
		p++;
		num_i++;
	}
	
	
	//取第三个数据：alarm_value2或者后台请求数据周期设置
	num_i_second_data = num_i;
	q = p;
	while((*p != '\n')&&(*p != ' ')&&(num_i < temp_buf_size)&&(num_i-num_i_second_data < 10)) //配置价格最多10个字符
	{
		p++;
		num_i++;
	}
		
	memcpy(temp_alarm_value2,q,(p-q));
	temp_alarm_value2[p-q] = '\0';

	strcpy(g_exchange_cfg_info[item_number].alarm_value2,temp_alarm_value2);
	//os_printk("g_exchange_cfg_info[%d].alarm_value2: %s \n", item_number, g_exchange_cfg_info[item_number].alarm_value2);	
	
}


void alarm_notify(const char* temp_symbol,const char *alarm_price)
{	
	//os_printk("temp_symbol:%d up_down:%d alarm_price:%s\n",temp_symbol,up_down,alarm_price);
	
	os_printk("Enter alarm_notify! \n");
	
	GBitmap bitmap;
	res_get_user_bitmap(NOTICE_BITMAP, &bitmap);

	NotifyParam notic_ad = {	\
		.bmp = bitmap,	\
		.main_title = {0},	\
		.sub_title = {0},	\
		.pulse_type = VibesPulseTypeLong,		\
		.pulse_time = 6,		\
		.context = NULL,	\
	};
	
	sprintf(notic_ad.sub_title,"%s  %s",temp_symbol,alarm_price);

	maibu_service_sys_notify(&notic_ad);

}



//比较两个数字字符串的大小，a大于b返回1，a小于b返回-1，等于返回0
//因为操作系统没有提供atof函数，所以如此操作！否则用atof函数很简单
int8_t compare_digital_string(char str1[10], char str2[10])
{
	uint8_t i;
	char a[10];
	char b[10];
	
	memcpy(a,str1,10);
	memcpy(b,str2,10);
	
	//os_printk("strlen(%s): %d \n", a, strlen(a));
	//os_printk("strlen(%s): %d \n", b, strlen(b));
	//os_printk("strlen(%s) - strlen(%s): %d \n", a, b, strlen(a) - strlen(b));
	
	uint8_t m = strlen(a);
	uint8_t n = strlen(b);
	
	//配置价格位数小于实际当前价格位数，把配置价格位数补齐
	if(m > n)
	{
		for(i=0;i<m-n;i++)
		{
			b[n+i] = '0';
		}
		b[m] = '\0';
	}
	
	//配置价格位数大于实际当前价格位数
	if(m < n)
	{
		for(i=0;i<n-m;i++)
		{
			a[m+i] = '0';
		}
		a[n] = '\0';
	}
		
	
	//开始比较
	if(atoi(a) > atoi(b))
	{
		return 1;
	}
	else if(atoi(a) == atoi(b))
	{
		char num1 = my_strchr(a, '.');  //a小数点前的位数
		char num2 = my_strchr(b, '.');  //b小数点前的位数
		//比较小数点后的大小
		if(atoi(&a[num1+1]) > atoi(&b[num2+1]))
		{
			return 1;
		}
		else if(atoi(&a[num1+1]) < atoi(&b[num2+1]))
		{
			return -1;
		}
		else if(atoi(&a[num1+1]) == atoi(&b[num2+1]))
		{
			return 0;
		}
	}
	else if(atoi(a) < atoi(b))
	{
		return -1;
	}
}


//添加文本图层
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


//配置回调函数
void watchapp_comm_callback(enum ESyncWatchApp type, uint8_t *buf, uint16_t len)
{
	
	uint8_t i, j = 0;
	
	//清空外汇（对）名称、卖出价格、用户配置缓存
	for(i=0;i<MAX_ITEM_NUM;i++)
	{
		memset(g_exchange_item[i].symbol, 0, 10);
		memset(g_exchange_item[i].bid, 0, 10);
		memset(g_exchange_item_before[i].bid, 0, 10);
		memset(&g_exchange_cfg_info[i],0,sizeof(ExchangeCfgInfo));
	}
	
	
	//用户配置标志置为不存在
	g_exchange_cfg_info[0].exist = false;
	
	
	//到价提醒标志置为可提醒
	memset(g_exchange_is_alarmed_flag1,0,sizeof(g_exchange_is_alarmed_flag1));
	memset(g_exchange_is_alarmed_flag2,0,sizeof(g_exchange_is_alarmed_flag2));
	
	
	//把用户配置的内容，小写字母转换为大写（配置不区分大小写）
	/*
	for(i=0;i<len;i++)
	{
		if(buf[i] >= 'a' && buf[i] <= 'z') //小写字母ascii范围
		{
			buf[i] = buf[i] - ' '; //大小写字母ascii码值相差32，空格的ascii值为32
		}
	}*/ //因为外汇对关键字区分大小写，所以暂时做不到，除非做一个完整映射表。
	
	
	//os_printk("config buf = %s \r\n", buf);
	
	
	uint8_t *tail_p = buf;
	uint8_t *head_q = buf;
	uint32_t count = 0;
	uint32_t count_total = 1;

	//最多MAX_ITEM_NUM行外汇对，再加1行请求数据周期设置
	for(i = 0;i < MAX_ITEM_NUM;i++)
	{
		//去掉前置空格+回车和上一次的后置回车+空格，找到配置开始位置
		while((*tail_p == ' ')||(*tail_p == '\n'))
		{
			tail_p++;
			count_total++;
		}
		head_q = tail_p;
		
		//找到行尾结束
		while((*tail_p != '\n')&&(*tail_p!= NULL)&&(count_total <= len))
		{
			tail_p++;
			count++;
			count_total++;
		}
		
		if(count > 0)
		{
			//解析每一行的数据
			data_handler(head_q,count,i);
		}
		else if(count == 0)
		{
			
		}
		count = 0;
	}


	//有用户配置
	if(strlen(g_exchange_cfg_info[0].symbol) > 0)	
	{
		//配置存在
		g_exchange_cfg_info[0].exist = true;
	}
	//无用户配置，恢复默认
	else if(strlen(g_exchange_cfg_info[0].symbol) == 0)
	{
		//配置不存在
		g_exchange_cfg_info[0].exist = false;
		
		//使用默认配置
		for(i=0, j=0; i<MAX_ITEM_NUM; i++, j++)
		{
			memset(&g_exchange_item[i], 0, sizeof(ExchangeData));
			memcpy(g_exchange_item[i].symbol, &default_exchange_symbol[j][0], 10);
		}
	}
	
	//保存配置
	app_persist_write_data_extend(EXCHANGE_FILE_KEY, &g_exchange_cfg_info, sizeof(g_exchange_cfg_info));

	
	//立刻请求一次数据
	g_exchange_update_count = 0;
	g_exchange_update_index = 0;
	web_request_exchange_data(g_exchange_item[g_exchange_update_index].symbol);
	
	os_printk("g_exchange_item[%d].symbol: %s \r\n", g_exchange_update_index, g_exchange_item[g_exchange_update_index].symbol);
}


static void web_request_exchange_data(char *str_exchange_symbol)
{
	
    /*拼接url请求地址, 注意url的缓存大小*/
    char url[200];
    sprintf(url, WEB_EXCHANGE_DATA_LINK, str_exchange_symbol);

	
    /*拼接过滤参数，即只接受和过滤参数匹配的返回值*/
    //os_printk("URL: %s \r\n", url);

    g_comm_id_web = maibu_comm_request_web(url, NULL, 0);
}


/*
//当前更新的数据与上一次数据对比，如果比上一次大，价格背景颜色为蓝色，比上一次小，背景为红色，相等黑色
static void exchange_price_back_color(uint8_t number)
{
	//防止第一次执行下面的比较时，上一次的数据为空
	sprintf(g_exchange_item_before[number].bid, "%s", (strlen(g_exchange_item_before[number].bid) > 0) ?
									g_exchange_item_before[number].bid : g_exchange_item[number].bid);
									
	//os_printk("g_exchange_item[%d].bid : %s \n", number, g_exchange_item[number].bid);
	//os_printk("g_exchange_item_before[%d].bid : %s \n", number, g_exchange_item_before[number].bid);
	
	//当前更新的数据与上一次数据对比
	if(compare_digital_string(g_exchange_item[number].bid, g_exchange_item_before[number].bid) > 0)
	{
		g_exchange_item[number].fcolor = GColorWhite;
		g_exchange_item[number].bcolor = GColorBlue;
	}
	else if(compare_digital_string(g_exchange_item[number].bid, g_exchange_item_before[number].bid) < 0)
	{
		g_exchange_item[number].fcolor = GColorWhite;
		g_exchange_item[number].bcolor = GColorRed;
	}
	else
	{
		g_exchange_item[number].fcolor = GColorWhite;
		g_exchange_item[number].bcolor = GColorBlack;
	}
}*/


//当前价格与配置的提醒价格作对比，到达提醒价格，弹出提示界面
static void exchange_price_alarm(uint8_t i)
{
	os_printk("Enter exchange_price_alarm! \r\n");
	
	//第一个设置的价格
	if((strlen(g_exchange_cfg_info[i].alarm_value1) > 0) && (strlen(g_exchange_item[i].bid) > 0) \
		&& (is_digital_string(g_exchange_cfg_info[i].alarm_value1)))
	{
		//设置的提醒价格落在前一次价格与当前价格之间，则到价提醒
		if( \
		((compare_digital_string(g_exchange_cfg_info[i].alarm_value1, g_exchange_item[i].bid) <= 0) \
			&&(compare_digital_string(g_exchange_cfg_info[i].alarm_value1, g_exchange_item_before[i].bid) >= 0)) \
		|| \
		((compare_digital_string(g_exchange_cfg_info[i].alarm_value1, g_exchange_item[i].bid) >= 0) \
			&&(compare_digital_string(g_exchange_cfg_info[i].alarm_value1, g_exchange_item_before[i].bid) <= 0)) \
		)
		{
			if(g_exchange_is_alarmed_flag1[i] == 0)
			{
				//第一个价格到价提醒
				alarm_notify(g_exchange_item[i].symbol, g_exchange_cfg_info[i].alarm_value1);
				
				//标志置为已提醒
				g_exchange_is_alarmed_flag1[i] = 1;
				
				//记录提醒时间
				struct date_time datetime1;
				app_service_get_datetime(&datetime1);
				alarm_time1[i] = app_get_time(&datetime1);
			}
		}
	}
	
	
	//第二个设置的价格
	if((strlen(g_exchange_cfg_info[i].alarm_value2) > 0) && (strlen(g_exchange_item[i].bid) > 0) \
		&& (is_digital_string(g_exchange_cfg_info[i].alarm_value2)))
	{
		//设置的提醒价格落在前一次价格与当前价格之间，则到价提醒
		if( \
		((compare_digital_string(g_exchange_cfg_info[i].alarm_value2, g_exchange_item[i].bid) <= 0) \
			&&(compare_digital_string(g_exchange_cfg_info[i].alarm_value2, g_exchange_item_before[i].bid) >= 0)) \
		|| \
		((compare_digital_string(g_exchange_cfg_info[i].alarm_value2, g_exchange_item[i].bid) >= 0) \
			&&(compare_digital_string(g_exchange_cfg_info[i].alarm_value2, g_exchange_item_before[i].bid) <= 0)) \
		)
		{
			if(g_exchange_is_alarmed_flag2[i] == 0)
			{
				//第二个价格到价提醒
				alarm_notify(g_exchange_item[i].symbol, g_exchange_cfg_info[i].alarm_value2);
				
				//提醒标志置为已提醒
				g_exchange_is_alarmed_flag2[i] = 1;
				
				//记录提醒时间
				struct date_time datetime2;
				app_service_get_datetime(&datetime2);
				alarm_time2[i] = app_get_time(&datetime2);
			}
		}
	}
}



//网络数据请求回调函数
static void web_recv_callback(const uint8_t *buff, uint16_t size)
{
	//os_printk("size: %d \r\n", size);
	//os_printk("web_recv_callback_buff: %s \r\n", buff);
	
	os_printk("Enter web request callback!!!\r\n");
	
	if(g_exchange_update_count > MAX_ITEM_NUM)
    {
    	return;
    }
    else if(g_exchange_update_count < MAX_ITEM_NUM)
    {
		g_exchange_update_count++;
        g_exchange_update_index = (g_exchange_update_index + 1) % MAX_ITEM_NUM;
		
		if(strlen(g_exchange_item[g_exchange_update_index].symbol) != 0)
		{
			web_request_exchange_data(g_exchange_item[g_exchange_update_index].symbol);
			//os_printk("g_exchange_item[%d].symbol: %s \r\n", g_exchange_update_index, g_exchange_item[g_exchange_update_index].symbol);
		}
    }

	uint8_t temp_buff[120] = "";
	
	//buff内容不是标准json格式，需要进行字符串处理
	//去除buff前面27个字符
	uint8_t i,j;
	
	for(i=27,j=0;;i++,j++)
	{
		if(buff[i] != '\0')
		{
			temp_buff[j] = buff[i];
		}
		else
		{
			temp_buff[j] = '\0';
			break;
		}
	}
	
	//os_printk("web_recv_callback_temp_buff_1: %s \r\n", temp_buff);
	
	
	//去除buff最后一个逗号，在buff倒数第6个字符
	uint8_t len;
	len = strlen(temp_buff);
	
	temp_buff[len-7] = '}';
	temp_buff[len-6] = ']';
	temp_buff[len-5] = '}';
	temp_buff[len-4] = '\0';
	
	os_printk("web_recv_callback_temp_buff_2: %s \r\n", temp_buff);
	
	char exchange_symbol[10];
	char exchange_bid[10];
	//char exchange_ask[10];
  	//char exchange_spread[6];
	
	maibu_get_json_str(temp_buff, "Symbol", exchange_symbol, 10);
	maibu_get_json_str(temp_buff, "Bid", exchange_bid, 10);
	//maibu_get_json_str(temp_buff, "Ask", exchange_ask, 10);
	//maibu_get_json_str(temp_buff, "Spread", exchange_spread, 6);
	
	//os_printk("index-----%d count-----%d \r\n", g_exchange_update_index, g_exchange_update_count);
	//os_printk("exchange Symbol: %s \r\n", exchange_symbol);
    //os_printk("exchange Bid: %s \r\n", exchange_bid);
    //os_printk("exchange Ask: %s \r\n", exchange_ask);
    //os_printk("exchange Spread: %s \r\n", exchange_spread);
	
	
	//找到对应的外汇项并更新数据
	uint8_t k;
    for(k=0; k< MAX_ITEM_NUM; k++)
    {
        if( strcmp(exchange_symbol, g_exchange_item[k].symbol) == 0 )
        {
            strcpy(g_exchange_item[k].symbol, exchange_symbol);
			strcpy(g_exchange_item[k].bid, exchange_bid);
			//strcpy(g_exchange_item[k].ask, exchange_ask);
			//strcpy(g_exchange_item[k].spread, exchange_spread);
			
			//记录获取数据时间
			struct date_time datetime;
			app_service_get_datetime(&datetime);
			g_exchange_item[k].time = app_get_time(&datetime);

	
			os_printk("g_exchange_item[%d].bid: %s \r\n", k, g_exchange_item[k].bid);
			
			//判断价格相比上一次的高低，设置价格背景颜色
			//exchange_price_back_color(k);
			
			//刷新外汇窗口信息
			P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
			if(p_window != NULL)
			{
				refresh_exchange_data(p_window, true);
			}
			
			//os_printk("--------------------------\r\n"); 
			
			//如果到达提醒价格，弹出提示界面
			exchange_price_alarm(k);
			
			
			//保存此时数据至另一变量，以便下次数据作对比
			sprintf(g_exchange_item_before[k].bid, "%s", g_exchange_item[k].bid);
	
		}
	}
}
    


static void exchange_comm_result_callback(enum ECommResult result, uint32_t comm_id, void *context)
{
    /*如果上一次请求WEB通讯失败，并且通讯ID相同，则重新发送*/
    if ((result == ECommResultFail) && (comm_id == g_comm_id_web))
    {
        //os_printk("exchange data request failed!!!\r\n");
        web_request_exchange_data(g_exchange_item[g_exchange_update_index].symbol);
    }
}


//检测数据是否更新（通过获取数据的时间来判断），如果没有更新，价格的显示颜色反转
//当前时间与最近一次数据更新的时间比较，如果大于60秒，价格的显示颜色反转（白底黑字）
static void is_exchange_updating()
{
	struct date_time datetime;
	app_service_get_datetime(&datetime);
	uint32_t datetime_sec = app_get_time(&datetime);
	
	uint8_t i;
	for(i=0;i<MAX_ITEM_NUM;i++)
	{
		//防止第一次执行下面的比较时，上一次的数据为空
		if(g_exchange_item[i].time == 0)
		{
			g_exchange_item[i].time = datetime_sec;
		}
			
		if(abs(datetime_sec - g_exchange_item[i].time) < 3*60)
		{
			g_exchange_item[i].bcolor = GColorBlack;
			g_exchange_item[i].fcolor = GColorWhite;
		}
		else
		{
			g_exchange_item[i].bcolor = GColorWhite;
			g_exchange_item[i].fcolor = GColorBlack;
			
			//刷新外汇窗口信息
			P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
			if (p_window == NULL)
			{
				return;
			}
			refresh_exchange_data(p_window, true);
		}
	}
	
}


//判断到价提醒是否提醒，提醒过后一定时间内即使到价也不再提醒
static void is_price_has_alarmed()
{
	struct date_time datetime;
	app_service_get_datetime(&datetime);
	uint32_t datetime_sec = app_get_time(&datetime);
	
	uint8_t i;
	for(i=0;i<MAX_ITEM_NUM;i++)
	{
		//防止第一次执行下面的比较时，上一次的数据为空
		if(alarm_time1[i] == 0)
		{
			alarm_time1[i] = datetime_sec;
		}
		
		if(alarm_time2[i] == 0)
		{
			alarm_time2[i] = datetime_sec;
		}		
			
		//第一个价格
		if(abs(datetime_sec - alarm_time1[i]) < 10*60)	//10分钟
		{
			//提醒标志置为已提醒
			g_exchange_is_alarmed_flag1[i] = 1;
		}
		else
		{
			//提醒标志置为可以提醒
			g_exchange_is_alarmed_flag1[i] = 0;
		}
		
		//第二个价格
		if(abs(datetime_sec - alarm_time2[i]) < 10*60)	//10分钟
		{
			//提醒标志置为已提醒
			g_exchange_is_alarmed_flag2[i] = 1;
		}
		else
		{
			//提醒标志置为可以提醒
			g_exchange_is_alarmed_flag2[i] = 0;
		}
	}
	
}
	

//后台定时回调函数
static void exchange_sevice_callback(date_time_t tick_time, uint32_t millis, void *context)
{
	os_printk("Enter background request-----------! \r\n");
	
	//外汇数据定时请求一次
	g_exchange_update_count = 0;
	g_exchange_update_index = 0;
	web_request_exchange_data(g_exchange_item[g_exchange_update_index].symbol);
}

	
uint32_t timing_count_1 = 0;
uint32_t timing_count_2 = 0;
uint32_t timing_count_3 = 0;

//前台1秒定时器
static void exchange_timer_callback(date_time_t tick_time, uint32_t millis, void *context)
{	
	//os_printk("Enter request! \r\n");
	
	timing_count_1++;
	//timing_count_2++;
	timing_count_3++;
	
	if(timing_count_1 == 5)	//5秒钟
	{
		//设置到价提醒标志
		is_price_has_alarmed();
		
		//检测数据是否在更新
		is_exchange_updating();
		
		timing_count_1 = 0;
		//timing_count_2 = 0;
	}
	
	if(timing_count_3 == DEFAULT_FRONT_CYCLE)	//配置的秒数
	{
		
		os_printk("Enter front request-----------! \r\n");
		
		//外汇数据定时请求一次
		g_exchange_update_count = 0;
		g_exchange_update_index = 0;
		web_request_exchange_data(g_exchange_item[g_exchange_update_index].symbol);
		
		timing_count_3 = 0;
	}
}


		
//定时函数，1分钟执行一次
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
        char text_buf[10] = "";
        struct date_time t;
        app_service_get_datetime(&t);

		//星期
		memset(text_buf, 0, sizeof(text_buf));
        sprintf(text_buf, "%s", weekday[t.wday]);
		
        GRect frame;
        frame.origin.x = WEEK_TEXT_POS_X;
        frame.origin.y = WEEK_TEXT_POS_Y;
        frame.size.h   = WEEK_TEXT_SIZE_H;
        frame.size.w   = WEEK_TEXT_SIZE_W;
        add_text_layer(p_window, &g_layer_id_week, (char*)text_buf, &frame, GAlignLeft, U_ASCII_ARIALBD_14, GColorWhite);
		
		//日期
		memset(text_buf, 0, sizeof(text_buf));
        sprintf(text_buf, "%02d-%02d", t.mon, t.mday);

        frame.origin.x = DATE_TEXT_POS_X;
        frame.origin.y = DATE_TEXT_POS_Y;
        frame.size.h   = DATE_TEXT_SIZE_H;
        frame.size.w   = DATE_TEXT_SIZE_W;
        add_text_layer(p_window, &g_layer_id_date, (char*)text_buf, &frame, GAlignCenter, U_ASCII_ARIALBD_16, GColorWhite);
		
		//时间
		memset(text_buf, 0, sizeof(text_buf));
        sprintf(text_buf, "%02d:%02d", t.hour, t.min);
		
        frame.origin.x = TIME_TEXT_POS_X;
        frame.origin.y = TIME_TEXT_POS_Y;
        frame.size.h   = TIME_TEXT_SIZE_H;
        frame.size.w   = TIME_TEXT_SIZE_W;
        add_text_layer(p_window, &g_layer_id_time, (char*)text_buf, &frame, GAlignRight, U_ASCII_ARIALBD_16, GColorWhite);

		app_window_update(p_window);
    }
	
}


/* 添加时间图层 */
static void add_time_bar(P_Window p_window)
{
    char text_buf[10] = "";
    struct date_time t;
    app_service_get_datetime(&t);

	//星期
	memset(text_buf, 0, sizeof(text_buf));
	sprintf(text_buf, "%s", weekday[t.wday]);
	
	GRect frame;
	frame.origin.x = WEEK_TEXT_POS_X;
	frame.origin.y = WEEK_TEXT_POS_Y;
	frame.size.h   = WEEK_TEXT_SIZE_H;
	frame.size.w   = WEEK_TEXT_SIZE_W;
	add_text_layer(p_window, &g_layer_id_week, (char*)text_buf, &frame, GAlignLeft, U_ASCII_ARIALBD_14, GColorWhite);
	
	//日期
	memset(text_buf, 0, sizeof(text_buf));
	sprintf(text_buf, "%02d-%02d", t.mon, t.mday);

	frame.origin.x = DATE_TEXT_POS_X;
	frame.origin.y = DATE_TEXT_POS_Y;
	frame.size.h   = DATE_TEXT_SIZE_H;
	frame.size.w   = DATE_TEXT_SIZE_W;
	add_text_layer(p_window, &g_layer_id_date, (char*)text_buf, &frame, GAlignCenter, U_ASCII_ARIALBD_16, GColorWhite);
	
	//时间
	memset(text_buf, 0, sizeof(text_buf));
	sprintf(text_buf, "%02d:%02d", t.hour, t.min);
	
	frame.origin.x = TIME_TEXT_POS_X;
	frame.origin.y = TIME_TEXT_POS_Y;
	frame.size.h   = TIME_TEXT_SIZE_H;
	frame.size.w   = TIME_TEXT_SIZE_W;
	add_text_layer(p_window, &g_layer_id_time, (char*)text_buf, &frame, GAlignRight, U_ASCII_ARIALBD_16, GColorWhite);
}


/* 更新外汇信息 */
void refresh_exchange_data(P_Window p_window, int update)
{
    GRect text1_frame, text2_frame, text3_frame, text4_frame;
	
	uint8_t i;
	for(i=0; i<MAX_ITEM_NUM; i++)
    {
		text1_frame.origin.x = EXCHANGE_SYMBOL_POS_X + MENU_ITEM_POS_X;
		text1_frame.origin.y = EXCHANGE_SYMBOL_POS_Y + MENU_ITEM_POS_Y;
		text1_frame.size.h   = EXCHANGE_SYMBOL_SIZE_H;
		text1_frame.size.w   = EXCHANGE_SYMBOL_SIZE_W;

		text2_frame.origin.x = EXCHANGE_BID_POS_X + MENU_ITEM_POS_X;
		text2_frame.origin.y = EXCHANGE_BID_POS_Y + MENU_ITEM_POS_Y;
		text2_frame.size.h   = EXCHANGE_BID_SIZE_H;
		text2_frame.size.w   = EXCHANGE_BID_SIZE_W;
		
		/*
		text3_frame.origin.x = EXCHANGE_ASK_POS_X + MENU_ITEM_POS_X;
		text3_frame.origin.y = EXCHANGE_ASK_POS_Y + MENU_ITEM_POS_Y;
		text3_frame.size.h   = EXCHANGE_ASK_SIZE_H;
		text3_frame.size.w   = EXCHANGE_ASK_SIZE_W;
		
		text4_frame.origin.x = EXCHANGE_SPREAD_POS_X + MENU_ITEM_POS_X;
		text4_frame.origin.y = EXCHANGE_SPREAD_POS_Y + MENU_ITEM_POS_Y;
		text4_frame.size.h   = EXCHANGE_SPREAD_SIZE_H;
		text4_frame.size.w   = EXCHANGE_SPREAD_SIZE_W;
		*/

		text1_frame.origin.y += i*MENU_ITEM_SIZE_H;
		text2_frame.origin.y += i*MENU_ITEM_SIZE_H;
		text3_frame.origin.y += i*MENU_ITEM_SIZE_H;
		text4_frame.origin.y += i*MENU_ITEM_SIZE_H;

		add_text_layer(p_window, &g_layer_id_exchange_symbol[i], g_exchange_item[i].symbol, &text1_frame, GAlignLeft, U_ASCII_ARIALBD_14, GColorBlack);
		add_text_layer(p_window, &g_layer_id_exchange_bid[i], g_exchange_item[i].bid, &text2_frame, GAlignRight, U_ASCII_ARIALBD_14, g_exchange_item[i].bcolor);
		//add_text_layer(p_window, &g_layer_id_exchange_ask[i], g_exchange_item[i].ask, &text3_frame, GAlignLeft, U_ASCII_ARIAL_16, GColorBlack);
		//add_text_layer(p_window, &g_layer_id_exchange_spread[i], g_exchange_item[i].spread, &text4_frame, GAlignLeft, U_ASCII_ARIAL_16, GColorBlack);
    }

    if(update)
    {
        app_window_update(p_window);
    }
	
}

/* 初始化外汇表盘窗口 */
static void init_start_window(P_Window p_window)
{
    /* 添加表盘背景 */
    GRect frame = {{0, 0}, {128, 128}};
    GBitmap bitmap;

    res_get_user_bitmap(BMP_EXCHANGE_BG, &bitmap);
    LayerBitmap layer_bitmap = {bitmap, frame, GAlignCenter};
    P_Layer layer = app_layer_create_bitmap(&layer_bitmap);
	app_layer_set_bg_color(layer, GColorBlack);
    app_window_add_layer(p_window, layer);

    /* 添加时间栏 */
    add_time_bar(p_window);
	

    /* 添加外汇项目 */
	uint8_t i;
	for(i=0; i< MAX_ITEM_NUM; i++)
	{
		//初始化价格颜色
		g_exchange_item[i].fcolor = GColorWhite;
		g_exchange_item[i].bcolor = GColorBlack;
	}
	
    refresh_exchange_data(p_window, false);
	

    /*把窗口放入窗口栈中显示*/
    g_window_id = app_window_stack_push(p_window);
}

void foreign_exchange_init(void)
{
    int i;

    if(cache_data_available == false)
    {
        memset(&g_exchange_item, 0, sizeof(g_exchange_item));
		memset(&g_exchange_item_before, 0, sizeof(g_exchange_item_before));
        memset(&g_exchange_cfg_info, 0, sizeof(g_exchange_cfg_info));

        //创建保存外汇信息的结构体
        app_persist_create(EXCHANGE_FILE_KEY, 4096);
        app_persist_read_data(EXCHANGE_FILE_KEY, 0, &g_exchange_cfg_info, sizeof(g_exchange_cfg_info));
        if(g_exchange_cfg_info[0].exist == true)
        {

            //还原保存的外汇（包括了set行）
            for(i=0; i<MAX_ITEM_NUM; i++)
            {
                memcpy(g_exchange_item[i].symbol, &g_exchange_cfg_info[i].symbol[0], 10);
            }
        }
        else
        {
            //使用默认外汇
            for(i=0; i<5; i++)
            {
                memcpy(g_exchange_item[i].symbol, &default_exchange_symbol[i][0], 10);
				//sprintf(g_exchange_item[i].symbol, "%s", &default_exchange_symbol[i][0]);
				//os_printk("SYMbol--- : %s \r\n", g_exchange_item[i].symbol);
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
    maibu_comm_register_result_callback(exchange_comm_result_callback);
	
    //前台定时网络请求函数
    app_window_timer_subscribe(p_window, 1*1000, exchange_timer_callback, (void *)p_window);
	
	//后台台定时网络请求函数
	exchange_alarm_timer_id = app_service_timer_subscribe(60*1000, exchange_sevice_callback, NULL);

    //立刻请求一次数据
	g_exchange_update_count = 0;
    g_exchange_update_index = 0;
    web_request_exchange_data(g_exchange_item[g_exchange_update_index].symbol); 

}


/*开机启动函数*/
static void maibu_app_init()
{
	int i;

    if(cache_data_available == false)
    {
        memset(&g_exchange_item, 0, sizeof(g_exchange_item));
		memset(&g_exchange_item_before, 0, sizeof(g_exchange_item_before));
        memset(&g_exchange_cfg_info, 0, sizeof(g_exchange_cfg_info));

        //创建保存外汇信息的结构体
        app_persist_create(EXCHANGE_FILE_KEY, 4096);
        app_persist_read_data(EXCHANGE_FILE_KEY, 0, &g_exchange_cfg_info, sizeof(g_exchange_cfg_info));
        if(g_exchange_cfg_info[0].exist == true)
        {

            //还原保存的外汇（包括了set行）
            for(i=0; i<MAX_ITEM_NUM; i++)
            {
                memcpy(g_exchange_item[i].symbol, &g_exchange_cfg_info[i].symbol[0], 10);
            }
        }
        else
        {
            //使用默认外汇
            for(i=0; i<5; i++)
            {
                memcpy(g_exchange_item[i].symbol, &default_exchange_symbol[i][0], 10);
				//sprintf(g_exchange_item[i].symbol, "%s", &default_exchange_symbol[i][0]);
				//os_printk("SYMbol--- : %s \r\n", g_exchange_item[i].symbol);
            }
        }

        cache_data_available = true;
    }
	
	//后台台定时网络请求函数
    exchange_alarm_timer_id = app_service_timer_subscribe(60*1000, exchange_sevice_callback, NULL);
	
	//注册网络请求回调函数
	maibu_comm_register_web_callback(web_recv_callback);
	
	//注册通讯结果回调
    maibu_comm_register_result_callback(exchange_comm_result_callback);
	
	//注册请求手表app配置数据回调
    maibu_comm_register_watchapp_callback(watchapp_comm_callback);
	
	//立刻请求一次数据
	g_exchange_update_count = 0;
    g_exchange_update_index = 0;
    web_request_exchange_data(g_exchange_item[g_exchange_update_index].symbol); 
}

/*关机启动函数*/
static void maibu_app_deinit()
{
    app_service_timer_unsubscribe(exchange_alarm_timer_id);
}

int main(void)
{
    foreign_exchange_init();
}


