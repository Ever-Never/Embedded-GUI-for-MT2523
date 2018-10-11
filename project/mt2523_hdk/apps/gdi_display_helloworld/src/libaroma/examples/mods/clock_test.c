/*
 * clock_test.c
 *
 *  Created on: 2016年8月7日
 *      Author: fine
 */
#include <unistd.h>
#include <aroma.h>

void clock_test() {
	// pid_t pid = fork();
	// if (pid == 0) {
	// 	 char* stop = "/system/bin/stop";
	// 	char * argv[] = { stop, NULL };
	// 	execv(stop, argv);
	// }

	LIBAROMA_WINDOWP win = libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL,
			LIBAROMA_SIZE_FULL); /* fullscreen */

	// LIBAROMA_CONTROLP clock = libaroma_ctl_clock(win, 1, 0, 0,
	// 		LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL, "finezhang",
	// 		LIBAROMA_CTL_BUTTON_FLAT, 0);

	LIBAROMA_CONTROLP clock2 = libaroma_ctl_clock(win, 1, 0, 0,
			LIBAROMA_SIZE_HALF, LIBAROMA_SIZE_FULL, "finezhang",
			LIBAROMA_CTL_BUTTON_FLAT, 0);

	LIBAROMA_CONTROLP clock2 = libaroma_ctl_clock(win, 2, LIBAROMA_POS_HALF, 0,
			LIBAROMA_SIZE_HALF, LIBAROMA_SIZE_FULL, "finezhang",
			LIBAROMA_CTL_BUTTON_FLAT, 0);


	libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_BOTTOM, 400);

	byte onpool = 1;
	do {
		LIBAROMA_MSG msg;
		dword command = libaroma_window_pool(win, &msg);
		byte cmd = LIBAROMA_CMD(command);
		word id = LIBAROMA_CMD_ID(command);
		byte param = LIBAROMA_CMD_PARAM(command);

		if (msg.msg == LIBAROMA_MSG_KEY_SELECT) {
			if (msg.state == 0) {
				printf("Screenshoot... and exit\n");
				onpool = 0;
				break;
			}
		}

	} while (onpool);

	libaroma_window_free(win);

	//  char* start = "/system/bin/start";
	// char * argv[] = { start, NULL };
	// execv(start, argv);

}

