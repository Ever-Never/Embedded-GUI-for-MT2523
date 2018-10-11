#ifndef __libaroma_ctl_clock_c__
#define __libaroma_ctl_clock_c__
#include <aroma_internal.h>
#include "../ui/ui_internal.h"
#include <stdio.h>
#include <math.h>
#include "hal_rtc.h"
#define MYPI 3.14159265358979323846

dword _libaroma_ctl_clock_msg(LIBAROMA_CONTROLP, LIBAROMA_MSGP);
void _libaroma_ctl_clock_draw(LIBAROMA_CONTROLP, LIBAROMA_CANVASP);
void _libaroma_ctl_clock_destroy(LIBAROMA_CONTROLP);
byte _libaroma_ctl_clock_thread(LIBAROMA_CONTROLP);
dword _libaroma_ctl_clock_msg(LIBAROMA_CONTROLP ctl, LIBAROMA_MSGP msg);

void gettime(int *hour, int *min, int *sec)
{
	hal_rtc_time_t cur_time;
	hal_rtc_get_time(&cur_time);
	*hour = cur_time.rtc_hour;
	*min = cur_time.rtc_min;
	*sec = cur_time.rtc_sec;
}

static LIBAROMA_CONTROL_HANDLER _libaroma_ctl_clock_handler =
	{
		message : _libaroma_ctl_clock_msg,
		draw : _libaroma_ctl_clock_draw,
		focus : NULL,
		destroy : _libaroma_ctl_clock_destroy,
		thread : _libaroma_ctl_clock_thread
	};

typedef struct __LIBAROMA_CTL_CLOCK _LIBAROMA_CTL_CLOCK;
typedef struct __LIBAROMA_CTL_CLOCK *_LIBAROMA_CTL_CLOCKP;
struct __LIBAROMA_CTL_CLOCK
{
	char *text;
	byte style;
	byte isdark;
	byte s;
	int hour;
	int min;
	int sec;
	word color;
	byte forcedraw;

	LIBAROMA_RIPPLE ripple;
	LIBAROMA_MUTEX mutex;
};

dword _libaroma_ctl_clock_msg(LIBAROMA_CONTROLP ctl, LIBAROMA_MSGP msg)
{
	/* internal check */
	_LIBAROMA_CTL_CHECK(
		_libaroma_ctl_clock_handler, _LIBAROMA_CTL_CLOCKP, 0);

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
		//ALOGD("=============>touch x %d y %d<=============== \r\t", x, y);
		/* touch handler */
		if (msg->state == LIBAROMA_HID_EV_STATE_UP)
		{
			libaroma_window_post_command(
				LIBAROMA_CMD_SET(LIBAROMA_CMD_CLICK, 0, ctl->id));
		}
	}
	break;
	}
	return 0;
}

LIBAROMA_CONTROLP libaroma_ctl_clock(LIBAROMA_WINDOWP win, word id, int x,
									 int y, int w, int h, const char *text, byte button_style,
									 word button_color)
{
	/* init internal data */
	_LIBAROMA_CTL_CLOCKP me = (_LIBAROMA_CTL_CLOCKP)
		calloc(sizeof(_LIBAROMA_CTL_CLOCK), 1);
	if (!me)
	{
		ALOGW("libaroma_ctl_clock alloc clock memory failed");
		return NULL;
	}

	/* set internal data */
	libaroma_mutex_init(me->mutex);
	// me->text = strdup(text);
	me->style = button_style;
	me->color = button_color;
	gettime(&me->hour, &me->min, &me->sec);

	/* init control */
	LIBAROMA_CONTROLP ctl = libaroma_control_new(id, x, y, w, h,
												 libaroma_dp(48), libaroma_dp(48), /* min size */
												 (voidp)me, &_libaroma_ctl_clock_handler, win);
	if (!ctl)
	{
		libaroma_mutex_free(me->mutex);
		if (me->text != NULL)
		{
			free(me->text);
		}
		free(me);
		return NULL;
	}
	me->forcedraw = 1;
	return ctl;
} /* End of libaroma_ctl_button */

