

#include <aroma.h>

void applist_test()
{

    LIBAROMA_COLORSETP colorset = (LIBAROMA_COLORSETP)calloc(sizeof(LIBAROMA_COLORSET), 1);
    libaroma_colorset(colorset, 0);

    LIBAROMA_WINDOWP win = libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL,
                                           LIBAROMA_SIZE_FULL); /* fullscreen */
    win->colorset = colorset;
   
    LIBAROMA_CONTROLP image_music = libaroma_ctl_image(win,1,"/tmp/music.png",libaroma_px(84),libaroma_px(48),libaroma_px(72),libaroma_px(72)) ;
    LIBAROMA_CONTROLP image_wechatpay = libaroma_ctl_image(win,2,"/tmp/wetchatpay.png",libaroma_px(84),libaroma_px(144),libaroma_px(72),libaroma_px(72)) ;
    LIBAROMA_CONTROLP image_alipay = libaroma_ctl_image(win,3,"/tmp/alipay.png",libaroma_px(84),libaroma_px(240),libaroma_px(72),libaroma_px(72)) ;
    libaroma_ctl_label_valign(win, 4, "音乐", libaroma_px(174), libaroma_px(63), libaroma_px(56), libaroma_px(42), RGB(FFFFFF), 1, 5, LIBAROMA_TEXT_LEFT, 0, 10);
    libaroma_ctl_label_valign(win, 5, "微信支付", libaroma_px(174), libaroma_px(159), libaroma_px(112), libaroma_px(42), RGB(FFFFFF), 1, 5, LIBAROMA_TEXT_LEFT, 0, 10);
    libaroma_ctl_label_valign(win, 6, "支付宝", libaroma_px(174), libaroma_px(255), libaroma_px(84), libaroma_px(42), RGB(FFFFFF), 1, 5, LIBAROMA_TEXT_LEFT, 0, 10);

    libaroma_ctl_image_hidden(image_music,0) ;

    libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_BOTTOM, 400);

    byte onpool = 1;
    do
    {
        LIBAROMA_MSG msg;
        dword command = libaroma_window_pool(win, &msg);
        byte cmd = LIBAROMA_CMD(command);
        word id = LIBAROMA_CMD_ID(command);
        byte param = LIBAROMA_CMD_PARAM(command);

        if (msg.msg == LIBAROMA_MSG_KEY_SELECT)
        {
            if (msg.state == 0)
            {
                printf("Screenshoot... and exit\n");
                onpool = 0;
                break;
            }
        }

    } while (onpool);

    libaroma_window_free(win);
}