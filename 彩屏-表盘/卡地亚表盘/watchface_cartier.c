/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  app_cartier_watch.c
 *
 *    Description:  卡地亚手表
 *    Corporation:
 * 
 *         Author:  gliu (), gliu@damaijiankang.com
 *        Created:  2015年03月31日 15时09分27秒
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

#include "maibu_res.h"
#include "maibu_sdk.h"



static int32_t g_cartier_window = -1;


/*窗口图层位置*/
/*背景图层*/
#define CARTIER_BG_ORIGIN_X			0
#define CARTIER_BG_ORIGIN_Y			0
#define CARTIER_BG_SIZE_H			176
#define CARTIER_BG_SIZE_W			176


/*中心图片*/
#define CARTIER_CENTER_ORIGIN_X		86
#define CARTIER_CENTER_ORIGIN_Y		86
#define CARTIER_CENTER_SIZE_H		4
#define CARTIER_CENTER_SIZE_W		4


/*表盘中心位置*/
#define CX 88
#define CY 88

static const uint8_t hour_pos[36][3][2] = 
{
//0:
{{88,49},{83,88},{94,88}},
//0:20: 
{{95,50},{83,87},{94,90}},
//0:40:
{{102,51},{83,87},{94,90}},

//1:
{{109,54},{84,86},{93,91}},
//1:20: 
{{115,60},{86,84},{93,91}},
//1:40:
{{119,64},{86,84},{93,91}},

//2:
{{123,69},{86,84},{91,93}},
//2:20: 
{{126,76},{87,83},{90,94}},
//2:40:
{{127,82},{88,83},{90,94}},

//3:
{{127,88},{88,83},{88,94}},
//3:20: 
{{127,94},{90,83},{88,94}},
//3:40:
{{126,99},{90,83},{87,93}},

//4:
{{123,106},{90,83},{84,93}},
//4:20: 
{{119,112},{91,84},{84,91}},
//4:40:
{{115,116},{91,84},{84,91}},

//5:
{{109,121},{93,84},{84,90}},
//5:20: 
{{102,124},{93,86},{83,88}},
//5:40:
{{95,126},{93,86},{83,88}},

//6:
{{88,127},{94,87},{83,87}},
//6:20: 
{{80,126},{94,90},{83,87}},
//6:40:
{{73,124},{94,90},{84,86}},

//7:
{{66,121},{94,90},{84,84}},
//7:20: 
{{61,116},{93,93},{84,84}},
//7:40:
{{57,112},{91,93},{84,84}},

//8:
{{53,106},{91,93},{84,84}},
//8:20: 
{{50,99},{90,94},{84,84}},
//8:40:
{{49,94},{87,94},{87,83}},

//9:
{{49,88},{87,94},{87,83}},
//9:20: 
{{49,82},{87,94},{87,83}},
//9:40:
{{50,76},{84,93},{88,83}},

//10:
{{53,69},{84,93},{90,83}},
//10:20: 
{{57,64},{84,93},{90,83}},
//10:40:
{{61,60},{84,91},{91,84}},

//11:
{{66,54},{83,90},{93,84}},
//11:20: 
{{73,51},{83,90},{93,86}},
//11:40:
{{80,50},{83,90},{94,87}},

};


