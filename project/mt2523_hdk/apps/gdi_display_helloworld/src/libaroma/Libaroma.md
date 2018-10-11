

# 嵌入式GUI Libaroma

> 摘要： 
>	Libaroma是一个用纯C写的控件库，支持Linux、X86、QNX等不同平台。针对不同平台API做了一层封装，可以很快的移植到不同平台，参照Android的材料设计风格实现控件库，已经写好了Fragment、ViewPager、Button、ProgressBar、ListView、Dialog等控件，开发者可以实现自己的控件。目前Libaroma框架只绝对布局，控件的开发基本和Android类似，也有Window，WindowManagr、EventHub等概念，支持Window转场动画。



[TOC]




## 1 概述

Libaroma 框架其实和安卓很类似，有一个UI线程每16ms刷新下界面，一个触屏上报线程来反馈用户点击位置。WindowManager是一个管理者，负责管理Window的激活和非激活，也就是Window的切换，Window上面可以添加子控件，windowanger中的一个线程16ms刷新激活window的界面。


## 2 Libaroma框架初始化的过程

Libaroma 初始化过程主要包括fb初始化、hid初始化、font初始化、msg初始化、wm初始化。其中fb初始化主要是确定显示中需要的各种参数，注册相关平台刷新界面的回调函数、hid根据fb初始化后的参数来初始化自己的配置，在使用Libaroma框架的时候总要先初始化着几个流程，在程序最后释放初始化过程分配的内存，销毁线程等操作。

``` java

byte libaroma_start() {

  /* Mute Parent */
  if (libaroma_config()->runtime_monitor == LIBAROMA_START_MUTEPARENT) {
    libaroma_runtime_mute_parent();
  }
  
  ALOGI("===================================================");
  
  if (libaroma_config()->multicore_init_num>0){
    /* activate processor/cores */prefree
    libaroma_runtime_activate_cores(libaroma_config()->multicore_init_num);
  }
  
  if (!libaroma_fb_init()) {
    ALOGE("libaroma_start cannot start framebuffer...");
    return 0;
  }
  
  if (!libaroma_font_init()) {
    ALOGE("libaroma_start cannot start font engine...");
    return 0;
  }

  if (!libaroma_hid_init()) {
    ALOGE("libaroma_start cannot start hid engine...");
    return 0;
  }

  if (!libaroma_msg_init()) {
    ALOGE("libaroma_start cannot start message queue...");
    return 0;
  }else{
	  ALOGD("libaroma_start start message queue end ...");
  }
  
  if (!libaroma_lang_init()) {
    ALOGE("libaroma_start cannot start language engine...");
    return 0;
  }
  
  if (!libaroma_timer_init()) {
    ALOGE("libaroma_start cannot start timer engine...");
    return 0;
  }
  
  if (!libaroma_wm_init()){
    ALOGE("libaroma_start cannot start window manager...");
    return 0;
  }
  
}


```


### 2.1 fb初始化过程
fb初始化过程，这里只简单分析下Linux平台下的流程，其实主要确定初始化一个全局结构体。结构体如下

``` java
struct _LIBAROMA_FB{
  /* main info */
  int w;              /* width */
  int h;              /* height */
  int sz;             /* width x height */
  byte double_buffer; /* is double buffer driver */
  voidp internal;     /* driver internal data */
  
  /* callbacks */
  void (*release)(LIBAROMA_FBP);
  byte (*snapshoot)(LIBAROMA_FBP, wordp);
  byte (*config)(LIBAROMA_FBP, const char *, const char *, dword);
  
  /* post callbacks */
  byte (*start_post)(LIBAROMA_FBP);
  byte (*post)(LIBAROMA_FBP, wordp __restrict,
    int, int, int, int, int, int, int, int);
  byte (*end_post)(LIBAROMA_FBP);
  
  /* Optional - DPI */
  int dpi;
  byte bigscreen;
  
  /* post flag */
  byte onpost;
  
  /* AROMA CORE Runtime Data */
  LIBAROMA_CANVASP canvas;
  byte internal_canvas;
};

```

这里主要确定显示区域的宽度高度，显示区域的大小，以及确定处理显存的各种回调接口


### 2.2 hid初始化过程
hid初始化过程其实也是就是初始化一个全局的结构体

``` java
struct _LIBAROMA_HID {
  voidp internal;
  
  /* driver callbacks */
  void (*release)(LIBAROMA_HIDP);
  byte (*getinput)(LIBAROMA_HIDP, LIBAROMA_HID_EVENTP);
  byte (*config)(LIBAROMA_HIDP, const char *, const char *, dword);
  
  /* libaroma usable data */
  int   screen_width;
  int   screen_height;
  byte  key_pressed[LIBAROMA_HID_KEYCODE_MAX / 8];
  
  /* touch move informations */
  int   touch_last_x;
  int   touch_last_y;
};

```

其中getinput 回调返回触摸屏触摸位置以及触摸事件的类型【down move up】
screen_width screen_height 是根据fb初始化过程赋值的

``` java
	/* set screen information */
	_libaroma_hid->screen_width = libaroma_fb()->w;
	_libaroma_hid->screen_height = libaroma_fb()->h;
	_libaroma_hid->touch_last_x = 0;
	_libaroma_hid->touch_last_y = 0;
```


### 2.3 msg初始化过程
msg初始化过程是初始化两个队列和一个线程
``` java
struct _LIBAROMA_MSGQUEUE {
	LIBAROMA_STACKP input; /* input queue data */
	LIBAROMA_STACKP queue; /* queue data */
	LIBAROMA_THREAD input_thread; /* input waiter thread */
};
```
这里关注一个Thread,这个和Android EventHub设计思想基本一致，一个Thread不对读取底层上报事件，读取到事件后抛到一个队列中。这里特别关注一个方法 **libaroma_msg**
```  java
byte libaroma_msg(LIBAROMA_MSGP msg) {
	/* mutex lock */
	libaroma_cond_lock(&_libaroma_msgqueue_mutex);
	/* wait until data available in queue */
	while ((_libaroma_msgqueue->queue->n + _libaroma_msgqueue->input->n) < 1) {
		/* wait for signal */
        libaroma_cond_wait(&_libaroma_msgqueue_cond, &_libaroma_msgqueue_mutex);
		/* if not valid, return it */
		if (!_libaroma_msgqueue_isrun) {
			libaroma_cond_unlock(&_libaroma_msgqueue_mutex);
			return LIBAROMA_MSG_NONE;
		}
	}
	LIBAROMA_STACKP get_stack = _libaroma_msgqueue->input;
	if (_libaroma_msgqueue->queue->n > 0) {
		get_stack = _libaroma_msgqueue->queue;
	}
	byte ret = LIBAROMA_MSG_NONE;
	/* shift stack */
	LIBAROMA_MSGP shift_event = (LIBAROMA_MSGP) libaroma_stack_shift(get_stack);
	if (shift_event != NULL) {
		/* copy into destination */
		memcpy(msg, shift_event, sizeof(LIBAROMA_MSG));
		/* set return value as msg item */
		ret = shift_event->msg;
		/* free shifted data */
		free(shift_event);
	}
	/* unlock mutex */
	libaroma_cond_unlock(&_libaroma_msgqueue_mutex);
	return ret;
} /* End of libaroma_msg */

```
这个方法其实目中的很单纯，就是从这两个队列中获取一个上报的消息，这里注意到一个**libaroma_cond_wait(&_libaroma_msgqueue_cond, &_libaroma_msgqueue_mutex);**这里等待一个条件满足，如果不满足则休眠，目测libaroma_msg方法是在一个线程中循环调用的。这里先不解释等分析到wm时候具体解释。其实这个线程就像EventHub的分发线程类似，当有触屏事件或者按键事件上报的时候，抛到队列中，然后唤醒分发线程，分发事件，分发完毕后继续等待事件上报

### 2.4 wm初始化过程
windowmanger 这个是重头戏，这个涉及到UI的刷新，和事件的上报。同时也涉及window的专场动画，这里只是简单的分析下windowmanger的初始化过程，具体的细节在windowmanger章节具体分析。只是简单看看初始过程
``` java
byte libaroma_wm_init() {
	if (_libaroma_wm) {
		ALOGW("libaroma_wm_init window manager already initialized");
		return 0;
	}ALOGV("libaroma_wm_init init window manager");
	_libaroma_wm = (LIBAROMA_WMP) calloc(sizeof(LIBAROMA_WM), 1);
	if (!_libaroma_wm) {
		ALOGW("libaroma_wm_init alloc window manager memory failed");
		return 0;
	}
	libaroma_cond_init(&_libaroma_wm_cond, &_libaroma_wm_mutex);
	libaroma_mutex_init(_libaroma_wm_ui_mutex);
	libaroma_mutex_init(_libaroma_wm_sync_mutex);
	_libaroma_wm_onprocessing = 0;
	_libaroma_wm->theme = libaroma_sarray(_libaroma_wm_theme_release);
	_libaroma_wm->color = libaroma_sarray(NULL);
	_libaroma_wm->w = libaroma_fb()->w;
	_libaroma_wm->h = libaroma_fb()->h;
	_libaroma_wm_default_set(
	LIBAROMA_WM_FLAG_RESET_COLOR | LIBAROMA_WM_FLAG_RESET_THEME);
	_libaroma_wm_workspace_canvas();
	return 1;
} /* End of libaroma_wm_init */
```
乍一看好像没做啥，只是确定wm的高宽 初始化几个互斥锁，和一个互斥变量，其实怎能说，看似没什么，其实有很多东西可以猜测，申请有三个互斥锁，和一个条件变量可知道这里涉及多线程，和线程同步。至少有两个线程在协同工作，这里不做分析，在windowmanger章节具体分析，下面先简单看看windowmanger初始化的一个全局结构体。
```  java
struct _LIBAROMA_WM{
  LIBAROMA_COLORSET colorset; /* default colorset */
  
  LIBAROMA_SARRAYP theme; /* themeset */
  LIBAROMA_SARRAYP color; /* colorset */
  
  /* hooker callbacks */
  LIBAROMA_WMCB_MESSAGE_HANDLER message_handler;
  LIBAROMA_WMCB_RESET_HANDLER reset_handler;
  LIBAROMA_WMCB_UI_THREAD ui_thread;

  /* canvas location */
  int x;
  int y;
  int w;
  int h;
  
  /* window sync */
  byte sync_all;
  int sync_x;
  int sync_y;
  int sync_w;
  int sync_h;
  
  /* message state */
  LIBAROMA_STACKP queue;
  
  byte client_started;
  LIBAROMA_WINDOWP active_window;
  LIBAROMA_CANVASP workspace_bg;
};
```

初始化只初始化结构体几个简单的成员变量，可见重头戏不在这里。具体分析在写一个app章节具体分析，这里注意结构体的成员变量**active_window**改成员就是windowmanger维护的一个激活的window，这个激活的window上面有一系列控件。这里先不卖关子，其实在写app的时候都会设置一个激活的window到windowmanger，在这个设置的过程，完成两个线程的启动。
``` java
byte libaroma_wm_client_start() {
	if (_libaroma_wm == NULL) {
		ALOGW("window manager uninitialized");
		return 0;
	}
	if (!_libaroma_wm->client_started) {
		/* start message queue */
		libaroma_msg_start();

		/* start message thread */
		_libaroma_wm->client_started = 1;
		_libaroma_wm->queue = libaroma_stack(NULL);

		libaroma_wm_resetsync();

		/* start message thread */
		libaroma_thread_create(&_libaroma_wm_message_thread_var,
				_libaroma_wm_message_thread, NULL);

		/* start ui thread */
		libaroma_thread_create(&_libaroma_wm_ui_thread_var,
				_libaroma_wm_ui_thread, NULL);

		/* high priority thread */
		libaroma_thread_set_hiprio(_libaroma_wm_ui_thread_var);
		return 1;
	}
    
```
这里主要干了四件事：
- msg线程真正开始工作，之前虽然工作但是，触屏事件被过滤掉，并不上报。
- 初始化一个队列
- 创建两个线程
	- _libaroma_wm_message_thread
	- _libaroma_wm_ui_thread
- 设置ui线程的优先级更高一点。

这里先简单猜测下 **_libaroma_wm_ui_thread**只是为了不断的刷新界面，具体是否重绘上屏，window决定，进一步由window上的控件决定。
**_libaroma_wm_message_thread**这个简单推测，其实应该就是向_libaroma_wm->queue的队列中抛消息。
这里简单看下**_libaroma_wm_ui_thread**
``` java
static void * _libaroma_wm_ui_thread(void * cookie) {
	ALOGV("starting wm ui thread");
	byte need_sync = 0;
	while (_libaroma_wm->client_started) {
		need_sync = 0;

		/* run child thread process */
		if (_libaroma_wm->client_started) {
			libaroma_mutex_lock(_libaroma_wm_ui_mutex);
			if (!_libaroma_wm_onprocessing) {
				if (_libaroma_wm->active_window != NULL) {
					if (_libaroma_wm->active_window->ui_thread != NULL) {
						if (_libaroma_wm->active_window->ui_thread(
								_libaroma_wm->active_window)) {
							need_sync = 1;
						}
					}
				}
				if (_libaroma_wm->ui_thread != NULL) {
					if (_libaroma_wm->ui_thread()) {
						need_sync = 1;
					}
				}
			}
			libaroma_mutex_unlock(_libaroma_wm_ui_mutex);
			if (need_sync) {
				libaroma_wm_syncarea();
				need_sync = 0;
				continue;
			}
		}
		libaroma_sleep(16);
	} ALOGV("wm ui thread ended");
	return NULL;
} 
```
这里做了如下几件事
- 调用windowmanager的active_window的ui_thread
- 调用windowmanager的ui_thread的方法
- 根据前两步骤决定need_sync 变量，根据nee_sync决定是否上屏
- 最后libaroma_sleep(16)  **其实这个和安卓中vsync机制有点像，只不过这里用的是软件模拟的方法实现的**

