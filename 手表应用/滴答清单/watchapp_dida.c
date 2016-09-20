/*
 * =====================================================================================
 * 
 *     Modulename:
 *       Filename:  watchapp_dida.c
 *
 *    Description:  滴答清单
 *    Corporation:
 * 
 *         Author:  gliu (), gliu@maibu.cc
 *        Created:  2016年01月19日 10时52分52秒
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


#ifdef LINUX
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "linux_screen_show.h"
#include "os_store_manage.h"
#include "os_time.h"
#include "os_comm.h"
#include "os_res_bitmap.h"
#include "os_sys_app_id.h"
#include "os_memory_manage.h"
#include "res_user_bitmap.h"
#include "os_sys_event.h"
#include "window_stack.h"
#include "window.h"
#include "screen_show.h"
#include "matrix.h"
#include "plug_status_bar.h"
#include "res_bitmap_base.h"
#include "os_sys_notify.h"
#else
#include "maibu_sdk.h"
#include "maibu_res.h"
#endif



/*窗口ID, 通过该窗口ID获取窗口句柄*/
static int32_t g_window_id = -1;


/*图层ID，通过该图层ID获取图层句柄*/
static int8_t g_layer_menu_id = -1;





/*定义各个图层的位置*/

/*菜单图层*/
#define LAYER_MENU_ORIGIN_X		0
#define LAYER_MENU_ORIGIN_Y		12
#define LAYER_MENU_SIZE_H		116
#define LAYER_MENU_SIZE_W		128

/*菜单项*/
#define MENU_ITEM_ORIGIN_X		0
#define MENU_ITEM_ORIGIN_Y		0
#define MENU_ITEM_SIZE_H		36
#define MENU_ITEM_SIZE_W		128

/*菜单项图片元素*/
#define  MENU_ELEMENT_BMP_ORIGIN_X 	4
#define  MENU_ELEMENT_BMP_ORIGIN_Y	8
#define  MENU_ELEMENT_BMP_SIZE_H 	20
#define  MENU_ELEMENT_BMP_SIZE_W	20

/*菜单项标题元素*/
#define  MENU_ELEMENT_TITLE_ORIGIN_X 	28
#define  MENU_ELEMENT_TITLE_ORIGIN_Y	4
#define  MENU_ELEMENT_TITLE_SIZE_H 	14
#define  MENU_ELEMENT_TITLE_SIZE_W	100

/*菜单项日期元素*/
#define  MENU_ELEMENT_DATE_ORIGIN_X 	28
#define  MENU_ELEMENT_DATE_ORIGIN_Y	21
#define  MENU_ELEMENT_DATE_SIZE_H 	12
#define  MENU_ELEMENT_DATE_SIZE_W	100




/*滴答通讯协议*/
#define PROTOCOL_UPDATE_LIST			0x01	
#define PROTOCOL_CHECK_LIST			0x02
#define PROTOCOL_GET_CHECK_LIST			0x03
#define PROTOCOL_UPDATE_LIST_ACK		0x21	
#define PROTOCOL_CHECK_LIST_ACK			0x22
#define PROTOCOL_GET_LIST_ACK			0x23


/*通讯相关*/

/*通讯ID：更新任务应答*/
static int32_t g_comm_id_update_list_ack = -1;
static int32_t g_comm_id_check_ok = -1;
static int32_t g_comm_id_get_list_ack = -1;

/*Linkid*/
static char g_link_id[10] = "";

/*上一个check的任务ID*/
static int32_t g_task_id = -1;


/*应答错误码*/
static int8_t g_error_code_update_ack = 0;

/*任务项结构体*/
typedef struct tag_STaskItem
{
	uint32_t id;	//任务ID
	uint32_t date;	//任务日期时间
	char title[24];	//任务标题
}STaskItem;

/*任务项信息*/
typedef struct tag_STaskInfo
{
	int8_t checked;	//是否已check	0 未check 1 checking  2 checked
	int8_t used;	//是否已使用	0 未使用 1 已使用
	int8_t alarm;	//是否已提醒，只针对Android
	STaskItem item; 
}STaskInfo;


#define TASK_LIST_MAX_SIZE	5
#define NOT_USED	0
#define USED	1
#define NOT_HAVE_CHECKED	0
#define CHECKING		1
#define HAVE_CHECKED		2
#define NOT_HAVE_ALARM		0
#define HAVE_ALARM		1


/*任务列表结构体*/
typedef struct tag_STaskInfoList
{
	uint8_t nums;	//当前任务项个数
	STaskInfo list[TASK_LIST_MAX_SIZE];	//任务项列表
}STaskInfoList;



