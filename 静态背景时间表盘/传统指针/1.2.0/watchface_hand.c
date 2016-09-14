/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  watchface_hand.c
 *
 *    Description:  指针表盘
 *    Corporation:
 * 
 *         Author:  gliu (), gliu@damaijiankang.com
 *        Created:  2015年04月02日 09时41分36秒
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




static uint8_t g_triangle_layer_h = 0, g_triangle_layer_m = 0;
static uint32_t g_triangle_window = 0;


/*窗口图层位置*/
/*背景图层*/
#define TRIANGLE_BG_ORIGIN_X			0
#define TRIANGLE_BG_ORIGIN_Y			0
#define TRIANGLE_BG_SIZE_H			128
#define TRIANGLE_BG_SIZE_W			128

/*中心图片*/
#define TRIANGLE_CENTER_ORIGIN_X		62
#define TRIANGLE_CENTER_ORIGIN_Y		62
#define TRIANGLE_CENTER_SIZE_H			5
#define TRIANGLE_CENTER_SIZE_W			5



/*表盘中心位置*/
#define CX 64
#define CY 64

/*大圆半径*/
#define BIG_CIRCLE_RADIUS	4

/*小圆半径*/	
#define SMALL_CIRCLE_RADIUS	2


const uint8_t hour_pos[][3][2] = 
{

//0:
{{64,35},{60,64},{68,64}},
//0:20: 
{{69,36},{60,63},{68,65}},
//0:40:
{{74,37},{60,63},{68,65}},

//1: 
{{79,39},{61,62},{67,66}},
//1:20:
{{83,43},{62,61},{67,66}},
//1:40:
{{86,46},{62,61},{67,66}},

//2:
{{89,50},{62,61},{66,67}},
//2:20:
{{91,55},{63,60},{65,68}},
//2:40:
{{92,59},{64,60},{65,68}},

//3:
{{92,64},{64,60},{64,68}},
//3:20:
{{92,68},{65,60},{64,68}},
//3:40: 
{{91,72},{65,60},{63,67}},

//4:   
{{89,77},{65,60},{61,67}},
//4:20:
{{86,81},{66,61},{61,66}},
//4:40: 
{{83,84},{66,61},{61,66}},

//5:
{{79,88},{67,61},{61,65}},
//5:20: 
{{74,90},{67,62},{60,64}},
//5:40: 
{{69,91},{67,62},{60,64}},
	
//6:  
{{64,92},{68,63},{60,63}},
//6:20:
{{58,91},{68,65},{60,63}},
//6:40: 
{{53,90},{68,65},{61,62}},

//7:
{{48,88},{68,65},{61,61}},
//7:20:
{{44,84},{67,67},{61,61}},
//7:40:
{{41,81},{66,67},{61,61}},

//8:
{{38,77},{66,67},{61,61}},
//8:20:
{{36,72},{65,68},{61,61}},
//8:40:
{{35,68},{63,68},{63,60}},

//9: 
{{35,64},{63,68},{63,60}},
//9:20: 
{{35,59},{63,68},{63,60}},
//9:40: 
{{36,55},{61,67},{64,60}},

//10 :
{{38,50},{61,67},{65,60}},
//10:20:
{{41,46},{61,67},{65,60}},
//10:40:
{{44,43},{61,66},{66,61}},

//11: 
{{48,39},{60,65},{67,61}},
//11:20:
{{53,37},{60,65},{67,62}},
//11:40: 
{{58,36},{60,65},{68,63}}

};

const uint8_t min_pos[][3][2] = 
{	
	//0分：
	{{64,13},{61,63},{67,63}},
	//1分：
	{{69,12},{62,62},{67,62}},
	//2分：
	{{75,13},{62,62},{68,62}},
	//3分：
	{{80,14},{62,62},{68,63}},
	//4分：
	{{85,17},{62,61},{67,64}},
	//5分：
	{{90,19},{62,61},{68,64}},
	//6分：
	{{94,22},{62,62},{67,66}},
	//7分：
	{{98,25},{62,62},{67,66}},
	//8分：
	{{102,30},{63,61},{66,67}},
	//9分：
	{{105,34},{63,61},{66,67}},
	//10分：
	{{108,38},{64,61},{66,67}},
	//11分：
	{{111,43},{63,61},{66,67}},
	//12分：
	{{113,48},{64,61},{65,67}},
	//13分：
	{{114,53},{65,61},{65,67}},
	//14分：
	{{115,59},{65,61},{65,67}},
	//15分：
	{{115,64},{64,61},{64,68}},
	//16分：
	{{116,69},{66,61},{66,67}},
	//17分：
	{{114,75},{65,61},{65,67}},
	//18分：
	{{113,80},{65,61},{64,67}},
	//19分：
	{{111,85},{65,61},{63,67}},
	//20分：
	{{108,90},{66,62},{64,68}},
	//21分：
	{{105,94},{66,62},{62,66}},
	//22分：
	{{102,98},{66,62},{63,67}},
	//23分：
	{{98,102},{66,61},{62,66}},
	//24分：
	{{94,105},{67,62},{62,66}},
	//25分：
	{{89,108},{67,62},{63,68}},
	//26分：
	{{85,110},{67,63},{61,66}},
	//27分：
	{{80,113},{67,64},{61,65}},
	//28分：
	{{75,114},{67,65},{61,65}},
	//29分：
	{{69,115},{67,65},{61,65}},
	//30分：
	{{64,115},{61,64},{68,66}},
	//31分：
	{{58,115},{67,65},{61,65}},
	//32分：
	{{53,114},{67,65},{61,65}},
	//33分：
	{{48,113},{67,66},{61,65}},
	//34分：
	{{43,111},{67,66},{61,64}},
	//35分：
	{{38,108},{67,65},{61,64}},
	//36分：
	{{34,105},{66,67},{61,63}},
	//37分：
	{{29,102},{65,68},{62,62}},
	//38分：
	{{25,98},{66,67},{62,62}},
	//39分：
	{{22,94},{66,67},{61,63}},
	//40分：
	{{19,90},{64,68},{61,62}},
	//41分：
	{{16,85},{62,68},{61,63}},
	//42分：
	{{14,80},{63,68},{62,62}},
	//43分：
	{{13,75},{62,67},{64,61}},
	//44分：
	{{12,69},{62,67},{62,61}},
	//45分：
	{{13,64},{64,67},{64,61}},
	//46分：
	{{12,59},{63,68},{62,62}},
	//47分：
	{{13,53},{62,67},{62,61}},
	//48分：
	{{14,48},{64,67},{61,60}},
	//49分：
	{{16,43},{61,66},{64,61}},
	//50分：
	{{19,38},{64,67},{63,60}},
	//51分：
	{{22,34},{62,66},{65,61}},
	//52分：
	{{25,30},{61,66},{65,61}},
	//53分：
	{{29,25},{62,66},{65,61}},
	//54分：
	{{34,22},{61,65},{66,61}},
	//55分---------------：
	{{38,19},{66,61},{61,65}},
	//56分：
	{{43,17},{61,64},{66,61}},
	//57分--------------：
	{{48,14},{67,61},{61,63}},
	//58分：
	{{53,13},{61,62},{67,62}},
	//59分：
	{{58,12},{61,62},{67,62}}

};