总的来说其实这里就是每间隔16ms决定是否上屏，这个上屏过程由两方面决定，最主要的是window，的ui_thread回调。写到这里才进入这里才介绍到**window**在接下章节介绍，接下来关注下**_libaroma_wm_message_thread**.
``` java
static void * _libaroma_wm_message_thread(void * cookie) {
	ALOGV("starting wm messaging");
	while (_libaroma_wm->client_started) {
		LIBAROMA_MSG msg;
		byte ret = libaroma_msg(&msg);
		if (ret) {
			if (_libaroma_wm->message_handler != NULL) {
				ret = _libaroma_wm->message_handler(_libaroma_wm, &msg);
				if (ret == LIBAROMA_WM_MSG_HANDLED) {
					continue;
				} else if (ret == LIBAROMA_WM_MSG_EXIT) {
					break;
				}
			}
			/* send to window */
			if (_libaroma_wm->active_window != NULL) {
				libaroma_cond_lock(&_libaroma_wm_mutex);
				if (msg.msg == LIBAROMA_MSG_TOUCH) {
					/* update touch coordinate */
					// 如果wm的初始位置不为 00  就要修正坐标
					msg.x -= libaroma_wm()->x;
					msg.y -= libaroma_wm()->y;
				}
				if (libaroma_stack_push(_libaroma_wm->queue, &msg,
						sizeof(LIBAROMA_MSG))) {
					libaroma_cond_signal(&_libaroma_wm_cond);
				}
				libaroma_cond_unlock(&_libaroma_wm_mutex);
			}
		}
	}ALOGV("wm messaging ended");
	return NULL;
}
```
在这个thread的里面其实也没做多少事情。
- 从消息队列获取一个消息。
- 如果是触屏消息则修正点击坐标
- 将消息放到active_window对应的消息队列中

## 2.5 初始化总过程回顾
其实怎么说这个初始化过程是不能颠倒顺序，后边初始化依赖与前面的初始化过程。总的来说初始过程就是为了初始化几个全局变量。
- 先初始化FB最主要的是为了确定屏幕的宽高。
- 之后hid初始化依赖与fb的宽高。这里为主要是为了校准触屏事件注册获取上报事件的回调函数。
- msg的初始化，借助hid初始化注册的回调函数获取上报事件，不断轮巡，直到msg_start 才放到事件队列。
- wm初始化主要依赖fb初始化过程的宽高，确定windowmanager对应Canvas的宽高，并分配内存。

总的初始化只是为以后布局做好提前准备，其实框架并没有真正的开始工作。开始工作在实现一个小应用说起。


## 3 window理解
其实之前对android的window其实并不太清楚，研究过Libaroma的window之后才有所了解，首先window对应一片线性内存，这片内存中的数据决定追终屏幕上显示的内容，其实window作为控件的载体，也对应一个输入事件源，当由事件带来时候，分发给对应的控件。这里笼统的说其实事件无非是刷新事件源，和触屏事件源。
### 3.1 window对应的数据结构。
首先先简单了解小一个window对应的数据结构。

``` java
struct _LIBAROMA_WINDOW{
  LIBAROMA_COLORSETP colorset;
  
  /* px measured */
  int ax;
  int ay;
  int x;
  int y;
  int w;
  int h;
  /* requested size */
  int rx;
  int ry;
  int rw;
  int rh;
  /* measured size */
  int left;
  int top;
  int width;
  int height;
  
  /* states */
  byte active;
  byte lock_sync;
  
  /* graphs */
  char theme_bg[256];
  LIBAROMA_CANVASP dc;
  LIBAROMA_CANVASP bg;
  
  /* childs */
  int childn;
  LIBAROMA_CONTROLP * childs;
  LIBAROMA_CONTROLP focused;
  LIBAROMA_CONTROLP touched;
  
  /* thread manager */
  byte (*ui_thread)(LIBAROMA_WINDOWP);
  
  /* update background */
  void (*onupdatebg)(LIBAROMA_WINDOWP,LIBAROMA_CANVASP);
  
  /* client window */
  LIBAROMA_WINDOWP parent;
  LIBAROMA_WINDOW_HANDLERP handler;
  voidp client_data;
};
```
这个简单介绍下，几个成员
- LIBAROMA_CONTROLP * childs  一个window上附属的控件头指针
- byte active;  window的状态，激活非激活。
-  byte (*ui_thread)(LIBAROMA_WINDOWP); 在windowmanger的ui线程中16ms一回调的接口
-  LIBAROMA_CONTROLP focused; 当前window上聚焦的控件
-  parent window的父window，也就意味着一个window上面可以附属很多子window
-  handler 这个是window对应的回调方法结构体，不同时刻有不同回调，这里只是先简单贴出来，之后详细理一理

``` java
typedef struct _LIBAROMA_WINDOW_HANDLER{
  void (*prefree)(LIBAROMA_WINDOWP);  //释放window之前调用
  void (*postfree)(LIBAROMA_WINDOWP);//释放window之后调用
  byte (*updatebg)(LIBAROMA_WINDOWP); //背景更新时候回调
  byte (*invalidate)(LIBAROMA_WINDOWP,byte);
  byte (*sync)(LIBAROMA_WINDOWP,int,int,int,int); //window同步上屏时候回调
  byte (*message_hooker)(
    LIBAROMA_WINDOWP, LIBAROMA_MSGP, dwordp
  ); //window自己的事件处理函数，自己可根据需要实现
  
  /* controls */
  byte (*control_draw_flush)(
    LIBAROMA_WINDOWP,LIBAROMA_CONTROLP,LIBAROMA_CANVASP,byte
  ); //绘制完毕control之后调用
  byte (*control_erasebg)(
    LIBAROMA_WINDOWP,LIBAROMA_CONTROLP,LIBAROMA_CANVASP
  );
  byte (*control_isvisible)(
    LIBAROMA_WINDOWP,LIBAROMA_CONTROLP
  );//绘制control之前判断control是否可见
  LIBAROMA_CANVASP (*control_draw_begin)(
    LIBAROMA_WINDOWP,LIBAROMA_CONTROLP
  );//可见之后分配canvas回调
} LIBAROMA_WINDOW_HANDLER, * LIBAROMA_WINDOW_HANDLERP;

```

### 3.2 初始化一个window
初始化一个window的过程无非就是初始化window对应结构体的成员。这里先看下初始化window对应的接口。

``` java
LIBAROMA_WINDOWP libaroma_window(char * bg_theme_name, int x, int y, int w,
		int h) {
	__CHECK_WM(NULL);
	LIBAROMA_WINDOWP win =
			(LIBAROMA_WINDOWP) calloc(sizeof(LIBAROMA_WINDOW), 1);
	if (!win) {
		ALOGW("libaroma_window alloc window data failed");
		return NULL;
	}

	if (bg_theme_name) {
		snprintf(win->theme_bg, 256, "%s", bg_theme_name);
	} else {
		win->theme_bg[0] = 0;
	}
	win->rx = x;
	win->ry = y;
	win->rw = w;
	win->rh = h;
	win->ui_thread = _libaroma_window_ui_thread;
	libaroma_window_measure_size(win);
	return win;
} 
```

这里主要关注两方面，

- window的相对尺寸赋值。
        win->rx = x;
        win->ry = y;
        win->rw = w;
        win->rh = h;
- ui_thread回调函数的注册
- libaroma_window_measure_size(win)， window的measure过程。

#### 3.2.1 window的measure过程。
细心观察，在初始化一个window的时候 是赋值给win的 rx、ry、rw、rh。我这里推断下**r**开头有相对的意思，这里的相对肯定是相对与屏幕的像素宽高，在fb_init阶段已经确定。具体先粗略看下函数实现。

``` java
byte libaroma_window_measure_size(LIBAROMA_WINDOWP win) {
	if (win) {
		if (win->parent != NULL) {
			ALOGW("window_resize cannot be used for child window");
			return 0;
		}
		if (_libaroma_window_measurement_dp) {
			win->x = libaroma_dp(win->rx);
			win->y = libaroma_dp(win->ry);
			win->w = libaroma_dp(win->rw);
			win->h = libaroma_dp(win->rh);
		} else {
			win->x = win->rx;
			win->y = win->ry;
			win->w = win->rw;
			win->h = win->rh;
		}
		win->ax = win->x;
		win->ay = win->y;

		win->x = libaroma_window_measure_calculate(win->x, win->rx,
				libaroma_wm()->w, 0, 0);
		win->y = libaroma_window_measure_calculate(win->y, win->ry,
				libaroma_wm()->h, 0, 0);
		win->w = libaroma_window_measure_calculate(win->w, win->rw,
				libaroma_wm()->w, 1, win->x);
		win->h = libaroma_window_measure_calculate(win->h, win->rh,
				libaroma_wm()->h, 1, win->y);

		if (win->w + win->x > libaroma_wm()->w) {
			win->w = libaroma_wm()->w - win->x;
		}
		if (win->h + win->y > libaroma_wm()->h) {
			win->h = libaroma_wm()->h - win->y;
		}
		_libaroma_window_measure_save(win, NULL);
		LIBAROMA_MSG _msg;
		libaroma_window_process_event(
				win,
				libaroma_wm_compose(&_msg, LIBAROMA_MSG_WIN_MEASURED, NULL, 0,
						0));
		return 1;
	}
	return 0;
```
这里简单做了如下几件事

- 确定window的右上角坐标x、y和window的宽高
- 修正window的宽高，不能超过wm的宽高。
- 保存修正的宽高到window对应的结构体中的left、top、width、height中
- 主动调用libaroma_window_process_event 触发一次LIBAROMA_MSG_WIN_MEASURED 过程(其实首次构造window时候 window中的控件个数为零，个人觉得没必要主动measure下子控件)

其实在构造window时候是支持相对尺寸，**这些相对尺寸如果是window，则是相对对于windowmanger的宽高，如果是控件，则是相对于window的宽高**。
    #define LIBAROMA_SIZE_FULL     0 
    #define LIBAROMA_SIZE_HALF    -1
    #define LIBAROMA_SIZE_THIRD   -2
    #define LIBAROMA_SIZE_QUARTER -3
    #define LIBAROMA_POS_HALF     -4
    #define LIBAROMA_POS_1P3      -5
    #define LIBAROMA_POS_2P3      -6
    #define LIBAROMA_POS_1P4      -7
    #define LIBAROMA_POS_3P4      -8
    
    
#### 3.2.2 window 默认ui_thread做了些什么
初步猜想是ui_thread在wm16ms一调用，这里应该是刷新window上控件对应区域的内容，详情具体看如下代码
``` java
byte _libaroma_window_ui_thread(LIBAROMA_WINDOWP win) {
	int i;
	byte need_sync = 0;
	if (win->active == 1) {
#ifdef LIBAROMA_CONFIG_OPENMP
#pragma omp parallel for
#endif
		for (i = 0; i < win->childn; i++) {
			LIBAROMA_CONTROLP c = win->childs[i];
			if (c->handler->thread != NULL) {
				if (c->handler->thread(c)) {
					if (libaroma_control_draw(c, 0)) {
						libaroma_wm_updatesync(c->x + win->x, c->y + win->y,
								c->w, c->h, 0);
						need_sync = 1;
					}
				}
			}
		}
	}
	return need_sync;
    
```
**其实简单的来说遍历window里面的所有子控件，然后调用控件的thread回调方法，根据返回值决定是否重绘控件，根据重绘结果确定是否重绘wm中对应区域的显存。** 
其实这里就简单的体现了一个控件应该实现的回调方法。

#### 3.2.3 向window添加一个控件
添加一个控件到window过程其实也是确定控件大小和位置的过程。所有的控件添加到window上最终都会调用下面的函数

``` java
byte libaroma_window_add(LIBAROMA_WINDOWP win, LIBAROMA_CONTROLP ctl) {
	__CHECK_WM(0);
	if (win == NULL) {
		ALOGW("window_add win is NULL");
		return 0;
	}
	if (ctl == NULL) {
		ALOGW("window_add ctl is NULL");
		return 0;
	}
	if (ctl->window != NULL) {
		ALOGW("window_add ctl already have window");
		return 0;
	}
	libaroma_window_measure(win, ctl);
	if (win->childn == 0) {
		win->childs = (LIBAROMA_CONTROLP *) malloc(sizeof(LIBAROMA_CONTROLP));
		if (!win->childs) {
			ALOGW("window_add malloc failed");
			win->childs = NULL;
			return 0;
		}
		win->childs[0] = ctl;
	} else {
		LIBAROMA_CONTROLP * newchilds = (LIBAROMA_CONTROLP *) realloc(
				win->childs, sizeof(LIBAROMA_CONTROLP) * (win->childn + 1));
		if (!newchilds) {
			ALOGW("window_add realloc failed");
			return 0;
		}
		win->childs = newchilds;
		win->childs[win->childn] = ctl;
	}
	ctl->window = win;
	win->childn++;
	_libaroma_window_recalculate(win);
	return 1;
}
```
无非是做了以下几件事

