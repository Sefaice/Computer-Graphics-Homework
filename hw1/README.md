# 计算机图形学作业一

## 1. 结合上述参考链接（或其他参考资料），谈谈自己对计算机图形学的理解

计算机图形学使用计算机表示图形图像的学科，主要有三个领域：Modeling建模，Rendering渲染，Animation动画。主要内容是用计算机构建场景/人物的图像，并且显示到2D的屏幕上。如果说计算机视觉是一个有输入的处理过程，图形学就需要自己创造，并且通常还需要考虑硬件的性能、运行时间、效果等。

##  2. 结合上述参考链接（或其他参考资料），回答什么是OpenGL? OpenGL ES? Web GL? Vulkan? DirectX? 

OpenGL是一种图形API，它“包含了一系列可以操作图形、图像的函数”。通常提到OpenGL，也会设计DirectX，DirectX是由微软开发的一种图形API。

OpenGL ES (OpenGL for Embedded Systems) 是 OpenGL 三维图形 API 的子集，针对手机、PDA和游戏主机等嵌入式设备而设计。

WebGL（Web Graphics Library）是“一种3D绘图协议，这种绘图技术标准允许把JavaScript和OpenGL ES 2.0结合在一起，通过增加OpenGL ES 2.0的一个JavaScript绑定，WebGL可以为HTML5 Canvas提供硬件3D加速渲染，这样Web开发人员就可以借助系统显卡来在浏览器里更流畅地展示3D场景和模型了，还能创建复杂的导航和数据视觉化”，WebGL是让opengl可以用于web端开发的API，使用js语言。

Vulkan也是一种图形API，和OpenGL和DirectX一样，通过调用这些API可以调用计算机的GPU进行图形运算。
 
## 3. gl.h glu.h glew.h 的作用分别是什么? 

gl.h是OpenGL运行需要调用的头文件，包含了OpenGL定义的函数。

glu.h是glu库的头文件，glu封装了OpenGL的函数以简化操作。

glew.h是glew库的头文件，“不同的显卡公司，也会发布一些只有自家显卡才支 持的扩展函数，你要想用这数涵数，不得不去寻找最新的glext.h,有了GLEW扩展库，你就再也不用为找不到函数的接口而烦恼，因为GLEW能自动识别你的平台所支持的全部OpenGL高级扩展函数。也就是说，只要包含一个glew.h头文件，你就能使用gl,glu,glext,wgl,glx的全部函数。”

## 4. 使用GLFW和freeglut的目的是什么？

>在我们画出出色的效果之前，首先要做的就是创建一个OpenGL上下文(Context)和一个用于显示的窗口。然而，这些操作在每个系统上都是不一样的，OpenGL有目的地从这些操作抽象(Abstract)出去。这意味着我们不得不自己处理创建窗口，定义OpenGL上下文以及处理用户输入。

使用GLFW库/freegult库可以帮助我们简化这些操作，省去根据不同的硬件创建上下文，创建窗口的操作。

## 5. 结合上述参考链接（或其他参考资料），选择一个SIGGRAPH 2017/2018上 你最喜欢的专题，介绍该专题是做什么的，使用了什么CG技术？（不少于100字）

[FaceStyle](https://research.adobe.com/project/facestyle-example-based-synthesis-of-stylized-facial-animations/)。这是一个识别人物头像然后进行各种艺术效果描绘的应用，它首先识别出人物头像的关键部分，然后根据识别的关键点加上所选效果渲染，在识别成功后可以对人物后续的动作进行动态添加效果。这里除了计算机视觉的识别技术外，图形学的建模技术，实时渲染技术都在这个项目中用到。
