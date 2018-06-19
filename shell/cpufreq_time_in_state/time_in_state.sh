#!/usr/bin/bash
DEBUG=0
function cal_time_in_state {
	local time=2
	local cpu_dir="/sdcard/result/cpu/"
	if [ -e $cpu_dir ] ; then
		rm -rf $cpu_dir
	else
		mkdir $cpu_dir
	fi
	mkdir $cpu_dir

	cat /sys/devices/system/cpu/${1}/cpufreq/stats/time_in_state > ${cpu_dir}${1}_time_state_start.txt
	sleep ${time}
	cat /sys/devices/system/cpu/${1}/cpufreq/stats/time_in_state > ${cpu_dir}${1}_time_state_end.txt
	busybox awk 'NR==FNR{a[i]=$2; i++} 
				NR>FNR{sum+=$2-a[j]; b[j]=$2-a[j]; j++;}
				END{printf("module0_state: "); while (k<j){printf("%-8.2f", b[k]/sum);k++}; printf("\n")}
				' ${cpu_dir}${1}_time_state_start.txt ${cpu_dir}${1}_time_state_end.txt
	if [ "$DEBUG" = "0" ] ; then
		rm ${cpu_dir}${1}_time_state_start.txt ${cpu_dir}${1}_time_state_end.txt
	fi
}

cal_time_in_state cpu0
cal_time_in_state cpu4






   