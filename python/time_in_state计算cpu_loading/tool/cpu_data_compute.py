#!/usr/bin/python
# coding=utf-8

from collections import OrderedDict

import os
import sys

file_dir = sys.argv[1]

def open_file_get_time_data(file_name, dic_data,order_dic_freq={}):
	dic_data["lit"] = OrderedDict()
	dic_data["big"] = OrderedDict()
	try:
		with open ( file_name, "r", encoding="UTF-8", errors='ignore' ) as file_obj:
			while True:
				content = file_obj.readline ()
				if not content:
					break
				cpu_id=content[0:content.index ( " " )]
				#print(cpu_id)
				cpu_freq = content.replace(cpu_id+" ","")[0:content.replace(cpu_id+" ","").index(" ")]
				order_dic_freq[cpu_id+cpu_freq] = cpu_id
				#print(cpu_freq)
				cpu_jeffies=content.replace(cpu_id+" ","").replace(cpu_freq+" ","").replace("\n","")
				#print(cpu_jeffies)
				dic_data[cpu_id][cpu_freq] = int(cpu_jeffies)
	except:
		print ( "there is some wrong during open" + file_name )

def open_file_get_idle_data(file_name, dic_data):
	dic_data["lit"] = OrderedDict()
	dic_data["big"] = OrderedDict()
	try:
		with open ( file_name, "r", encoding="UTF-8", errors='ignore' ) as file_obj:
				while True:
					content = file_obj.readline ()
					if not content:
						break
					cluster = content[0:content.index(" ")]
					#print(cluster)
					cpu_id = content.replace(cluster+" ","")[0:content.replace(cluster+" ","").index(" ")]

					#print(cpu_id)
					idle_desc = content.replace(cluster+" ","").replace(cpu_id+" ","")[0:content.replace(cluster+" ","").replace(cpu_id+" ","").index(" ")]
					#print(idle_desc)
					idle_jeffies = content.replace(cluster+" ","").replace(cpu_id+" ","").replace(idle_desc+" ","").replace("\n","")
					#print(idle_jeffies)
					if (cpu_id not in dic_data[cluster].keys ()):
						dic_data[cluster][cpu_id] = int(idle_jeffies)
					else:
						dic_data[cluster][cpu_id] =dic_data[cluster][cpu_id] +int(idle_jeffies)
	except:
		print ( "there is some wrong during open " + file_name )

