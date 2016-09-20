/*
 * =====================================================================================
 * 
 *    Corporation:  Shenzhen Maibu Technology Co., Ltd. All Rights Reserved.
 *       Filename:  app_seven_sports.c
 *         Author:  gliu , gliu@maibu.cc
 *        Created:  2015年09月16日 09时58分07秒
 * 
 *    Description:  七分钟运动 
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


/*开始倒计时时间图层位置*/
#define SEVEN_SPORTS_START_TIME_ORIGIN_X		0	
#define SEVEN_SPORTS_START_TIME_ORIGIN_Y		27
#define SEVEN_SPORTS_START_TIME_SIZE_H			42	
#define SEVEN_SPORTS_START_TIME_SIZE_W			128	

/*休息文本提示图层位置*/
#define SEVEN_SPORTS_START_HINT_ORIGIN_X		0	
#define SEVEN_SPORTS_START_HINT_ORIGIN_Y		82
#define SEVEN_SPORTS_START_HINT_SIZE_H			12	
#define SEVEN_SPORTS_START_HINT_SIZE_W			128	

/*开始倒计时提示图层位置*/
#define SEVEN_SPORTS_START_READY_ORIGIN_X		0	
#define SEVEN_SPORTS_START_READY_ORIGIN_Y		82
#define SEVEN_SPORTS_START_READY_SIZE_H			46	
#define SEVEN_SPORTS_START_READY_SIZE_W			128	

/*动作文本图片提示图层*/
#define SEVEN_SPORTS_ACTION_BMP_TEXT_HINT_ORIGIN_X	0	
#define SEVEN_SPORTS_ACTION_BMP_TEXT_HINT_ORIGIN_Y	0
#define SEVEN_SPORTS_ACTION_BMP_TEXT_HINT_SIZE_H	128
#define SEVEN_SPORTS_ACTION_BMP_TEXT_HINT_SIZE_W	128

/*休息图层*/
#define SEVEN_SPORTS_REST_HINT_ORIGIN_X			0
#define SEVEN_SPORTS_REST_HINT_ORIGIN_Y			34
#define SEVEN_SPORTS_REST_HINT_SIZE_H			94	
#define SEVEN_SPORTS_REST_HINT_SIZE_W			128

/*进度条变化矩形位置*/
#define SEVEN_SPORTS_PROGRESS_BAR_GEO_ORIGIN_X		0	
#define SEVEN_SPORTS_PROGRESS_BAR_GEO_ORIGIN_Y		119	
#define SEVEN_SPORTS_PROGRESS_BAR_GEO_SIZE_H		8	
#define SEVEN_SPORTS_PROGRESS_BAR_GEO_SIZE_W		4


/*锻炼完成图标位置*/
#define SEVEN_SPORTS_END_BMP_ORIGIN_X		32	
#define SEVEN_SPORTS_END_BMP_ORIGIN_Y		24
#define SEVEN_SPORTS_END_BMP_SIZE_H			78	
#define SEVEN_SPORTS_END_BMP_SIZE_W			63


/*窗口ID*/
static int32_t g_seven_sports_window_id = -1;

/*进度条图层ID*/
static int8_t g_progress_bar_layer_id = -1;

/*开始倒计时图层ID*/
static int8_t g_start_time_text_layer_id = -1;



#define SEVEN_SPORTS_STATE_START		1
#define SEVEN_SPORTS_STATE_ACTION		2
#define SEVEN_SPORTS_STATE_REST			3
#define SEVEN_SPORTS_STATE_FINISH		4


#define SEVEN_SPORTS_START_DURATION		3
#define SEVEN_SPORTS_ACTION_DURATION		30
#define SEVEN_SPORTS_REST_DURATION		10
#define SEVEN_SPORTS_ACTION_REST_DURATION	40

#define SEVEN_SPORTS_TOTAL_TIME	(3+(30+10)*12 - 10)


#define SEVEN_SPORTS_PROGRESS_BAR_WIDTH		128


/*总计时*/
int16_t g_seven_sports_time_count = 0;

/*定时器ID*/
int8_t g_seven_sports_time_id = -1;

