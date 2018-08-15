#!/bin/sh

DEST_DIR=/sdcard/cpu_loading/
test_time=$1

cpu_path="/sys/devices/system/cpu/"

cpu_core_cnt=0

function show_exit_note {
	echo "if you have any questions Please read readme(In the same directory) Priority!!!"
	echo "exit ...."
	exit
}

function determine_exit {
	if [ ! -e $1 ] ; then
		echo "we do not find correct node: "$1
		show_exit_note
	fi
}

function clear_eviroment {
	if [ -e ${DEST_DIR} ] ; then
		rm -rf ${DEST_DIR}
	fi
	mkdir ${DEST_DIR}
}

function set_test_time {
	if [ "$test_time" = "" -o "$test_time" = "0" ] ; then
		test_time=60
	fi
}

function get_cpu_core_cnt {
	# set as gloale value
	cpu_core_cnt=`cat /proc/cpuinfo| grep "processor"| wc -l`
}

function get_cpu_online_or_offline()
{
	local n=0
	while (( n<cpu_core_cnt ))
	do
		if [ -d ${cpu_path}cpu${n}/cpufreq/ ] ; then
			cpu_online_or_offline[$n]=1
		else
			cpu_online_or_offline[$n]=0
		fi
		
		if [ "$n" -lt "4" ] ; then
			echo "lit" "cpu${n} ${cpu_online_or_offline[$n]}" >> ${DEST_DIR}cpu_online_offline.txt
		else
			echo "big" "cpu${n} ${cpu_online_or_offline[$n]}" >> ${DEST_DIR}cpu_online_offline.txt
		fi
		let "n++"
	done
}

function get_cpufreq_time()
{
	while read line ; do
		echo "lit" ${line} >> ${DEST_DIR}cpu_freq_time_${1}.txt
	done < ${cpu_path}cpu0"/cpufreq/stats/time_in_state"
	
	if [ -e "${cpu_path}cpu4/cpufreq/stats/time_in_state" ] ; then
		while read line ; do
			echo "big" ${line} >> ${DEST_DIR}cpu_freq_time_${1}.txt
		done < ${cpu_path}cpu4"/cpufreq/stats/time_in_state"
	fi
}

function get_cpuidle_time()
{
	local tmp=$cpu_path$1"/cpuidle/state*"
	local i=0
	local n

	for n in `ls -d $tmp` ; do
		local CPU_DESC=`cat $n"/desc"`
		local CPU_TIME=`cat $n"/time"`
		echo $3 $1 "${CPU_DESC// /'_'} $CPU_TIME" >> ${DEST_DIR}cpu_idle_time_$2.txt
		let "i++"
	done
}

function get_cpuidle_state()
{
	local i=0
	while (( $i < $cpu_core_cnt )) ; do
		if [ "$i" -lt "4" ] ;  then
			get_cpuidle_time cpu$i $1 "lit"
		else
			get_cpuidle_time cpu$i $1 "big"
		fi
		let "i++"
	done
}

set_test_time
clear_eviroment

echo "start running the script..."
get_cpu_core_cnt
get_cpu_online_or_offline

get_cpufreq_time "start"
get_cpuidle_state "start"

sleep $test_time

get_cpufreq_time "end"
get_cpuidle_state "end"