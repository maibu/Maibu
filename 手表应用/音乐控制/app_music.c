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

#define  LOG_INFO os_printk

#endif


#define SIDE_BAR_POS_X                      104
#define SIDE_BAR_POS_Y                      0
#define SIDE_BAR_SIZE_H                     128
#define SIDE_BAR_SIZE_W                     24       

#define PREV_BUTN_POS_X                     108
#define PREV_BUTN_POS_Y                     12
#define PREV_BUTN_SIZE_H                    16 
#define PREV_BUTN_SIZE_W                    16 

#define PAUSE_BUTN_POS_X                    108
#define PAUSE_BUTN_POS_Y                    56
#define PAUSE_BUTN_SIZE_H                   16  
#define PAUSE_BUTN_SIZE_W                   16

#define NEXT_BUTN_POS_X                     108
#define NEXT_BUTN_POS_Y                     100
#define NEXT_BUTN_SIZE_H                    16
#define NEXT_BUTN_SIZE_W                    16

#define MUSIC_DEFAULT_ICON_POS_X            32
#define MUSIC_DEFAULT_ICON_POS_Y            16
#define MUSIC_DEFAULT_ICON_SIZE_H           64
#define MUSIC_DEFAULT_ICON_SIZE_W           64

#define TIME_BAR_POS_X                      0
#define TIME_BAR_POS_Y                      0
#define TIME_BAR_SIZE_H			            16	
#define TIME_BAR_SIZE_W			            104	

#define SINGER_POS_X                        4
#define SINGER_POS_Y                        22
#define SINGER_SIZE_H                       14
#define SINGER_SIZE_W                       96

#define MAX_SINGER_CHAR_NUM                 6 //歌手名不超过6个14x14汉字 

#define SONG_NAME_LINE_1_POS_X              4
#define SONG_NAME_LINE_1_POS_Y              50
#define SONG_NAME_LINE_1_SIZE_H             40
#define SONG_NAME_LINE_1_SIZE_W             96

#define SONG_NAME_LINE_2_POS_X              4
#define SONG_NAME_LINE_2_POS_Y              70
#define SONG_NAME_LINE_2_SIZE_H             40
#define SONG_NAME_LINE_2_SIZE_W             96

#define ONE_LINE_MAX_SONG_NAME_CHAR_NUM     6 //歌名每行显示最多6个16x16汉字            




static int32_t  g_prev_butn_id  = -1;
static int32_t  g_next_butn_id  = -1;
static int32_t  g_pause_butn_id = -1;
//static int8_t  g_music_icon_id = -1;
static int32_t  g_time_layer_id = -1;
static int32_t g_window_id     = -1;
static int32_t  g_comm_id_music_ctrl  = -1;

static int32_t  g_singer_id     = -1;
static int32_t  g_song_name_line_1_id  = -1;
static int32_t  g_song_name_line_2_id  = -1;

static char    g_singer[30] = "";
static char    g_song_name_1[30] = "";
static char    g_song_name_2[30] = "";
static int8_t  g_music_paused  = 0;

int enc_get_utf8_size(unsigned char input);

/*
enum EMusicControl
{
    EMusicPrev = 0x1,
    EMusicNext,
    EMusicPause,
    EMusicVolumeUp,
    EMusicVolumeDown,
    EMusicProcess,
    EMusicInfo,
};
*/

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

void add_bmp_layer(P_Window p_window, int32_t *p_layer_id, uint32_t bmp_file_key, GRect *p_frame, enum GAlign align, enum GColor color)
{
	GBitmap  bitmap;
	
	res_get_user_bitmap(bmp_file_key, &bitmap);
    LayerBitmap  layer_bitmap = {bitmap, *p_frame, align}; 
    P_Layer      layer = app_layer_create_bitmap(&layer_bitmap); 
    app_layer_set_bg_color(layer, color);
	
    Layer * p_old_layer = app_window_get_layer_by_id(p_window, *p_layer_id);
    
	if(p_old_layer)
	{
		*p_layer_id = app_window_replace_layer(p_window, p_old_layer, layer);
	}
	else
	{
		*p_layer_id  = app_window_add_layer(p_window, layer);
	}
	
}