/*动作图片数组*/
int32_t seven_sports_get_action_bmp_key[12]=
{
	RES_BITMAP_WATCHAPP_SEVEN_SPORTS_ACTION1,
	RES_BITMAP_WATCHAPP_SEVEN_SPORTS_ACTION2,
	RES_BITMAP_WATCHAPP_SEVEN_SPORTS_ACTION3,
	RES_BITMAP_WATCHAPP_SEVEN_SPORTS_ACTION4,
	RES_BITMAP_WATCHAPP_SEVEN_SPORTS_ACTION5,
	RES_BITMAP_WATCHAPP_SEVEN_SPORTS_ACTION6,
	RES_BITMAP_WATCHAPP_SEVEN_SPORTS_ACTION7,
	RES_BITMAP_WATCHAPP_SEVEN_SPORTS_ACTION8,
	RES_BITMAP_WATCHAPP_SEVEN_SPORTS_ACTION9,
	RES_BITMAP_WATCHAPP_SEVEN_SPORTS_ACTION10,
	RES_BITMAP_WATCHAPP_SEVEN_SPORTS_ACTION11,
	RES_BITMAP_WATCHAPP_SEVEN_SPORTS_ACTION12		
};

/*其他图片数组*/
int32_t seven_sports_others_bmp_key[3]=
{
	RES_BITMAP_WATCHAPP_SEVEN_SPORTS_READY,
	RES_BITMAP_WATCHAPP_SEVEN_SPORTS_REST,
	RES_BITMAP_WATCHAPP_SEVEN_SPORTS_END_BMP,

};


/*动作名字数组*/
char seven_sports_get_action_name[12][16]=
{
	"开合跳",
	"靠墙直角坐",
	"俯卧撑",
	"仰卧起坐",
	"单腿上椅立",
	"蹲起",
	"背椅仰卧撑",
	"平板支撑",
	"原地抬腿跑",
	"弓箭步",
	"俯卧侧转",
	"侧平板支撑"
};



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

/*创建并显示文本图层*/
int32_t display_target_layerText(P_Window p_window,GRect  *temp_p_frame,enum GAlign how_to_align,enum GColor color,char * str,uint8_t font_type)
{
	LayerText temp_LayerText = {0};
	temp_LayerText.text = str;
	temp_LayerText.frame = *temp_p_frame;
	temp_LayerText.alignment = how_to_align;
	temp_LayerText.font_type = font_type;
	
	P_Layer p_layer = app_layer_create_text(&temp_LayerText);
	
	if(p_layer != NULL)
	{
		app_layer_set_bg_color(p_layer, color);
		return app_window_add_layer(p_window, p_layer);
	}
	return 0;
}


/*定义后退按键事件*/
static void seven_sports_select_back(void *context)
{
	P_Window p_window = (P_Window)context;
	if (NULL != p_window)
	{
		app_window_stack_pop(p_window);
	}

	/*取消定时器*/	
	app_service_timer_unsubscribe(g_seven_sports_time_id);	

	/*清零*/
	g_seven_sports_time_count = 0;
	g_seven_sports_time_id = -1;
	g_progress_bar_layer_id = -1;
}



/*
 *--------------------------------------------------------------------------------------
 *     function:  seven_sports_get_state
 *    parameter: 
 *       return:
 *  description:  获取当前状态
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
void seven_sports_get_state(int8_t *state, int8_t *sub_state, int8_t *progress)
{


	/*开始计时状态*/
	if (g_seven_sports_time_count < SEVEN_SPORTS_START_DURATION)
	{
		*state = SEVEN_SPORTS_STATE_START;
		*progress = g_seven_sports_time_count;			
		return;
	}

	/*任务完成状态*/
	if (g_seven_sports_time_count >= SEVEN_SPORTS_TOTAL_TIME)
	{
		*state = SEVEN_SPORTS_STATE_FINISH;
		return;
	}

		
	int32_t action_rest_time = g_seven_sports_time_count - SEVEN_SPORTS_START_DURATION;
	int8_t action_rest_count = action_rest_time / SEVEN_SPORTS_ACTION_REST_DURATION;
	int8_t action_rest_interval = action_rest_time % SEVEN_SPORTS_ACTION_REST_DURATION;	
	
	if ((action_rest_interval >= SEVEN_SPORTS_ACTION_DURATION)) 	
	{

		*state = SEVEN_SPORTS_STATE_REST;
		*sub_state = action_rest_count + 2;
		*progress = action_rest_interval == 0 ? SEVEN_SPORTS_REST_DURATION: (action_rest_interval - SEVEN_SPORTS_ACTION_DURATION);

		//printf("rrrrrrrrrrrrrrrrrrrrrrrrrrrrrr  %d,  %d, %d\n", g_seven_sports_time_count, *sub_state, *progress);

		return; 
	}


	*state = SEVEN_SPORTS_STATE_ACTION;
	*sub_state = action_rest_count + 1;
	*progress = action_rest_interval; 
	return;	
} 