/*任务列表*/
static STaskInfoList g_s_task_list = {0}; 


/*设置菜单当前选择项*/
static int8_t g_select_index = 0;





/*
 *--------------------------------------------------------------------------------------
 *     function:  send_check_task
 *    parameter: 
 *       return:
 *  description:  发送完成任务命令
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
int32_t send_check_task(int32_t task_id)
{
	uint8_t data[8] = {0xdd, 0x01, 0x02, 0x04};
	memcpy(&data[4], &task_id, sizeof(int32_t));	

	return (maibu_comm_send_msg(g_link_id, data, sizeof(data)));
}



/*向下按键*/
void click_down(void *context)
{
	P_Window wtmp = (P_Window)context;
	app_window_set_down_button(wtmp);		
}

/*向上按键*/
void click_up(void *context)
{
	P_Window wtmp = (P_Window)context;
	app_window_set_up_button(wtmp);		
}

/*选择按键*/
void click_select(void *context)
{
	P_LayerMultiMenu plmm = NULL;
	P_Window wtmp = (P_Window)context;
		
	P_Layer p_layer = app_window_get_layer_by_id(wtmp, g_layer_menu_id);
	plmm = (P_LayerMultiMenu)p_layer->layer;

	/*发送给手机APP该任务已check, 并标记*/
	g_task_id = g_s_task_list.list[plmm->items[plmm->selected].key].item.id;
	send_check_task(g_task_id);
	g_s_task_list.list[plmm->items[plmm->selected].key].checked = CHECKING;

	/*更改图标，打钩*/
	GBitmap bmp;
	res_get_user_bitmap(RES_BITMAP_WATCHAPP_DIDA_CHECK_OK, &bmp);	
	app_layer_set_multi_menu_item_bitmap(p_layer, plmm->selected, 0, &bmp);		
	app_window_update(wtmp);
	
}

/*后退按键*/
void click_back(void *context)
{
	P_Window p_window = (P_Window)context;
	app_window_stack_pop(p_window);
}


/*
 *--------------------------------------------------------------------------------------
 *     function:  create_layer_dida_menu
 *    parameter: 
 *       return:
 *  description:  创建滴答菜单图层
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
P_Layer create_layer_dida_menu(void)
{
	MultiMenuItem item;	//菜单项
	LayerMultiMenu lmm;	//菜单结构
	Element element;	//菜单项中元素
	P_Layer	 menu_layer = NULL;	//菜单图层指针
	memset(&element, 0, sizeof(Element));
	memset(&item, 0, sizeof(MultiMenuItem));
	memset(&lmm, 0, sizeof(LayerMultiMenu));

	/*设置菜单图层大小*/
	GPoint menu_p = {LAYER_MENU_ORIGIN_X, LAYER_MENU_ORIGIN_Y};
	GSize menu_size = {LAYER_MENU_SIZE_H, LAYER_MENU_SIZE_W};
	GRect menu_frame = {menu_p, menu_size};
	lmm.frame = menu_frame; 

	char buf[40] = "";
	int8_t i = 0;

		
	


	for (i = 0; i < TASK_LIST_MAX_SIZE; i++)
	{
		if (g_s_task_list.list[i].used == NOT_USED)
		{
			continue;
		}

		/*非常重要, 要清空*/	
		memset(&item, 0, sizeof(MultiMenuItem));
		memset(buf, 0, sizeof(buf));
		
		/*添加菜单项中图片元素*/
		GBitmap bmp;
		if (g_s_task_list.list[i].checked == CHECKING)
		{
			res_get_user_bitmap(RES_BITMAP_WATCHAPP_DIDA_CHECK_OK, &bmp);
		}
		else
		{
			res_get_user_bitmap(RES_BITMAP_WATCHAPP_DIDA_CHECK_NULL, &bmp);
		}
		element.type = ELEMENT_TYPE_BITMAP;
		element.content = (void *)&bmp;
		GRect frame_bmp = {{MENU_ELEMENT_BMP_ORIGIN_X, MENU_ELEMENT_BMP_ORIGIN_Y}, { MENU_ELEMENT_BMP_SIZE_H, MENU_ELEMENT_BMP_SIZE_W}}; 
		element.frame = frame_bmp; 
		app_layer_add_multi_menu_element(&item, &element);

		/*添加菜单项中标题元素*/
		memcpy(buf, g_s_task_list.list[i].item.title, sizeof(g_s_task_list.list[i].item.title));
		element.type = ELEMENT_TYPE_TEXT;
		element.content = (void *)buf;
		GRect frame_title = {{ MENU_ELEMENT_TITLE_ORIGIN_X, MENU_ELEMENT_TITLE_ORIGIN_Y},{ MENU_ELEMENT_TITLE_SIZE_H, MENU_ELEMENT_TITLE_SIZE_W}};
		element.frame = frame_title; 
		element.font = U_GBK_SIMSUN_14;
		app_layer_add_multi_menu_element(&item, &element);

		/*添加菜单项中时间元素*/
		struct date_time t;
		if (0 == g_s_task_list.list[i].item.date)	//为0则需要显示为全天
		{
			strcpy(buf, "全天");
		}
		else
		{
			app_get_dt_by_num(g_s_task_list.list[i].item.date, &t);
			sprintf(buf, "%02d:%02d", t.hour, t.min);
		}
		element.type = ELEMENT_TYPE_TEXT;
		element.content = (void *)buf;
		GRect frame_date = {{ MENU_ELEMENT_DATE_ORIGIN_X, MENU_ELEMENT_DATE_ORIGIN_Y},{ MENU_ELEMENT_DATE_SIZE_H, MENU_ELEMENT_DATE_SIZE_W}};
		element.frame = frame_date; 
		element.font = U_GBK_SIMSUN_12;
		app_layer_add_multi_menu_element(&item, &element);


		/*把菜单项添加到菜单中*/
		GRect frame_item = {{ MENU_ITEM_ORIGIN_X, MENU_ITEM_ORIGIN_Y},{ MENU_ITEM_SIZE_H, MENU_ITEM_SIZE_W}};
		item.frame = frame_item; 
		item.key = i; 
		app_layer_add_multi_menu_item(&lmm, &item);
	}


	/*创建菜单图层*/
	menu_layer = app_layer_create_multi_menu(&lmm);

	/*设置菜单项被选择的菜单项*/
