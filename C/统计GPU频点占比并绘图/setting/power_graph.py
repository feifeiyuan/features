#!/usr/bin/python
# coding=utf-8

from collections import OrderedDict
import xlsxwriter
import os

class deal_data_to_chart(object):
	def __init__(self,result_book):
		self.result_book = result_book

	def make_chart(self,dic_data,file_name,dic_chart,min_freq, max_freq):
		sheet1 = self.result_book.add_worksheet ( dic_chart["sheet_name"])
		chart_col = self.result_book.add_chart ({'type':'scatter','subtype': 'straight'})
		list_colums_key = []
		list_colums_value = []
		sheet1.write ( "A1", dic_chart["x_axis"] )
		sheet1.write ( "B1", dic_chart["y_axis"] )
		sheet1.write ("D1", file_name )
		for x in dic_data.keys():
			list_colums_key.append (x/10)
			list_colums_value.append (dic_data[x])
		sheet1.write_column ( "A2", list_colums_key )
		sheet1.write_column ( "B2", list_colums_value )
		chart_col.add_series ( {
			"name": dic_chart["sheet_name"],
			'categories': '='+dic_chart["sheet_name"]+'!$A$2:$A$'+str(len(list_colums_key)),
			'values': '='+dic_chart["sheet_name"]+'!$B$2:$'  + "B$" + str (len ( list_colums_value )),
			'line': {'color': "#1874CD",'width': 1.4},
		} )
		chart_col.set_title ( {'name': dic_chart["title"]} )
		chart_col.set_x_axis ( {'name':dic_chart["x_axis"]} )
		chart_col.set_y_axis ( {'name': dic_chart["y_axis"],'min':min_freq, 'max':max_freq+50000000,'interval_unit': 5})
		sheet1.insert_chart ( "D3", chart_col,{'x_offset':0, 'y_offset': 0,'x_scale': 1.5, 'y_scale': 1.5})


if __name__ == '__main__':
	file_dir = os.path.abspath(os.getcwd())
	ls = os.listdir(file_dir+"\datas")
	dic_power = OrderedDict ()
	result_path = file_dir + "\\result.xlsx"
	result_book = xlsxwriter.Workbook ( result_path )
	deal_data_to_chart_obj = deal_data_to_chart ( result_book )
	i=0
	j=0
	min_freq = 0
	max_freq = 0
	avalid_freq = ""
	for file in ls:
		file_total = file_dir+"\datas\\"+file
		if(file[-4:] == ".txt"):
			index = 0
			dic_power[file]={}
			i = i + 1
			for line in open ( file_total ):
				if(index == 0):
					avalid_freq = line
					for item in line.split ( ' '):
						if(j==0):
							min_freq = int(item)
							max_freq = int(item)
						else:
							min_freq = min(min_freq,int(item))
							max_freq = max(max_freq,int(item))
						j  = j+1
					index = index + 1
					continue
				dic_power[file][index-1] = int(line)
				index= index+1
			dic_power_cap_chart = {"sheet_name": file.replace(".txt",""), "x_axis": "Time(s)",
								   "y_axis": "freq(Khz)","title":"GPU_Proportion"}
			deal_data_to_chart_obj.make_chart ( dic_power[file],avalid_freq ,dic_power_cap_chart, min_freq, max_freq)

	result_book.close ()