/*
 *--------------------------------------------------------------------------------------
 *     function:  seven_sports_create_action_window
 *    parameter: 
 *       return:
 *  description:  创建动作展示窗口
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Window seven_sports_create_action_window(int8_t action_index)
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	GRect action_frame ={{SEVEN_SPORTS_ACTION_BMP_TEXT_HINT_ORIGIN_X,SEVEN_SPORTS_ACTION_BMP_TEXT_HINT_ORIGIN_Y},
						 {SEVEN_SPORTS_ACTION_BMP_TEXT_HINT_SIZE_H,SEVEN_SPORTS_ACTION_BMP_TEXT_HINT_SIZE_W}};

	/*创建动作图层*/
	display_target_layer(p_window,&action_frame,GAlignCenter,GColorWhite,seven_sports_get_action_bmp_key,action_index-1);

	/*后退按键回调*/
	app_window_click_subscribe(p_window, ButtonIdBack, seven_sports_select_back);

	return (p_window);
}




/*
 *--------------------------------------------------------------------------------------
 *     function:  seven_sports_create_rest_window
 *    parameter: 
 *       return:
 *  description:  创建休息窗口
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Window seven_sports_create_rest_window()
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}


	/*添加休息图层*/
	GRect time_rest_frame = {{SEVEN_SPORTS_REST_HINT_ORIGIN_X, SEVEN_SPORTS_REST_HINT_ORIGIN_Y}, 
				{SEVEN_SPORTS_REST_HINT_SIZE_H, SEVEN_SPORTS_REST_HINT_SIZE_W}};

	display_target_layer(p_window,&time_rest_frame,GAlignCenter,GColorWhite,seven_sports_others_bmp_key,1);


	/*添加休息文本提示*/
	time_rest_frame.origin.x = SEVEN_SPORTS_START_HINT_ORIGIN_X;
	time_rest_frame.origin.y = SEVEN_SPORTS_START_HINT_ORIGIN_Y;
	time_rest_frame.size.h = SEVEN_SPORTS_START_HINT_SIZE_H;
	time_rest_frame.size.w = SEVEN_SPORTS_START_HINT_SIZE_W;
	
	int8_t state = 0, sub_state = 0, progress = 0;
	seven_sports_get_state(&state, &sub_state, &progress);

	char buf[30] = "";
	sprintf(buf, "下一步 %s", seven_sports_get_action_name[sub_state-1]);
	
	display_target_layerText(p_window,&time_rest_frame,GAlignCenter,GColorWhite,buf,U_GBK_SIMSUN_12);

	/*后退按键回调*/
	app_window_click_subscribe(p_window, ButtonIdBack, seven_sports_select_back);

	return (p_window);
}


/*
 *--------------------------------------------------------------------------------------
 *     function:  seven_sports_create_start_window
 *    parameter: 
 *       return:
 *  description:  创建开始倒计时窗口
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Window seven_sports_create_start_window()
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	/*添加倒计时数字图层*/
	GRect time_start_frame = {{SEVEN_SPORTS_START_TIME_ORIGIN_X, SEVEN_SPORTS_START_TIME_ORIGIN_Y}, 
				{SEVEN_SPORTS_START_TIME_SIZE_H, SEVEN_SPORTS_START_TIME_SIZE_W}};

	char buf[2] = "3";

	g_start_time_text_layer_id = display_target_layerText(p_window,&time_start_frame,GAlignCenter,GColorWhite,buf,U_ASCII_ARIAL_42);

	/*创建文字进度条背景*/
	time_start_frame.origin.x = SEVEN_SPORTS_START_READY_ORIGIN_X;
	time_start_frame.origin.y = SEVEN_SPORTS_START_READY_ORIGIN_Y;
	time_start_frame.size.h = SEVEN_SPORTS_START_READY_SIZE_H;
	time_start_frame.size.w = SEVEN_SPORTS_START_READY_SIZE_W;

	display_target_layer(p_window,&time_start_frame,GAlignCenter,GColorWhite,seven_sports_others_bmp_key,0);
	
	/*后退按键回调*/
	app_window_click_subscribe(p_window, ButtonIdBack, seven_sports_select_back);

	return (p_window);
}