static const uint8_t min_pos[60][3][2] = 
{	
//0分：
{{88,18},{84,87},{93,87}},
//1分：
{{95,17},{86,86},{93,86}},
//2分：
{{104,18},{86,86},{94,86}},
//3分：
{{110,20},{86,86},{94,87}},
//4分：
{{117,24},{86,84},{93,88}},
//5分：
{{124,27},{86,84},{94,88}},
//6分：
{{130,31},{86,86},{93,91}},
//7分：
{{135,35},{86,86},{93,91}},
//8分：
{{141,42},{87,84},{91,93}},
//9分：
{{145,47},{87,84},{91,93}},
//10分：
{{149,53},{88,84},{91,93}},
//11分：
{{153,60},{87,84},{91,93}},
//12分：
{{156,66},{88,84},{90,93}},
//13分：
{{157,73},{90,84},{90,93}},
//14分：
{{159,82},{90,84},{90,93}},
//15分：
{{159,88},{88,84},{88,94}},
//16分：
{{160,95},{91,84},{91,93}},
//17分：
{{157,104},{90,84},{90,93}},
//18分：
{{156,110},{90,84},{88,93}},
//19分：
{{153,117},{90,84},{87,93}},
//20分：
{{149,124},{91,86},{88,94}},
//21分：
{{145,130},{91,86},{86,91}},
//22分：
{{141,135},{91,86},{87,93}},
//23分：
{{135,141},{91,84},{86,91}},
//24分：
{{130,145},{93,86},{86,91}},
//25分：
{{123,149},{93,86},{87,94}},
//26分：
{{117,152},{93,87},{84,91}},
//27分：
{{110,156},{93,88},{84,90}},
//28分：
{{104,157},{93,90},{84,90}},
//29分：
{{95,159},{93,90},{84,90}},
//30分：
{{88,159},{84,88},{94,91}},
//31分：
{{80,159},{93,90},{84,90}},
//32分：
{{73,157},{93,90},{84,90}},
//33分：
{{66,156},{93,91},{84,90}},
//34分：
{{60,153},{93,91},{84,88}},
//35分：
{{53,149},{93,90},{84,88}},
//36分：
{{47,145},{91,93},{84,87}},
//37分：
{{40,141},{90,94},{86,86}},
//38分：
{{35,135},{91,93},{86,86}},
//39分：
{{31,130},{91,93},{84,87}},
//40分：
{{27,124},{88,94},{84,86}},
//41分：
{{22,117},{86,94},{84,87}},
//42分：
{{20,110},{87,94},{86,86}},
//43分：
{{18,104},{86,93},{88,84}},
//44分：
{{17,95},{86,93},{86,84}},
//45分：
{{18,88},{88,93},{88,84}},
//46分：
{{17,82},{87,94},{86,86}},
//47分：
{{18,73},{86,93},{86,84}},
//48分：
{{20,66},{88,93},{84,83}},
//49分：
{{22,60},{84,91},{88,84}},
//50分：
{{27,53},{88,93},{87,83}},
//51分：
{{31,47},{86,91},{90,84}},
//52分：
{{35,42},{84,91},{90,84}},
//53分：
{{40,35},{86,91},{90,84}},
//54分：
{{47,31},{84,90},{91,84}},
//55分：
{{53,27},{91,84},{84,90}},
//56分：
{{60,24},{84,88},{91,84}},
//57分：
{{66,20},{93,84},{84,87}},
//58分：
{{73,18},{84,86},{93,86}},
//59分：
{{80,17},{84,86},{93,86}},

};
static P_Window init_cartier_watch(void);



/*
 *--------------------------------------------------------------------------------------
 *     function:  get_circle_layer1
 *    parameter: 
 *       return:
 *  description:  
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
static P_Layer get_circle_layer1()
{
	LayerGeometry lg;
	P_Geometry p_geometry[1];
	memset(&lg, 0, sizeof(LayerGeometry));
	memset(p_geometry, 0, sizeof(p_geometry));
	

	/*先大圆心*/
	GPoint center1 = {CX, CY};
	Circle c1 = {center1, 5};
	Geometry cg1 = {GeometryTypeCircle, FillArea, GColorBlack, (void*)&c1}; 
	p_geometry[lg.num++] = &cg1;

	lg.p_g = p_geometry;

	/*图层1*/
	P_Layer	 layer1 = NULL;
	layer1 = app_layer_create_geometry(&lg);

	return (layer1);
}



/*
 *--------------------------------------------------------------------------------------
 *     function:  get_bmp_layer
 *    parameter: 
 *       return:
 *  description:  
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Layer get_bmp_layer(GRect *frame_p ,int32_t bmp_key)
{

	GBitmap bitmap;
	
	/*获取系统图片*/
	res_get_user_bitmap(bmp_key, &bitmap);

	LayerBitmap lb1 = { bitmap,*frame_p, GAlignLeft};	

	/*图层1*/
	P_Layer	 layer1 = NULL;
	layer1 = app_layer_create_bitmap(&lb1);

	return (layer1);
}