static int send_music_control(enum EMusicControl cmd)
{
	uint8_t ctrl_cmd = cmd;
	
	if(maibu_get_phone_type() == PhoneTypeAndroid)
    {
        g_comm_id_music_ctrl = maibu_comm_request_phone(ERequestPhoneMusicControl, &ctrl_cmd, 1);
    }
    else if(maibu_get_phone_type() == PhoneTypeIOS)
    {
		//安卓、IOS控制协议未修改，目前控制命令键值不同，临时解决
		MediaKey ios_music_cmd = MediaKeyRelease;
		switch(cmd)
		{
			case EMusicPrev:  ios_music_cmd=MediaKeyScanPrevTrack; break;
			case EMusicNext:  ios_music_cmd=MediaKeyScanNextTrack; break;
			case EMusicPause: ios_music_cmd=MediaKeyPlayPause; break;
		}
		
        //sprintf(phone_type, "%s", IOS);
		music_media_key_send(ios_music_cmd);
		music_media_key_send(MediaKeyRelease);
    }
	
}

void music_data_receive_callback(enum ERequestPhone type, void * context)
{
	uint8_t *p_tmp = (uint8_t *)context;
	os_printk("type: %d 0x%x 0x%x 0x%x \r\n", type, p_tmp[0], p_tmp[1], p_tmp[2]);
	if(type == ERequestPhoneMusicControl)
	{
		uint8_t *p = (uint8_t *)context;
		uint8_t index = 1; //0为播放进度，暂未实现
		//uint8_t progress = context[0];
		uint8_t singer_len = p[index++];
		if(singer_len)
		{
			char singer[singer_len+1]; //字符串空字符未传来，需要手动补空字符
			singer[singer_len] = '\0';
			memcpy(singer, &p[index], singer_len);
			index += singer_len;
			music_set_singer(singer, singer_len);
			os_printk("singer_len: %d %s \r\n", singer_len, singer);
		}
		else
		{
			music_set_singer(NULL, 0);
		}
		uint8_t song_len = p[index++];
		if(song_len)
		{
			char song[song_len+1]; //字符串空字符未传来，需要手动补空字符
			song[song_len] = '\0';
			memcpy(song, &p[index], song_len);
			index += song_len;
			music_set_song_name(song, song_len);
			os_printk("song_len: %d %s\r\n", song_len, song);
		}
		else
		{
			music_set_song_name(NULL, 0);
		}
	}
}

static void music_comm_result_callback(enum ECommResult result, uint32_t comm_id, void *context)
{
    /*如果上一次请求WEB通讯失败，并且通讯ID相同，则重新发送*/
    if ((result == ECommResultFail) && (comm_id == g_comm_id_music_ctrl))
    {
		//TODO:失败重试
        //os_printk("stock data request failed!!!\r\n");
        //send_music_control( );
    }
}

static void music_select_back(void *context)
{
	P_Window p_window = (P_Window)context;
	app_window_stack_pop(p_window);

    LOG_INFO("Pop window");
}

static void music_select_up(void *context)
{
    LOG_INFO("music_select_up window id : %d", g_window_id);
	P_Window p_old_window = app_window_stack_get_window_by_id(g_window_id);
	
	if(p_old_window == NULL)
	{
		return;
	}

	//发送音乐控制命令
	send_music_control(EMusicPrev);
	
	//按钮闪烁
	GRect    frame = {PREV_BUTN_POS_X, PREV_BUTN_POS_Y, PREV_BUTN_SIZE_H, PREV_BUTN_SIZE_W}; 

	add_bmp_layer(p_old_window, &g_prev_butn_id, BMP_MUSIC_BLACK, &frame, GAlignCenter, GColorWhite);
	app_window_update(p_old_window);
	
	add_bmp_layer(p_old_window, &g_prev_butn_id, BMP_MUSIC_PREV_BUTN, &frame, GAlignCenter, GColorWhite);
    app_window_update(p_old_window);
}

static void music_select_down(void *context)
{
    LOG_INFO("music_select_down window id : %d", g_window_id);
	P_Window p_old_window = app_window_stack_get_window_by_id(g_window_id);
	
	if(p_old_window == NULL)
	{
		return;
	}
    
	//发送音乐控制命令
	send_music_control(EMusicNext);

	GRect    frame = {NEXT_BUTN_POS_X, NEXT_BUTN_POS_Y, NEXT_BUTN_SIZE_H, NEXT_BUTN_SIZE_W}; ; 

	add_bmp_layer(p_old_window, &g_next_butn_id, BMP_MUSIC_BLACK, &frame, GAlignCenter, GColorWhite);
	app_window_update(p_old_window);
	
	add_bmp_layer(p_old_window, &g_next_butn_id, BMP_MUSIC_NEXT_BUTN, &frame, GAlignCenter, GColorWhite);
    app_window_update(p_old_window);
}

