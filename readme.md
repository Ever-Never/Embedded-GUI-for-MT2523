
>Libaroma是一个用纯C写的控件库，支持Linux、X86、QNX等不同平台。针对不同平台API做了一层封装，可以很快的移植到不同平台，参照Android的材料设计风格实现控件库，已经写好了Fragment、ViewPager、Button、ProgressBar、ListView、Dialog等控件，开发者可以实现自己的控件。目前Libaroma框架只绝对布局，控件的开发基本和Android类似，也有Window，WindowManagr、EventHub等概念，支持Window转场动画。

现在可以运行在Linux平台使用SDL实现HAL层,也同样可以运行Android 上,在MTK平台上由于TP上报的协议不同导致触屏不能使用,不过本人粗略调整,已经可以使用了,同样本人也添加FreeRTOS的支持,现实了MT2523 HAL层的对接,现在TP和屏幕都可以工作,整个GUI框架主体部分已经全部工作,libaroma 已经验证过了,可以在主频208M ,RAM 4M  ROM 4M 屏幕360x360, 和 主频48M RAM256K 屏幕160x64 ROM 1M

#### works:
  1. FreeType work on MT2523,支持从ROM中加载ttf 字体和文件系统加载ttf字体.
  2. png decode Support.
  3. 大部分内置控件都在2523 上面可以使用.
  4. window 专场动画支持,支持平移,覆盖等多种形式的动画.
  5. 文字排版支持居中方式和CSS标签
  6. 支持565的屏幕和888的屏幕
  
#### 内置控件列表

  1. label 文本框
  2. image 图片框,支持帧动画
  3. progress 进度条,长方形和圆形进度条,loading.
  4. viewpager.
  5. listview 列表.
  6. fragment.
  7. scroller.
  8. qrcode 二维码
  
#### build

只是支持GCC编译,keil下编译请自行参考makefile添加文件

    cd project/mt2523_hdk/apps/gdi_display_helloworld/GCC
    make

  SDK:https://labs.mediatek.com/zh-cn/download/Cobne0ON

  libaroma: https://github.com/amarullz/libaroma
  
  libaroma GUI 分析请参考本人博客:http://ever-never.github.io/2016/08/28/Libaroma-GUI/
