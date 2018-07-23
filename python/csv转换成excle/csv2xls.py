#!/usr/bin/python
# -*-coding: utf-8 -*-

import csv
import xlrd
import xlwt
import sys, getopt
import matplotlib.pyplot as plt

from openpyxl import Workbook
from xlwt.Style import XFStyle
from xlutils.copy import copy

opts, args = getopt.getopt(sys.argv[1:], "hi:o:")
input_file = ""
output_file = ""

for op, value in opts:
    if op == "-i":
        input_file = value
    elif op == "-o":
        output_file = value
    elif op == "-h":
        print("python csv2graph.py -i xxx.csv -o xxx.xls")
        sys.exit()

print("Just a moment...")        

#csv转换成xls
wb = Workbook()
sheet = wb.active

with open(input_file, 'r') as f:
    f.seek(433, 0)
    for row in csv.reader(f, dialect = 'excel', quoting = csv.QUOTE_NONNUMERIC):
        sheet.append(row)
wb.save(output_file)
print(".csv convert to .xls")

#获取xls数据
data = xlrd.open_workbook(output_file)
sheet1 = data.sheets()[0]

#新建excel并导入数据
new_excel = xlwt.Workbook(encoding = 'utf-8')
new_sheet = new_excel.add_sheet("test")

#设置列宽
for i in range(13):
    style = XFStyle()
    style.alignment
    new_sheet.col(i).width = 256 * 16

#获取所需列数据   
c = 0
for col in range(5, 39, 3):
    cols = sheet1.col_values(col)
    for i in range(sheet1.nrows):#
        if 0 == i:
            continue
        new_sheet.write(i, c, (cols[i]/1024/1024/1024*100))
    c += 1
new_excel.save(output_file)

#求和，填充数据
oldWb = xlrd.open_workbook(output_file, formatting_info = True)
table = oldWb.sheets()[0]

newWb = copy(oldWb)
ws = newWb.get_sheet(0)
sum_aver = 0
sum_count = 0 
for i in range(table.nrows):#
    if 0 == i:
        continue
    ws.write(i, 12, sum(table.row_values(i)))
    if i > 6000:
        continue
    sum_aver +=sum(table.row_values(i))
    sum_count += 1 
try:
	ws.write(table.nrows+1, 12, round(sum_aver/sum_count, 4))
except:
	print(" division by zero ")
	
ever_count = 6000
for j in range(table.ncols):
	list_temp = table.col_values(j)
	#print(list_temp)
	if len(list_temp)<6000:
		ever_count = len(list_temp)-1
	ever_sum = 0
	for k in range(ever_count+1):
		if k == 0:
			continue
		ever_sum += list_temp[k]
#		print(list_temp[k])
#	print(ever_sum)
#	print(ever_count)
	ws.write(table.nrows+1, j, round(ever_sum/ever_count, 4))
	

values = ['BIA/AP rb', 'BIA/AP wb', 'VSP/GSP rb', 'VSP/GSP wb',
          'DISP/CAM rb', 'DISP/CAM wb', 'GPU rb', 'GPU wb',
          'PUBCP rb', 'PUBCP wb', 'WTLCP/AGCP rb', 'WTLCP/AGCP wb', 'SUM']
for i in range(0, len(values)):
    ws.write(0, i, values[i])

newWb.save(output_file)

