
>Libaroma是一个用纯C写的控件库，支持Linux、X86、QNX等不同平台。针对不同平台API做了一层封装，可以很快的移植到不同平台，参照Android的材料设计风格实现控件库，已经写好了Fragment、ViewPager、Button、ProgressBar、ListView、Dialog等控件，开发者可以实现自己的控件。目前Libaroma框架只绝对布局，控件的开发基本和Android类似，也有Window，WindowManagr、EventHub等概念，支持Window转场动画。

#### works:
  1. FreeType work on MT2523,支持从ROM中加载ttf 字体和文件系统加载ttf字体.
  2. png decode Support.
  3. 大部分内置控件都在2523 上面可以使用.
  4. window 专场动画支持,支持平移,覆盖等多种形式的动画.
  5. 文字排版支持居中方式和CSS标签

#### 内置控件列表

  1. label 文本框
  2. image 图片框,支持帧动画
  3. progress 进度条,长方形和圆形进度条,loading.
  4. viewpager.
  5. listview 列表.
  6. fragment.
  7. scroller.
  8. qrcode 二维码
  

  SDK:https://labs.mediatek.com/zh-cn/download/Cobne0ON

  libaroma: https://github.com/amarullz/libaroma
  
  libaroma GUI 分析请参考本人博客:http://ever-never.github.io/2016/08/28/Libaroma-GUI/
