#!/usr/bin/bash

#top -n 1| tail -n 20 | head -n 10
# 7251 root         20   0  10M 3.7M 3.0M R 36.8(这一列为cpu使用率)   0.2   0:00.15 top -n 1()
# 1413 root         RT   0    0    0    0 S  5.2   					  0.0   0:10.54 [sugov:4]
# 7253 root         20   0  10M 3.2M 2.7M S  0.0   					  0.1   0:00.02 head -n 10
# 7252 root         20   0  10M 3.3M 2.7M S  0.0   					  0.1   0:00.02 tail -n 20
# 7113 root         20   0 8.7M 2.9M 2.3M S  0.0   					  0.1   0:00.08 sh -
# 6585 root         20   0    0    0    0 S  0.0   					  0.0   0:01.29 [kworker/u16:3]
# 5721 root         20   0    0    0    0 S  0.0   					  0.0   0:01.50 [kworker/u16:2]
# 5664 root         20   0    0    0    0 S  0.0   					  0.0   0:02.49 [kworker/u16:0]
# 5644 root         20   0    0    0    0 S  0.0   					  0.0   0:02.68 [kworker/u16:1]
# 5604 root         20   0    0    0    0 S  0.0   					  0.0   0:00.07 [kworker/0:2]

#top -t -m 10 -n 1 | tail -n 10
# 4722  4722 root     20   0  12% R   9916K   3728K  fg top             top
# 2688  2688 system   12  -8   0% S  86720K  19200K unk surfaceflinger  /system/bin/surfaceflinger
# 3494  3601 radio    20   0   0% S 1146104K 114400K  fg RILSender0      com.android.phone
#    4     4 root     20   0   0% S      0K      0K  fg kworker/0:0
#    5     5 root      0 -20   0% S      0K      0K  fg kworker/0:0H
#    6     6 root     20   0   0% S      0K      0K  fg kworker/u16:0
#    7     7 root     20   0   0% S      0K      0K  fg rcu_preempt
#    8     8 root     20   0   0% S      0K      0K  fg rcu_sched
#    9     9 root     20   0   0% S      0K      0K  fg rcu_bh
#   10    10 root     RT   0   0% S      0K      0K  fg migration/0

function del_file {
	if [ -e $1 ] ; then
		rm $1
	fi
}

function cal_top_thread {
	echo "\nthread_cpu load(top):" 
	local time=10
	local i=0
	local top_result="/sdcard/top_result.txt"
	local top_temp_n="/sdcard/top_temp_n"
	local top_temp_t="/sdcard/top_temp_t"
	del_file $top_result
	del_file $top_temp_n
	del_file $top_temp_t
	
	while [ "$i" -lt "${time}" ]
	do
		# top --help   or top -h 可以查看top线程信息
		# -n 表示只显示一次 tail -n 是显示后面的20个线程， head -n 是现实过滤后的前面10个线程
		top -n 1| tail -n 20 | head -n ${time} > ${top_temp_n}
		# top -m是只显示10个线程 -n b表示显示一次 tail同上
		top -t -m ${time} -n 1 | tail -n ${time} > ${top_temp_t}
		let i++
	done

	# -s表示文件存在，且文件不为空
	if [ -s $top_temp_t ] ; then
		#$12表示的是12列, $9表示的是第9列
		cat $top_temp_t | busybox awk '{
			printf("%-20s\t%s\n", $11, $6) 
		}' >> $top_result
	else
		# 同理如上
		cat $top_temp_n | busybox awk '{
		printf("%-20s\t%s\n", $12, $9) 
		}' >> $top_result
	fi
	#a[$i]是将第一列的数据作为索引,值为第二列的数据
	#END是遍历到文件结束时
	#for(i in a)应该是i在a的key arry里面
	#功能是统计相同的行并进行相加
	busybox awk '{if(a[$1]){a[$1]=a[$1]+$2}else{a[$1]=$2}}
			END{for(i in a)printf("thread: %-23s%.2f%\n",i,a[i]/10)}' $top_result 
	
}

cal_top_thread