- 将控件指针添加到window的childs数组中，并增加window的childn变量
- 确定控件相对与window的真实大小和位置。



## 4 windowmanger理解
windowmanger 这是其实维护一个激活状态的window，每隔16ms主动回调激活window的ui_thread回调方法，至于自己想做什么可以自己实现，也可以用默认的构造window时候的函数，默认的之上就贴出了，其实之上讲解window时候疏忽了window结构体的**LIBAROMA_WINDOW_HANDLERP handler**,该成员决定了window上控件在绘制的时候的一些特殊处理，在讲解自定义控件章节解释。windowmanger还有一个关键的使命**分发事件给激活的window**，所以windowmanger起来两个线程做事。

- UI线程刷新window
- 事件线程分发事件给激活的window






## 5 control 理解

control就是相当与安卓中的View，对android的自定义控件的有了解都知道，自定义view最主要是重写onDraw、onMeasure、onLayout方法，其实还有一个onTouch方法。这里先简单看下一个control的结构体表示。
### 5.1 control结构体
``` java

struct _LIBAROMA_CONTROL{
  word id;
  voidp internal;
  LIBAROMA_WINDOWP window;
  LIBAROMA_COLORSETP colorset;
  
  /* px measured */
  int x;
  int y;
  int w;
  int h;
  
  /* requested */
  int rx;
  int ry;
  int rw;
  int rh;
  
  /* measured size */
  int left;
  int top;
  int width;
  int height;
  
  /* minimum control size */
  int minw;
  int minh;
  
  /* callbacks */
  LIBAROMA_CONTROL_HANDLERP handler;
};


```
这里可以看出control和window的数据结构比较类似多了一个最小的宽高。还有一个回调函数结构体**LIBAROMA_CONTROL_HANDLERP handler**


``` java
typedef struct {
  dword (*message)(LIBAROMA_CONTROLP, LIBAROMA_MSGP);
  void (*draw)(LIBAROMA_CONTROLP, LIBAROMA_CANVASP);
  byte (*focus)(LIBAROMA_CONTROLP, byte);
  void (*destroy)(LIBAROMA_CONTROLP);
  byte (*thread)(LIBAROMA_CONTROLP);
} LIBAROMA_CONTROL_HANDLER, * LIBAROMA_CONTROL_HANDLERP;
```
 - draw回调和android的onDraw回调类似，这里有两个参数（LIBAROMA_CONTROLP, LIBAROMA_CANVASP第一个是控件本身，第二个是一张画布。
 - thread回调相当于android view里面的draw方法，只不过这里如果是激活window，每16ms会调用一次，其作用是更新控件的一些状进状态，并进一步决定是否重绘control
 - message 这个回调有点类似android view中的onTouch方法，是在一个事件处理线程中被调用，改函数的参数LIBAROMA_MSGP,是读取来自改control依附的window的事件队列。
 - focus 这个回调暂时没发现用的场景，暂不分析。
 - destroy control附属于的window销毁的时候调用，用来释放control对应的内存，和android的onDetachFromWindow有点类似


这里需要特别关注几个成员**word id、 voidp internal、 LIBAROMA_WINDOWP window**

- id和android 中view对应一个id类似。通过id可以遍历window中的control 树找到对应的控件。
- internal 改成员是voidp，用的时候一般强转成对应的结构体，该成员一般表示控件的属性和状态等（颜色，文字、样式等）。
- window control依赖的window一般在构造一个control的时候会传进一个window。

### 5.2 control 绘制流程
在windowmanager分析章节中，直到windowmanger有个线程，相当于渲染线程，改线程每16ms调用window的ui_thread回调和，wm的ui_thread回调，其中在window的ui_thread会遍历window里面的control，首先回调control的thread方法，根据thread回调决是否调用**libaroma_control_draw**，也就是在这个方法中绘制control，根据libaroma_control_draw，返回值，进一步决定是否要更新wm里面更新区域（现阶段只是更新windowmanger里面的几个变量）所有control中如果没有一个要更新界面的，windowmanger就不需要从新上屏，只要有一个需要更新，就需要更新windowmanger中几个变量决定更新区域，最后就是刷新上屏，上屏区域，就是之上几个变量决定的区域。

说这么多其实我们最关心control是怎么绘制出来的。就要详细分析下**libaroma_control_draw**方法。改方法有两个参数，第一个参数当前control，第二个参数决定是否立即上屏。
``` java
byte libaroma_control_draw(LIBAROMA_CONTROLP ctl, byte sync) {
	LIBAROMA_CANVASP c = libaroma_control_draw_begin(ctl);
	if (c != NULL) {
		if (ctl->handler->draw != NULL) {
			ctl->handler->draw(ctl, c);
		}
		libaroma_control_draw_end(ctl, c, sync);
		return 1;
	}
	return 0;
}
```
其实看起来是很简介，其实里面暗含好多玄机，大致看做了以下几件事

- 拿到一个Canvas（没错 Canvas，这里和android很类似）
- 回调control的回调draw方法，把刚才的Canvas传进去
- 最后调用**libaroma_control_draw_end**

写自己的控件一般是实现自己的draw方法，在画布上作图。画布其实就是控件依赖的window对应区域的存储空间。所以这里要先分析下画布拿到的过程。

#### 5.2.1 Canvas获取的过程
安卓有Canvas，当时对这个Canvas其实不太理解，现在研究Libaroma之后，其实Canvas对应一片内存，先简单看下Canvas对应的数据结构。

``` java
struct _LIBAROMA_CANVAS{
  int      w;       /* width */
  int      h;       /* height */
  int      l;       /* line size */
  int      s;       /* width x height */
  wordp    data;    /* color data */
  bytep    alpha;   /* alpha data */
  bytep    hicolor; /* hicolor data */
  byte     flags;   /* flags */
};
```
这里简单解释下 w、h就是canvas的宽高 l现在暂时返现是window的line size ，s=w*h,这里有几个指针这里才是重点，这几个指针决定了绘制区域对应的存储空间。
``` java
LIBAROMA_CANVASP libaroma_control_draw_begin(LIBAROMA_CONTROLP ctl) {
	if (!libaroma_window_isactive(ctl->window) || !libaroma_control_isvisible(
			ctl)) {
		return NULL;
	}
	LIBAROMA_WINDOWP win = ctl->window;
	if (win->handler != NULL) {
		if (win->handler->control_draw_begin != NULL) {
			return win->handler->control_draw_begin(win, ctl);
		}
	}
	if (win->dc == NULL) {
		return NULL;
	}
	LIBAROMA_CANVASP c = libaroma_canvas_area(win->dc, ctl->x, ctl->y, ctl->w,
			ctl->h);
	return c;
} 
```

看看这个libaroma_control_draw_begin方法其实做的事情真的不少，大致分为以下几方面

- 看当前control在所依赖的window是否激活，或者是或否可见，不可见直接返回NULL，control就不绘制了，这里其实就是过滤下，因为渲染线程是遍历window上的所有control，这里就是对于不激活不可见的window进行过滤，减轻渲染线程的负担。
- 这里又有一个比较好玩的hook，如果window设置了control_draw_begin 回调就有window的回调分配对应的canvas，这里相当交给开发者自己实现自己的window，加大的可定制性。
- 这里看下默认获取Canvas指针的过程**libaroma_canvas_area**

``` java
LIBAROMA_CANVASP libaroma_canvas_area(
    LIBAROMA_CANVASP parent,
    int x,
    int y,
    int w,
    int h) {
  if (!parent) {
    ALOGW("canvas_area parent is null");
    return NULL;
  }
  /* initializing canvas memory */
  LIBAROMA_CANVASP c = (LIBAROMA_CANVASP) calloc(sizeof(LIBAROMA_CANVAS),1);
  if (!c) {
    ALOGW("canvas_area malloc(LIBAROMA_CANVASP) Error");
    return NULL;
  }
  if (!libaroma_canvas_area_update(c,parent,x,y,w,h)){
    free(c);
    return NULL;
  }
  return c;
}
```

做了以下几件事

- 分配canvas结构体存储空间
- 更新canvas树结构体的成员变量

其实最重要的就是**libaroma_canvas_area_update** 函数
``` java
byte libaroma_canvas_area_update(
    LIBAROMA_CANVASP c,
    LIBAROMA_CANVASP parent,
    int x,
    int y,
    int w,
    int h) {
  if (!parent) {
    ALOGW("canvas_area_update parent is null");
    return 0;
  }
  if (!c) {
    ALOGW("canvas_area_update canvas is null");
    return 0;
  }
  
  /* Set Target Positions */
  int x2 = x + w;
  int y2 = y + h;
  
  /* Fix Positions */
  if (x2 > parent->w) {
    x2 = parent->w;
  }
  
  if (y2 > parent->h) {
    y2 = parent->h;
  }
  
  if (x < 0) {
    x = 0;
  }
  
  if (y < 0) {
    y = 0;
  }
  
  /* Set Fixed Size */
  w = x2 - x;
  h = y2 - y;
  
  if ((w < 1) || (h < 1)) {
    ALOGW("canvas_area_update calculated width or height < 1");
    return 0;
  }
  
  c->w      = w;
  c->h      = h;
  c->s      = w * h;
  c->flags  = LIBAROMA_CANVAS_CHILD;
  c->l      = parent->l;
  c->data   = parent->data + (y * parent->l) + x;
  
  if (parent->alpha != NULL) {
    c->alpha = parent->alpha + (y * parent->l) + x;
  }
  else {
    c->alpha = NULL;
  }
  
  if (parent->hicolor != NULL) {
    c->hicolor = parent->hicolor + (y * parent->l) + x;
  }
  else {
    c->hicolor = NULL;
  }
  
  return 1;
}
```
其实就是无非就是做几件事情。
- 修正canvas 宽高，因为canvas对应的区域有可能超过window的区域。
- 把control在window对应位置的指针付给canvas成员，其实核心就在这里（找到对应的存储空间）
- 这里canvas有个flags比较有趣（这里应该是canvas结构体在释放的时候有用）

这里我才搞懂为啥canvas的l 为parent的l，这个可能在绘制的时候找到，对应的存储空间，因为这里我们得到的只是头指针，其实一个方形区域对应的window里面的内存是不连续的。window分配的空间应该是连续的。

#### 5.2.2 绘制完毕控件之后
绘制控件完毕，无非就是是否直接上屏。这里就贴出下代码。
``` java
void libaroma_control_draw_end(LIBAROMA_CONTROLP ctl, LIBAROMA_CANVASP c,
		byte sync) {
	if (sync) {
		libaroma_control_draw_flush(ctl, c, sync);
	}
```
其实就是根据sync决定是否上屏，继续跟进去看。
``` java
byte libaroma_control_draw_flush(LIBAROMA_CONTROLP ctl,
		LIBAROMA_CANVASP canvas, byte sync) {
	if (ctl == NULL) {
		ALOGW("window_control_draw ctl is null");
		return 0;
	}
	if (ctl->window == NULL) {
		ALOGW("window_control_draw ctl doesn't have window");
		return 0;
	}
	LIBAROMA_WINDOWP win = ctl->window;
	if (win->handler != NULL) {
		if (win->handler->control_draw_flush != NULL) {
			return win->handler->control_draw_flush(win, ctl, canvas, sync);
		}
	}
	if (win->dc == NULL) {
		ALOGW("window_control_draw window dc uninitialized");
		return 0;
	}
	if (sync) {
		int sx = ctl->x;
		int sy = ctl->y;
		libaroma_window_sync(win, sx, sy, ctl->w, ctl->h);
	}
	return 1;
}

```
这里面也有hook-**control_draw_flush** 这里也加大了自定义程度，一般在写复杂控件的时候用，这里看下默认的流程，其中核心就在libaroma_window_sync.这里先不深入分析，里面也有一个hook（sync）之后分析从头理一般window 对应的Canvas的存储空间的分配，获取子Canvas，上屏的流程。
其实这里上屏过程会从先从window到windowmanger，再到fb回调过程。，这里先埋个引子（window对应的dc 是在哪里获取的？）

### 5.3 control绘制流程回顾
control绘制的流程其实是特定的主要入后就是在ui渲染线程

- windowmamger的ui渲染线程遍历window里面的控件，调用控件的thread回调
- 根据thread回调决定是否绘制控件（thread回调一定程度上减轻了渲染线程的负担）
- 获取控件对应的canvas
	- 控件是否激活
	- 控件在所依附window是否可见其中激活可以个自定义hook回调**control_isvisible**
	- 不激活，不可见则不绘制控件，这里一定程度也减轻了渲染线程的负担
- 调用控件的draw方法 绘制控件
- 决定是否同步上屏，这里面会从window 到 windowmanger 到fb 一层层的call下去


再总结下window对应回调调用的时机,从上到依次回调
- control_isvisible  hook判断control是否可见，可见之后才找对应的canvas。
- control_draw_begin hook分配control依赖window对应位置的canvas。
- control_draw_flush hook上屏过程，如果没有hook就是默认上屏过程
- sync 默认上屏过程hook。

## 6 事件
在上面简单分析windowmanger时候，主要到windowmanger起来两个线程，一个ui_thread用于ui渲染，一个msg thread，不断从msgqueue里面，读取上报事件，并把事件抛到windowmanger的queue中。其实现在还需要一个线程来读取windowmanager的quque里面的事件并处理。所以这次分析的主角要登场了，先看方法实现。

