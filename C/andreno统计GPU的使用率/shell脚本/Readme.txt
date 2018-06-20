一、功能
抓取gpu loading数据

二、使用方法
不同场景下使用方法不同

1、连接usb抓取参数信息
双击gpu_loading.bat脚本，出现如下界面时输入测试次数(默认测试600次)，enter开始执行
please input test times :
执行结束后console界面,出现如下提示“请按任意键继续. . .”此时可通过按任意键退出

2、连接串口抓取参数信息
在normal状态下将gpu_loading.sh脚本push到/data目录下
adb root
adb remount
adb push xxxx/gpu_loading.sh(可直接通过拉取文件至命令行生成该路径) /data/

通过串口工具执行脚本
su root
sh /data/gpu_loading.sh 测试次数(默认测试600次)

3、示例
C:\Users\xxx\Desktop\脚本\contrast_phone\红米5\gpu_loading>adb root
adbd is already running as root

C:\Users\xxx\Desktop\脚本\contrast_phone\红米5\gpu_loading>adb remount
remount succeeded

C:\Users\xxx\Desktop\脚本\contrast_phone\红米5\gpu_loading>set /p test_times=please input test times :
please input test times :

C:\Users\xxx\Desktop\脚本\contrast_phone\红米5\gpu_loading>set dir=C:\Users\xxx\Desktop\脚本\contrast_phone\红米5\gpu_loading\gpu_loading.sh

C:\Users\xxx\Desktop\脚本\contrast_phone\红米5\gpu_loading>adb push C:\Users\xxx\Desktop\脚本\contrast_phone\红米5\gpu_loading\gpu_loading.sh /data/
189 KB/s (3022 bytes in 0.015s)

C:\Users\xxx\Desktop\脚本\contrast_phone\红米5\gpu_loading>adb shell sh /data/gpu_loading.sh
133: 490     216: 13     320: 97     400: 0     510: 0     560: 0     600: 0

133 = 81.67%  GPU Utilisation: 25.28%
216 = 2.17%  GPU Utilisation: 8.85%
320 = 16.17%  GPU Utilisation: 5.23%

polling_interval:10
Test Times: 600   GPU Utilisation: 21.68
C:\Users\xxx\Desktop\脚本\contrast_phone\红米5\gpu_loading>pause
请按任意键继续. . .

三、备注
1、当前已验证platform
红米5

2、因资料有限，loading信息节点并不一定完全正确，该脚本扫描出来的数据仅供参考