static void music_select_pause(void *context)
{
    LOG_INFO("music_select_pause window id : %d", g_window_id);
	P_Window p_old_window = app_window_stack_get_window_by_id(g_window_id);
	
	if(p_old_window == NULL)
	{
		return;
	}

	//发送音乐控制命令
	send_music_control(EMusicPause);

#if 1
	GRect    frame = {PAUSE_BUTN_POS_X, PAUSE_BUTN_POS_Y, PAUSE_BUTN_SIZE_H, PAUSE_BUTN_SIZE_W}; ; 

	add_bmp_layer(p_old_window, &g_pause_butn_id, BMP_MUSIC_BLACK, &frame, GAlignCenter, GColorWhite);
	app_window_update(p_old_window);
	
	add_bmp_layer(p_old_window, &g_pause_butn_id, BMP_MUSIC_PLAY_BUTN, &frame, GAlignCenter, GColorWhite);
    app_window_update(p_old_window);
#else
	GRect    frame; 
	GBitmap  bitmap;
    
    frame.origin.x = PAUSE_BUTN_POS_X;
    frame.origin.y = PAUSE_BUTN_POS_Y;
    frame.size.h   = PAUSE_BUTN_SIZE_H;
    frame.size.w   = PAUSE_BUTN_SIZE_W;
    
    if(g_music_paused)  
    {
        g_music_paused = 0;
        res_get_user_bitmap(BMP_MUSIC_PLAY_BUTN, &bitmap);   
    }
    else                
    {
        g_music_paused = 1;
        res_get_user_bitmap(BMP_MUSIC_PAUSE_BUTN, &bitmap);   
    }

    LayerBitmap  pause_butn_layer_bitmap = {bitmap, frame, GAlignCenter}; 
    P_Layer      new_pause_butn_layer = app_layer_create_bitmap(&pause_butn_layer_bitmap); 
    //app_layer_set_bg_color(new_pause_butn_layer, GColorBlack);

    Layer * p_old_layer = app_window_get_layer_by_id(p_old_window, g_pause_butn_id);
    
    LOG_INFO("Get old layer: 0x%x, g_pause_butn_id: %d", (uint32_t)p_old_layer, g_pause_butn_id);
    
	if(p_old_layer)
	{
        LOG_INFO("update pause icon.");
		g_pause_butn_id = app_window_replace_layer(p_old_window, p_old_layer, new_pause_butn_layer);
		app_window_update(p_old_window);
	}
#endif	
}

void music_set_singer(char *str, uint16_t len)
{
    LOG_INFO("music_set_singer window id : %d", g_window_id);
    
	 /*根据窗口ID获取窗口句柄*/
	P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
	if (NULL == p_window)
	{
        LOG_INFO("p_window = null");
		return ;
	}
    
	memset(g_singer, 0, 30);
	memcpy(g_singer, str, len);
    LOG_INFO("Singer: %s, len: %d", g_singer, len);
    
	GRect  frame = {SINGER_POS_X, SINGER_POS_Y, SINGER_SIZE_H, SINGER_SIZE_W}; 

	add_text_layer(p_window, &g_singer_id, g_singer, &frame, GAlignLeft, U_GBK_SIMSUN_14, GColorWhite);
	app_window_update(p_window);
	
    // LayerText singer_text = {(str==NULL)?"":(const char*)str, frame, GAlignLeft, U_GBK_SIMSUN_14};
    // P_Layer p_new_singer_layer = app_layer_create_text(&singer_text);
    
    // P_Layer p_old_singer_layer = app_window_get_layer_by_id(p_window, g_singer_id);
    // if(p_old_singer_layer)
    // {
        // LOG_INFO("music_set_singer update windows");
        // g_singer_id = app_window_replace_layer(p_window, p_old_singer_layer, p_new_singer_layer);
        // app_window_update(p_window);
    // }
}

static int get_utf8_size(char first_char)
{
	int num = 0, i = 0;

	//printf("input:%02x\n", input);
	for (i = 0; i < 8; i++)
	{
		if (((first_char >> (7 - i)) & 0x01))
		{
			num++;
		}
		else
		{
			break;
		}
	}

	return (num > 0) ? num : 1;
}

