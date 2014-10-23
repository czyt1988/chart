说明
----------------------
Qwt 控件2次封装
1.精简配置步骤，直接加载chart.pri即可导入相关类和对应的内容
2.SAPlotChart.h，属于其它工程文件，可以在chart.pri中去掉

配置方法
----------------------
chart文件夹拷贝入工程目录下（Main文件所在目录）
在Qt的pro文件中加入如下这句，
include($$PWD\chart\chart.pri)