if __name__ == '__main__':
	cpu0_freq_time_start=file_dir+"cpu_freq_time_start.txt"
	cpu0_freq_time_end=file_dir+"cpu_freq_time_end.txt"
	order_dic_cpu_time_start = OrderedDict()
	order_dic_cpu_time_end = OrderedDict ()
	order_dic_cpu_time_minus = OrderedDict()
	order_dic_cpu_time_minus["lit"] = OrderedDict()
	order_dic_cpu_time_minus["big"] = OrderedDict()
	cpu_online_offline = file_dir+"cpu_online_offline.txt"
	order_dic_cpu_online_offiline = OrderedDict()
	open_file_get_time_data ( cpu_online_offline, order_dic_cpu_online_offiline)
	open_file_get_time_data ( cpu0_freq_time_end, order_dic_cpu_time_end )
	open_file_get_time_data ( cpu0_freq_time_start, order_dic_cpu_time_start )



	#debug
	"""
	for cluster in order_dic_cpu_time_start:
		print(cluster)
		for cpu_id in order_dic_cpu_time_start[cluster]:
			print(cpu_id)
			print(order_dic_cpu_time_start[cluster][cpu_id])

	print()
	for cluster in order_dic_cpu_time_end:
		print ( cluster )
		for cpu_id in order_dic_cpu_time_end[cluster]:
			print ( cpu_id )
			print ( order_dic_cpu_time_end[cluster][cpu_id] )
	"""

	total_time = {"lit":0, "big":0}
	for cluster_start, cluster_end in zip(order_dic_cpu_time_start, order_dic_cpu_time_end):
		if(cluster_start==cluster_end):
			for cpuid_start, cpuid_end in zip(order_dic_cpu_time_start[cluster_start],order_dic_cpu_time_end[cluster_end]):
				#print(cpuid_start)
				#print(cpuid_end)
				if(cpuid_start==cpuid_end):
					#print(order_dic_cpu_time_start[cluster_start][cpuid_start])
					#print(order_dic_cpu_time_end[cluster_end][cpuid_end])
					order_dic_cpu_time_minus[cluster_end][cpuid_end] = -order_dic_cpu_time_start[cluster_start][cpuid_start]+\
					order_dic_cpu_time_end[cluster_end][cpuid_end]
					total_time[cluster_end] = total_time[cluster_end]+order_dic_cpu_time_minus[cluster_end][cpuid_end]
					#print(order_dic_cpu_time_minus[cluster_end][cpuid_end])


	#debug
	"""
	for key in total_time:
		print(key)
		print(total_time[key])
	"""


	for cluster_start, cluster_end in zip(order_dic_cpu_time_start, order_dic_cpu_time_end):
		if(cluster_start==cluster_end):
			for cpuid_start, cpuid_end in zip(order_dic_cpu_time_start[cluster_start],order_dic_cpu_time_end[cluster_end]):
				if(cpuid_start==cpuid_end):
					order_dic_cpu_time_minus[cluster_end][cpuid_end] = order_dic_cpu_time_minus[cluster_end][cpuid_end]/total_time[cluster_end]
					#print(order_dic_cpu_time_minus[cluster_end][cpuid_end])


	#debug
	"""
	for cluster in order_dic_cpu_online_offiline:
		print ( cluster )
		for cpu_id in order_dic_cpu_online_offiline[cluster]:
			print ( cpu_id )
			print ( order_dic_cpu_online_offiline[cluster][cpu_id] )
	"""

	cpu0_idle_time_start=file_dir+"cpu_idle_time_start.txt"
	cpu0_idle_time_end=file_dir+"cpu_idle_time_end.txt"
	order_dic_cpu_idle_start = OrderedDict()
	order_dic_cpu_time_end = OrderedDict()
	order_dic_cpu_idle_end_detal = OrderedDict ()
	order_dic_cpu_idle_minus = OrderedDict()
	order_dic_cpu_idle_minus["lit"]=OrderedDict()
	order_dic_cpu_idle_minus["big"]=OrderedDict()
	open_file_get_idle_data(cpu0_idle_time_start,order_dic_cpu_idle_start)
	open_file_get_idle_data ( cpu0_idle_time_end, order_dic_cpu_time_end )


	#debug
	"""
	for cluster in order_dic_cpu_idle_start:
		print(cluster)
		for cpu_id in order_dic_cpu_idle_start[cluster]:
			print(cpu_id)
			print(order_dic_cpu_idle_start[cluster][cpu_id])
	for cluster in order_dic_cpu_time_end:
		print(cluster)
		for cpu_id in order_dic_cpu_time_end[cluster]:
			print(cpu_id)
			print(order_dic_cpu_time_end[cluster][cpu_id])
	"""

	for start_idle_cluster, end_idle_cluster in zip(order_dic_cpu_idle_start,order_dic_cpu_time_end ):
		#print(start_idle_cluster)
		#print(end_idle_cluster)
		if(start_idle_cluster==end_idle_cluster):
			for start_idle_id , end_idle_id in \
					zip(order_dic_cpu_idle_start[start_idle_cluster],order_dic_cpu_time_end[end_idle_cluster]):
				if(start_idle_id==end_idle_id):
					#print ( start_idle_id )
					#print ( end_idle_id )
					order_dic_cpu_idle_minus[start_idle_cluster][start_idle_id]= \
						order_dic_cpu_time_end[end_idle_cluster][start_idle_id]\
						-order_dic_cpu_idle_start[start_idle_cluster][start_idle_id]
					if(order_dic_cpu_idle_minus[start_idle_cluster][start_idle_id]>=(total_time[start_idle_cluster]*10000)):
						order_dic_cpu_idle_minus[start_idle_cluster][start_idle_id] = 0
					else:
						order_dic_cpu_idle_minus[start_idle_cluster][start_idle_id] = \
							total_time[start_idle_cluster]*10000 - \
							order_dic_cpu_idle_minus[start_idle_cluster][start_idle_id]
					order_dic_cpu_idle_minus[start_idle_cluster][start_idle_id] = \
						order_dic_cpu_idle_minus[start_idle_cluster][start_idle_id]/(total_time[start_idle_cluster]*10000)
					#print(order_dic_cpu_idle_minus[start_idle_cluster][start_idle_id])


	cluster_total = 0.0
	sum_total = 0.0
	for cluster in order_dic_cpu_time_minus:
		cluster_total = 0.0
		if(len(order_dic_cpu_time_minus[cluster])!=0):
			print ( "%-10s" % cluster, end="" )
			for cpu_freq in order_dic_cpu_time_minus[cluster]:
				print("%10s%s"%(cpu_freq," "),end="")
			print ( "%10s%s%10s%s" % ("online"," ", "offline"," ") )
			for cpu_idle_id in order_dic_cpu_idle_minus[cluster]:
				print("%-10s"%cpu_idle_id,end="")
				cpu_online = 0
				for cpu_freq in order_dic_cpu_time_minus[cluster]:
					if (order_dic_cpu_online_offiline[cluster][cpu_idle_id]==1):
						active_percent = order_dic_cpu_time_minus[cluster][cpu_freq]* \
									 order_dic_cpu_idle_minus[cluster][cpu_idle_id]*100
						cluster_total = cluster_total+(float(active_percent)*(float)(cpu_freq)/100)
						print("%10.2f%s"%(active_percent,"%"),end="")
						cpu_online = cpu_online+active_percent
					else:
						print("%10s%s"%("unpluged"," "),end="")
				if (order_dic_cpu_online_offiline[cluster][cpu_idle_id]==1):
					print("%10.2f%s%10.2f%s"%(cpu_online,"%",(100-cpu_online),"%"))
				else:
					print ( "%10s%s%10s%s" % ("unpluged", " ","unpluged"," "))
			print("%-10s"%"total",end="")
			sum_total = sum_total + cluster_total
			for cpu_freq in order_dic_cpu_time_minus[cluster]:
				print("%10.2f%s"%(order_dic_cpu_time_minus[cluster][cpu_freq]*100,"%"),end="")
		print()
		print("%-10s%.2f"%("SUM",cluster_total))
		print ( "\n" )
	print("%-10s%.2f"%("TOTAL",sum_total))










