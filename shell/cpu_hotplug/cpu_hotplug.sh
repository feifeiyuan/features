#!/bin/sh
core_num=0

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

function close_dynamic_hotplug {
	if [ -e "/sys/devices/system/cpu/cpuhotplug/dynamic_load_disable" ] ; then
		echo 1 > /sys/devices/system/cpu/cpuhotplug/dynamic_load_disable
	fi
}

function close_thermal_IPA {
	local zone_num=0
	while [ -d "/sys/devices/virtual/thermal/thermal_zone"${zone_num} ] ; do
		determine_exit "/sys/devices/virtual/thermal/thermal_zone"${zone_num}"/type"
		if [ "`cat "/sys/devices/virtual/thermal/thermal_zone"${zone_num}"/type" | grep "cpu"`" != "" -o "`cat "/sys/devices/virtual/thermal/thermal_zone"${zone_num}"/type" | grep "bia"`" != "" ] ; then
			determine_exit "/sys/devices/virtual/thermal/thermal_zone"${zone_num}"/thm_enable"
			echo 0 > "/sys/devices/virtual/thermal/thermal_zone"${zone_num}"/thm_enable"
			determine_exit "/sys/devices/virtual/thermal/thermal_zone"${zone_num}"/mode"
			echo disabled > "/sys/devices/virtual/thermal/thermal_zone"${zone_num}"/mode"
			break
		fi
		let zone_num++
	done
}

function close_power_hint {
	local power_hint=`getprop | grep  "power.hint"`
	if [ "${power_hint}" == "" ] ; then
		echo "please connect vincent to close power hint"
	else
		# It seems not use
		setprop persist.sys.power.hint 0
	fi
}

function show_core_online {
	determine_exit "/sys/devices/system/cpu/online"
	local online_core=`cat /sys/devices/system/cpu/online`
	determine_exit "/sys/devices/system/cpu/offline"
	local offline_core=`cat /sys/devices/system/cpu/offline`
	echo "online_core:	"${online_core}
	echo "offline_core:   "${offline_core}
}

function get_input_hotplug {
	echo "please choose 0(off)/1(on)"
	read state
	if  [ "${state}" == "0" -o "${state}" == "1" ] ; then
		return
	else
		echo "please input legal data 0/1"
		echo "0: Pull out cpu core"
		echo "1: Pull in cpu core"
		exit
	fi
}

function get_core_num {
	core_num=0
	while [ -d "/sys/devices/system/cpu/cpu"$core_num ] ; do
		let core_num++
	done
}

function get_input_core {
	let core_num--
	echo "please choose operate core 0~"$core_num
	read N
	if [ "${N}" -lt "0" -o "${N}" -gt "${core_num}" ] ; then
		echo "please input legal data 0~"${core_num}
		exit
	fi
}

function hotplug_cpu {
	show_core_online

	get_input_hotplug

	get_core_num

	get_input_core
	
	core_num=0
	while [ -d "/sys/devices/system/cpu/cpu"${core_num} ] ; do
		if [ "$core_num" = "${N}" ] ; then
			determine_exit "/sys/devices/system/cpu/cpu"${N}"/online"
			echo ${state} > "/sys/devices/system/cpu/cpu"${N}"/online"
		fi
		let core_num++
	done
	
	show_core_online
}

close_dynamic_hotplug

close_thermal_IPA

#close_power_hint

hotplug_cpu




