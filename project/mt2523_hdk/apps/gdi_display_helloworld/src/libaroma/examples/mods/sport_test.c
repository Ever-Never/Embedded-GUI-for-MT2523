

#include <aroma.h>

void sport_test()
{

    LIBAROMA_COLORSETP colorset = (LIBAROMA_COLORSETP)calloc(sizeof(LIBAROMA_COLORSET), 1);
    libaroma_colorset(colorset, 0);

    LIBAROMA_WINDOWP win = libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL,
                                           LIBAROMA_SIZE_FULL); /* fullscreen */
    win->colorset = colorset;
    LIBAROMA_CONTROLP clock = libaroma_ctl_sport(win, 1, libaroma_px(18), libaroma_px(18), libaroma_px(324), libaroma_px(162), RGB(083224), RGB(10B886), 12);
    libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_BOTTOM, 400);

    libaroma_ctl_label_valign(win, 0, "10:20", libaroma_px(155), libaroma_px(55), libaroma_px(51), libaroma_px(24), RGB(808080), 0, 3, LIBAROMA_TEXT_CENTER, 100, 10);
    libaroma_ctl_label_valign(win, 1, "2568", libaroma_px(97), libaroma_px(75), libaroma_px(166), libaroma_px(115), RGB(10DCA0), 0, 30, LIBAROMA_TEXT_CENTER | LIBAROMA_TEXT_BOLD, 0, 10);
    libaroma_ctl_label_valign(win, 2, "步数", libaroma_px(150), libaroma_px(160), libaroma_px(60), libaroma_px(29), RGB(808080), 1, 3, LIBAROMA_TEXT_CENTER, 0, 10);

    LIBAROMA_CONTROLP image = libaroma_ctl_image(win, 3, "/tmp/00.png", libaroma_px(0), libaroma_px(180), libaroma_px(360), libaroma_px(180));
    libaroma_ctl_image_set_animation(image, "/tmp", 100, 63, 1);
    libaroma_ctl_image_hidden(image, 0);

    byte onpool = 1;
    do
    {
        LIBAROMA_MSG msg;
        dword command = libaroma_window_pool(win, &msg);
        byte cmd = LIBAROMA_CMD(command);
        word id = LIBAROMA_CMD_ID(command);
        byte param = LIBAROMA_CMD_PARAM(command);

        printf("msg %d \n", msg.msg);
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

    } while (onpool);

    libaroma_window_free(win);
}