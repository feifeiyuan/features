#!/usr/bin/bash

function show_exit_note {
	echo "if you have any questions Please read readme(In the same directory) Priority!!!"
	echo "exit ...."
	exit
}

function determine_exit () {
	if [ ! -e $1 ] ; then
		echo "we do not find correct node: "$1
		show_exit_note
	fi
}

function switch_governor {
	determine_exit "/sys/devices/system/cpu/$1/cpufreq/scaling_governor"
	local governor_state=`cat /sys/devices/system/cpu/$1/cpufreq/scaling_governor`
	if [ "$governor_state" != "$2" ] ; then
		echo $2 > /sys/devices/system/cpu/$1/cpufreq/scaling_governor
	fi
}

function get_freq_ava {
	determine_exit "/sys/devices/system/cpu/$1/cpufreq/scaling_available_frequencies"
	local cpu_freq_aval=`cat /sys/devices/system/cpu/$1/cpufreq/scaling_available_frequencies`
	cpu_freq_aval_arr=(${cpu_freq_aval})
}

#cpu固定在一个频点，然后开始读取他的电压值
function cal_vdd {
	get_freq_ava $1
	switch_governor $1 "userspace"
	local key=0
	local vdd=0
	
	for key in ${cpu_freq_aval_arr[@]}
	do
		determine_exit "/sys/devices/system/cpu/$1/cpufreq/scaling_setspeed"
		echo ${key} > /sys/devices/system/cpu/$1/cpufreq/scaling_setspeed
		vdd=`cat $2`	
		echo ${key} | busybox awk '{printf("%10d\t", $1)}'
		echo ${vdd} | busybox awk '{printf("%10d\t", $1)}' 
		echo ""
	done
	#interactive即上层策略开始调节
	switch_governor $1 "interactive"
}

#cpuidle实际上分为很多个档位，对应的是state0、state1、state2。。。
function cpuidle_state {
	local state=0
	determine_exit "/sys/devices/system/cpu/cpu${1}/cpuidle/state0"
	state=`ls -d /sys/devices/system/cpu/cpu${1}/cpuidle/state*`
	echo ${state}
}

function look_freq_vdd {
	# 不同的平台具有不同的节点
	# 实际上这个是需要spec来定的
	local vdd_node=0
	if [ "$1" = "0" ] ; then
		if [ -e "/sys/kernel/debug/sprd-regulator/vddarm0/voltage" ] ; then
			vdd_node="/sys/kernel/debug/sprd-regulator/vddarm0/voltage"
		elif [ -e "/sys/kernel/debug/sprd-regulator/vddcpu/voltage" ] ; then
			vdd_node="/sys/kernel/debug/sprd-regulator/vddcpu/voltage"
		elif [ -e "/sys/kernel/debug/sprd-regulator/vddcore/voltage" ] ; then
			vdd_node="/sys/kernel/debug/sprd-regulator/vddcore/voltage"
		fi
	elif [ "$1" = "4" ] ; then
		if [ -e "/sys/kernel/debug/sprd-regulator/vddarm1/voltage" ] ; then
			vdd_node="/sys/kernel/debug/sprd-regulator/vddarm1/voltage"
		elif [ -e "/sys/kernel/debug/sprd-regulator/vddcpu/voltage" ] ; then
			vdd_node="/sys/kernel/debug/sprd-regulator/vddcpu/voltage"
		fi
	fi

	cal_vdd cpu${1} ${vdd_node}
}

function restore_cpu_idle {
	local i=0
	local len=$2
	if [ "$1" == "0" ] ; then
		while [ "$i" -lt "$len" ] ; do
			echo ${state_disable_cpu0_arr[$i]} > ${state_arr_cpu0[$i]}/disable
			let i++
		done
	elif [ "$1" == "4" ] ; then
		while [ "$i" -lt "$len" ] ; do
			echo ${state_disable_cpu4_arr[$i]} > ${state_arr_cpu4[$i]}/disable
			let i++
		done
	fi
}

function operate_idle_state {
	local cpu_core_num=0
	local state=0
	local arr_len=0

	while [ -d "/sys/devices/system/cpu/cpu"${cpu_core_num} ] ; do
		if [ "$cpu_core_num" = "0" ] ; then
			#record cpuidle state
			state=`cpuidle_state $cpu_core_num`
			state_arr_cpu0=(${state})
			arr_len=${#state_arr_cpu0[*]}
			local i=0
			#设置电压和频率时，需要先关闭cpuidle，但是由于进入idle有延时(猜想)所以修改并不一定立刻生效
			while [ "$i" -lt "${arr_len}" ] ; do
				state_disable_cpu0_arr[$i]=`cat ${state_arr_cpu0[$i]}"/disable"`
				if [ "${state_disable_cpu0_arr[$i]}" = "0" ] ; then
					echo 1 > ${state_arr_cpu0[$i]}/disable
				fi
				let i++
			done
			
			#look cpufreq and vdd
			look_freq_vdd ${cpu_core_num}
			
			#restore cpu_idle
			#之后需要恢复idle的状态
			restore_cpu_idle ${cpu_core_num} ${arr_len}
			
		elif [ "$cpu_core_num" = "4" ] ; then
		
			#record cpuidle state
			state=`cpuidle_state $cpu_core_num`
			state_arr_cpu4=(${state})
			arr_len=${#state_arr_cpu4[*]}
			local i=0
			while [ "$i" -lt "${arr_len}" ] ; do
				state_disable_cpu4_arr[$i]=`cat ${state_arr_cpu4[$i]}"/disable"`
				if [ "${state_disable_cpu4_arr[$i]}" = "0" ] ; then
					echo 1 > ${state_arr_cpu4[$i]}/disable
				fi
				let i++
			done
			# look cpufreq and vdd
			look_freq_vdd ${cpu_core_num}
			#restore cpu_idle
			restore_cpu_idle ${cpu_core_num} ${arr_len}
		fi
		let cpu_core_num++
	done
}

operate_idle_state





