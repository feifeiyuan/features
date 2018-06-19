#!/bin/sh
#
# Description: get the gpu loading on different platforms 
#

gpu_times=$1
counts=0
gpu_path=""

function set_default_times {
	if [ "${gpu_times}" = "" -o "${gpu_times}" = "0" ] ; then
		gpu_times=600
	fi
}

function show_exit_note {
	echo "if you have any questions Please read readme(In the same directory) Priority!!!"
	echo "exit ...."
	exit	
}

function determine_node_exist {
	if [ ! -e "$1" ] ; then
		echo "we do not find node: ""$1"
		show_exit_note
	fi
}

function enter_diff_dir {
	if [ -d "/sys/class/kgsl/kgsl-3d0/" ] ; then
		gpu_path="/sys/class/kgsl/kgsl-3d0/"
	else
		echo "we do not any node about gpu at this plat"
		show_exit_note
	fi
}

function print_table_dic {
	local key=0;
	for key in ${!freq_utilisation_dic[@]} ; do
		echo $key" "${freq_utilisation_dic[$key]}
	done
}

function print_utilisation_dic {
	local key=0;
	for key in ${!freq_table_dic[@]} ; do
		echo $key" "${freq_table_dic[$key]}
	done
}

function get_freq_table {
	determine_node_exist ${gpu_path}"gpu_available_frequencies"
	freq_table_arr=(`cat ${gpu_path}gpu_available_frequencies`)
	local key=0
	for key in ${freq_table_arr[@]} ; do
		freq_table_dic[$key]=0
		freq_utilisation_dic[$key]=0
	done
	#print_table_dic
}

function get_poling_interval {
	local polling_interval=0
	determine_node_exist ${gpu_path}"devfreq/polling_interval"
	polling_interval=`cat ${gpu_path}"devfreq/polling_interval"`
	echo ${polling_interval}
}

function cal_gpu_loading {
	local cur_freq=0
	local key=0
	local gpu_cur_until=0
	local polling_interval=`get_poling_interval | busybox awk '{printf("%f", $1/1000)}'`
	while [ $counts -lt $gpu_times ] ; do
		let counts++
		gpu_cur_until=`cal_utilisation`
		
		determine_node_exist ${gpu_path}"gpuclk"
		cur_freq=`cat ${gpu_path}gpuclk`
		for key in ${!freq_table_dic[@]} ; do
			if [ $key == $cur_freq ] ; then
				let freq_table_dic[$key]++
				freq_utilisation_dic[$key]=`expr ${freq_utilisation_dic[${key}]} + ${gpu_cur_until}`
			fi
		done
		sleep ${polling_interval}
	done
	#print_table_dic
}

function show_loading {
	local key=0
	local per_key=0;
	local until_total=0
	local polling_interval=`get_poling_interval`
	for key in ${!freq_table_dic[@]} ; do
		echo $key ${freq_table_dic[$key]} | busybox awk '{printf("%d: %d     "),$1, $2}'
	done
	echo "\n"
	
	for per_key in ${!freq_table_dic[@]} ; do
		until_total=`expr $until_total + ${freq_utilisation_dic[$per_key]}`
		if [ "${freq_table_dic[$per_key]}" != "0" ] ; then
			echo $per_key ${freq_table_dic[$per_key]} ${counts} | busybox awk '{printf("%d = %.2f%%  ", $1, $2*100/$3)}'
			echo ${freq_utilisation_dic[${per_key}]} ${freq_table_dic[$per_key]} | busybox awk '{printf("GPU Utilisation: %.2f%%\n"), $1/$2}' 
		fi
	done
	echo ""
	
	echo -e "polling_interval:$polling_interval" 
	busybox awk 'BEGIN{ printf( "Test Times: '$gpu_times'   GPU Utilisation: %.2f%\n",('$until_total')/('$gpu_times') ) }'
}

function cal_utilisation {
	
	local gpu_cur_until=0
	determine_node_exist ${gpu_path}"gpu_busy_percentage"
	gpu_cur_until=`cat ${gpu_path}"gpu_busy_percentage"`
	gpu_cur_until=${gpu_cur_until:%(%*)}
	echo ${gpu_cur_until}
}

set_default_times

enter_diff_dir

get_freq_table

cal_gpu_loading

show_loading