void getLoacationOnCircles(int cx, int cy, int radius, int angle, int *outx,
						   int *outy)
{
	float tmp = angle * (2 * MYPI / 360);
	if (angle >= 0 && angle <= 90)
	{
		*outx = cx + radius * sin(tmp);
		*outy = cy - radius * cos(tmp);
	}
	else if (angle > 90 && angle <= 180)
	{
		tmp = tmp - MYPI / 2;
		*outx = cx + radius * cos(tmp);
		*outy = cy + radius * sin(tmp);
	}
	else if (angle > 180 && angle <= 270)
	{
		tmp = tmp - MYPI;
		*outx = cx - radius * sin(tmp);
		*outy = cy + radius * cos(tmp);
	}
	else if (angle > 270 && angle <= 360)
	{
		tmp = tmp - (MYPI + MYPI / 2);
		*outx = cx - radius * cos(tmp);
		*outy = cy - radius * sin(tmp);
	}
	//printf("loacation on x:%d  y:%d  \t\n", *outx, *outy);
}

void _libaroma_ctl_clock_draw(LIBAROMA_CONTROLP ctl, LIBAROMA_CANVASP c)
{
	//_LIBAROMA_CTL_CHECK(_libaroma_ctl_clock_handler, _LIBAROMA_CTL_CLOCKP);
	_LIBAROMA_CTL_CHECK(_libaroma_ctl_clock_handler, _LIBAROMA_CTL_CLOCKP, );
	libaroma_control_erasebg(ctl, c);
	int sz = MIN(c->w >> 1, c->h >> 1);
	int outx, outy;
	libaroma_draw_circle(c, RGB(ff0000), sz, sz, sz - 130, 0xff);
	libaroma_draw_circle(c, RGB(00ff00), sz, sz, sz - 150, 0xff);
	libaroma_draw_arc(c, sz, sz, sz, sz, 15, 0, 360, RGB(10DCA0),
					  0x80, 0, 0.8);
	int tmphour = me->hour > 12 ? me->hour - 12 : me->hour;
	getLoacationOnCircles(sz, sz, sz, tmphour * 1.0 / 12 * 360,
						  &outx, &outy);
	libaroma_draw_line(c, sz, sz, outx, outy, 13, RGB(342756),
					   0x7f, 0);
	getLoacationOnCircles(sz, sz, sz, me->min * 1.0 / 60 * 360,
						  &outx, &outy);
	libaroma_draw_line(c, sz, sz, outx, outy, 8, RGB(348856),
					   0x7f, 0);
	getLoacationOnCircles(sz, sz, sz - 15,
						  me->sec * 1.0 / 60 * 360, &outx, &outy);
	libaroma_draw_line(c, sz, sz, outx, outy, 5, RGB(349999),
					   0x7f, 0);

	libaroma_draw_circle(c, RGB(ff00ff), sz, sz, 20, 0xff);
}

void _libaroma_ctl_clock_destroy(LIBAROMA_CONTROLP ctl)
{
	/* internal check */
	_LIBAROMA_CTL_CHECK(_libaroma_ctl_clock_handler, _LIBAROMA_CTL_CLOCKP, );
	libaroma_mutex_lock(me->mutex);

	libaroma_mutex_unlock(me->mutex);
	libaroma_mutex_free(me->mutex);
	free(me);
}

byte _libaroma_ctl_clock_thread(LIBAROMA_CONTROLP ctl)
{
	/* internal check */
	_LIBAROMA_CTL_CHECK(_libaroma_ctl_clock_handler, _LIBAROMA_CTL_CLOCKP, 0);
	int hour = 0, min = 0, sec = 0;
	gettime(&hour, &min, &sec);
	if (me->hour != hour || me->min != min || me->sec != sec)
	{
		me->hour = hour;
		me->min = min;
		me->sec = sec;
		me->forcedraw = 1;
	}
	if (me->forcedraw)
	{
		me->forcedraw = 0;
		return 1;
	}
	return 0;
}

#endif
