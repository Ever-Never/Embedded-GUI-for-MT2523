/*
 * clock_test.c
 *
 *  Created on: 2016年8月7日
 *      Author: fine
 */
#include <unistd.h>
#include <aroma.h>
#include "resource.h"
#include "message_map_queue.h"

void clock_touch_handler(dword command, LIBAROMA_MSGP msg)
{
		byte cmd = LIBAROMA_CMD(command);
		word id = LIBAROMA_CMD_ID(command);
		byte param = LIBAROMA_CMD_PARAM(command);
		int freq = 3000 ;
		if(id ==3){
			printf("bike click !!!! \r\n") ;
			LIBAROMA_MSG msg ;
			msg.sent = 3000 ;
			MessageSend(UI_SET_UPDATE_FREQUECY,&msg,sizeof(LIBAROMA_MSG),MESSAGE_IS_POINTER) ;
			printf("send !!!! \r\n") ;
			
		}

	    printf("Window Command = (CMD: %x, ID: %x, Param: %x) \r\n",
					LIBAROMA_CMD(command), LIBAROMA_CMD_ID(command),
					LIBAROMA_CMD_PARAM(command));

}

void clock_test() {


	printf(" clock test start --------------> \r\n") ;
	LIBAROMA_WINDOWP win = libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL,
			LIBAROMA_SIZE_FULL); /* fullscreen */

	LIBAROMA_CONTROLP clock = libaroma_ctl_clock(win, 1, 0, 0,
			LIBAROMA_SIZE_HALF, LIBAROMA_SIZE_HALF, "finezhang",
			LIBAROMA_CTL_BUTTON_FLAT, 0);

	LIBAROMA_CONTROLP clock2 = libaroma_ctl_clock(win, 2, LIBAROMA_POS_HALF, 0,
			LIBAROMA_SIZE_HALF, LIBAROMA_SIZE_HALF, "finezhang",
			LIBAROMA_CTL_BUTTON_FLAT, 0);
	LIBAROMA_CONTROLP image = libaroma_ctl_image(win,3,NULL,libaroma_px(0),libaroma_px(160),LIBAROMA_SIZE_HALF,LIBAROMA_SIZE_HALF);

	LIBAROMA_STREAMP stream = libaroma_stream_mem(res_get(RES_INDEX_RUN_BIKE),res_get_length(RES_INDEX_RUN_BIKE)) ;
	LIBAROMA_CANVASP canvas = libaroma_image_ex(stream,1,0) ;
	libaroma_ctl_image_set_canvas(image,canvas,1) ;
	libaroma_ctl_progress(
        win, 5,
        libaroma_px(160), libaroma_px(160), LIBAROMA_SIZE_HALF, libaroma_px(20),
        LIBAROMA_CTL_PROGRESS_INDETERMINATE,
        100,
        0);

	// LIBAROMA_CONTROLP btn3 = libaroma_ctl_button(win ,6 ,libaroma_px(160), libaroma_px(180), LIBAROMA_SIZE_HALF,
	// 		 libaroma_px(80), "Change Style", LIBAROMA_CTL_BUTTON_COLORED,
	// 		RGB(ff00ff));

	//libaroma_ctl_label_valign(win, 6, "10:20", libaroma_px(160), libaroma_px(180), libaroma_px(51), libaroma_px(24), RGB(808080), 0, 3, LIBAROMA_TEXT_CENTER, 100, 10);

	libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_BOTTOM, 200);
	libaroma_window_set_touch_handler(win,clock_touch_handler) ;
	while(1) ;
	byte onpool = 1;
	do {
		LIBAROMA_MSG msg;
		dword command = libaroma_window_pool(win, &msg);
		byte cmd = LIBAROMA_CMD(command);
		word id = LIBAROMA_CMD_ID(command);
		byte param = LIBAROMA_CMD_PARAM(command);

		if (msg.msg == LIBAROMA_MSG_KEY_SELECT) {
			if (msg.state == 0) {
				printf("Screenshoot... and exit\r\n");
				onpool = 0;
				break;
			}
		}
		if(id ==3){
			printf(" exit current window \r\n") ;
			break ;
		}

		printf("Window Command = (CMD: %x, ID: %x, Param: %x) \r\n",
					LIBAROMA_CMD(command), LIBAROMA_CMD_ID(command),
					LIBAROMA_CMD_PARAM(command));

	} while (onpool);

	libaroma_window_free(win);
	libaroma_canvas_free(canvas) ;
	// libaroma_stream_close(stream);


}