``` java
dword libaroma_window_pool(LIBAROMA_WINDOWP win, LIBAROMA_MSGP msg) {
	if (!win) {
		return 0;
	}
	if (win->parent != NULL) {
		ALOGW("cannot pool child window...");
		return 0;
	}
	LIBAROMA_MSG _msg;
	LIBAROMA_MSGP cmsg = (msg != NULL) ? msg : &_msg;
	byte ret = libaroma_wm_getmessage(cmsg);
	if (ret) {
		return libaroma_window_process_event(win, cmsg);
	}
	return 0;
}
```
可以主要到这个方法是从windowmanger对应激活window获取一个事件，如果有返回值，则调用libaroma_window_process_event,注意到只是取到一个消息，所以这个方法一般是循环调用的，一般是在**主线程**调用，当所有控件都准备就绪之后就就一个无穷循环调用。先看看
```java
byte libaroma_wm_getmessage(LIBAROMA_MSGP msg) {
	if (!_libaroma_wm->client_started) {
		return LIBAROMA_MSG_NONE;
	}
	/* mutex lock */libaroma_cond_lock(&_libaroma_wm_mutex);
	while (_libaroma_wm->queue->n < 1) {
		libaroma_cond_wait(&_libaroma_wm_cond, &_libaroma_wm_mutex);
		if (!_libaroma_wm->client_started) {
			libaroma_cond_unlock(&_libaroma_wm_mutex);
			return LIBAROMA_MSG_NONE;
		}
	}
	byte ret = LIBAROMA_MSG_NONE;
	/* shift stack */
	LIBAROMA_MSGP shift_event = (LIBAROMA_MSGP) libaroma_stack_shift(
			_libaroma_wm->queue);
	if (shift_event != NULL) {
		memcpy(msg, shift_event, sizeof(LIBAROMA_MSG));
		ret = shift_event->msg;
		free(shift_event);
	}
	libaroma_cond_unlock(&_libaroma_wm_mutex);
	return ret;
} 
```
注意这里的libaroma_cond_wait，也就是说主线程有可能休眠，当windowmanger的事件线程有事件的时候，就唤醒主线程，唤醒之后就就从windowmanger queue获取一个事件，让主线程处理。其实这里应该也可以做个ANR机制，当主线程业务过多的时候抛出异常，是软件更加健壮。因为每个msg发到msg的队列的时候都有时间戳。android里面就有一个事件分发超时的ANR,这里暂时没做，后期可以做做个统计，看主线程能忍耐限度，重量级别的处理函数是libaroma_window_process_event.
``` java
dword libaroma_window_process_event(LIBAROMA_WINDOWP win, LIBAROMA_MSGP msg) {
	__CHECK_WM(0);
	if (win == NULL) {
		ALOGW("window_event win is null");
		return 0;
	}
	if (win->parent != NULL) {
		ALOGW("window_event cannot used for child window...");
		return 0;
	}
	dword ret = 0;
	if (win->handler) {
		if (win->handler->message_hooker) {
			if (win->handler->message_hooker(win, msg, &ret)) {
				return ret;
			}
		}
	}
	switch (msg->msg) {
	case LIBAROMA_MSG_WIN_ACTIVE: {
		/* set current window size */
		win->focused = NULL;
		win->touched = NULL;
		if (msg->x != 10) {
			_libaroma_window_ready(win);
		}
		if ((!win->lock_sync) || (msg->x == 10)) {
			if ((!win->active) || (msg->x == 10)) {
				int i;
				win->active = 1;
				/* signal child */
				for (i = 0; i < win->childn; i++) {
					if (win->childs[i]->handler->message) {
						win->childs[i]->handler->message(win->childs[i], msg);
					}
				}
			}
		}
	}
		break;
	case LIBAROMA_MSG_WIN_RESIZE: {
		int i;
		_libaroma_window_ready(win);
		for (i = 0; i < win->childn; i++) {
			if (win->childs[i]->handler->message) {
				win->childs[i]->handler->message(win->childs[i], msg);
			}
		}
	}
		break;
	case LIBAROMA_MSG_WIN_INACTIVE: {
		if (win->active) {
			/* stop thread manager */
			win->active = 0;

			/* send inactive message to child */
			int i;
			for (i = 0; i < win->childn; i++) {
				if (win->childs[i]->handler->message) {
					win->childs[i]->handler->message(win->childs[i], msg);
				}
			}
			win->focused = NULL;
			win->touched = NULL;
		}
	}
		break;
	case LIBAROMA_MSG_WIN_MEASURED: {
		/* remeasured all childs */
		int i;
		for (i = 0; i < win->childn; i++) {
			libaroma_window_measure(win, win->childs[i]);
		}
	}
		break;
	case LIBAROMA_MSG_WIN_DIRECTMSG: {
		return (dword) msg->x;
	}
		break;
	case LIBAROMA_MSG_WIN_INVALIDATE: {
		libaroma_window_invalidate(win, 1);
	}
		break;
	case LIBAROMA_MSG_TOUCH: {
		/* touch handler */
		if (msg->state == LIBAROMA_HID_EV_STATE_DOWN) {
			win->touched = NULL;
			int x = msg->x;
			int y = msg->y;
			libaroma_window_calculate_pos(win, NULL, &x, &y);
			int i;
			for (i = 0; i < win->childn; i++) {
				if (_libaroma_window_is_inside(win->childs[i], x, y)) {
					win->touched = win->childs[i];
					break;
				}
			}
			if (win->touched != NULL) {
				if (win->touched->handler->message) {
					ret = win->touched->handler->message(win->touched, msg);
				}
			}
		} else if (win->touched != NULL) {
			if (msg->state == LIBAROMA_HID_EV_STATE_MOVE) {
				if (win->touched->handler->message) {
					ret = win->touched->handler->message(win->touched, msg);
				}
			} else if (msg->state == LIBAROMA_HID_EV_STATE_UP) {
				if (win->touched->handler->message) {
					ret = win->touched->handler->message(win->touched, msg);
				}
				win->touched = NULL;
			}
		}
	}
		break;
	}
	return ret;
```
可以看出主线程处理的事件还是比较多的有以下几种下来分别分析。
``` java
#define LIBAROMA_MSG_WIN_ACTIVE       LIBAROMA_MSG_SYS(0x1)
#define LIBAROMA_MSG_WIN_INACTIVE     LIBAROMA_MSG_SYS(0x2)
#define LIBAROMA_MSG_WIN_INVALIDATE   LIBAROMA_MSG_SYS(0x3)
#define LIBAROMA_MSG_WIN_RESIZE       LIBAROMA_MSG_SYS(0x4)
#define LIBAROMA_MSG_WIN_MEASURED     LIBAROMA_MSG_SYS(0x5)
#define LIBAROMA_MSG_WIN_DIRECTMSG    LIBAROMA_MSG_SYS(0x6)
#define LIBAROMA_MSG_WIN_FOCUS        LIBAROMA_MSG_SYS(0x7)
#define LIBAROMA_MSG_WIN_BLUR         LIBAROMA_MSG_SYS(0x8)
#define LIBAROMA_MSG_WIN_TITLE        LIBAROMA_MSG_SYS(0x9)


/* Message Values */
#define LIBAROMA_MSG_USR(X)       (0xC0|X)  /* user messages */
#define LIBAROMA_MSG_SYS(X)       (0x80|X)  /* system/window messages */
#define LIBAROMA_MSG_KEY(X)       (0x40|X)  /* key event */
#define LIBAROMA_MSG_NONE         0x00      /* not valid message */
#define LIBAROMA_MSG_EXIT         0x01      /* should break the dispatch */
#define LIBAROMA_MSG_OK           0x02      /* ok message */
#define LIBAROMA_MSG_BACK         0x03      /* back message */
#define LIBAROMA_MSG_NEXT         0x04      /* next message */
#define LIBAROMA_MSG_MENU         0x05      /* show menu event */
#define LIBAROMA_MSG_TOUCH        0x06      /* touch event */
#define LIBAROMA_MSG_KEY_SELECT   LIBAROMA_MSG_KEY(LIBAROMA_HID_EV_RET_SELECT)
#define LIBAROMA_MSG_KEY_MENU     LIBAROMA_MSG_KEY(LIBAROMA_HID_EV_RET_MENU)
#define LIBAROMA_MSG_KEY_BACK     LIBAROMA_MSG_KEY(LIBAROMA_HID_EV_RET_BACK)
#define LIBAROMA_MSG_KEY_UP       LIBAROMA_MSG_KEY(LIBAROMA_HID_EV_RET_UP)
#define LIBAROMA_MSG_KEY_DOWN     LIBAROMA_MSG_KEY(LIBAROMA_HID_EV_RET_DOWN)
#define LIBAROMA_MSG_KEY_RAWKEY   LIBAROMA_MSG_KEY(LIBAROMA_HID_EV_RET_RAWKEY)
#define LIBAROMA_MSG_ISUSER(X)    ((0xC0&X)==0xC0)  /* user event */
#define LIBAROMA_MSG_ISSYS(X)     ((0xC0&X)==0x80)  /* sys event */
#define LIBAROMA_MSG_ISKEY(X)     ((0xC0&X)==0x40)  /* key event */
```
这里我按代码的调用顺序分析

### 6.2 LIBAROMA_MSG_WIN_MEASURED
在学习android都直到View的绘制的过程经理了onMeasure-->onLayout--->onDraw阶段。其实这里和android基本有点类似，android向一个布局里面添加一个View的时候要指定Layoutparam，最典型的就是match_parent,在add时候就可以确定一个View的大小和相对于布局的位置，Libaroma框架也支持几种简单的相对尺寸，所以在MEASURED阶段就会确定control的大小和尺寸。
``` java
#define LIBAROMA_SIZE_FULL     0
#define LIBAROMA_SIZE_HALF    -1
#define LIBAROMA_SIZE_THIRD   -2
#define LIBAROMA_SIZE_QUARTER -3
#define LIBAROMA_POS_HALF     -4
#define LIBAROMA_POS_1P3      -5
#define LIBAROMA_POS_2P3      -6
#define LIBAROMA_POS_1P4      -7
#define LIBAROMA_POS_3P4      -8
```
既然是相对尺寸，肯定需要一个基准，所以这里就要分析下基准是参照的那个，这里就不卖关子了直接上获取的流程。
- fb_init的时候获取屏幕的像素宽高。
- windowmanager一般会设置一个工作空间，默认是全屏，如果屏幕有的区域是不更新的显示的是静态内容就可以设置工作空间小一点，减轻UI渲染线程的负担，所以windowmanger管理的激活的window的大小尺寸位置信息是在这里确定，window支持相对尺寸，当一个window激活就会相对于屏幕高宽像素measure下真实的尺寸。
- 当window的尺寸确定时候，再向window添加一个control时候也会measure一次，这次确定的是control相对于window的尺寸和位置。


Measure事件比较简单，一般实现一个界面的时候就会Measure以下，其实也不排除某些需求需要使工作空间变小，就要从新measure window的大小，然后根据window的尺寸measure control的大小。

### 6.3 LIBAROMA_MSG_WIN_ACTIVE、LIBAROMA_MSG_WIN_INACTIVE
windowmanger管理window，可以切换激活window，如果一个window被激活，windowmanger的ui线程就渲染改window上面的内容，最终是界面变化，当windowmanger设置一个激活的window时候就会启动ui线程和msg线程，为什么这小节同时介绍着两种事件，因为当windowmanger管理的两个window切换时候，一个必然是激活状态，一个必然是非激活状态。最典型就是dialog显示的时候，dialog依附于一个新的window，dialog显示的时候就是激活状态，以前激活状态的window变成非激活状态的，先简单看下实现。
激活的时候
#### 6.3.1 LIBAROMA_MSG_WIN_ACTIVE
``` java
case LIBAROMA_MSG_WIN_ACTIVE: {
		/* set current window size */
		win->focused = NULL;
		win->touched = NULL;
		if (msg->x != 10) {
			_libaroma_window_ready(win);
		}
		if ((!win->lock_sync) || (msg->x == 10)) {
			if ((!win->active) || (msg->x == 10)) {
				int i;
				win->active = 1;
				/* signal child */
				for (i = 0; i < win->childn; i++) {
					if (win->childs[i]->handler->message) {
						win->childs[i]->handler->message(win->childs[i], msg);
					}
				}
			}
		}
	}
```
这里乍看没做啥，就是遍历window的子控件，告诉control我现在处于激活状态，control向做啥，赶紧做..... 不得不吐槽下作者框架很屌，但是这里的**msg->x != 10**确实难读其中深意。其中_libaroma_window_ready看似轻描淡写其实里面干了好多事情。
``` java

byte _libaroma_window_ready(LIBAROMA_WINDOWP win) {
	__CHECK_WM(0);
	if (win == NULL) {
		ALOGW("window_resize win is NULL");
		return 0;
	}
	int x = win->x;
	int y = win->y;
	int w = win->w;
	int h = win->h;
	if (w == 0) {
		w = libaroma_wm()->w;
		x = 0;
	}
	if (h == 0) {
		h = libaroma_wm()->h;
		y = 0;
	}
	/* set position */
	if (win->dc != NULL) {
		libaroma_canvas_free(win->dc);
		win->dc = NULL;
	}
	win->dc = libaroma_wm_canvas(x, y, w, h);
	if (win->dc == NULL) {
		ALOGW("window_ready cannot allocate workspace drawing canvas");
		return 0;
	}
	if (libaroma_window_isactive(win)) {
		libaroma_wm_clean_workspace();
	}
	win->x = x;
	win->y = y;
	win->w = win->dc->w;
	win->h = win->dc->h;
	_libaroma_window_measure_save(win, NULL);
	_libaroma_window_recalculate(win);
	return 1;
}

```
分析下，其实里面做了一个很重要的一步骤

    win->dc = libaroma_wm_canvas(x, y, w, h);
