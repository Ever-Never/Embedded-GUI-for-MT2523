#ifndef __libaroma_ctl_pay_c__
#define __libaroma_ctl_pay_c__
#include <aroma_internal.h>
#include "../../ui/ui_internal.h"
#include <stdio.h>
#include <math.h>
#include "qrencode.h"
// #include <code128.h>
#define MYPI 3.14159265358979323846

dword _libaroma_ctl_pay_msg(LIBAROMA_CONTROLP, LIBAROMA_MSGP);
void _libaroma_ctl_pay_draw(LIBAROMA_CONTROLP, LIBAROMA_CANVASP);
void _libaroma_ctl_pay_destroy(LIBAROMA_CONTROLP);
byte _libaroma_ctl_pay_thread(LIBAROMA_CONTROLP);
dword _libaroma_ctl_pay_msg(LIBAROMA_CONTROLP ctl, LIBAROMA_MSGP msg);

static LIBAROMA_CONTROL_HANDLER _libaroma_ctl_pay_handler =
    {
        message : _libaroma_ctl_pay_msg,
        draw : _libaroma_ctl_pay_draw,
        focus : NULL,
        destroy : _libaroma_ctl_pay_destroy,
        thread : _libaroma_ctl_pay_thread
    };

typedef struct __LIBAROMA_CTL_PAY _LIBAROMA_CTL_PAY;
typedef struct __LIBAROMA_CTL_PAY *_LIBAROMA_CTL_PAYP;
struct __LIBAROMA_CTL_PAY
{
    char *text;
    byte style;
    word color;
    char *barcode_data;
    size_t bar_length;
    byte forcedraw;
    QRcode *qrcode;
    LIBAROMA_MUTEX mutex;
};

dword _libaroma_ctl_pay_msg(LIBAROMA_CONTROLP ctl, LIBAROMA_MSGP msg)
{
    /* internal check */
    _LIBAROMA_CTL_CHECK(
        _libaroma_ctl_pay_handler, _LIBAROMA_CTL_PAYP, 0);

    switch (msg->msg)
    {
    case LIBAROMA_MSG_WIN_ACTIVE:
    case LIBAROMA_MSG_WIN_INACTIVE:
    case LIBAROMA_MSG_WIN_RESIZE:
    {
        libaroma_mutex_lock(me->mutex);
        me->forcedraw = 1;
        libaroma_mutex_unlock(me->mutex);
    }
    break;
    case LIBAROMA_MSG_TOUCH:
    {
        int x = msg->x;
        int y = msg->y;
        ALOGD("=============> ctl_pay touch x %d y %d<===============", x, y);
        /* touch handler */
        if (msg->state == LIBAROMA_HID_EV_STATE_UP)
        {
            libaroma_window_post_command(LIBAROMA_CMD_SET(LIBAROMA_CMD_CLICK, 0, ctl->id));
        }
    }
    break;
    }
    return 0;
}

LIBAROMA_CONTROLP libaroma_ctl_pay(LIBAROMA_WINDOWP win, word id, int x,
                                   int y, int w, int h, const char *text, byte pay_style)
{
    /* init internal data */
    _LIBAROMA_CTL_PAYP me = (_LIBAROMA_CTL_PAYP)
        calloc(sizeof(_LIBAROMA_CTL_PAY), 1);
    if (!me)
    {
        ALOGW("libaroma_ctl_pay alloc pay memory failed");
        return NULL;
    }

    /* set internal data */
    libaroma_mutex_init(me->mutex);
    me->style = pay_style;
    me->text = strdup(text);
    if (me->style == 1)
    {
        size_t barcode_length = code128_estimate_len(me->text);
        me->barcode_data = (char *)malloc(barcode_length);
        me->bar_length = code128_encode_gs1(me->text, me->barcode_data, barcode_length);
    }
    else
    {
        me->qrcode = QRcode_encodeString(me->text, 1, QR_ECLEVEL_L, QR_MODE_8, 0);
    }

    /* init control */
    LIBAROMA_CONTROLP ctl = libaroma_control_new(id, x, y, w, h,
                                                 libaroma_dp(48), libaroma_dp(48), /* min size */
                                                 (voidp)me, &_libaroma_ctl_pay_handler, win);
    if (!ctl)
    {
        libaroma_mutex_free(me->mutex);
        if (me->text != NULL)
        {
            free(me->barcode_data);
            free(me->text);
        }
        if (me->barcode_data != NULL)
        {
            free(me->barcode_data);
        }
        free(me);
        return NULL;
    }
    me->forcedraw = 1;
    return ctl;
} /* End of libaroma_ctl_button */

