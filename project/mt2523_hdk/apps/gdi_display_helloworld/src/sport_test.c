

#include <aroma.h>
// #include <aroma_internal.h>
#include "FreeRTOS.h"
#include "task.h"
#include "resource.h"

void sport_test()
{
    // while(1);

    // LIBAROMA_COLORSETP colorset = (LIBAROMA_COLORSETP)calloc(sizeof(LIBAROMA_COLORSET), 1);
    // libaroma_colorset(colorset, 0);

    // win->colorset = colorset;
    LIBAROMA_WINDOWP win = libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL,
                                           LIBAROMA_SIZE_FULL); /* fullscreen */
    LIBAROMA_CONTROLP pager = libaroma_ctl_pager(win, 1, 2, 0, 0,LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL);

    LIBAROMA_WINDOWP win2 = libaroma_ctl_pager_get_window(pager);
    LIBAROMA_CONTROLP image = libaroma_ctl_image(win2, 3, NULL, 0, 0, libaroma_px(320), libaroma_px(320));
    // LIBAROMA_CONTROLP image1 = libaroma_ctl_image(win2, 4, NULL, libaroma_px(320), 0, libaroma_px(320), libaroma_px(320));

    LIBAROMA_STREAMP stream = libaroma_stream_mem(res_get(RES_INDEX_RUN_BIKE), res_get_length(RES_INDEX_RUN_BIKE));
    LIBAROMA_CANVASP canvas = libaroma_image_ex(stream, 1, 0);
    // LIBAROMA_STREAMP stream1 = libaroma_stream_mem(res_get(RES_INDEX_NOTICE_WECHAT), res_get_length(RES_INDEX_NOTICE_WECHAT));
    // LIBAROMA_CANVASP canvas1 = libaroma_image_ex(stream1, 1, 0);
    libaroma_ctl_image_set_canvas(image, canvas, 1);
    // libaroma_ctl_image_set_canvas(image1, canvas1, 1);
    // LIBAROMA_CONTROLP clock = libaroma_ctl_clock(win2, 2, 0, 0,
    //                                              libaroma_px(320), libaroma_px(320), "finezhang",
    // 		LIBAROMA_CTL_BUTTON_FLAT, 0);
    LIBAROMA_CONTROLP qrcode = libaroma_ctl_pay(win2, 5, libaroma_px(320), 0, libaroma_px(320), libaroma_px(320), "[FNC1] 00 12345678 0000112201", 0);
    // LIBAROMA_CONTROLP bar = libaroma_ctl_pay(win2, 6, libaroma_px(320) * 2, libaroma_px(120), libaroma_px(320), libaroma_px(80), "[FNC1] 00 12345678 00001122  ", 1);
    libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_LEFT, 300);

    // libaroma_ctl_label_valign(win, 0, "10:20", libaroma_px(155), libaroma_px(55), libaroma_px(51), libaroma_px(24), RGB(808080), 0, 3, LIBAROMA_TEXT_CENTER, 100, 10);
    // libaroma_ctl_label_valign(win, 1, "2568", libaroma_px(97), libaroma_px(75), libaroma_px(166), libaroma_px(115), RGB(10DCA0), 0, 30, LIBAROMA_TEXT_CENTER | LIBAROMA_TEXT_BOLD, 0, 10);
    // libaroma_ctl_label_valign(win, 2, "步数", libaroma_px(150), libaroma_px(160), libaroma_px(60), libaroma_px(29), RGB(808080), 1, 3, LIBAROMA_TEXT_CENTER, 0, 10);

    // LIBAROMA_CONTROLP image = libaroma_ctl_image(win, 3, "/tmp/00.png", libaroma_px(0), libaroma_px(180), libaroma_px(360), libaroma_px(180));
    // libaroma_ctl_image_set_animation(image, "/tmp", 100, 63, 1);
    // libaroma_ctl_image_hidden(image, 0);
    while (1)
        ;
    byte onpool = 1;
    do
    {
        LIBAROMA_MSG msg;
        dword command = libaroma_window_pool(win, &msg);
        byte cmd = LIBAROMA_CMD(command);
        word id = LIBAROMA_CMD_ID(command);
        byte param = LIBAROMA_CMD_PARAM(command);
        // vTaskPrioritySet(xTaskGetCurrentTaskHandle(),0) ;

        // xTaskGetCurrentTaskHandle

        // printf("msg %d \n", msg.msg);
        if (msg.msg == LIBAROMA_MSG_KEY_SELECT)
        {
            if (msg.state == 0)
            {
                printf("Screenshoot... and exit\n");
                onpool = 0;
                break;
            }
        }

        if (msg.msg == LIBAROMA_MSG_KEY_RAWKEY)
        {
            if (msg.state == 0)
            {
                printf("Screenshoot... and exit\n");
                onpool = 0;
                break;
            }
        }
        printf("Window Command = (CMD: %x, ID: %x, Param: %x) \r\n",
               LIBAROMA_CMD(command), LIBAROMA_CMD_ID(command),
               LIBAROMA_CMD_PARAM(command));
        //  vTaskSwitchContext() ;
        taskYIELD();

    } while (onpool);

    libaroma_window_free(win);
}