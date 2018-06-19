一、功能:查看dvfs_table

二、用法:
1、命令行依次输入
adb root				
adb remount
adb push 文件路劲(可直接通过拖拉.sh文件至命令行获取路劲) /data/
adb shell
sh /data/lookat_dvfs_table.sh	
运行之后，即可显示结果

2、示例
Microsoft Windows [版本 6.1.7601]
版权所有 (c) 2009 Microsoft Corporation。保留所有权利。

C:\Users\xxx>adb push C:\Users\xxx\Desktop\power_script\dvfs_table\lookat_dvfs_table.sh /data/
272 KB/s (4352 bytes in 0.015s)

C:\Users\xxx>adb shell
sp9832e_1h10_go:/ # sh /data/lookat_dvfs_table.sh
sp9832e_1h10_go:/ # sh /data/lookat_dvfs_table.sh
    768000             868
    900000             921
   1100000            1000
   1300000            1081
   1400000            1121
sp9832e_1h10_go:/ #


三、备注:

1、因为regulator  driver命名的节点名称每个项目每个board均可能存在差异
使用时需提前确认节点名称，必要时需对脚本做适当修改

2、只适用于userdebug版本(user版本权限不足)

3、如有问题请致电3216，感谢支持与配合


