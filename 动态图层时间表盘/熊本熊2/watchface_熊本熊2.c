#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "maibu_sdk.h"
#include "maibu_res.h"



#define WATCHFACE_FACE_BMP_ORIGIN_X 	0
#define WATCHFACE_FACE_BMP_ORIGIN_Y 	38
#define WATCHFACE_FACE_BMP_WIZE_H		90
#define WATCHFACE_FACE_BMP_SIZE_W 		128

#define WATCHFACE_STYLE_1_TIME_HOUR1_ORIGIN_X 	3
#define WATCHFACE_STYLE_1_TIME_HOUR1_ORIGIN_Y 	3
#define WATCHFACE_STYLE_1_TIME_HOUR1_WIZE_H		16
#define WATCHFACE_STYLE_1_TIME_HOUR1_SIZE_W 	9


#define WATCHFACE_STYLE_1_TIME_HOUR2_ORIGIN_X 	13
#define WATCHFACE_STYLE_1_TIME_HOUR2_ORIGIN_Y 	3
#define WATCHFACE_STYLE_1_TIME_HOUR2_WIZE_H		16
#define WATCHFACE_STYLE_1_TIME_HOUR2_SIZE_W 	9

#define WATCHFACE_STYLE_1_TIME_MIN1_ORIGIN_X 	39
#define WATCHFACE_STYLE_1_TIME_MIN1_ORIGIN_Y 	3
#define WATCHFACE_STYLE_1_TIME_MIN1_WIZE_H		16
#define WATCHFACE_STYLE_1_TIME_MIN1_SIZE_W 		9

#define WATCHFACE_STYLE_1_TIME_MIN2_ORIGIN_X 	49
#define WATCHFACE_STYLE_1_TIME_MIN2_ORIGIN_Y 	3
#define WATCHFACE_STYLE_1_TIME_MIN2_WIZE_H		16
#define WATCHFACE_STYLE_1_TIME_MIN2_SIZE_W 		9


#define WATCHFACE_STYLE_2_TIME_HOUR1_ORIGIN_X 	41
#define WATCHFACE_STYLE_2_TIME_HOUR1_ORIGIN_Y 	20
#define WATCHFACE_STYLE_2_TIME_HOUR1_WIZE_H		16
#define WATCHFACE_STYLE_2_TIME_HOUR1_SIZE_W 	9


#define WATCHFACE_STYLE_2_TIME_HOUR2_ORIGIN_X 	51
#define WATCHFACE_STYLE_2_TIME_HOUR2_ORIGIN_Y 	20
#define WATCHFACE_STYLE_2_TIME_HOUR2_WIZE_H		16
#define WATCHFACE_STYLE_2_TIME_HOUR2_SIZE_W 	9

#define WATCHFACE_STYLE_2_TIME_MIN1_ORIGIN_X 	77
#define WATCHFACE_STYLE_2_TIME_MIN1_ORIGIN_Y 	20
#define WATCHFACE_STYLE_2_TIME_MIN1_WIZE_H		16
#define WATCHFACE_STYLE_2_TIME_MIN1_SIZE_W 		9

#define WATCHFACE_STYLE_2_TIME_MIN2_ORIGIN_X 	87
#define WATCHFACE_STYLE_2_TIME_MIN2_ORIGIN_Y 	20
#define WATCHFACE_STYLE_2_TIME_MIN2_WIZE_H		16
#define WATCHFACE_STYLE_2_TIME_MIN2_SIZE_W 		9


#define WATCHFACE_STYLE_1_TEXT_ORIGIN_X 	24
#define WATCHFACE_STYLE_1_TEXT_ORIGIN_Y 	3
#define WATCHFACE_STYLE_1_TEXT_WIZE_H		16
#define WATCHFACE_STYLE_1_TEXT_SIZE_W 		52

#define WATCHFACE_STYLE_2_TEXT_ORIGIN_X 	27
#define WATCHFACE_STYLE_2_TEXT_ORIGIN_Y 	20
#define WATCHFACE_STYLE_2_TEXT_WIZE_H		16
#define WATCHFACE_STYLE_2_TEXT_SIZE_W 		74