P_Window init_triangle_watch(void);


/*
 *--------------------------------------------------------------------------------------
 *     function:  get_circle_laye2
 *    parameter: 
 *       return:
 *  description:  
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Layer get_circle_layer1()
{
	LayerGeometry lg;
	P_Geometry p_geometry[1];
	memset(&lg, 0, sizeof(LayerGeometry));
	memset(p_geometry, 0, sizeof(p_geometry));
	
	/*大圆心*/
	GPoint center1 = {CX, CY};
	Circle c1 = {center1, 4};
	Geometry cg1 = {GeometryTypeCircle, FillArea, GColorWhite, (void*)&c1}; 
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

	//printf("XXXXXXXXXXXXXXXXXXXXXX:%d, %d, %d, %d\n", bitmap.app_id, bitmap.key, bitmap.height, bitmap.width);
	LayerBitmap lb1 = { bitmap,*frame_p, GAlignLeft};	

	/*图层1*/
	P_Layer	 layer1 = NULL;
	layer1 = app_layer_create_bitmap(&lb1);
	//app_layer_set_bg_color(layer1, GColorBlack);

	return (layer1);
}




/*
 *--------------------------------------------------------------------------------------
 *     function:  get_hour_layer
 *    parameter: 
 *       return:
 *  description:  获取时针图层
 * 	  other:
 *--------------------------------------------------------------------------------------
 */

P_Layer get_time_hand_layer(uint8_t min,uint8_t time_pos[][3][2])
{
	GPoint p1 = {time_pos[min][0][0], time_pos[min][0][1]}, p2 = {time_pos[min][1][0], time_pos[min][1][1]}, p3 = {time_pos[min][2][0], time_pos[min][2][1]};

	/*多边形*/
	GPoint points1[3] = {p1, p2, p3};
	Polygon po1 = {3, points1};
	Geometry pg1 = {GeometryTypePolygon, FillArea, GColorWhite, (void*)&po1}; 
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
	P_Window p_old_window = app_window_stack_get_window_by_id(g_triangle_window); 
	if (NULL != p_old_window)
	{
		P_Window p_window = init_triangle_watch();
		if (NULL != p_window)
		{
			g_triangle_window = app_window_stack_replace_window(p_old_window, p_window);
		}	
	}
	
}

void triangle_watch_time_change(enum SysEventType type, void *context)
{
	/*时间更改*/
	if (type == SysEventTypeTimeChange)
	{
		window_reloading();	
	}
}


P_Window init_triangle_watch(void)
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

	GRect frame = {{TRIANGLE_BG_ORIGIN_X, TRIANGLE_BG_ORIGIN_Y}, {TRIANGLE_BG_SIZE_H, TRIANGLE_BG_SIZE_W}};
	bgl = get_bmp_layer(&frame,RES_BITMAP_WATCHFACE_OFFICIAL_BG01);
	
	ml = get_time_hand_layer(datetime.min,min_pos);

	hl = get_time_hand_layer(datetime.hour%12*3 + datetime.min/20,hour_pos);
	
	cl1 = get_circle_layer1();

	frame.origin.x = TRIANGLE_CENTER_ORIGIN_X;
	frame.origin.y = TRIANGLE_CENTER_ORIGIN_Y;
	frame.size.h = TRIANGLE_CENTER_SIZE_H;
	frame.size.w = TRIANGLE_CENTER_SIZE_W;

	cl2 = get_bmp_layer(&frame,RES_BITMAP_WATCHFACE_OFFICIAL_CENTER01);


	/*添加图层到窗口*/
	app_window_add_layer(p_window, bgl);
	g_triangle_layer_m = app_window_add_layer(p_window, ml);
	g_triangle_layer_h = app_window_add_layer(p_window, hl);
	app_window_add_layer(p_window, cl1);
	app_window_add_layer(p_window, cl2);

	/*注册一个事件通知回调，当有时间改变是，立即更新时间*/
	maibu_service_sys_event_subscribe(triangle_watch_time_change);

	return p_window;
}

int main()
{

	P_Window p_window = init_triangle_watch();
	g_triangle_window = app_window_stack_push(p_window);


	return 0;
}