//	app_layer_set_multi_menu_selected(menu_layer, 0);


	return (menu_layer);
}



/*
 *--------------------------------------------------------------------------------------
 *     function:  create_window_dida
 *    parameter:  
 *       return:
 *  description: 创建滴答窗口 
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
static P_Window create_window_dida()
{
	P_Window p_window = app_window_create();
	if (NULL == p_window)
	{
		return NULL;
	}

	/*创建状态栏*/
	app_plug_status_bar_create(p_window, NULL, NULL, NULL);
	app_plug_status_bar_add_time(p_window);
	app_plug_status_bar_add_battery(p_window);
	app_plug_status_bar_add_ble(p_window);


	/*创建菜单图层*/	
	P_Layer p_layer_menu = create_layer_dida_menu();
	if(NULL != p_layer_menu)
	{
		g_layer_menu_id = app_window_add_layer(p_window, p_layer_menu);

		/*设置选择该图层*/
		app_window_set_current_selected_layer(p_window, p_layer_menu);	
	}



	/*创建按键回调*/
	app_window_click_subscribe(p_window, ButtonIdDown, click_down);
	app_window_click_subscribe(p_window, ButtonIdUp, click_up);
	app_window_click_subscribe(p_window, ButtonIdSelect, click_select);
	app_window_click_subscribe(p_window, ButtonIdBack, click_back);
		

	return p_window;
}



/*
 *--------------------------------------------------------------------------------------
 *     function:  send_get_task_list_ack
 *    parameter: 
 *       return:
 *  description:  发送获取任务列表应答
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
int32_t send_get_task_list_ack()
{
	uint8_t ack[30] = {0xdd, 0x01, 0x23};
	int8_t i = 0, count = 0;

	for (i = 0; i < TASK_LIST_MAX_SIZE; i++)
	{
		if (g_s_task_list.list[i].checked == CHECKING)
		{
			memcpy(&ack[4+sizeof(int32_t)*count], (char *)&g_s_task_list.list[i].item.id,  sizeof(int32_t));
			count++;
		}
	}
	ack[3] = sizeof(int32_t)*count;

	return (maibu_comm_send_msg(g_link_id, ack, ack[3]+4)); 
}



/*
 *--------------------------------------------------------------------------------------
 *     function:  send_update_task_list_ack
 *    parameter: 
 *       return:
 *  description:  发送更新任务列表应答包
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
int32_t send_update_task_list_ack()
{
	uint8_t ack[5] = {0xdd, 0x01, 0x21, 0x01, g_error_code_update_ack};

	return (maibu_comm_send_msg(g_link_id, ack, sizeof(ack))); 
}  



/*
 *--------------------------------------------------------------------------------------
 *     function:  update_dida_memu
 *    parameter: 
 *       return:
 *  description:  
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
void update_dida_menu()
{
	/*根据窗口ID获取窗口句柄*/
	P_Window p_window = app_window_stack_get_window_by_id(g_window_id);
	if (NULL == p_window)
	{
		return;
	}
	
	P_Layer p_old_menu = app_window_get_layer_by_id(p_window, g_layer_menu_id);	
	if (NULL == p_old_menu)
	{
		return;	
	}
	/*重新创建菜单并更新*/
	P_Layer p_new_layer = create_layer_dida_menu();
	if ((NULL == p_new_layer))
	{
		return;
	}
	app_window_replace_layer(p_window, p_old_menu, p_new_layer);	

	/*窗口显示*/	
	app_window_update(p_window);

}


