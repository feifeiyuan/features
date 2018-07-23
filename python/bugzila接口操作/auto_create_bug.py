#coding=utf-8
import re
import urllib.request
from urllib import parse,request
import json
import requests
import sys
import xlrd
from xlwt import *

BUGZILLA_HOME='http://bugzilla.spreadtrum.com/bugzilla/rest/bug'
API_KEY="xxxx"

try:
    xlspath = sys.argv[1]
except:
    print("Please input the path of fail xls")
    exit()


# for read xls using xlrd
workbook = xlrd.open_workbook(xlspath)
sheet = workbook.sheet_by_index(0)

file = Workbook(encoding='utf-8')
sheeet_wt = file.add_sheet('fail_case') # ramdon a sheet
first_col=sheeet_wt.col(0)
second_col=sheeet_wt.col(1)
first_col.width=256*110
second_col.width=256*20

# find excle data
for x in range(sheet.nrows):
    #print(sheet.row_values(x)[0]) # sumarry
    #print(sheet.row_values(x)[2]) # pac url
    values={
        'api_key':API_KEY,
        'product':"xxx",
        'component':'AP-Powermanagement',
        'version':'unspecified',
        'summary':sheet.row_values(x)[0],
        'op_sys':'Windows',
        "cf_base_on_ver":sheet.row_values(x)[3],
        "cf_change_type":"Defect",
        "cf_come_from":"Development_Area",
        "cf_occurence_count":"100%",
        "cf_probability": "1-Always Recurrence",
        "creator":"xxxx",
        "assigned_to": "xxxx",
        "comment": "自动提交bug"+sheet.row_values(x)[0],
        "is_cc_accessible": True,
        "is_creator_accessible": True,
        "cc": [
            "xxx",
        ],
    }
    sheeet_wt.write ( x, 0, sheet.row_values(x)[0])
    sheeet_wt.write ( x, 1, sheet.row_values ( x )[1] )
    sheeet_wt.write ( x, 3, sheet.row_values ( x )[3] )
    r=requests.post(BUGZILLA_HOME,data=values)
    print(r.json())
    sheeet_wt.write ( x, 2, r.json()['id'] )

file.save(xlspath)

#还有我们准备用Post传的值，这里值用字典的形式









