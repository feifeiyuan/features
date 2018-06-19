#!/usr/bin/bash

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

function show_brightness {
	# shell脚本中执行死循环
	while (( 1 ))
	do
		# sleep 1是以1秒为单位，可以设置0.5s
		sleep 1
		determine_exit "/sys/class/backlight/sprd_backlight/brightness"
		local brightness=`cat /sys/class/backlight/sprd_backlight/brightness`
		echo "the brightness : "$brightness
	done
}

show_brightness
   