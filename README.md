# 内存调度模拟 - 动态分区分配方式的模拟

---

> 操作系统 内存模拟 - 动态分区分配方式的模拟


1751130

![](https://badgen.net/badge/Windows%2010/1903/blue) ![](https://badgen.net/badge/Qt/v5.12.3/green) ![](https://badgen.net/badge/MinGW%20x64/v8.1.0/cyan) ![](https://badgen.net/badge/build/passing/green) 

![](/img/1.png)

## 项目背景及需求

假设初始态下，可用内存空间为640K，并有下列请求序列，请分别用首次适应算法和最佳适应算法进行内存块的分配和回收，并显示出每次分配和回收后的空闲分区链的情况来。

    作业1申请130K
    作业2申请60K
    作业3申请100k
    作业2释放60K
    作业4申请200K
    作业3释放100K
    作业1释放130K
    作业5申请140K
    作业6申请60K
    作业7申请50K
    作业6释放60K

## 开发/运行环境

环境: Windows 10 1903

语言: C++ / MinGW x64 8.1.0 / Qt 5.12.3

----

## 系统架构

项目文件:
```sh
    └─ram
        │  README.md
        │
        ├─release
        │  │  Qt5Core.dll
        │  │  Qt5Gui.dll
        │  │  Qt5Widgets.dll
        │  │  ram.exe
        │  │
        │  ├─platforms
        │  │      qwindows.dll
        │  │
        │  └─styles
        │          qwindowsvistastyle.dll
        │
        └─ram
            main.cpp
            mainwindow.cpp
            mainwindow.h
            mainwindow.ui
            ram.pro
            ram.pro.user
```

### mainwindow 类

>程序主窗口

1. 内存显示
2. `Mode` : 分配模式选择
3. 控制按钮
4. 当前操作列表
5. 状态栏, 指示当前内存使用情况.

```c++
    void draw(); // Draw RAM infomation on the label
    void reset(); // Run when the Button reset was clicked
    void setStutusBar(); // send infomation to the StatusBar
    void freeRam(int begin, int end);
    void run_a_step(std::pair<int, int>);
    bool eventFilter(QObject *watched, QEvent *event);
```

## 分配算法

维护两个表, `remainRam` 和 `used`. 前者存储空闲内存信息, 后者存储各任务使用内存的信息.

1. 申请内存

对 `remainRam` 按照选定的规则排序, 选择第一个适应的空插入, 同时更新 `used`.

2. 释放内存

删除 `used` 对应的项, 同时操作 `remainRam`, 如果释放的空间与已有的空间相连, 将它们合并; 否则, 在 `remainRam` 中新增这条记录.

