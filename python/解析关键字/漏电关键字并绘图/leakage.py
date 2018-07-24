#!/usr/bin/python
# coding=utf-8
import datetime
import re
from collections import OrderedDict
import sys
import xlsxwriter
import os

dic_cpu_temp = OrderedDict ()
dic_cpu_temp_total = OrderedDict ()
dic_power_cap = OrderedDict ()
dic_power_current = OrderedDict ()
dic_sprdbat_temp_monitor = OrderedDict ()
dic_cpu_cluster = OrderedDict ()
dic_cpu_freq_total = OrderedDict ()
dic_backlight_level = OrderedDict ()
dic_board_temp_level = OrderedDict ()

cpu_temp = 'cpu\d* temp:\d*' ;  # for cpux temp
dic_power_cap["power_cap"] = []
dic_power_current["power_current"] = []
dic_sprdbat_temp_monitor["chg_cur"] = []
dic_backlight_level["backlight_level"] = []
dic_board_temp_level["board_temp"] = []

DEBUG = 1

class debug_leakage:
	def __init__(self,obj):
		self.obj = obj
		if(DEBUG == 1):
			if(type(self.obj) is list):
				for x in self.obj:
					print(x)

			if(type(self.obj) is OrderedDict):
				for x in  self.obj:
					print(x)
					print(self.obj[x])