/*
 *--------------------------------------------------------------------------------------
 *     function:  seven_sports_create_end_window
 *    parameter: 
 *       return:
 *  description:  创建完成窗口
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Window seven_sports_create_end_window()
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	/*添加完成图片图层*/
	GRect frame_end_bmp = {{SEVEN_SPORTS_END_BMP_ORIGIN_X, SEVEN_SPORTS_END_BMP_ORIGIN_Y}, 
			{ SEVEN_SPORTS_END_BMP_SIZE_H, SEVEN_SPORTS_END_BMP_SIZE_W}};

	display_target_layer(p_window,&frame_end_bmp,GAlignCenter,GColorWhite,seven_sports_others_bmp_key,2);


	/*后退按键回调*/
	app_window_click_subscribe(p_window, ButtonIdBack, seven_sports_select_back);

	return (p_window);
}


uint8_t seven_sports_get_progress_width(uint8_t progress, uint8_t total_time)
{
        static uint8_t pre_progress = 0;
        uint8_t gap = SEVEN_SPORTS_PROGRESS_BAR_WIDTH / total_time;
        uint8_t res = SEVEN_SPORTS_PROGRESS_BAR_WIDTH - (gap * total_time);
        uint8_t extra = progress > res ? 0 : 1;


        if (progress == 1)
        {
                pre_progress = 0;
                pre_progress = gap;
        }
        else
        {
                pre_progress = (pre_progress + (SEVEN_SPORTS_PROGRESS_BAR_WIDTH - pre_progress)/(total_time + 1 - progress)) + extra;
        }

        return pre_progress;
}