/*
 *--------------------------------------------------------------------------------------
 *     function:  msg_recv_callback
 *    parameter: 
 *       return:
 *  description:  处理来此手机第三方APP协议数据
 * 	  other:  协议格式：{(固定标志,0xDD), (协议版本,0x01)，(命令ID,1字节),(命令结构内容，命令ID不同，内容不同)}
 *--------------------------------------------------------------------------------------
 */
void msg_recv_callback(const char *link_id, const uint8_t *buff, uint16_t size)
{

	memset(g_link_id, 0, sizeof(g_link_id));	
	strcpy(g_link_id, link_id);

	/*是否滴答协议*/	
	if ((size < 3) || (0xDD != buff[0]) || (0x01 != buff[1]))
	{
		return;
	}

	/*如果是更新任务列表*/
	if (buff[2] == PROTOCOL_UPDATE_LIST)
	{
		/*长度不对，或者数据结构不对*/
		if ((size < 4) || (size != (4+buff[3])))
		{
			/*发送错误应答*/
			g_error_code_update_ack = -2;
			g_comm_id_update_list_ack = send_update_task_list_ack();
			return;
		}

		/*保存任务列表,之前的信息全部清空*/
		memset(&g_s_task_list, 0, sizeof(STaskInfoList));
		g_s_task_list.nums = buff[3] / sizeof(STaskItem);	
		int i = 0;
		for(i = 0; i < g_s_task_list.nums; i++)
		{
			memcpy(&g_s_task_list.list[i].item, &buff[4 + sizeof(STaskItem)*i], sizeof(STaskItem));
			g_s_task_list.list[i].used = USED;
		}

		/*发送成功应答*/
		g_error_code_update_ack = 0;
		g_comm_id_update_list_ack = send_update_task_list_ack();

	}/*完成任务命令应答*/
	else if (buff[2] == PROTOCOL_CHECK_LIST_ACK)
	{

		/*获取完成任务的ID*/	
		int32_t id_checked = 0;
		int8_t id_nums = 0;
		id_nums = buff[3]/sizeof(int32_t);
		int8_t i = 0, j = 0;	
		for(i = 0; i < id_nums; i++)
		{
			memcpy(&id_checked, &buff[4+sizeof(int32_t)*i], sizeof(int32_t));
			for (j = 0; j < TASK_LIST_MAX_SIZE; j++)
			{
				if (g_s_task_list.list[j].item.id == id_checked)
				{
					memset(&g_s_task_list.list[j], 0, sizeof(STaskInfo));
				}
			}	
		}

			
	}/*获取已check任务列表*/	
	else if (buff[2] == PROTOCOL_GET_CHECK_LIST)
	{
		g_comm_id_get_list_ack = send_get_task_list_ack();
	}	

	/*更新窗口, 获取已check任务列表也会更新菜单，但是必须在收到通讯成功应答后*/
	if ((buff[2] == PROTOCOL_CHECK_LIST_ACK) || (buff[2] == PROTOCOL_UPDATE_LIST))
	{
		update_dida_menu();
	}

	return;
}


/*
 *--------------------------------------------------------------------------------------
 *     function:  msg_result_callback
 *    parameter: 
 *       return:
 *  description:  通讯结果回调
 * 	  other:
 *--------------------------------------------------------------------------------------
 */
void msg_result_callback(enum ECommResult status, uint32_t comm_id, void *context)
{
	
	/*如果更新任务应答发送失败，则重发*/
	if ((ECommResultFail == status) && (comm_id == g_comm_id_update_list_ack))
	{
		send_update_task_list_ack();
	}

	/*如果任务完成发送失败，则重发*/
	if((ECommResultFail == status) && (comm_id == g_comm_id_check_ok))
	{

	}

	/*如果任务完成发送失败，则重发*/
	if((ECommResultFail == status) && (comm_id == g_comm_id_get_list_ack))
	{
		send_get_task_list_ack();
	}/*成功，则清除*/
	else if((ECommResultSuccess == status) && (comm_id == g_comm_id_get_list_ack))
	{

#if 0
		int8_t i = 0;
		for (i = 0; i < TASK_LIST_MAX_SIZE; i++)
		{
			if (g_s_task_list.list[i].checked == CHECKING)
			{
				memset(&g_s_task_list.list[i], 0, sizeof(STaskInfo));
			}
		}

		/*更新菜单*/
		update_dida_menu();
#endif
	}

	return;
}


