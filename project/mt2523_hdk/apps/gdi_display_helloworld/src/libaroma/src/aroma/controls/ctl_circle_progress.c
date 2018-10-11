#ifndef __libaroma_ctl_sport_c__
#define __libaroma_ctl_sport_c__
#include <aroma_internal.h>
#include "../ui/ui_internal.h"
#include <math.h>
#define MYPI 3.14159265358979323846

dword _libaroma_ctl_sport_msg(LIBAROMA_CONTROLP, LIBAROMA_MSGP);
void _libaroma_ctl_sport_draw(LIBAROMA_CONTROLP, LIBAROMA_CANVASP);
void _libaroma_ctl_sport_destroy(LIBAROMA_CONTROLP);
byte _libaroma_ctl_sport_thread(LIBAROMA_CONTROLP);

static LIBAROMA_CONTROL_HANDLER _libaroma_ctl_sport_handler =
    {
      message : NULL,
      draw : _libaroma_ctl_sport_draw,
      focus : NULL,
      destroy : _libaroma_ctl_sport_destroy,
      thread : _libaroma_ctl_sport_thread
    };

typedef struct __LIBAROMA_CTL_SPORT _LIBAROMA_CTL_SPORT;
typedef struct __LIBAROMA_CTL_SPORT *_LIBAROMA_CTL_SPORTP;
struct __LIBAROMA_CTL_SPORT
{
    word fcolor;
    word bcolor;
    int progress;
    float width;
    byte forcedraw;
    LIBAROMA_MUTEX mutex;
};

LIBAROMA_CONTROLP libaroma_ctl_sport(LIBAROMA_WINDOWP win, word id, int x,
                                     int y, int w, int h, word b_color,
                                     word f_color, float f_width)
{
    /* init internal data */
    _LIBAROMA_CTL_SPORTP me = (_LIBAROMA_CTL_SPORTP)
        calloc(sizeof(_LIBAROMA_CTL_SPORT), 1);
    if (!me)
    {
        ALOGW("libaroma_ctl_sport alloc clock memory failed");
        return NULL;
    }

    /* set internal data */
    libaroma_mutex_init(me->mutex);
    me->width = f_width;
    me->bcolor = b_color;
    me->fcolor = f_color;
    me->forcedraw = 1;
    /* init control */
    LIBAROMA_CONTROLP ctl = libaroma_control_new(id, x, y, w, h,
                                                 libaroma_dp(48), libaroma_dp(48), /* min size */
                                                 (voidp)me, &_libaroma_ctl_sport_handler, win);
    if (!ctl)
    {
        libaroma_mutex_free(me->mutex);
        free(me);
        return NULL;
    }
    return ctl;
} /* End of libaroma_ctl_button */

void _libaroma_ctl_sport_draw(LIBAROMA_CONTROLP ctl, LIBAROMA_CANVASP c)
{
    _LIBAROMA_CTL_CHECK(_libaroma_ctl_sport_handler, _LIBAROMA_CTL_SPORTP, );
    libaroma_control_erasebg(ctl, c);
    float r_x = c->w >> 1;
    float r_y = c->h;
    libaroma_draw_arc(c, r_x, r_x, r_x, r_x, me->width, -180, 0, me->bcolor, 0XFF, 0, 0.8);
    libaroma_draw_arc(c, r_x, r_x, r_x, r_x, me->width, -180, -150, me->fcolor, 0XFF, 0, 0.8);
}

void _libaroma_ctl_sport_destroy(LIBAROMA_CONTROLP ctl)
{
    /* internal check */
    _LIBAROMA_CTL_CHECK(_libaroma_ctl_sport_handler, _LIBAROMA_CTL_SPORTP, );
    libaroma_mutex_lock(me->mutex);

    libaroma_mutex_free(me->mutex);
    free(me);
}

byte _libaroma_ctl_sport_thread(LIBAROMA_CONTROLP ctl)
{
    /* internal check */
    _LIBAROMA_CTL_CHECK(_libaroma_ctl_sport_handler, _LIBAROMA_CTL_SPORTP, 0);
    if (me->forcedraw)
    {
        me->forcedraw = 0;
        return 1;
    }
    return 0;
}

#endif