static void seven_sports_timer_callback(date_time_t tick_time, uint32_t millis, void *context)
{
	int16_t w = 0;
	g_seven_sports_time_count++;

	/*时间到，先关掉定时器*/
	if (g_seven_sports_time_count >= SEVEN_SPORTS_TOTAL_TIME)
	{
		/*取消定时器*/	
		app_service_timer_unsubscribe(g_seven_sports_time_id);	
	}


	P_Window p_window = app_window_stack_get_window_by_id(g_seven_sports_window_id);	
	if (NULL == p_window)
	{
		return;
	}

	/*时间到, 展示完成界面*/
	if (g_seven_sports_time_count >= SEVEN_SPORTS_TOTAL_TIME)
	{

		/*完成窗口*/
		P_Window p_new_window = seven_sports_create_end_window();
		g_seven_sports_window_id = app_window_stack_replace_window(p_window, p_new_window);	

		/*震动*/
		maibu_service_vibes_pulse(VibesPulseTypeShort, 0);

		return;
	}


	/*获取当前状态*/
	int8_t state = 0, sub_state = 0, progress = 0;
	seven_sports_get_state(&state, &sub_state, &progress);
	//printf("gggggggggggggggg: %d, %d, %d\n", state, sub_state, progress);

	if (state == SEVEN_SPORTS_STATE_START)
	{
		P_Layer p_text_layer = app_window_get_layer_by_id(p_window, g_start_time_text_layer_id);
		if (NULL != p_text_layer)	
		{
			char tmp[2] = "";
			sprintf(tmp, "%d", SEVEN_SPORTS_START_DURATION - progress);
			app_layer_set_text_text(p_text_layer, tmp);	
		
			w = seven_sports_get_progress_width(progress, SEVEN_SPORTS_START_DURATION);
		
		}

	}

	else if (state == SEVEN_SPORTS_STATE_ACTION)
	{
		/*如果progress==0, 则需要创建新的窗口*/
		if (progress == 0)
		{
			g_progress_bar_layer_id = -1;
			P_Window p_new_window = seven_sports_create_action_window(sub_state);
			g_seven_sports_window_id = app_window_stack_replace_window(p_window, p_new_window);	
			p_window = p_new_window;

			/*震动*/
			maibu_service_vibes_pulse(VibesPulseTypeShort, 0);
		}	
		else 
		{

			w = seven_sports_get_progress_width(progress,   SEVEN_SPORTS_ACTION_DURATION);
		}	

	}

	else if (state == SEVEN_SPORTS_STATE_REST)
	{
		
		/*如果progress==0, 则需要创建新的窗口*/
		if (progress == 0)
		{
			g_progress_bar_layer_id = -1;
			P_Window p_new_window = seven_sports_create_rest_window();
			g_seven_sports_window_id = app_window_stack_replace_window(p_window, p_new_window);	
			p_window = p_new_window;

			/*震动*/
			maibu_service_vibes_pulse(VibesPulseTypeShort, 0);

		}
		else
		{
			w = seven_sports_get_progress_width(progress,   SEVEN_SPORTS_REST_DURATION);
		}	
	}


	/*任何一种状态进度为0时，不需要创建矩形去覆盖进度条图片*/
	if (progress != 0)
	{

		/*多边形*/
		Geometry *geometry[1]; 
		uint8_t num = 0;
		LayerGeometry layer_geometry;	/*几何图层结构体*/
		memset(geometry, 0, sizeof(geometry));
		GPoint ppoint[4] = {{SEVEN_SPORTS_PROGRESS_BAR_GEO_ORIGIN_X, SEVEN_SPORTS_PROGRESS_BAR_GEO_ORIGIN_Y}, 
					{w, SEVEN_SPORTS_PROGRESS_BAR_GEO_ORIGIN_Y}, 
					{w, SEVEN_SPORTS_PROGRESS_BAR_GEO_ORIGIN_Y+SEVEN_SPORTS_PROGRESS_BAR_GEO_SIZE_H}, 
					{SEVEN_SPORTS_PROGRESS_BAR_GEO_ORIGIN_X, SEVEN_SPORTS_PROGRESS_BAR_GEO_ORIGIN_Y+SEVEN_SPORTS_PROGRESS_BAR_GEO_SIZE_H}};
		Polygon p = {4, ppoint};	
		Geometry pg = {GeometryTypePolygon, FillArea, GColorWhite,(void*)&p}; 
		geometry[num++] = &pg;

		/*初始化几何结构体*/
		layer_geometry.num = num;
		layer_geometry.p_g = geometry;


		//printf("pppppppppppp: %d----%d, %d-----%d, %d-----%d, %d-----%d\n", ppoint[0].x, ppoint[0].y, ppoint[1].x,  ppoint[1].y, ppoint[2].x,  ppoint[2].y,  ppoint[3].x, ppoint[3].y);

		P_Layer	 p_new_layer = app_layer_create_geometry(&layer_geometry);
		P_Layer p_old_layer = app_window_get_layer_by_id(p_window, g_progress_bar_layer_id);
		if (NULL != p_old_layer)
		{
			/*替换*/
			app_window_replace_layer(p_window, p_old_layer, p_new_layer);		
		}
		else
		{
			/*创建几何图层*/
			g_progress_bar_layer_id = app_window_add_layer(p_window, p_new_layer);
		}

		app_window_update(p_window);
	}


	//printf("wwwwwwww: %d, count: %d\n ", w, g_seven_sports_time_count);


}



/*
 *--------------------------------------------------------------------------------------
 *     function:  seven_sports_create_init_window
 *    parameter: 
 *       return:
 *  description:  
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Window seven_sports_create_init_window()
{
	P_Window p_window = NULL;


	/*获取当前状态*/
	int8_t state = 0, sub_state = 0, progress = 0;
	seven_sports_get_state(&state, &sub_state, &progress);
	if (state == SEVEN_SPORTS_STATE_START)
	{
		p_window = seven_sports_create_start_window();	

		/*添加定时器*/	
		g_seven_sports_time_id = app_service_timer_subscribe(1000, seven_sports_timer_callback, (void *)p_window);	
	}
	else if (state == SEVEN_SPORTS_STATE_ACTION)
	{
		p_window = seven_sports_create_action_window(sub_state);	
	}
	else if (state == SEVEN_SPORTS_STATE_REST)
	{
		p_window = seven_sports_create_rest_window();	
	}
	else if (state == SEVEN_SPORTS_STATE_FINISH)
	{
		/*取消定时器, 清零*/	
		app_service_timer_unsubscribe(g_seven_sports_time_id);	
		p_window = seven_sports_create_end_window();
	}


	return p_window;
}


int main()
{


	P_Window p_window = seven_sports_create_init_window(); 

	/*放入窗口栈显示*/
	g_seven_sports_window_id = app_window_stack_push(p_window);

	return 0;

}