激活window的dc变量在这赋值，之前control draw方法canvas获取就是依赖与window的dc，dc其实指向初始化阶段分配的好的内存，如果为每个canvas都分配相应的内存，恐怕内存早就爆掉了，继续进去看
``` java
LIBAROMA_CANVASP libaroma_wm_canvas(int x, int y, int w, int h) {
	if (_libaroma_wm == NULL) {
		ALOGW("window manager uninitialized");
		return NULL;
	}
	int x2 = x + _libaroma_wm->x;
	int y2 = y + _libaroma_wm->y;
	int w2 = w;
	int h2 = h;
	if (x2 < _libaroma_wm->x) {
		w2 -= x2 - _libaroma_wm->x;
		x2 = 0;
	}
	if (y2 < _libaroma_wm->y) {
		h2 -= y2 - _libaroma_wm->y;
		y2 = 0;
	}
	if ((w2 < 1) || (h2 < 1)) {
		/* sync doesn't needed */
		ALOGW("libaroma_wm_canvas invalid requested size (%ix%i)", w2, h2);
		return NULL;
	}
	if (w2 > _libaroma_wm->w) {
		w2 = _libaroma_wm->w;
	}
	if (h2 > _libaroma_wm->h) {
		h2 = _libaroma_wm->h;
	}
	return libaroma_canvas_area(libaroma_fb()->canvas, x2, y2, w2, h2);
} /* End of libaroma_wm_canvas */

```
重点在最后一句，记得以前分析control的时候control对应区域的canvas获取也是通过libaroma_canvas_area方法获取，只不过当时传入的参数是window的dc，这里传入的是fb_init时候分配的一段显存，其实所有绘制都是向改内存区域更新数据.

接下来就是如果window是激活状态就,就先绘制window对应的背景到framebuffer中这里就不细看了.之后又是一番measure,确定window的尺寸位置,最后就是重新绘制window里面的control上屏,这里还是有必要跟进看看做了啥.
``` java
byte _libaroma_window_recalculate(LIBAROMA_WINDOWP win) {
	if (win == NULL) {
		ALOGW("window_recalculate win is NULL");
		return 0;
	}
	if (libaroma_window_isactive(win)) {
		_libaroma_window_updatebg(win);
		libaroma_window_invalidate(win, 1);
	}
	return 1;
```


``` java
byte _libaroma_window_updatebg(LIBAROMA_WINDOWP win) {
	if (win == NULL) {
		ALOGW("window_recalculate win is NULL");
		return 0;
	}
	if (win->handler != NULL) {
		if (win->handler->updatebg != NULL) {
			if (win->handler->updatebg(win)) {
				if (win->onupdatebg) {
					win->onupdatebg(win, win->bg);
				}
				return 1;
			}
			return 0;
		}
	}
	if (win->parent != NULL) {
		return 0;
	}
	int w = win->w;
	int h = win->h;

	/* draw background */
	if (win->bg != NULL) {
		if ((win->bg->w == w) && (win->bg->h == h)) {
			/* not need recreate background */
			return 1;
		}
		libaroma_canvas_free(win->bg);
	}
	win->bg = libaroma_canvas(w,h);

	/* default canvas color */
	libaroma_canvas_setcolor(win->bg, libaroma_colorget(NULL, win)->window_bg,
			0xff);

	/* from theme canvas */
	if (win->theme_bg[0] != 0) {
		libaroma_wm_draw_theme(win->bg, win->theme_bg, 0, 0, win->bg->w,
				win->bg->h, NULL);
	}

	/* from updatebg callback */
	if (win->onupdatebg != NULL) {
		win->onupdatebg(win, win->bg);
	}
	return 1;
} /* End of _libaroma_window_updatebg */
```
这里有个又出来两个window的hook,unpdatebg和onupdate,暂时不知道有啥用,可能自己控件玩的少,有些场景还是需要用到.这里只做留意.
接下来就是默认流程,这里分配window bg对应空间,这里真的要分配也就意味着,一个window需要用fb初始化的内存,和自己分配的内存,对于嵌入式设备恐怕吃不消,如果将来要裁剪可以从这里裁剪,并且在分配空间时候又几种不同的策略,共享内存匿名内存什么的.如果bin过大可以从这里动手.


接下来就要分析下libaroma_window_invalidate
``` java
byte libaroma_window_invalidate(LIBAROMA_WINDOWP win, byte sync) {
	__CHECK_WM(0);
	if (win == NULL) {
		ALOGW("window_invalidate win is null");
		return 0;
	}
	if (win->handler != NULL) {
		if (win->handler->invalidate != NULL) {
			return win->handler->invalidate(win, sync);
		}
	}
	if (win->parent != NULL) {
		return 0;
	}

	if (!libaroma_window_isactive(win)) {
		ALOGW("window_invalidate win is not active window");
		return 0;
	}
	if (win->dc == NULL) {
		ALOGW("window_invalidate dc is null");
		return 0;
	}

	if ((!win->lock_sync) || (sync == 10)) {
		/* draw bg */libaroma_draw(
				win->dc,
				win->bg,
				0, 0, 1);

		/* draw childs */
		int i;
#ifdef LIBAROMA_CONFIG_OPENMP
#pragma omp parallel for
#endif
		for (i = 0; i < win->childn; i++) {
			/* draw no sync */
			libaroma_control_draw(win->childs[i], 0);
		}

		/* sync */
		if (sync) {
			libaroma_window_sync(win, 0, 0, win->w, win->h);
		}
	}
	return 1;
```
这里也有一个hook--- window的invalidate回调.暂时还不知道能干啥,研究复杂control的时候多留意.
无非就是干了两件是,绘制子控件,上屏.

#### 6.3.2 LIBAROM_MSG_WIN_INACTIVE
这个比较简单,就是告诉window的control 我现在非激活状态了,你们该干啥干啥了.
``` java
	case LIBAROMA_MSG_WIN_INACTIVE: {
		if (win->active) {
			/* stop thread manager */
			win->active = 0;

			/* send inactive message to child */
			int i;
			for (i = 0; i < win->childn; i++) {
				if (win->childs[i]->handler->message) {
					win->childs[i]->handler->message(win->childs[i], msg);
				}
			}
			win->focused = NULL;
			win->touched = NULL;
		}
	}
```
### 6.4 LIBAROMA_MSG_WIN_RESIZE
这个主要是工作控件变化或者window大小变化的时候处理改消息.其实和LIBAROMA_MSG_WIN_ACTIVE差不多,这里要从新更新window的dc,因为window的大小变化了.最后就是告诉我变化了 control你们也要变化了,跟人觉得这里少做一步**既然window大小变了,因为control是相对尺寸,所以这里control要从新measure一次**难道要放在control的message里面特殊处理?.
```  java
	case LIBAROMA_MSG_WIN_RESIZE: {
		int i;
		_libaroma_window_ready(win);
		for (i = 0; i < win->childn; i++) {
			if (win->childs[i]->handler->message) {
				win->childs[i]->handler->message(win->childs[i], msg);
			}
		}
	}
```
### 6.5 LIBAROMA_MSG_WIN_INVALIDATE
这个比较简单就是为了重绘整个画面.但是这个都放在主线程肯定会觉得卡卡的,暂时还没发现哪里在用.

### 6.6 LIBAROMA_MSG_WIN_DIRECTMSG
这个更简单,只是返回一个值,具体干什么现在还不太清楚

	return (dword) msg->x;
    
至于LIBAROMA_MSG_TOUCH事件,在control事件绑定和响应章节具体分析.安卓对于点击事件有长按,点击基本事件,Libaroma框架也有实现基本类似,安卓是在action_up时候触发点击事件,长按事件,则是一个超时机制.

## 7 控件单击长按上报
依然是控件,点击之后要有返回,然后做相关的业务,android提供,单击长按绑定,着套框架也有单击长按事件,在了解事件,修改对触屏反馈有所理解,当点击屏幕使用会触发硬件中断,然后在中断处理函数上报触摸位置信息,一个点击事件一般由一些列基本事件组成,分别是down move up组成其中move可以有很多,上报的信息根据解析解析,可以接触面积,压强,xy坐标等,当初在这方面脑补不少东西,这套框架支持Linux多点触控的B协议,但是MTK的手机用的是多点触控A协议,因为要玩,无奈自己就先简单解析下,基本能用.但是触摸屏下面的虚拟按键还是没调通,凑合先玩着.接下来分析下触屏事件上报的流程.

### 7.1 触屏信息上报的流程
这里先简单梳理下.在msg_init时候会起来一个线程专门读取触屏或者按键上报,没有事件的时候,将阻塞.windowmanager在没有工作时候,过滤掉触屏和按键上报,当windowmanger工作的时候也会起来一个msg工作线程,只不过此线程读取事件源有msg的input队列和queque队列,如果这两个队列没东东就休眠,当有按键和触屏事件时候会向msg的input的队列抛事件,并唤醒windowmanger的msg thread来读取该上报信息,然后抛到windomanger的queque队列,之后就是唤醒main线程来读取该上报信息.

#### 7.1.1 msg_init的工作线程
改线程主要是读取底层上报.抛到msg的input队列

``` java
static void * _libaroma_msgqueue_hid_thread(void * cookie) {
	/* hid loop */
	while (_libaroma_msgqueue_isrun) {
		/* wait for hid event */
		LIBAROMA_HID_EVENT e;
		byte ret = libaroma_hid_get(&e);
		if (_libaroma_msgqueue_isrun == 2) {
			/* process input message */
			switch (e.type) {
			case LIBAROMA_HID_EV_TYPE_TOUCH:
				/* post touch message */
				libaroma_msg_post_hid(LIBAROMA_MSG_TOUCH, e.state, e.key, e.x,
						e.y);
				break;
			case LIBAROMA_HID_EV_TYPE_KEY:
				/* post key message */
				libaroma_msg_post_hid(LIBAROMA_MSG_KEY(ret), e.state, e.key,
						e.x, e.y);
				break;
			case LIBAROMA_HID_EV_RET_EXIT:
				libaroma_msg_post_hid(LIBAROMA_MSG_EXIT, 0, 0, 0, 0);
				break;
			}
		}
	}
	return NULL;
}
```
这里就不深究了,不同平台有不同实现Libaroma针对触屏按键上报的接口都进行的了封装,很容易移植到不同平台,具体就是实现libaroma_hid_get函数,针对android就是通过poll来读取上报消息,按照多点触控协议解析上报的的,这里就是主要是按键和触屏信息上报.

``` java
byte libaroma_msg_post_hid(byte msg, byte state, int key, int x, int y) {
	/* ignore non-start messages */
	if (_libaroma_msgqueue_isrun != 2) {
		return 0;
	}
	/* set data */
	LIBAROMA_MSG _msg;
	_msg.msg = msg;
	_msg.state = state;
	_msg.key = key;
	_msg.x = x;
	_msg.y = y;
	_msg.d = NULL;
	_msg.sent = libaroma_tick();
	/* mutex lock */
	libaroma_cond_lock(&_libaroma_msgqueue_mutex);
	/* push message */
	byte ret = libaroma_stack_push(_libaroma_msgqueue->input, &_msg,
			sizeof(LIBAROMA_MSG));
	/* send signal if message was pushed */
	if (ret) {
		libaroma_cond_signal(&_libaroma_msgqueue_cond);
	}
	/* mutex unlock */
	libaroma_cond_unlock(&_libaroma_msgqueue_mutex);
	/* return status */
	return ret;
```
然后就是抛到msg的input队列中,唤醒windowmanger的msg线程.

