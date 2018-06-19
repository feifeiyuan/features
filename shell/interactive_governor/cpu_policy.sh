#!/usr/bin/bash
# interactive(互动)是目前最常用的调速器, CPU 频率是根据负载实现交互式反应,
# ondemand(一经请求) 
# conserative(保守)
# interative拥有一些不同的参数配置方式。
# 所以并不像一些人说的，什么 ondemand 非常敏感升频降频都快、interative升频快降频慢、conserative升频慢降频快

#实际上每个机制后面都会有一个时间
#--------------负载 - 频率机制--------------
#        target_loads:负载；这个参数的目的是根据 CPU 负载来调整频率：
#                     当 CPU 负载升高到该参数时，内核就会升高 CPU 的运行频率以便降低 CPU 负载。该参数的默认值为 90。
#                     该参数的格式是单个固定数值，或者是频率和负载值成对出现用冒号隔开。
#                     比如 85 1000000:90 1700000:99 表示负载在 85% 以下时，CPU 频率要运行在 1GHz 以下；
#                     负载达到 90% 时，CPU 频率要运行在 1.0GHz~1.7GHz，直到 CPU 负载达到 99% 时，频率才会升到 1.7GHz 以上。
#                     一般地，该参数设置的越低，CPU 升频就会越快、越频繁。
#					  90 1000000:95 表示在90以下运行在1G以下频点，90～95运行在1G的频点
#     min_sample_time:CPU 开始降低频率前的最小时间
#                     也就是当负载下降到达较低的区间时还需要再经过多少时间
#                     CPU 才开始降频（也可以简单理解成 CPU 两次降频之间的间隔值）。该值越小，对降频的反应就越敏感。该参数的默认值是 80000uS.
#
#		   负载采样率,timer_rate+timer_slack: 受限于 min_sample_time above_hispeed_delay 等延迟参数
#		   调节这个参数一般不能使手机省电。比如将该值设置的较长，固然可以降低内核探看 CPU 负载的频率，
#		   节省电量，但是内核就不能及时感知到负载的下降而及时降频。这个参数的设置取决于你手机对于长线程进程的需求。
#          timer_rate:计算workload的采样频率，是不在idle状态的采样周期
#         timer_slack:当进入idle状态之后，需要一个延时计时器，会导致CPU不能从idel状态苏醒来响应定时器. 
#                     定时器的最大的可延时时间用timer_slack表示，默认值80000uS,timer_clack 值为 -1 时则无上限
#                     question:1、是不是只要取了-1就进入idle之后就无法退出idle了 2、为什么timer_rate只针对p_state状态
#                     answer: 退出idle实际上内核中断产生，不受timer_slack的控制  2、因为idle状态是无负载的

#--------------应急反应机制--------------
#     go_hispeed_load:高负载阈值
#                     这个参数就是规定当 CPU 负载突然到达该值时且当前 CPU 处于闲置状态（离线亦或者是运行在较低频率），
#                     CPU 就会瞬间将频率升到 hispeed_freq 以便应对突发状况。该参数的默认值是 99
#                     一般地，这个值越低，对于突发的大负荷工作，CPU 的反应就会越敏感。
#                     如果该值设置的太高则比较容易面对一些突发负载无法立刻升频做出反应引起卡顿，设置的太低则会出现过于频繁的升频导致发热和耗电。
#        hispeed_freq:当负载突然升高到 go_hispeed_load 且该核心正在运行在较低的频率时，CPU 运行频率会瞬间升高到这个参数指定的频率。
#                     这种情况一般都发生在暂时处于闲置的核心上。hispeed_freq 是一个理想的高性能工作频率, 
#                     假定该频率足以应对大多数高负载工作。当 CPU 运行在这个频率超过一段时间（即 above_hispeed_delay），CPU 才会继续升高频率
# above_hispeed_delay:这个参数是设置成当 CPU 频率运行在 hispeed_freq 甚至更高以后，突发的负载并没有立刻降低。当 CPU 运行在该频率的时间超过该参数的指定值后，
#                     CPU 就会进一步进行升频，以便应对大长线程的大负荷。该参数的默认值是 20000us。设置该参数时可以根据 CPU 所在的不同频率设置不同的延迟。
#                     该参数的格式是单个固定数值，或者是频率和频率区间成对出现用冒号隔开。当参数中涉及到频率时，频率必须采用升序数列。
#                     比如 19000 1400000:39000 1700000:19000 表示当 CPU 频率在 1.4GHz 以下时 CPU 要在 go_hispeed_load 甚至更高的负载下运行超过 19000 us 才会进一步升频；
#                     运行频率在 1.4Ghz~1.7GHz 时延时设置为 39000us；超过 1.7GHz 时则采用 19000us 。
#                     当该参数设置得较小时 CPU 则会较快地升频（可能会引起卡顿），设置得较大时 CPU 可能会因为不能及时升频而引起卡顿。
#                     和hispeed_freq相关