/*
 *--------------------------------------------------------------------------------------
 *     function:  get_time_hand_layer
 *    parameter: 
 *       return:
 *  description:  获取指针图层
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
static	P_Layer get_time_hand_layer(uint8_t min,uint8_t time_pos[][3][2])
{
	GPoint p1 = {time_pos[min][0][0], time_pos[min][0][1]}, p2 = {time_pos[min][1][0], time_pos[min][1][1]}, p3 = {time_pos[min][2][0], time_pos[min][2][1]};

	/*多边形*/
	GPoint points1[3] = {p1, p2, p3};
	Polygon po1 = {3, points1};
	Geometry pg1 = {GeometryTypePolygon, FillArea, GColorBlack, (void*)&po1}; 
	P_Geometry p_pg[1];
	LayerGeometry lg;
	memset(&lg, 0, sizeof(LayerGeometry));
	p_pg[lg.num++] = &pg1;
	lg.p_g = p_pg;


	/*图层1*/
	P_Layer	 layer1 = NULL;
	layer1 = app_layer_create_geometry(&lg);


	return layer1;
}


//重新载入并刷新窗口所有图层
void window_reloading(void)
{
	/*根据窗口ID获取窗口句柄*/
	P_Window p_old_window = app_window_stack_get_window_by_id(g_cartier_window); 
	if (NULL != p_old_window)
	{
		P_Window p_window = init_cartier_watch();
		if (NULL != p_window)
		{
			g_cartier_window = app_window_stack_replace_window(p_old_window, p_window);
		}	
	}
	
}


static void app_cartier_watch_time_change(enum SysEventType type, void *context)
{

	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		window_reloading();	

	}
}




#if 0
/*
 *--------------------------------------------------------------------------------------
 *     function:  cartier_watch_timer_callback
 *    parameter: 
 *       return:
 *  description:  
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
void cartier_watch_timer_callback(date_time_t tick_time, uint32_t millis, void *context)
{
	app_cartier_watch_update(tick_time->hour, tick_time->min, 0);
}
#endif


static P_Window init_cartier_watch(void)
{
	P_Window p_window = NULL;
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	P_Layer bgl = NULL, hl = NULL, ml = NULL, cl1 = NULL, cl2 = NULL;

	struct date_time datetime;
	app_service_get_datetime(&datetime);

	GRect frame = {{CARTIER_BG_ORIGIN_X, CARTIER_BG_ORIGIN_Y}, {CARTIER_BG_SIZE_H, CARTIER_BG_SIZE_W}};

	bgl = get_bmp_layer(&frame,RES_BITMAP_WATCHFACE_CARTIER_BG);
	
	ml = get_time_hand_layer(datetime.min,min_pos);
	hl = get_time_hand_layer(datetime.hour%12*3 + datetime.min/20,hour_pos);

	cl1 = get_circle_layer1();	

	frame.origin.x = CARTIER_CENTER_ORIGIN_X;
	frame.origin.y = CARTIER_CENTER_ORIGIN_Y;
	frame.size.h = CARTIER_CENTER_SIZE_H;
	frame.size.w = CARTIER_CENTER_SIZE_W;

	cl2 = get_bmp_layer(&frame,RES_BITMAP_WATCHFACE_CARTIER_CENTER);

	/*添加图层到窗口*/
	app_window_add_layer(p_window, bgl);
	app_window_add_layer(p_window, ml);
	app_window_add_layer(p_window, hl);
	app_window_add_layer(p_window, cl1);
	app_window_add_layer(p_window, cl2);


	/*注册一个定时timer，更新时分指针*/
//	app_window_timer_subscribe(p_window, 60000, cartier_watch_timer_callback, NULL);

	/*注册一个事件通知回调，当有时间改变是，立即更新时间*/
	maibu_service_sys_event_subscribe(app_cartier_watch_time_change);

	return p_window;
}



int main()
{
#ifdef LINUX

	/*非APP编写*/	
	screen_init(SCREEN_ROW_NUMS,SCREEN_COL_NUMS);
	os_store_manage_init();
	window_stack_init();
	SHOW;
#endif


	/*创建显示表盘窗口*/
	P_Window p_window = init_cartier_watch();
	if (p_window != NULL)
	{
		/*放入窗口栈显示*/
		g_cartier_window = app_window_stack_push(p_window);
	}


#ifdef LINUX
	SHOW;
	while(1)
	{

		char input;	
	
		/*输入操作*/
		scanf("%c", &input);
		if (input == 'q')
		{
			break;	
		}
		else if (input == 'c')
		{
			app_cartier_watch_time_change(0, NULL);
		}
	}

	/*非APP编写*/	
	app_window_stack_pop(p_window);
	window_stack_destory();
	os_store_manage_destory();
	screen_destory();
	SHOW;
#endif


	return 0;
}