void _libaroma_ctl_pay_draw(LIBAROMA_CONTROLP ctl, LIBAROMA_CANVASP c)
{
    //_LIBAROMA_CTL_CHECK(_libaroma_ctl_pay_handler, _LIBAROMA_CTL_CLOCKP);
    _LIBAROMA_CTL_CHECK(_libaroma_ctl_pay_handler, _LIBAROMA_CTL_PAYP, );
    libaroma_control_erasebg(ctl, c);
    int sz = MIN(c->w, c->h);
    if (me->style == 1)
    {
        for (int i = 0; i < c->h; i++)
        {
            if (me->bar_length <= c->w)
            {
                int start = (c->w - me->bar_length) / 2;
                for (int z = 0; z < me->bar_length; z++)
                {
                    if ((me->barcode_data)[z] == 0xFF)
                    {
                        libaroma_draw_pixel(c, start + z, i, RGB(00000), 0xff);
                    }
                    else
                    {
                        libaroma_draw_pixel(c, start + z, i, RGB(FFFFFF), 0xff);
                    }
                }
            }
        }
    }
    else
    {

        if (sz >= me->qrcode->width)
        {

            int scale = sz / me->qrcode->width;

            int i = 0;
            int j = 0;
            int start = 0;
            for (i = 0; i < me->qrcode->width; i++)
            {
                for (j = 0; j < me->qrcode->width; j++)
                {
                    if (me->qrcode->data[i * me->qrcode->width + j] & 0x01)
                    {

                        if (c->w >= c->h)
                        {
                            start = (c->w - me->qrcode->width * scale) / 2;
                            libaroma_draw_rect(c, scale * j + start, scale * i, scale, scale, RGB(00000), 0xff);
                        }
                        else
                        {
                            start = (c->h - me->qrcode->width * scale) / 2;
                            libaroma_draw_rect(c, scale * j, scale * i + start, scale, scale, RGB(00000), 0xff);
                        }
                    }
                    else
                    {
                        if (c->w >= c->h)
                        {
                            start = (c->w - me->qrcode->width * scale) / 2;
                            libaroma_draw_rect(c, scale * j + start, scale * i, scale, scale, RGB(FFFFFF), 0xff);
                        }
                        else
                        {
                            start = (c->h - me->qrcode->width * scale) / 2;
                            libaroma_draw_rect(c, scale * j, scale * i + start, scale, scale, RGB(FFFFFF), 0xff);
                        }
                    }
                }
            }
        }
    }
}

void _libaroma_ctl_pay_destroy(LIBAROMA_CONTROLP ctl)
{
    /* internal check */
    _LIBAROMA_CTL_CHECK(_libaroma_ctl_pay_handler, _LIBAROMA_CTL_PAYP, );
    libaroma_mutex_lock(me->mutex);
    if (me->text != NULL)
    {
        free(me->text);
    }
    if (me->barcode_data != NULL)
    {
        free(me->barcode_data);
    }
    if (me->qrcode != NULL)
    {
        if (me->qrcode->data != NULL)
        {
            free(me->qrcode->data) ;
        }
        free(me->qrcode);
    }
    libaroma_mutex_unlock(me->mutex);
    libaroma_mutex_free(me->mutex);
    free(me);
}

byte _libaroma_ctl_pay_thread(LIBAROMA_CONTROLP ctl)
{
    /* internal check */
    _LIBAROMA_CTL_CHECK(_libaroma_ctl_pay_handler, _LIBAROMA_CTL_PAYP, 0);
    if (me->forcedraw)
    {
        me->forcedraw = 0;
        return 1;
    }
    return 0;
}

#endif