#define WATCHFACE_STYLE_1_WORDS_TEXT_ORIGIN_X 	3
#define WATCHFACE_STYLE_1_WORDS_TEXT_ORIGIN_Y 	20
#define WATCHFACE_STYLE_1_WORDS_TEXT_WIZE_H		16
#define WATCHFACE_STYLE_1_WORDS_TEXT_SIZE_W 	90

#define WATCHFACE_STYLE_2_WORDS_TEXT_ORIGIN_X 	0
#define WATCHFACE_STYLE_2_WORDS_TEXT_ORIGIN_Y 	3
#define WATCHFACE_STYLE_2_WORDS_TEXT_WIZE_H		14
#define WATCHFACE_STYLE_2_WORDS_TEXT_SIZE_W 	128


#define MAX_TEXT_SIZE			25

static GRect bmp_origin_size_bg = {
	{WATCHFACE_FACE_BMP_ORIGIN_X,WATCHFACE_FACE_BMP_ORIGIN_Y},
	{WATCHFACE_FACE_BMP_WIZE_H,WATCHFACE_FACE_BMP_SIZE_W}
};


const static GRect bmp_origin_size[] = {

	{
		{WATCHFACE_STYLE_1_TIME_HOUR1_ORIGIN_X,WATCHFACE_STYLE_1_TIME_HOUR1_ORIGIN_Y},
		{WATCHFACE_STYLE_1_TIME_HOUR1_WIZE_H,WATCHFACE_STYLE_1_TIME_HOUR1_SIZE_W}
	},
	{
		{WATCHFACE_STYLE_1_TIME_HOUR2_ORIGIN_X,WATCHFACE_STYLE_1_TIME_HOUR2_ORIGIN_Y},
		{WATCHFACE_STYLE_1_TIME_HOUR2_WIZE_H,WATCHFACE_STYLE_1_TIME_HOUR2_SIZE_W}
	},
	{
		{WATCHFACE_STYLE_1_TIME_MIN1_ORIGIN_X,WATCHFACE_STYLE_1_TIME_MIN1_ORIGIN_Y},
		{WATCHFACE_STYLE_1_TIME_MIN1_WIZE_H,WATCHFACE_STYLE_1_TIME_MIN1_SIZE_W}
	},	

	{
		{WATCHFACE_STYLE_1_TIME_MIN2_ORIGIN_X,WATCHFACE_STYLE_1_TIME_MIN2_ORIGIN_Y},
		{WATCHFACE_STYLE_1_TIME_MIN2_WIZE_H,WATCHFACE_STYLE_1_TIME_MIN2_SIZE_W}
	},
	{
		{WATCHFACE_STYLE_2_TIME_HOUR1_ORIGIN_X,WATCHFACE_STYLE_2_TIME_HOUR1_ORIGIN_Y},
		{WATCHFACE_STYLE_2_TIME_HOUR1_WIZE_H,WATCHFACE_STYLE_2_TIME_HOUR1_SIZE_W}
	},
	{
		{WATCHFACE_STYLE_2_TIME_HOUR2_ORIGIN_X,WATCHFACE_STYLE_2_TIME_HOUR2_ORIGIN_Y},
		{WATCHFACE_STYLE_2_TIME_HOUR2_WIZE_H,WATCHFACE_STYLE_2_TIME_HOUR2_SIZE_W}
	},
	{
		{WATCHFACE_STYLE_2_TIME_MIN1_ORIGIN_X,WATCHFACE_STYLE_2_TIME_MIN1_ORIGIN_Y},
		{WATCHFACE_STYLE_2_TIME_MIN1_WIZE_H,WATCHFACE_STYLE_2_TIME_MIN1_SIZE_W}
	},	

	{
		{WATCHFACE_STYLE_2_TIME_MIN2_ORIGIN_X,WATCHFACE_STYLE_2_TIME_MIN2_ORIGIN_Y},
		{WATCHFACE_STYLE_2_TIME_MIN2_WIZE_H,WATCHFACE_STYLE_2_TIME_MIN2_SIZE_W}
	},

	{
		{WATCHFACE_STYLE_1_TEXT_ORIGIN_X,WATCHFACE_STYLE_1_TEXT_ORIGIN_Y},
		{WATCHFACE_STYLE_1_TEXT_WIZE_H,WATCHFACE_STYLE_1_TEXT_SIZE_W}
	},
	{
		{WATCHFACE_STYLE_2_TEXT_ORIGIN_X,WATCHFACE_STYLE_2_TEXT_ORIGIN_Y},
		{WATCHFACE_STYLE_2_TEXT_WIZE_H,WATCHFACE_STYLE_2_TEXT_SIZE_W}
	},
	{
		{WATCHFACE_STYLE_1_WORDS_TEXT_ORIGIN_X,WATCHFACE_STYLE_1_WORDS_TEXT_ORIGIN_Y},
		{WATCHFACE_STYLE_1_WORDS_TEXT_WIZE_H,WATCHFACE_STYLE_1_WORDS_TEXT_SIZE_W}
	},
	{
		{WATCHFACE_STYLE_2_WORDS_TEXT_ORIGIN_X,WATCHFACE_STYLE_2_WORDS_TEXT_ORIGIN_Y},
		{WATCHFACE_STYLE_2_WORDS_TEXT_WIZE_H,WATCHFACE_STYLE_2_WORDS_TEXT_SIZE_W}
	}

};


