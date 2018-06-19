功能：统计一段时间内CPU平均使用率前N的进程

1、连接usb获取进程信息
双击sgm.bat脚本，出现如下界面时输入测试时间(默认测试60s)，enter开始执行
please input test times(s) :
执行结束后consule界面时输入显示的进程数(默认显示CPU的平均使用率前10的进程)，enter开始执行
please input top number :
设置结束将生成数据结果

2、连接串口抓取
在normal状态下将sprd_top脚本push到/data/local/tmp/目录下
adb push xxxx/sprd_top(可直接通过拉取文件至命令行生成该路劲) /data/local/tmp/

通过串口工具执行脚本
chmod 777 /data/local/tmp/sprd_top
./data/local/tmp/sprd_top -t 10 -n 10

3、手动执行脚本
由于bat脚本中的环境变量和windows配置相关，在遇到bat脚本出错时
可依次执行如下命令来手动执行脚本
adb push C:\Users\xxx\Desktop\power_script\top\top /data/local/tmp/
adb shell
chmod 777 /data/local/tmp/top
./data/local/tmp/top -t 10 -n 10
-t 后面接的是测试时长(默认是10s)
-n 后面接的是显示的CPU平均使用率前N的进程(默认是前10的进程)

4、示例如下
C:\Users\xxx\Desktop\power_script\top>adb shell "rm /data/local/tmp/top"

C:\Users\xxx\Desktop\power_script\top>adb push C:\Users\xxx\Desktop\power_script\top\top /data/local/tmp/
2163 KB/s (656656 bytes in 0.296s)

C:\Users\xxx\Desktop\power_script\top>set /p test_times=please input test times(s) :
please input test times(s) :

C:\Users\xxx\Desktop\power_script\top>if "" == "" set test_times=10

C:\Users\xxx\Desktop\power_script\top>set /p num=please input top number :
please input top number :

C:\Users\xxx\Desktop\power_script\top>if "" == "" set num=10

C:\Users\xxx\Desktop\power_script\top>adb shell "chmod 777 /data/local/tmp/top"

C:\Users\xxx\Desktop\power_script\top>adb shell "./data/local/tmp/top -t 10 -n 10"
PID     %[CPU]  ARGS
1847    41.9    com.imangi.temp+
 1979   19.3    top -n 1 -d 3
 346    12.9    surfaceflinger
 328    3.2     android.hardwar+
 488    3.2     android.hardwar+
 500    3.2     [kworker/u16:9]
 678    3.2     system_server
 893    3.2     com.android.sys+
 1411   3.2     android.process+
 1954   3.2     com.android.key+


C:\Users\xxx\Desktop\power_script\top>pause
请按任意键继续. . .

备注

1、适用于user版本和userdebug版本