#### 7.1.2 windowmamger的 msg线程.
其实windowmanger的msg线程不是简单处理按键,触屏上报,还是从另一个queue中读取事件,这个事件可以是别的线程抛到这里的.先看windowmanager的msg thread
```  java
static void * _libaroma_wm_message_thread(void * cookie) {
	ALOGV("starting wm messaging");
	while (_libaroma_wm->client_started) {
		LIBAROMA_MSG msg;
		byte ret = libaroma_msg(&msg);
		if (ret) {
			if (_libaroma_wm->message_handler != NULL) {
				ret = _libaroma_wm->message_handler(_libaroma_wm, &msg);
				if (ret == LIBAROMA_WM_MSG_HANDLED) {
					continue;
				} else if (ret == LIBAROMA_WM_MSG_EXIT) {
					break;
				}
			}
			/* send to window */
			if (_libaroma_wm->active_window != NULL) {
				libaroma_cond_lock(&_libaroma_wm_mutex);
				if (msg.msg == LIBAROMA_MSG_TOUCH) {
					/* update touch coordinate */
					msg.x -= libaroma_wm()->x;
					msg.y -= libaroma_wm()->y;
				}
				if (libaroma_stack_push(_libaroma_wm->queue, &msg,
						sizeof(LIBAROMA_MSG))) {
					libaroma_cond_signal(&_libaroma_wm_cond);
				}
				libaroma_cond_unlock(&_libaroma_wm_mutex);
			}
		}
	}ALOGV("wm messaging ended");
	return NULL;
}

```
这里就是调用libaroma_msg从msg对应的input或者queue中读取消息,如果没有将休眠.
``` java
byte libaroma_msg(LIBAROMA_MSGP msg) {
	/* mutex lock */
	libaroma_cond_lock(&_libaroma_msgqueue_mutex);
	/* wait until data available in queue */
	while ((_libaroma_msgqueue->queue->n + _libaroma_msgqueue->input->n) < 1) {
		/* wait for signal */
		libaroma_cond_wait(&_libaroma_msgqueue_cond, &_libaroma_msgqueue_mutex);
		/* if not valid, return it */
		if (!_libaroma_msgqueue_isrun) {
			libaroma_cond_unlock(&_libaroma_msgqueue_mutex);
			return LIBAROMA_MSG_NONE;
		}
	}
	LIBAROMA_STACKP get_stack = _libaroma_msgqueue->input;
	if (_libaroma_msgqueue->queue->n > 0) {
		get_stack = _libaroma_msgqueue->queue;
	}
	byte ret = LIBAROMA_MSG_NONE;
	/* shift stack */
	LIBAROMA_MSGP shift_event = (LIBAROMA_MSGP) libaroma_stack_shift(get_stack);
	if (shift_event != NULL) {
		/* copy into destination */
		memcpy(msg, shift_event, sizeof(LIBAROMA_MSG));
		/* set return value as msg item */
		ret = shift_event->msg;
		/* free shifted data */
		free(shift_event);
	}
	/* unlock mutex */
	libaroma_cond_unlock(&_libaroma_msgqueue_mutex);
	return ret;
}
```
可见就是等待按键或者其他事件,然后抛到windowmanger的 queque中.  windowmanger的queue一般是在主线程中被读取.
#### 7.1.3 主线程事件处理
通常在最线程最后都会写这样的循环
``` java
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
```

继续更新libaroma_window_pool
``` java
dword libaroma_window_pool(LIBAROMA_WINDOWP win, LIBAROMA_MSGP msg) {
	if (!win) {
		return 0;
	}
	if (win->parent != NULL) {
		ALOGW("cannot pool child window...");
		return 0;
	}
	LIBAROMA_MSG _msg;
	LIBAROMA_MSGP cmsg = (msg != NULL) ? msg : &_msg;
	byte ret = libaroma_wm_getmessage(cmsg);
	if (ret) {
		return libaroma_window_process_event(win, cmsg);
	}
	return 0;
} /* End of libaroma_window_pool */

```
继续看libaroma_wm_getmessage
```  java
byte libaroma_wm_getmessage(LIBAROMA_MSGP msg) {
	if (!_libaroma_wm->client_started) {
		return LIBAROMA_MSG_NONE;
	}
	/* mutex lock */libaroma_cond_lock(&_libaroma_wm_mutex);
	while (_libaroma_wm->queue->n < 1) {
		libaroma_cond_wait(&_libaroma_wm_cond, &_libaroma_wm_mutex);
		if (!_libaroma_wm->client_started) {
			libaroma_cond_unlock(&_libaroma_wm_mutex);
			return LIBAROMA_MSG_NONE;
		}
	}
	byte ret = LIBAROMA_MSG_NONE;
	/* shift stack */
	LIBAROMA_MSGP shift_event = (LIBAROMA_MSGP) libaroma_stack_shift(
			_libaroma_wm->queue);
	if (shift_event != NULL) {
		memcpy(msg, shift_event, sizeof(LIBAROMA_MSG));
		ret = shift_event->msg;
		free(shift_event);
	}
	libaroma_cond_unlock(&_libaroma_wm_mutex);
	return ret;
```
可见主线程是等待windowmanger的queue里面有事件,木有咱就睡,别人有消息的时候再把我唤醒,我才开始做自己的事情.休眠也就意味着主线程进入休眠状态.

### 7.2 触屏事件上报给control的流程
触屏信息一般先上报给windowmanger,windowmanger进行一次修正,因为windowmanger的工作空间有可能不是全屏,并且control是相对于window的位置不是屏幕的位置,如果工作空间不是全屏就要减去工作空间左上角的坐标xy
``` java
			if (msg.msg == LIBAROMA_MSG_TOUCH) {
					/* update touch coordinate */
					msg.x -= libaroma_wm()->x;
					msg.y -= libaroma_wm()->y;
				}
```

接着就是通知主线程处理触屏信息

``` java
case LIBAROMA_MSG_TOUCH: {
		/* touch handler */
		if (msg->state == LIBAROMA_HID_EV_STATE_DOWN) {
			win->touched = NULL;
			int x = msg->x;
			int y = msg->y;
			libaroma_window_calculate_pos(win, NULL, &x, &y);
			int i;
			for (i = 0; i < win->childn; i++) {
				if (_libaroma_window_is_inside(win->childs[i], x, y)) {
					win->touched = win->childs[i];
					break;
				}
			}
			if (win->touched != NULL) {
				if (win->touched->handler->message) {
					ret = win->touched->handler->message(win->touched, msg);
				}
			}
		} else if (win->touched != NULL) {
			if (msg->state == LIBAROMA_HID_EV_STATE_MOVE) {
				if (win->touched->handler->message) {
					ret = win->touched->handler->message(win->touched, msg);
				}
			} else if (msg->state == LIBAROMA_HID_EV_STATE_UP) {
				if (win->touched->handler->message) {
					ret = win->touched->handler->message(win->touched, msg);
				}
				win->touched = NULL;
			}
		}
````
这里首先也是进行一次修正因为window的要减去window的左上角的位置,才能遍历window里面的所有控件确定点击的控件是谁.
其次就是遍历找到点击控件,并把点击事件交给control区处理.
下来看看control处理上报事件一般做了什么.以button为例子
``` java
	case LIBAROMA_MSG_TOUCH: {
		if (me->style & LIBAROMA_CTL_BUTTON_DISABLED) {
			/* no thing for disabled button */
			return 0;
		}
		int x = msg->x;
		int y = msg->y;
		libaroma_window_calculate_pos(NULL, ctl, &x, &y);

		/* touch handler */
		if (msg->state == LIBAROMA_HID_EV_STATE_DOWN) {
			libaroma_ripple_down(&me->ripple, x, y);
		} else if (msg->state == LIBAROMA_HID_EV_STATE_UP) {
			byte res = libaroma_ripple_up(&me->ripple, 0);
			if ((res & LIBAROMA_RIPPLE_TOUCHED)
					&& (!(res & LIBAROMA_RIPPLE_HOLDED))) {
				libaroma_window_post_command(
						LIBAROMA_CMD_SET(LIBAROMA_CMD_CLICK, 0, ctl->id));
			}
		} else if (msg->state == LIBAROMA_HID_EV_STATE_MOVE) {
			if (!((x >= 0) && (y >= 0) && (x < ctl->w) && (y < ctl->h))) {
				libaroma_ripple_cancel(&me->ripple);
			} else {
				libaroma_ripple_move(&me->ripple, x, y);
			}
		}
	}
```
做了如下几件事情:

- 修正xy坐标 这次是相对与自己的坐标
- 这里关注下 up的时候 libaroma_window_pos_command一个信息到msg的que队列. 这里就进去看了 post是LIBAROMA_MSG_WIN_DIRECTMSG事件,cmd信息报错当前控件的id,点击事件类型.
- 主线程获取事件,根据id做不同的响应


```  java
		dword command = libaroma_window_pool(win, &msg);
		byte cmd = LIBAROMA_CMD(command);
		word id = LIBAROMA_CMD_ID(command);
		byte param = LIBAROMA_CMD_PARAM(command);
```
至于长按事件,要配合control的thread回调来确定,这里就不分析了.

## 8 实现自定义控件
自定义控件实现其实和android的非常类似,只要实现具体的回调就可以,不过这里要自己处理点击事件,控件的界面绘制就是在draw方法里面实现,libaroma提供绘制圆形折线等能力,绘制字什么的.下面就以control_button控件为例子分析实现.
### 8.1 添加一个button到window上
添加button到window其实就是注册过程,并首次绘制button的window上.一边先会调用

``` java
LIBAROMA_CONTROLP libaroma_ctl_button(LIBAROMA_WINDOWP win, word id, int x,
		int y, int w, int h, const char * text, byte button_style,
		word button_color) {
	/* init internal data */
	_LIBAROMA_CTL_BUTTONP me = (_LIBAROMA_CTL_BUTTONP)
	calloc(sizeof(_LIBAROMA_CTL_BUTTON), 1);
	if (!me) {
		ALOGW("libaroma_ctl_button alloc button memory failed");
		return NULL;
	}

	/* set internal data */
	libaroma_mutex_init(me->mutex);
	me->text = strdup(text);
	me->style = button_style;
	me->color = button_color;

	/* init control */
	LIBAROMA_CONTROLP ctl = libaroma_control_new(id, x, y, w, h,
			libaroma_dp(48), libaroma_dp(48), /* min size */
			(voidp) me, &_libaroma_ctl_button_handler, win);
	if (!ctl) {
		libaroma_mutex_free(me->mutex);
		if (me->text != NULL) {
			free(me->text);
		}
		free(me);
		return NULL;
	}
	return ctl;
} /* End of libaroma_ctl_button */

```
首先分配button对应的数据结构,然后成员赋值数据结构如下
```  java
struct __LIBAROMA_CTL_BUTTON {
	char * text; //显示的文字信息
	byte style; //风格样式
	byte isdark;
	word color; //颜色
	byte forcedraw; //标识位

	LIBAROMA_RIPPLE ripple;
	LIBAROMA_MUTEX mutex; //同步锁
	LIBAROMA_CANVASP rest_canvas;
	LIBAROMA_CANVASP push_canvas;
};
```
接下来分给一个control对应的结构体并赋值.
``` java
LIBAROMA_CONTROLP libaroma_control_new(word id, int x, int y, int w, int h,
		int minw, int minh, voidp internal, LIBAROMA_CONTROL_HANDLERP handler,
		LIBAROMA_WINDOWP win) {
	if (handler == NULL) {
		return NULL;
	}
	LIBAROMA_CONTROLP ret = (LIBAROMA_CONTROLP) calloc(
			sizeof(LIBAROMA_CONTROL), 1);
	if (!ret) {
		ALOGW("window_control_new cannot allocating memory");
		return NULL;
	}
	ret->minw = minw;
	ret->minh = minh;
	ret->rx = x;
	ret->ry = y;
	ret->rw = w;
	ret->rh = h;
	ret->id = id;
	ret->handler = handler;
	ret->internal = internal;

	if (win) {
		return libaroma_window_attach(win, ret);
	}
	return ret;
} /* End of libaroma_control_new */
```

这里看下这个结构体

``` java
struct _LIBAROMA_CONTROL{
  word id; //控件id
  voidp internal; //控件自己结构体 附带信息
  LIBAROMA_WINDOWP window; //依赖的window
  LIBAROMA_COLORSETP colorset;
  
  /* px measured */
  int x; //相对window x坐标
  int y; // 相对window y坐标
  int w; //真实宽度
  int h; //真实高度
  
  /* requested */
  int rx; 
  int ry;
  int rw; //相对宽度
  int rh; //相对高度
  
  /* measured size */
  int left;
  int top;
  int width;
  int height;
  
  /* minimum control size */
  int minw; // 最小宽度
  int minh; //最小高度
  