void music_set_song_name(char *str, uint16_t len)
{
    LOG_INFO("music_set_song_name window id : %d", g_window_id);
    
	 /*根据窗口ID获取窗口句柄*/
	P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
	if (NULL == p_window)
	{
        LOG_INFO("p_window = null");
		return ;
	}
    
    LOG_INFO("Song Name: %s, len: %d", str, len);
    
	memset(g_song_name_1, 0, 30);
	memset(g_song_name_2, 0, 30);
	
    int    line_1_index = 0, line_2_index = 0;
    int    char_num = 0; //英文的算1个，中文算2个
    int    utf8_char_len, i;
	
	if(str)
	{
		if(len <= ONE_LINE_MAX_SONG_NAME_CHAR_NUM * 2) //小于6个中文字符
		{
			LOG_INFO("less than 6 ch char");
			memcpy(g_song_name_1, str, len);
		}
		else
		{
			for(i=0; i<len;)
			{
				utf8_char_len = get_utf8_size(str[i]);
				//LOG_INFO("utf8_char_len: %d", utf8_char_len);
				if( (utf8_char_len == 0) || (utf8_char_len > 3) )
				{
					LOG_INFO("utf8 not recognized, char:%c, 0x%x", str[i], str[i]);
					break;
				}
				else if(utf8_char_len == 1)
				{
					char_num++;
				}
				else
				{
					char_num += 2;
				}
				
				if(char_num <= ONE_LINE_MAX_SONG_NAME_CHAR_NUM*2)
				{
					memcpy(&g_song_name_1[line_1_index], &str[i], utf8_char_len);
					line_1_index += utf8_char_len;
				}
				else if(char_num <= ONE_LINE_MAX_SONG_NAME_CHAR_NUM*4)
				{
					memcpy(&g_song_name_2[line_2_index], &str[i], utf8_char_len);
					line_2_index += utf8_char_len;
				}
				
				i += utf8_char_len;
			}
		}
	}

    LOG_INFO("iiiiiiiiiiiiii:%d", i);
    
    GRect  frame;
    frame.origin.x = SONG_NAME_LINE_1_POS_X;
    frame.origin.y = SONG_NAME_LINE_1_POS_Y;
    frame.size.h   = SONG_NAME_LINE_1_SIZE_H;
    frame.size.w   = SONG_NAME_LINE_1_SIZE_W;
	
    add_text_layer(p_window, &g_song_name_line_1_id, g_song_name_1, &frame, GAlignLeft, U_GBK_SIMSUN_16, GColorWhite);
	
	frame.origin.x = SONG_NAME_LINE_2_POS_X;
    frame.origin.y = SONG_NAME_LINE_2_POS_Y;
    frame.size.h   = SONG_NAME_LINE_2_SIZE_H;
    frame.size.w   = SONG_NAME_LINE_2_SIZE_W;
	
	add_text_layer(p_window, &g_song_name_line_2_id, g_song_name_2, &frame, GAlignLeft, U_GBK_SIMSUN_16, GColorWhite);
	
	app_window_update(p_window);
	
    // LayerText song_name_line_1_text = {(const char*)line_1_buf, frame, GAlignLeft, U_GBK_SIMSUN_16};
    // P_Layer   p_new_song_name_line_1_layer = app_layer_create_text(&song_name_line_1_text);
    
    // frame.origin.x = SONG_NAME_LINE_2_POS_X;
    // frame.origin.y = SONG_NAME_LINE_2_POS_Y;
    // frame.size.h   = SONG_NAME_LINE_2_SIZE_H;
    // frame.size.w   = SONG_NAME_LINE_2_SIZE_W;
    
    // LayerText song_name_line_2_text = {(const char*)line_2_buf, frame, GAlignLeft, U_GBK_SIMSUN_16};
    // P_Layer    p_new_song_name_line_2_layer = app_layer_create_text(&song_name_line_2_text);
    
    // P_Layer    p_old_song_name_line_1_layer = app_window_get_layer_by_id(p_window, g_song_name_line_1_id);
    // if(p_old_song_name_line_1_layer)
    // {
        // LOG_INFO("music_set_song_name  app_window_replace_layer line1");
        // g_song_name_line_1_id = app_window_replace_layer(p_window, p_old_song_name_line_1_layer, p_new_song_name_line_1_layer);
    // }
    
    // P_Layer    p_old_song_name_line_2_layer = app_window_get_layer_by_id(p_window, g_song_name_line_2_id);
    // if(p_old_song_name_line_2_layer)
    // {
        // LOG_INFO("music_set_song_name  app_window_replace_layer line2");
        // g_song_name_line_2_id = app_window_replace_layer(p_window, p_old_song_name_line_2_layer, p_new_song_name_line_2_layer);
    // }
    
    // app_window_update(p_window);
}

