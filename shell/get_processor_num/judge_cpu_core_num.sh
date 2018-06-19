#!/bin/sh
cpu_path="/sys/devices/system/cpu/"
cpu_core_cnt=0

function cpu_core {
	cpu_core_cnt=0
	# 这个用来判断cpu的core num应该是更加合理的
	while [ -d ${cpu_path}cpu${cpu_core_cnt} ]
	do
		let "cpu_core_cnt++"
	done
}


# 查看逻辑CPU的个数
cat /proc/cpuinfo| grep "processor"| wc -l