//小号数字图片数组
static uint32_t get_number_icon_key[] =
{
	RES_BITMAP_WATCHFACE_NUMBER_0,
	RES_BITMAP_WATCHFACE_NUMBER_1,
	RES_BITMAP_WATCHFACE_NUMBER_2,
	RES_BITMAP_WATCHFACE_NUMBER_3,
	RES_BITMAP_WATCHFACE_NUMBER_4,
	RES_BITMAP_WATCHFACE_NUMBER_5,
	RES_BITMAP_WATCHFACE_NUMBER_6,
	RES_BITMAP_WATCHFACE_NUMBER_7,
	RES_BITMAP_WATCHFACE_NUMBER_8,
	RES_BITMAP_WATCHFACE_NUMBER_9
	
};
//中号数字图片数组
static uint32_t get_bmp_icon_key[] =
{
	RES_BITMAP_WATCHFACE_FACE_1,
	RES_BITMAP_WATCHFACE_FACE_2,
	RES_BITMAP_WATCHFACE_FACE_3,
	RES_BITMAP_WATCHFACE_FACE_4,
	RES_BITMAP_WATCHFACE_FACE_5,
	RES_BITMAP_WATCHFACE_FACE_6,
	RES_BITMAP_WATCHFACE_FACE_7,
	RES_BITMAP_WATCHFACE_FACE_8,
	RES_BITMAP_WATCHFACE_STYLE_1,
	RES_BITMAP_WATCHFACE_STYLE_2
	
};

const static int8_t text_array[][MAX_TEXT_SIZE] = 
{
	"我会不会被人打啊",//0
	"好紧脏",
	"谁在说我帅",
	"又饿了呢",
	"我不会轻易的狗带",
	"该吃药了",
	"假装在看书",
	"这个逼我装定了",
	"朕在修炼",
	"大爷来玩嘛",
	"吓死爹了",
	"人丑就要多读书",
	"憋说话 吻我",
	"骑车装逼去",
	"颤抖吧人类！",
	"这一定不是真的",//15
	
	"伦家辣么萌，",
	"我勒个去，",
	"心好累，",
	"什么鬼！",
	"辣鸡！",
	"太淫荡了！",
	"我需要安慰，",
	"累觉不爱，",
	"假装看不见，",
	"日了狗了，",
	"我好像懵逼了，",
	"气死宝宝了，"//27
	
};

/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_window_id = -1;

/*表盘应显示数据*/
static uint8_t watch_data[7] = {0};
static uint8_t style_1_or_2 = 0;



static P_Window init_window(void);

int8_t get_random_number(int8_t pram)
{
	int16_t x, y, z;
	maibu_get_accel_data(&x, &y, &z);
	return z%pram;
} 