  /* callbacks */
  LIBAROMA_CONTROL_HANDLERP handler; //回调函数结构体
};
```
添加control 到window
添加control到window会触发control相对window的measure的过程确定真实的高宽
```  java
LIBAROMA_CONTROLP libaroma_window_attach(LIBAROMA_WINDOWP win,
		LIBAROMA_CONTROLP ctl) {
	/* attach into window */
	if (win) {
		if (libaroma_window_add(win, ctl)) {
			return ctl;
		} ALOGW("window_attach cannot attach into window");
		libaroma_control_free(ctl);
		return NULL;
	}
	return ctl;
```
继续看libaroma_window_attach

``` java
byte libaroma_window_add(LIBAROMA_WINDOWP win, LIBAROMA_CONTROLP ctl) {
	__CHECK_WM(0);
	if (win == NULL) {
		ALOGW("window_add win is NULL");
		return 0;
	}
	if (ctl == NULL) {
		ALOGW("window_add ctl is NULL");
		return 0;
	}
	if (ctl->window != NULL) {
		ALOGW("window_add ctl already have window");
		return 0;
	}
	libaroma_window_measure(win, ctl);
	if (win->childn == 0) {
		win->childs = (LIBAROMA_CONTROLP *) malloc(sizeof(LIBAROMA_CONTROLP));
		if (!win->childs) {
			ALOGW("window_add malloc failed");
			win->childs = NULL;
			return 0;
		}
		win->childs[0] = ctl;
	} else {
		LIBAROMA_CONTROLP * newchilds = (LIBAROMA_CONTROLP *) realloc(
				win->childs, sizeof(LIBAROMA_CONTROLP) * (win->childn + 1));
		if (!newchilds) {
			ALOGW("window_add realloc failed");
			return 0;
		}
		win->childs = newchilds;
		win->childs[win->childn] = ctl;
	}
	ctl->window = win;
	win->childn++;
	_libaroma_window_recalculate(win);
	return 1;
}

```
做了如下几件事

- 相对于window的measure过程
- 添加control的结构体指针到window childs数组中
- 赋值control附属的window信息
- 增加widnow的 childn 变量
- 重绘界面,此时window不是激活状态就跳过了.

### 8.2 button回调实现
回调在不同的时机触发每个回调做的东东也不一样.

``` java
static LIBAROMA_CONTROL_HANDLER _libaroma_ctl_button_handler = {
		message:_libaroma_ctl_button_msg, //主线程被调用
		draw:_libaroma_ctl_button_draw, //ui线程被调用
		focus:NULL, //window的setFocus回调
		destroy:_libaroma_ctl_button_destroy,//window释放的时候回调
		thread	:_libaroma_ctl_button_thread //ui线程一定会被调用
};

```
####  8.2.1 thread回调
thread回调是一定会被调用的,每16ms一调用,这里决定是否绘制control中的内容.


``` java
byte _libaroma_ctl_button_thread(LIBAROMA_CONTROLP ctl) {
	/* internal check */
	_LIBAROMA_CTL_CHECK(_libaroma_ctl_button_handler, _LIBAROMA_CTL_BUTTONP, 0);
	byte is_draw = me->forcedraw;
	if (!(me->style & LIBAROMA_CTL_BUTTON_DISABLED)) {
		byte res = libaroma_ripple_thread(&me->ripple, 0);
		if (res & LIBAROMA_RIPPLE_REDRAW) {
			is_draw = 1;
		}
		if (res & LIBAROMA_RIPPLE_HOLDED) {
			libaroma_window_post_command(
					LIBAROMA_CMD_SET(LIBAROMA_CMD_HOLD, 0, ctl->id));
		}
	}
	if (is_draw) {
		return 1;
	}

	return 0;
}

```
其实就是玩弄一个自己的成员变量,为draw阶段准备数据源,因为这个button有水波纹效果,还听负责的就不仔细分析,就是为绘制水波纹准备

#### 8.2.2 draw回调

draw 回调其实就是玩弄canvas,向上面绘制东西,决定是否立即上绘制上屏


``` java
void _libaroma_ctl_button_draw(LIBAROMA_CONTROLP ctl, LIBAROMA_CANVASP c) {
	/* internal check */
	_LIBAROMA_CTL_CHECK(_libaroma_ctl_button_handler, _LIBAROMA_CTL_BUTTONP,); // 判断回调是是否注册,并获取内部数据
	libaroma_mutex_lock(me->mutex); // 同步锁
	if ((me->rest_canvas == NULL) || (me->forcedraw == 2)) {
		libaroma_mutex_unlock(me->mutex);
		_libaroma_ctl_button_internal_draw(ctl);
		libaroma_mutex_lock(me->mutex);
		if (me->rest_canvas == NULL) {
			libaroma_mutex_unlock(me->mutex);
			return;
		}
	}
	me->forcedraw = 0;

	libaroma_control_erasebg(ctl, c); // 更新canvas背景 或者说是清屏幕
	if (me->style & LIBAROMA_CTL_BUTTON_DISABLED) {
		libaroma_draw(c, me->rest_canvas, 0, 0, 0); // button处于disable状态就绘制rest_canvas内容到canvas上
	} else {
		int ripple_i = 0;
		int ripple_p = 0;
		libaroma_draw(c, me->rest_canvas, 0, 0, 0); //不是则先绘制 rest_canvas到c
		if (me->push_canvas != NULL) {
			byte is_circle = (me->style & LIBAROMA_CTL_BUTTON_CIRCLE) ? 1 : 0;
			while (libaroma_ripple_loop(&me->ripple, &ripple_i, &ripple_p)) {
				int x = 0;
				int y = 0;
				int size = 0;
				byte push_opacity = 0;
				byte ripple_opacity = 0;
				if (libaroma_ripple_calculation(&me->ripple, ctl->w, ctl->h,
						&push_opacity, &ripple_opacity, &x, &y, &size,
						ripple_p)) {
					if (me->push_canvas != NULL) {void _libaroma_ctl_button_internal_draw(LIBAROMA_CONTROLP ctl) {
    _LIBAROMA_CTL_BUTTONP me = (_LIBAROMA_CTL_BUTTONP) ctl->internal;
    if (ctl->window == NULL) {
        return;
    }

    libaroma_mutex_lock(me->mutex);
    LIBAROMA_CANVASP rest_canvas = NULL;
    LIBAROMA_CANVASP push_canvas = NULL;

    byte is_disabled = (me->style & LIBAROMA_CTL_BUTTON_DISABLED) ? 1 : 0;

						libaroma_draw_opacity(c, me->push_canvas, 0, 0, 0,
								push_opacity * 2);
					}
					word dcolor = me->isdark ? RGB(ffffff) : 0;
					byte push_opa = (byte) ((((int) ripple_opacity)
							* (me->isdark ? 64 : 30)) >> 8);
					if (is_circle) {
						LIBAROMA_CANVASP rdc = libaroma_canvas(c->w, c->h);
						if (rdc) {
							libaroma_draw(rdc, c, 0, 0, 0);
							libaroma_draw_circle(rdc, dcolor, x, y, size,
									push_opa);
							int cntx = rdc->w >> 1;
							int cnty = rdc->h >> 1;
							int szxy = MIN(rdc->w,rdc->h) - libaroma_dp(12);
							libaroma_draw_mask_circle(c, rdc, cntx, cnty, cntx,
									cnty, szxy, 0xff); //画圆形
							libaroma_canvas_free(rdc);
						}
					} else {
						LIBAROMA_CANVASP ca = libaroma_canvas_area(c,
								libaroma_dp(4), libaroma_dp(6),
								c->w - libaroma_dp(8), c->h - libaroma_dp(12));
						if (ca) {
							libaroma_draw_circle(ca, dcolor, x, y, size,
									push_opa);
							libaroma_canvas_free(ca);
						}
					}
				}
			}
		}
	}
	libaroma_mutex_unlock(me->mutex);
}
```

首先要缓存起来一些不变的东西,这是典型的以空间换时间的做法

``` java
void _libaroma_ctl_button_internal_draw(LIBAROMA_CONTROLP ctl) {
	_LIBAROMA_CTL_BUTTONP me = (_LIBAROMA_CTL_BUTTONP) ctl->internal;
	if (ctl->window == NULL) {
		return;
	}

	libaroma_mutex_lock(me->mutex);
	LIBAROMA_CANVASP rest_canvas = NULL;
	LIBAROMA_CANVASP push_canvas = NULL;

	byte is_disabled = (me->style & LIBAROMA_CTL_BUTTON_DISABLED) ? 1 : 0;
	rest_canvas = libaroma_canvas(ctl->w, ctl->h); //分贝rest_canvas对应的内存
	if (!is_disabled) {
		push_canvas = libaroma_canvas(ctl->w, ctl->h); //不是disable状态就分配push_canvas内存
	}
	LIBAROMA_CANVASP bg = libaroma_canvas(ctl->w, ctl->h); //分配背景
	libaroma_control_erasebg(ctl, rest_canvas); //清空rest_canvas
	if (!is_disabled) {
		libaroma_control_erasebg(ctl, push_canvas); //清空 push_canvas 首次
	}
	libaroma_control_erasebg(ctl, bg); //清空背景

	byte is_circle = (me->style & LIBAROMA_CTL_BUTTON_CIRCLE) ? 1 : 0;// button 方形和圆形判断
	int ix = libaroma_dp(is_circle ? 6 : 4);
	int iy = libaroma_dp(6);
	int iw = ctl->w - ix * 2;
	int ih = ctl->h - iy * 2;

	if (me->style & LIBAROMA_CTL_BUTTON_COLORED) {
		me->isdark = libaroma_color_isdark(me->color); //赋值isdark
	} else {
		me->isdark = libaroma_color_isdark(
				libaroma_colorget(ctl, NULL)->window_bg);
	}
	word push_color = me->isdark ? RGB(ffffff) : RGB(000000);
	word text_color = me->isdark ? 0xffff : 0; //文本颜色
	word rest_text_color = text_color;
	byte push_opa = me->isdark ? 50 : 30;

	/* buttons */
	LIBAROMA_CANVASP btnmask = libaroma_canvas_ex(iw, ih, 1); //蒙版
	libaroma_canvas_setcolor(btnmask, 0, 0); //设置蒙版颜色
	libaroma_gradient(btnmask, 0, 0, ctl->w - libaroma_dp(is_circle ? 12 : 8),
			ctl->h - libaroma_dp(12), 0, 0,
			is_circle?MAX(ctl->w,ctl->h):libaroma_dp(2), 0x1111);

	/* raised shadow */
	if (me->style & LIBAROMA_CTL_BUTTON_RAISED) {

		libaroma_draw_zshadow(rest_canvas, btnmask, ix, iy,
				(me->style & LIBAROMA_CTL_BUTTON_COLORED) ? 2 : 1);

		if (!is_disabled) {
			libaroma_draw_zshadow(push_canvas, btnmask, ix, iy, 3);
		}
	}

	if (me->style & LIBAROMA_CTL_BUTTON_COLORED) {
		if (is_disabled) {
			libaroma_draw_ex(btnmask, bg, 0, 0, ix, iy, btnmask->w, btnmask->h,
					0, 0xff);
			libaroma_draw_rect(btnmask, 0, 0, btnmask->w, btnmask->h,
					me->isdark ? 0xffff : 0, 127);
		} else {
			libaroma_draw_rect(btnmask, 0, 0, btnmask->w, btnmask->h, me->color,
					0xff);
		}
		if (me->style & LIBAROMA_CTL_BUTTON_RAISED) {
			libaroma_draw(rest_canvas, btnmask, ix, iy, 1);
		} else {
			rest_text_color = me->color;
		}
		if (!is_disabled) {
			libaroma_draw_rect(btnmask, 0, 0, btnmask->w, btnmask->h,
					push_color, push_opa);
			libaroma_draw(push_canvas, btnmask, ix, iy, 1);
		}
	} else {
		if (me->style & LIBAROMA_CTL_BUTTON_RAISED) {
			libaroma_draw_ex(btnmask, bg, 0, 0, ix, iy, btnmask->w, btnmask->h,
					0, 0xff);
			if (!is_disabled) {
				libaroma_draw_rect(btnmask, 0, 0, btnmask->w, btnmask->h,
						0xffff, me->isdark ? 20 : 49);
			}
			libaroma_draw(rest_canvas, btnmask, ix, iy, 1);
		}
		if (!is_disabled) {
			libaroma_draw_ex(btnmask, bg, 0, 0, ix, iy, btnmask->w, btnmask->h,
					0, 0xff);
			libaroma_draw_rect(btnmask, 0, 0, btnmask->w, btnmask->h,
					push_color, push_opa);
			libaroma_draw(push_canvas, btnmask, ix, iy, 1);
		}
	}

	/* draw text */
	LIBAROMA_TEXT textp = libaroma_text(me->text, text_color,
			iw - libaroma_dp(16),
			LIBAROMA_FONT(0,4) |
			LIBAROMA_TEXT_SINGLELINE |
			LIBAROMA_TEXT_CENTER |
			LIBAROMA_TEXT_FIXED_INDENT |
			LIBAROMA_TEXT_FIXED_COLOR |
			LIBAROMA_TEXT_NOHR, 100); //绘制文本
	// libaroma_mutex_unlock(me->mutex);
	int y = (ctl->h >> 1)
			- ((libaroma_text_height(textp) >> 1) + libaroma_dp(2));

	if (is_disabled) {
		rest_text_color = me->isdark ? 0xffff : 0;
	}
	libaroma_text_draw_color(rest_canvas, textp, libaroma_dp(8) + ix, y,
			rest_text_color);

	if (!is_disabled) {
		libaroma_text_draw(push_canvas, textp, libaroma_dp(8) + ix, y);
	} else {
		libaroma_draw_ex(rest_canvas, bg, 0, 0, 0, 0, ctl->w, ctl->h, 0,
				me->isdark ? 171 : 189);
	}
	libaroma_text_free(textp); //释放textp

	/* cleanup */
	libaroma_canvas_free(bg);// 释放背景
	libaroma_canvas_free(btnmask);//释放蒙版

	/* save it */
	// libaroma_mutex_lock(me->mutex);
	LIBAROMA_CANVASP rc = me->rest_canvas;
	LIBAROMA_CANVASP pc = me->push_canvas;
	me->rest_canvas = rest_canvas;
	me->push_canvas = push_canvas;
	if (rc != NULL) {
		libaroma_canvas_free(rc);
	}
	if (pc != NULL) {
		libaroma_canvas_free(pc);
	}
	me->forcedraw = 1; //不知道什么意义 1 2 3 好难懂
	libaroma_mutex_unlock(me->mutex);
} 
```
#### 8.2.3 message回调
message 回调在主线程,一般处理触屏时间


```   java
dword _libaroma_ctl_button_msg(LIBAROMA_CONTROLP ctl, LIBAROMA_MSGP msg) {
	/* internal check */
	_LIBAROMA_CTL_CHECK(_libaroma_ctl_button_handler, _LIBAROMA_CTL_BUTTONP, 0);

	switch (msg->msg) {
	case LIBAROMA_MSG_WIN_ACTIVE:
	case LIBAROMA_MSG_WIN_INACTIVE:
	case LIBAROMA_MSG_WIN_RESIZE: {
		libaroma_mutex_lock(me->mutex);
		me->forcedraw = 1;
		libaroma_mutex_unlock(me->mutex);
	}
		break;
	case LIBAROMA_MSG_TOUCH: {
		if (me->style & LIBAROMA_CTL_BUTTON_DISABLED) {
			/* no thing for disabled button */
			return 0;
		}
		int x = msg->x;
		int y = msg->y;
		libaroma_window_calculate_pos(NULL, ctl, &x, &y);

		/* touch handler */
		if (msg->state == LIBAROMA_HID_EV_STATE_DOWN) {
			libaroma_ripple_down(&me->ripple, x, y);
		} else if (msg->state == LIBAROMA_HID_EV_STATE_UP) {
			byte res = libaroma_ripple_up(&me->ripple, 0);
			if ((res & LIBAROMA_RIPPLE_TOUCHED)
					&& (!(res & LIBAROMA_RIPPLE_HOLDED))) {
				libaroma_window_post_command(
						LIBAROMA_CMD_SET(LIBAROMA_CMD_CLICK, 0, ctl->id));
			}
		} else if (msg->state == LIBAROMA_HID_EV_STATE_MOVE) {
			if (!((x >= 0) && (y >= 0) && (x < ctl->w) && (y < ctl->h))) {
				libaroma_ripple_cancel(&me->ripple);
			} else {
				libaroma_ripple_move(&me->ripple, x, y);
			}
		}
	}
		break;
	}
	return 0;
} /* End of _libaroma_ctl_button_msg */
```

touch 其实也在玩弄自己的内部数据
对于win的消息,只是更新了forcedraw的类型,在thread里面用, touch时间,也是在玩弄自己的数据,在up的时候触发单击或者长按事件

#### 8.2.4 destroy回调
destroy回调就是当control所依赖的window被销毁的时候,回调,这里无非就是释放内存.


``` java
void _libaroma_ctl_button_destroy(LIBAROMA_CONTROLP ctl) {
	/* internal check */
	_LIBAROMA_CTL_CHECK(_libaroma_ctl_button_handler, _LIBAROMA_CTL_BUTTONP,);
	libaroma_mutex_lock(me->mutex);
	if (me->rest_canvas != NULL) {
		libaroma_canvas_free(me->rest_canvas);
		me->rest_canvas = NULL;
	}
	if (me->push_canvas != NULL) {
		libaroma_canvas_free(me->push_canvas);
		me->push_canvas = NULL;
	}
	if (me->text != NULL) {
		free(me->text);
	}
	libaroma_mutex_unlock(me->mutex);
	libaroma_mutex_free(me->mutex);
	free(me);
} 
```

### 8.3 自定义control回顾

自定义control其实也是固定的流程,分为以下几个流程

- 定义control内部数据结构体
- 初始化control结构体,赋值相关变量 主要是位置信息 大小信息,回调信息和内部数据
- 添加window上确定control的实际大小
- 实现draw接口,draw的参数canvas就是control对应measure对应位置的内存指针
- 实现thread的方法,更新内部数据,进一步决定是否要调用draw方法,改方法大概16ms调用一次
- 实现message方法处理点击事件和window事件
- 实现destroy来释放内部数据内存

## 9 实现一个界面，以及处理相关交互
实现一个轻量级别的APP模式也是固定的,主要是如下几方面.
- 框架初始化
- 设置windowmanger的工作空间
- 得到一个根window,确定window相对工作空间的位置和大小
- 向window添加control集合
- 设置激活window,是windowmanger里面ui线程msg线程开始工作
- 主线程处理事件,并做相关业务,一般是一个循环
- 释放分配的window结构体
- 释放框架资源
先看mian方法里面做了什么.

``` java
int main(int argc, char **argv) {


	init_libaroma();

	/* start common test */
	common_test();

	/* end libaroma process */
	libaroma_end();

	return 0;
} /* End of main */

```

###　9.1 总初始化
初始化一般是初始化框架,并且加载字体文件.并做清理屏幕工作
``` java
void init_libaroma() {

	/*snprintf(libaroma_config()->fb_shm_name,64,"");*/
	libaroma_config()->fb_shm_name[0] = 0;
	libaroma_start();//框架初始化

	/* clean display */
	libaroma_canvas_blank(libaroma_fb()->canvas);//清屏
	libaroma_sync();//上屏

	/* load font - id=0 */
	libaroma_font(0, libaroma_stream("file:///sdcard/Roboto-Regular.ttf")); //加载字体
}
```
### 9.2 初始化window
window的尺寸信息是相对fb的宽度和高度,这里没有设置工作空间所以默认就是全屏
``` java
	LIBAROMA_WINDOWP win = libaroma_window(NULL, 0, 0, LIBAROMA_SIZE_FULL,
			LIBAROMA_SIZE_FULL); /* fullscreen */

```

### 9.3 添加control到window
这个环节其实就是给control分配id,确定control的位置信息尺寸信息和控件自己的信息.
``` java
	/* progress bar */
	LIBAROMA_CONTROLP progress = libaroma_ctl_progress(win, 50, 0, 20,
			LIBAROMA_SIZE_FULL, 10, LIBAROMA_CTL_PROGRESS_DETERMINATE, 100, 0);

	LIBAROMA_CONTROLP btn1 = libaroma_ctl_button(win, 1, 0, 60,
			LIBAROMA_SIZE_HALF, 60, "Toggle Disable", LIBAROMA_CTL_BUTTON_FLAT,
			0);

	LIBAROMA_CONTROLP btn2 = libaroma_ctl_button(win, 2, LIBAROMA_POS_HALF, 60,
			LIBAROMA_SIZE_HALF, 60, "Add List Item", LIBAROMA_CTL_BUTTON_RAISED,
			0);

	LIBAROMA_CONTROLP btn3 = libaroma_ctl_button(win, 3, 0, 120,
			LIBAROMA_SIZE_HALF, 60, "Change Style", LIBAROMA_CTL_BUTTON_COLORED,
			RGB(ffffff));

	LIBAROMA_CONTROLP btn4 = libaroma_ctl_button(win, 4, LIBAROMA_POS_HALF, 120,
			LIBAROMA_SIZE_HALF, 60, "Delete List Item",
			LIBAROMA_CTL_BUTTON_RAISED | LIBAROMA_CTL_BUTTON_COLORED,
			RGB(335599));

	LIBAROMA_CONTROLP btn5 = libaroma_ctl_button(win, 5, 0, 180,
			LIBAROMA_SIZE_HALF, 60, "Tap & Hold", LIBAROMA_CTL_BUTTON_COLORED,
			RGB(118822));

	LIBAROMA_CONTROLP btn6 = libaroma_ctl_button(win, 6, LIBAROMA_POS_HALF, 180,
			LIBAROMA_SIZE_HALF, 60, "Screenshoot",
			LIBAROMA_CTL_BUTTON_RAISED | LIBAROMA_CTL_BUTTON_COLORED,
			RGB(ffcccc));

	LIBAROMA_CONTROLP progress2 = libaroma_ctl_progress(win, 51, 0, 246,
			LIBAROMA_SIZE_HALF, 48,
			LIBAROMA_CTL_PROGRESS_DETERMINATE | LIBAROMA_CTL_PROGRESS_CIRCULAR,
			100, 0);

	LIBAROMA_CONTROLP btn_exit = libaroma_ctl_button(win, 7, LIBAROMA_POS_HALF,
			240, LIBAROMA_SIZE_HALF, 60, "<b>Tap & Hold</b> to Exit",
			LIBAROMA_CTL_BUTTON_RAISED | LIBAROMA_CTL_BUTTON_COLORED,
			RGB(F44336));

	LIBAROMA_CONTROLP list_test = libaroma_ctl_list(win, 90, /* win, id */
	0, 300, LIBAROMA_SIZE_FULL, LIBAROMA_SIZE_FULL, /* x,y,w,h */
	0, 8, /* horiz, vert padding */
	RGB(ffffff), /* bgcolor */
	LIBAROMA_CTL_SCROLL_WITH_SHADOW | LIBAROMA_CTL_SCROLL_WITH_HANDLE /* flags */
	);

	LIBAROMA_CANVASP list_icon = libaroma_image_uri(
			"file:///sdcard/ic_settings_data_usage.png");
	char main_text[256];
	char extra_text[256];
	int itm = 0;
	for (itm = 0; itm < 50; itm++) {
		snprintf(main_text, 256, "Item id#%i", itm);
		word add_flags = 0;
		if (itm % 3 == 1) {
			add_flags = LIBAROMA_LISTITEM_CHECK_SWITCH;
			snprintf(extra_text, 256,
					"Secondary text for list item %i is three line list item text",
					itm);
		} else if (itm % 3 == 2) {
			snprintf(extra_text, 256, "Secondary text %i", itm);
		}
		libaroma_listitem_check(list_test, itm, 0, main_text,
				(itm % 3 != 0) ? extra_text : NULL, list_icon,
				LIBAROMA_LISTITEM_CHECK_INDENT_NOICON
						| LIBAROMA_LISTITEM_WITH_SEPARATOR
						| LIBAROMA_LISTITEM_CHECK_SHARED_ICON | add_flags, -1);
	}

```

### 9.4 设置windowmanger激活window
设置激活window主要是为了是ui线程和msg线程工作
``` java
	libaroma_window_anishow(win, LIBAROMA_WINDOW_SHOW_ANIMATION_PAGE_LEFT, 400);
    
```
### 9.5 业务处理
这里主要根据更新控件属性,和做相关业务
``` java
	byte onpool = 1;
	int click_value = 0;
	do {
		LIBAROMA_MSG msg;
		dword command = libaroma_window_pool(win, &msg);
		byte cmd = LIBAROMA_CMD(command);
		word id = LIBAROMA_CMD_ID(command);
		byte param = LIBAROMA_CMD_PARAM(command);

		if (cmd) {

			if (cmd == LIBAROMA_CMD_CLICK) {
				if (id == 6) {
					if (click_value == 0) {
						libaroma_png_save(libaroma_fb()->canvas,
								"/sdcard/libaroma_screenshoot.png");
					}

					click_value++;
					char clstr[128];
					snprintf(clstr, 128, "Clicked %i", click_value);
					libaroma_ctl_button_text(btn6, clstr);
				} else if (id == 2) {
					snprintf(main_text, 256, "New Item id#%i", itm);
					snprintf(extra_text, 256,
							"<u>This</u> is <b>just extra text</b> for item %i NEW!",
							itm);
					libaroma_listitem_check(list_test, itm, 0, main_text,
							extra_text, list_icon,
							LIBAROMA_LISTITEM_CHECK_INDENT_NOICON
									| LIBAROMA_LISTITEM_CHECK_SHARED_ICON
									| LIBAROMA_LISTITEM_WITH_SEPARATOR, -1);
					itm++;
				} else if (id == 4) {
					libaroma_ctl_list_del_item_internal(list_test, 0, 0 /* delete first index */
					);
				} else if (id == 1) {
					if (libaroma_ctl_button_is_disabled(btn2)) {
						libaroma_ctl_button_disable(btn2, 0);
						libaroma_ctl_button_disable(btn4, 0);
					} else {
						libaroma_ctl_button_disable(btn2, 1);
						libaroma_ctl_button_disable(btn4, 1);
					}
				} else if (id == 3) {
					btn3state++;
					if (btn3state > 3) {
						btn3state = 0;
					}
					if (btn3state == 0) {
						libaroma_ctl_button_style(btn3,
								LIBAROMA_CTL_BUTTON_COLORED, RGB(ffffff));
					} else if (btn3state == 1) {
						libaroma_ctl_button_style(btn3,
								LIBAROMA_CTL_BUTTON_COLORED
										| LIBAROMA_CTL_BUTTON_RAISED,
								RGB(9C27B0));
					} else if (btn3state == 2) {
						libaroma_ctl_button_style(btn3,
								LIBAROMA_CTL_BUTTON_COLORED, RGB(FF5722));
					} else if (btn3state == 3) {
						libaroma_ctl_button_style(btn3,
								LIBAROMA_CTL_BUTTON_RAISED, RGB(FF5722));
					}
				} else if (id == 5) {
					progress_value += 10;
					if (progress_value > 100) {
						progress_value = 0;
					}
					printf("---> Change Progress value: %i...\n",
							progress_value);
					libaroma_ctl_progress_value(progress2, progress_value);
					libaroma_ctl_progress_value(progress, progress_value);
				}
			} else if (cmd == LIBAROMA_CMD_HOLD) {
				if (id == 5) {
					progress_type++;
					if (progress_type > 2) {
						progress_type = 0;
					}
					printf("---> Change Progress Type: %i\n", progress_type);
					libaroma_ctl_progress_type(progress2,
							progress_type | LIBAROMA_CTL_PROGRESS_CIRCULAR);
					libaroma_ctl_progress_type(progress, progress_type);
				} else if (id == 6) {
					click_value++;
					char clstr[128];
					snprintf(clstr, 128, "<u>Holded</u> (%i)", click_value);
					libaroma_ctl_button_text(btn6, clstr);
				} else if (id == btn_exit->id) {
					printf("Exit Button Pressed...\n");
					onpool = 0;
				}
			}

			printf("Window Command = (CMD: %x, ID: %x, Param: %x)\n",
					LIBAROMA_CMD(command), LIBAROMA_CMD_ID(command),
					LIBAROMA_CMD_PARAM(command));
		}
	} while (onpool);


```

### 9.6 释放资源
释放资源包括两方面,框架资源 window和其他资源比如图片信息

``` java
	} while (onpool);

	if (list_icon) {
		libaroma_canvas_free(list_icon);
	}

	/* free window */
	libaroma_window_free(win);
    
    libaroma_end();


```

<iframe height=498 width=510 src="http://player.youku.com/embed/XNjcyMDU4Njg0">
