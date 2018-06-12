#!/usr/bin/python
# coding=utf-8

from collections import OrderedDict

import os
import sys

file_path = sys.argv[1]

def open_file_get_time_data(file_name, dic_data,order_dic_freq={}):

	lit_freq = [768000, 884000, 1000000, 1100000, 1200000]
	big_freq = [768000, 1050000, 1225000, 1400000, 1500000, 1570000]
	try:
		with open ( file_name, "r", encoding="UTF-8", errors='ignore' ) as file_obj:
			lines = file_obj.readlines()
			for content in lines:
				if(content.split()):
					list = content.split()
					#print(list)
					if(list[0]=="cpu0" or list[0]=="cpu1" or\
					   list[0]=="cpu2" or list[0]=="cpu3"):
						i = 1
						for freq in lit_freq:
							dic_data["lit_"+list[0]][freq] = float(list[i].replace("%",""))
							#print(dic_data["lit_"+list[0]][freq])
							i = i+1
					if (list[0] == "cpu4" or list[0] == "cpu5" or \
									list[0] == "cpu6" or list[0] == "cpu7"):
					    i = 1
					    for freq in big_freq:
					    	dic_data["big_"+list[0]][freq] = float(list[i].replace("%",""))
					    	#print ( dic_data["lit"][freq] )
					    	i = i + 1
	except:
		print ( "there is some wrong during open" + file_name )


if __name__ == '__main__':
	cpu_result = file_path
	order_dic_cpu_result = OrderedDict()

	dic_sum = OrderedDict()
	dic_sum["lit"]=0
	dic_sum["big"]=0

	order_dic_cpu_result["lit_cpu0"] = OrderedDict ()
	order_dic_cpu_result["lit_cpu1"] = OrderedDict ()
	order_dic_cpu_result["lit_cpu2"] = OrderedDict ()
	order_dic_cpu_result["lit_cpu3"] = OrderedDict ()
	order_dic_cpu_result["big_cpu4"] = OrderedDict ()
	order_dic_cpu_result["big_cpu5"] = OrderedDict ()
	order_dic_cpu_result["big_cpu6"] = OrderedDict ()
	order_dic_cpu_result["big_cpu7"] = OrderedDict ()
	open_file_get_time_data ( cpu_result, order_dic_cpu_result)

	total = 0
	for cluster in dic_sum:
		print(cluster)
		print("%-10s"%"SUM",end="")
		for dict_cluster in order_dic_cpu_result:
			if cluster in dict_cluster:
				for freq in order_dic_cpu_result[dict_cluster]:
					dic_sum[cluster] = dic_sum[cluster]+order_dic_cpu_result[dict_cluster][freq]/100*freq
		print(dic_sum[cluster])
		total = total +dic_sum[cluster]
	print("%-10s"%"TOTAL",end="")
	print(total)