class deal_data_to_chart(object):
	def __init__(self,result_book):
		self.result_book = result_book

	def cpu_filter(self,dic_cpu,dic_cpu_total,parttern):
		for key in dic_cpu_total:
			cpu_n = ''.join ( re.findall ( "cpu\d*", dic_cpu_total[key] ) )
			if (cpu_n in dic_cpu.keys ()):
				cpu_value = ''.join ( re.findall ( parttern+"\d*", dic_cpu_total[key] ) ).replace ( parttern, "" )
				target_key = ''.join ( re.findall ( "\d{2}-\d{2} \d{2}:\d{2}:\d{2}.\d{3}", key ) )
				dic_cpu[cpu_n].append ( {target_key: cpu_value} )

	def make_chart(self,dic_data,dic_chart):
		sheet1 = self.result_book.add_worksheet ( dic_chart["sheet_name"])
		list_colum_line = [chr ( i ).upper () + '2' for i in range ( 97, 123 )]
		list_colum_color = ['red', 'yellow', 'green', 'blue', 'black']
		i = 0
		x_scale = 1
		chart_col = self.result_book.add_chart ({'type':'line', 'substyle':'stacked'})
		for x in dic_data:
			list_colums_key = []
			list_colums_value = []
			if((len (dic_data[x])+1)//14!=0):
				x_scale =(len (dic_data[x])+1)//14
				if(x_scale >3):
					x_scale = 3
			print ( len (dic_data[x]) )
			print(x_scale)
			for y in dic_data[x]:
				list_colums_key.append ( ''.join ( list ( y.keys () ) ) )
				list_colums_value.append ( float ( ''.join ( list ( y.values () ) ) ) )
			sheet1.write ( list_colum_line[i * 2].replace ( "2", "1" ), x )
			sheet1.write_column ( list_colum_line[i * 2], list_colums_key )
			sheet1.write_column ( list_colum_line[i * 2 + 1], list_colums_value )
			index_culom_categories = list_colum_line[i * 2].replace ( '2', '' )
			sheet1.set_column ( index_culom_categories + ':' + index_culom_categories, 20 ) # set culom width
			index_culom_values = list_colum_line[i * 2 + 1].replace ( '2', '' )
			chart_col.add_series ( {
				'name': '='+dic_chart["sheet_name"]+'!$' + index_culom_categories + '$1',
				'categories': '='+dic_chart["sheet_name"]+'!$' + index_culom_categories + '$2:$' + index_culom_categories + "$" + str (
					len ( list_colums_key ) + 1 ),
				'values': '='+dic_chart["sheet_name"]+'!$' + index_culom_values + '$2:$' + index_culom_values + "$" + str (
					len ( list_colums_value ) + 1 ),
				'line': {'color': list_colum_color[i]},
			} )
			i = i + 1
		chart_col.set_title ( {'name': dic_chart["title"]} )
		chart_col.set_x_axis ( {'name':dic_chart["x_axis"]} )
		chart_col.set_y_axis ( {'name': dic_chart["y_axis"]} )
		#chart_col.set_style ( 1 )
		sheet1.insert_chart ( list_colum_line[(i) * 2 + 1], chart_col,{'x_offset':0, 'y_offset': 0,'x_scale': x_scale, 'y_scale': 1})

class read_log():

	def read_kernel_log(self,filepath):
		with open ( filepath, "rt", encoding="UTF-8", errors='ignore' ) as kernel_file:
			while True:
				content = kernel_file.readline () ;
				if not content:
					break ;

				# for power last
				if (content.find ( "sprdbat: vbat:" ) != -1):
					if (content.find ( "cap:" ) != -1):
						power_cap_value = ''.join ( re.findall ( "cap:\d*", content ) ).replace ( "cap:", "" )
						power_cap_key = ''.join ( re.findall ( "\d{2}-\d{2} \d{2}:\d{2}:\d{2}.\d*", content ) )
						dic_power_cap["power_cap"].append ( {power_cap_key: power_cap_value} )
					if (content.find ( "current:" ) != -1):
						power_current_value = ''.join ( re.findall ( "current:-?\d*", content ) ).replace ( "current:",
																											"" )
						power_current_key = ''.join ( re.findall ( "\d{2}-\d{2} \d{2}:\d{2}:\d{2}.\d*", content ) )
						dic_power_current["power_current"].append ( {power_current_key: power_current_value} )
					continue

				# for list_sprdbat_temp_monitor
				if (content.find ( "sprdbat_temp_monitor" ) != -1 and content.find ( "chg_cur=" ) != -1):
					chag_cur_value = ''.join ( re.findall ( "chg_cur=\d*", content ) ).replace ( "chg_cur=", "" )
					chag_cur_key = ''.join ( re.findall ( "\d{2}-\d{2} \d{2}:\d{2}:\d{2}.\d*", content ) )
					dic_sprdbat_temp_monitor["chg_cur"].append ( {chag_cur_key: chag_cur_value} )
					continue

				# for cpu temp
				if (re.findall ( cpu_temp, content )):
					cpu_temp_value = ''.join ( re.findall ( "cpu\d* temp:\d*", content ) )
					dic_cpu_temp_total[content] = cpu_temp_value
					cpu_temp_x = ''.join ( re.findall ( 'cpu\d*', ''.join ( re.findall ( cpu_temp, content ) ) ) )
					dic_cpu_temp[cpu_temp_x] = []

					# for cpu freq
					if (content.find ( "online" ) != -1 and content.find ( "target_freq" ) != -1):
						cpu_freq_value = ''.join ( re.findall ( "cpu\d* temp:\d* target_freq:\d*", content ) )
						dic_cpu_freq_total[content] = cpu_freq_value
						cpu_freq_x = ''.join ( re.findall ( 'cpu\d*', cpu_freq_value ) )
						dic_cpu_cluster[cpu_freq_x] = []
					continue

				# for backlight
				if (content.find ( "set max_brightness to" ) != -1 and content.find ( "backlight:" ) != -1):
					backlight_level_value = ''.join ( re.findall ( "set max_brightness to \d*", content ) ).replace (
						"set max_brightness to", "" )
					backlight_level_key = ''.join ( re.findall ( "\d{2}-\d{2} \d{2}:\d{2}:\d{2}.\d*", content ) )
					dic_backlight_level["backlight_level"].append ( {backlight_level_key: backlight_level_value} )
					continue
		kernel_file.close ()

	def read_main_log(self,main_log_path):
		with open ( main_log_path, "r", encoding="UTF-8", errors='ignore' ) as main_file:
			while True:
				content = main_file.readline ()
				if not content:
					break
				if (re.findall ( "ThermalCvtGovernor: board \w* average temp", content ) or \
					content.find("ThermalCvtGovernor: board-thmzone average temp") != -1):
					if(re.findall ( "\d{3}-\d{2} \d{2}:\d{2}:\d{2}.\d*", content )):
						board_temp_key = ''.join ( re.findall ( "\d{3}-\d{2} \d{2}:\d{2}:\d{2}.\d*", content))
					elif(re.findall ( "\d{2}-\d{2} \d{2}:\d{2}:\d{2}.\d*", content )):
						board_temp_key = ''.join ( re.findall ( "\d{2}-\d{2} \d{2}:\d{2}:\d{2}.\d*", content ) )
					board_temp_value = ''.join ( re.findall ( "average temp: \d*", content ) ).replace (
						"average temp: ", "" )
					dic_board_temp_level["board_temp"].append ( {board_temp_key: board_temp_value} )
		main_file.close ()

class deal_result():
	def __init__(self, result_path):
		self.result_path = result_path
	def write_chart(self):
		result_book = xlsxwriter.Workbook ( result_path )
		deal_data_to_chart_obj = deal_data_to_chart ( result_book )

		dic_power_cap_chart = {"sheet_name": "power_cap", "title": "Power Cap change curve", "x_axis": "Time",
							   "y_axis": "Power"}
		dic_power_current_chart = {"sheet_name": "power_current", "title": "Power Current change curve",
								   "x_axis": "Time", "y_axis": "Power"}
		dic_chg_curr_chart = {"sheet_name": "chg_cur", "title": "Charge Current change curve", "x_axis": "Time",
							  "y_axis": "Power"}
		dic_backlight_level_chart = {"sheet_name": "backlight_level", "title": "Backlight level change curve",
									 "x_axis": "Time", "y_axis": "level"}
		dic_cpu_freq_chart = {"sheet_name": "cpu_freq", "title": "CPU freq change curve", "x_axis": "Time",
							  "y_axis": "KHZ"}
		deal_data_to_chart_obj.cpu_filter ( dic_cpu_cluster, dic_cpu_freq_total, "target_freq:" )
		dic_cpu_temp_chart = {"sheet_name": "cpu_temp", "title": "CPU temp change curve", "x_axis": "Time",
							  "y_axis": "Temp"}
		deal_data_to_chart_obj.cpu_filter ( dic_cpu_temp, dic_cpu_temp_total, "temp:" )
		dic_board_temp_chart = {"sheet_name": "board_temp", "title": "Board temp change curve", "x_axis": "Time",
								"y_axis": "Temp"}

		list_chart = [dic_power_cap_chart, dic_power_current_chart, dic_chg_curr_chart, dic_backlight_level_chart,
					  dic_cpu_freq_chart, dic_cpu_temp_chart, dic_board_temp_chart]
		list_data = [dic_power_cap, dic_power_current, dic_sprdbat_temp_monitor, dic_backlight_level, dic_cpu_cluster,
					 dic_cpu_temp, dic_board_temp_level]
		for dic_chart, dic_data in zip ( list_chart, list_data ):
			deal_data_to_chart_obj.make_chart ( dic_data, dic_chart )

		result_book.close ()


if __name__ == '__main__':
	try:
		kernel_log_path = sys.argv[1]
	except Exception:
		print("please input coorect kernel log path")
		exit()
	try:
		main_log_path = sys.argv[2]
	except Exception:
		print("please input coorect main log path")
		exit()

	file_dir = os.getcwd()
	result_path = file_dir+"\\result.xlsx"

	read_log_obj = read_log()
	read_log_obj.read_kernel_log(kernel_log_path)
	read_log_obj.read_main_log(main_log_path)

	deal_result(result_path).write_chart()