static void music_time_change(enum SysEventType type, void *context)
{    
	/*时间更改，分变化*/
	if (type == SysEventTypeTimeChange)
	{
        static int8_t pre_min = 0;
        
         /*根据窗口ID获取窗口句柄*/
        P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
        if (NULL == p_window)
        {
            LOG_INFO("NULL == p_window");
            return ;
        }
        
        /*更新时间*/
        char str[20];
        struct date_time datetime;
        app_service_get_datetime(&datetime);
        if (pre_min != datetime.min)
        {
            pre_min = datetime.min;
            P_Layer p_hm_layer = app_window_get_layer_by_id(p_window, g_time_layer_id);
            if (NULL == p_hm_layer)
            {
                return;
            }

            sprintf(str, "%02d:%02d", datetime.hour, datetime.min);
            app_layer_set_text_text(p_hm_layer, str);
        }
        
        app_window_update(p_window);
	}
}

// static void music_time_change(enum SysEventType type, void *context)
// {    
	// static uint32_t pre_min;
	
	// /*时间更改，分变化*/
	// if (type == SysEventTypeTimeChange)
	// {
         // /*根据窗口ID获取窗口句柄*/
        // P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
        // if (NULL == p_window)
        // {
            // LOG_INFO("NULL == p_window");
            // return ;
        // }
        
        // /*更新时间*/
        // char str[20];
        // struct date_time datetime;
        // app_service_get_datetime(&datetime);

        // if (pre_min != datetime.min)
        // {
			// pre_min = datetime.min;
			// sprintf(str, "%02d:%02d", datetime.hour, datetime.min);
			
			// add_text_layer(p_window, &g_time_layer_id, &str, GRect *p_frame, enum GAlign align, int8_t font, enum GColor color);
			
            
            // P_Layer p_hm_layer = app_window_get_layer_by_id(p_window, g_time_layer_id);
            // if (NULL == p_hm_layer)
            // {
                // return;
            // }

            // sprintf(str, "%02d:%02d", datetime.hour, datetime.min);
            // app_layer_set_text_text(p_hm_layer, str);
        // }
        
        // app_window_update(p_window);
	// }
// }