#--------------鸡血模式--------------
#               boost:由内核写入。如果非零，立即提高所有 CPU 的频率到该 CPU 的 hispeed_freq 甚至更高，
#                     直到零被写入此属性，期间无论负载是否降低都不会引起频率的改变。
#                     如果为零，则会允许 CPU 频率根据负载而降低到低于 hispeed_freq 设定的频率。默认值为零
#                     也就是优先级别最高
# boostpulse_duration:指在每次 boost 被写入后，CPU 的频率被提升到 hispeed_freq 后在该频率下运行的最低间隔。
#                     在该间隔之内，就算 boost 值重新设为零，CPU 仍不会降频

#--------------其他--------------
#          io_is_busy:这个参数决定是否根据设备的存储有关的 I/O （包括数据在存储上的读写、数据库的修改、熵的增加等）而提升 CPU 频率，以便加快 I/O 性能



function del_file {
	if [ -e $1 ] ; then
		rm $1
	fi
}

function determine_exit {
	if [ ! -e $1 ] ; then
		echo "we do not find correct node: "$1
		show_exit_note
	fi
}

function determine_echo {
	if [ ! -e $1 ] ; then
		echo "we do not find correct node: "$1
	else
		local data=`cat $1`
		echo ${data}
	fi
}

function show_policy {
	local temp=`determine_echo ${1}${2}`
	echo ${2} ${temp} | busybox awk '{printf("%20s: %s\n"), $1, $2}'
}

function cpu_policy()
{
	echo "**************************  CPU  *****************************" 
	echo "DVFS parameters:" 
	local i=0
	for i in `ls -d /sys/devices/system/cpu/cpufreq/policy*/interactive/` ; do
		echo ${i} | grep -o -E "policy."
		show_policy ${i} "above_hispeed_delay"
		show_policy ${i} "boost"
		show_policy ${i} "boostpulse_duration"
		show_policy ${i} "go_hispeed_load"
		show_policy ${i} "hispeed_freq"
		show_policy ${i} "io_is_busy"
		show_policy ${i} "min_sample_time"
		show_policy ${i} "target_loads"
		show_policy ${i} "timer_rate"
		show_policy ${i} "timer_slack"
		echo ""
	done
}

cpu_policy

#policy0
#               boost: 0
# boostpulse_duration: 500000
#     go_hispeed_load: 99
#        hispeed_freq: 1560000
#          io_is_busy: 1
#     min_sample_time: 20000
#        target_loads: 90
#          timer_rate: 20000
#         timer_slack: -1
# above_hispeed_delay: 40000
#
#policy4
#               boost: 0
# boostpulse_duration: 80000
#     go_hispeed_load: 99		(默认90)
#        hispeed_freq: 2028000
#          io_is_busy: 0
#     min_sample_time: 80000	(默认80000us,该值越小，变频越快)
#        target_loads: 90		(默认90)
#          timer_rate: 20000
#         timer_slack: 80000
# above_hispeed_delay: 20000	(默认20000us)