static void timer_callback(date_time_t tick_time, uint32_t millis, void *context)
{
	int8_t i = 0;
	NotifyParam	param;

	if(maibu_get_phone_type() == PhoneTypeIOS)
	{
		return;
	}

	for (i = 0; i < TASK_LIST_MAX_SIZE; i++)
	{
		if ((g_s_task_list.list[i].used == NOT_USED) || (g_s_task_list.list[i].alarm == HAVE_ALARM))
		{
			continue;
		}

		/*获取当前时间，从1970开始的秒数*/
//		struct date_time t1;
//		app_service_get_datetime(&t1);
		uint32_t seconds = app_get_time(tick_time);		

//		printf("now  :%u, item:%u\n", seconds, g_s_task_list.list[i].item.date);
	
		/*如果没有提醒过，并且是在120秒以内，则提醒*/	
		if (120 > (seconds - g_s_task_list.list[i].item.date))
		{

			g_s_task_list.list[i].alarm = HAVE_ALARM;
			memset(&param, 0, sizeof(NotifyParam));
			res_get_user_bitmap(RES_BITMAP_WATCHAPP_DIDA_ALARM,  &param.bmp);	
			sprintf(param.main_title, "%s", "滴答清单");
			strcpy(param.sub_title, g_s_task_list.list[i].item.title);
			param.pulse_type = VibesPulseTypeMiddle;		
			param.pulse_time = 6;		
			maibu_service_sys_notify(&param);

		}
	}


}



int main()
{

#ifdef LINUX	
	/*非APP编写*/	
	screen_init(SCREEN_ROW_NUMS,SCREEN_COL_NUMS);
	os_store_manage_init();	
	window_stack_init();
	set_current_app(0x7dd01);
	SHOW;
#endif


	/*创建窗口*/
	P_Window p_window = create_window_dida(); 
	if(p_window != NULL)
	{
		/*放入窗口栈显示*/
		g_window_id = app_window_stack_push(p_window);

//		printf("------------------\n");

		/*注册接受数据回调函数*/
		maibu_comm_register_msg_callback(msg_recv_callback);

		/*注册通讯结果状态回调*/
		maibu_comm_register_result_callback(msg_result_callback);

		/*注册定时器, Android任务到时提醒*/
		app_service_timer_subscribe(30000, timer_callback, (void *)p_window);
	}



//	printf("11111111111111\n");

#ifdef LINUX
	SHOW;
	/*非APP编写*/	
	while (1)
	{

		char input;	
	
		/*输入操作*/
		scanf("%c", &input);
		if (input == 'q')
		{
			break;	
		}
		else if (input == 'a')
		{
			static int s_index = 0;
			s_index++;
			uint8_t data[250] = {0xdd,0x01,0x01,0xa0};
			uint8_t i = 0;
	
			STaskItem task;
			for(i= 0; i < 5; i++)
			{
				memset(&task, 0, sizeof(STaskItem));
				task.id = i;
				task.date = 16777216;
				sprintf(task.title, "title:%d", i);	
				memcpy(&data[4+i*sizeof(STaskItem)], &task, sizeof(STaskItem));
			}

			msg_recv_callback("link_id_1", data, 4+sizeof(STaskItem)*5);	
	
		}else if(input == 'c')
		{
			uint8_t data[250] = {0xdd, 0x01, 0x22, 0x04, 0x02, 0x00, 0x00, 0x00};
			msg_recv_callback("link_id_1", data, 8);	
		}
		else if (input == 'g')
		{
			uint8_t data[250] = {0xdd, 0x01, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00};
			msg_recv_callback("link_id_1", data, 8);	
		}
		else if(input == 'k')
		{
			msg_result_callback(ECommResultSuccess, g_comm_id_get_list_ack, NULL);
		}
		else if(input == 'u')
		{
			window_stack_button(ButtonIdUp);	
		}else if(input == 'd')
		{
			window_stack_button(ButtonIdDown);	
		}else if(input == 's')
		{
			window_stack_button(ButtonIdSelect);	
		}

	}	

	app_window_stack_pop(p_window);
	window_stack_destory();
	screen_destory();
	os_store_manage_destory();

	SHOW;

#endif

	return 0;

}