void app_music_init(void)
{
	P_Window  p_window;
	
    LOG_INFO("Init Music App.");
	/*创建窗口，窗口中可以添加唯一的基本元素图层*/
	p_window = app_window_create();
    
	/*添加时间栏图层*/
	GRect time_frame = {{TIME_BAR_POS_X, TIME_BAR_POS_Y}, 
				{TIME_BAR_SIZE_H, TIME_BAR_SIZE_W}};
	struct date_time datetime;
	app_service_get_datetime(&datetime);
	char buf[6];
	sprintf(buf, "%02d:%02d", datetime.hour, datetime.min);
	
	add_text_layer(p_window, &g_time_layer_id, buf, &time_frame, GAlignCenter, U_ASCII_ARIAL_12, GColorWhite);
    
	/* 添加音乐控制条背景 */
	int32_t side_bar_id = -1;
	GRect        frame = {{SIDE_BAR_POS_X, SIDE_BAR_POS_Y}, {SIDE_BAR_SIZE_H, SIDE_BAR_SIZE_W}}; 
	
	add_bmp_layer(p_window, &side_bar_id, BMP_MUSIC_SIDE_BAR, &frame, GAlignCenter, GColorWhite);
	
	// GBitmap      bitmap;	
    // res_get_user_bitmap(BMP_MUSIC_SIDE_BAR, &bitmap);
    // LayerBitmap side_bar_layer_bitmap = {bitmap, frame, GAlignCenter}; 
    // P_Layer side_bar_layer = app_layer_create_bitmap(&side_bar_layer_bitmap); 
    // app_window_add_layer(p_window, side_bar_layer);
    
    /* 添加音乐上一首按钮 */
    frame.origin.x = PREV_BUTN_POS_X;
    frame.origin.y = PREV_BUTN_POS_Y;
    frame.size.h   = PREV_BUTN_SIZE_H;
    frame.size.w   = PREV_BUTN_SIZE_W;
    
	add_bmp_layer(p_window, &g_prev_butn_id, BMP_MUSIC_PREV_BUTN, &frame, GAlignCenter, GColorWhite);
	
    // res_get_user_bitmap(BMP_MUSIC_PREV_BUTN, &bitmap);
    // LayerBitmap  prev_butn_layer_bitmap = {bitmap, frame, GAlignCenter}; 
    // P_Layer      prev_butn_layer = app_layer_create_bitmap(&prev_butn_layer_bitmap); 
    // g_prev_butn_id = app_window_add_layer(p_window, prev_butn_layer);

    /* 添加音乐暂停按钮 */
    frame.origin.x = PAUSE_BUTN_POS_X;
    frame.origin.y = PAUSE_BUTN_POS_Y;
    frame.size.h   = PAUSE_BUTN_SIZE_H;
    frame.size.w   = PAUSE_BUTN_SIZE_W;
    
	add_bmp_layer(p_window, &g_pause_butn_id, BMP_MUSIC_PLAY_BUTN, &frame, GAlignCenter, GColorWhite);
	g_music_paused = 0;
    // res_get_user_bitmap(BMP_MUSIC_PLAY_BUTN, &bitmap);
    // LayerBitmap  pause_butn_layer_bitmap = {bitmap, frame, GAlignCenter}; 
    // P_Layer      pause_butn_layer = app_layer_create_bitmap(&pause_butn_layer_bitmap); 
    // g_pause_butn_id = app_window_add_layer(p_window, pause_butn_layer);
    
    /* 添加音乐下一首按钮 */
    frame.origin.x = NEXT_BUTN_POS_X;
    frame.origin.y = NEXT_BUTN_POS_Y;
    frame.size.h   = NEXT_BUTN_SIZE_H;
    frame.size.w   = NEXT_BUTN_SIZE_W;
    
	add_bmp_layer(p_window, &g_next_butn_id, BMP_MUSIC_NEXT_BUTN, &frame, GAlignCenter, GColorWhite);
	
    // res_get_user_bitmap(BMP_MUSIC_NEXT_BUTN, &bitmap);
    // LayerBitmap  next_butn_layer_bitmap = {bitmap, frame, GAlignCenter}; 
    // P_Layer      next_butn_layer = app_layer_create_bitmap(&next_butn_layer_bitmap); 
    // g_next_butn_id = app_window_add_layer(p_window, next_butn_layer);

	
    /* 添加歌手图层 */
    frame.origin.x = SINGER_POS_X;
    frame.origin.y = SINGER_POS_Y;
    frame.size.h   = SINGER_SIZE_H;
    frame.size.w   = SINGER_SIZE_W;

	add_text_layer(p_window, &g_singer_id, g_singer, &frame, GAlignLeft, U_GBK_SIMSUN_14, GColorWhite);
	
    /* 添加歌名图层 */
    frame.origin.x = SONG_NAME_LINE_1_POS_X;
    frame.origin.y = SONG_NAME_LINE_1_POS_Y;
    frame.size.h   = SONG_NAME_LINE_1_SIZE_H;
    frame.size.w   = SONG_NAME_LINE_1_SIZE_W;
    
	add_text_layer(p_window, &g_song_name_line_1_id, g_song_name_1, &frame, GAlignLeft, U_GBK_SIMSUN_16, GColorWhite);

    frame.origin.x = SONG_NAME_LINE_2_POS_X;
    frame.origin.y = SONG_NAME_LINE_2_POS_Y;
    frame.size.h   = SONG_NAME_LINE_2_SIZE_H;
    frame.size.w   = SONG_NAME_LINE_2_SIZE_W;
    
    add_text_layer(p_window, &g_song_name_line_2_id, g_song_name_2, &frame, GAlignLeft, U_GBK_SIMSUN_16, GColorWhite);
    

	/*添加按键事件，实现上下翻页功能*/
	app_window_click_subscribe(p_window, ButtonIdDown,   music_select_down);
	app_window_click_subscribe(p_window, ButtonIdUp,     music_select_up);
    app_window_click_subscribe(p_window, ButtonIdSelect, music_select_pause);
	app_window_click_subscribe(p_window, ButtonIdBack,   music_select_back);

	//注册通讯结果回调
    maibu_comm_register_result_callback(music_comm_result_callback);
	
	/* 注册音乐数据回调 */
	maibu_comm_register_phone_callback(music_data_receive_callback);
	
	/*注册一个事件通知回调，当有时间改变是，立即更新时间*/
    maibu_service_sys_event_subscribe(music_time_change);
    
    /*把窗口放入窗口栈中显示*/
	g_window_id = app_window_stack_push(p_window);
}


#ifndef QT_PLATFORM

int main(void)
{
    app_music_init();
}

#endif