/*
 *--------------------------------------------------------------------------------------
 *     function:  app_watch_data
 *    parameter:  0为watch_data所有元素值，1仅为watch_data中的秒
 *       return:
 *  description:  将数组参数赋值为当前表盘应显示值
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
static void app_watch_data(void)
{
	int8_t random = 0;
		
	struct date_time datetime;
	app_service_get_datetime(&datetime);

	
	watch_data[0] = datetime.hour/10;
	watch_data[1] = datetime.hour%10;
	watch_data[2] = datetime.min/10;
	watch_data[3] = datetime.min%10;

	random = get_random_number(8);
	watch_data[4] = (watch_data[4]+random)%8;//图片的序号
	
	random = get_random_number(2);
	watch_data[5] = random;
	
	if(random == 0)
	{
		watch_data[6] = get_random_number(16);//文字序号
	}
	else
	{
		watch_data[6] = 16+get_random_number(12);//文字序号
	}
	
}

//重新载入并刷新窗口所有图层
void window_reloading(void)
{
	/*根据窗口ID获取窗口句柄*/
	P_Window p_old_window = app_window_stack_get_window_by_id(g_window_id); 
	if (NULL != p_old_window)
	{
		P_Window p_window = init_window();
		if (NULL != p_window)
		{
			g_window_id = app_window_stack_replace_window(p_old_window, p_window);
		}	
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
		app_watch_data();
		window_reloading();
	}
}


/*
 *--------------------------------------------------------------------------------------
 *     function:  
 *    parameter: 
 *       return:
 *  description:  生成表盘窗口的各图层
 * 	      other:
 *--------------------------------------------------------------------------------------
 */

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

static P_Window init_window(void)
{
	P_Window p_window = NULL;

	/*创建一个窗口*/
	p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	/*创建背景图层*/
	display_target_layer(p_window,&bmp_origin_size_bg,GAlignCenter,GColorWhite,get_bmp_icon_key,watch_data[4]);

	display_target_layer(p_window,&bmp_origin_size[watch_data[5] + 8],GAlignLeft,GColorWhite,get_bmp_icon_key,watch_data[5] + 8);
	
	/*创建时分图层*/
	uint8_t i = 0;
	for(i = 0;i <= 3;i++)
	{
		display_target_layer(p_window,&bmp_origin_size[watch_data[5]*4 + i],GAlignLeft,GColorWhite,get_number_icon_key,watch_data[i]);
	}
	
	if(watch_data[5] == 0)
	{
		char str[19] = {0};
		memcpy(str,text_array[watch_data[6]],sizeof(str)-1);
		str[19] = '\0';
				
		display_target_layerText(p_window,&bmp_origin_size[10],GAlignLeft,GColorWhite,str,U_ASCII_ARIAL_14);

		memset(str,0,sizeof(str));
		memcpy(str,&(text_array[watch_data[6]][sizeof(str)-1]),6);

		GRect temp_bmp_origin_size = bmp_origin_size[10];
		temp_bmp_origin_size.origin.y = temp_bmp_origin_size.origin.y + 16;
		
		display_target_layerText(p_window,&temp_bmp_origin_size,GAlignLeft,GColorWhite,str,U_ASCII_ARIAL_14);

		
	}
	else
	{
	
		display_target_layerText(p_window,&bmp_origin_size[11],GAlignCenter,GColorWhite,text_array[watch_data[6]],U_ASCII_ARIAL_14);
	
	}
		
	return p_window;
}



/*
 *--------------------------------------------------------------------------------------
 *     function:  main
 *    parameter: 
 *       return:
 *  description:  主程序
 * 	      other:
 *--------------------------------------------------------------------------------------
 */
int main()
{
		
	app_watch_data();
	/*创建显示表盘窗口*/
	P_Window p_window = init_window();
	if (p_window != NULL)
	{
		/*放入窗口栈显示*/
		g_window_id = app_window_stack_push(p_window);

		/*注册一个事件通知回调，当有时间改变时，立即更新时间*/
		maibu_service_sys_event_subscribe(app_watch_time_change);
	}
		
	return 0;
}
