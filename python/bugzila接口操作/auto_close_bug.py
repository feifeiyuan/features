#coding=utf-8
import re
import urllib.request
from urllib import parse,request
import json
import requests
import sys
import xlrd

DEBUG=0

# 获取参数
try:
    Bug_ID = sys.argv[1]
except:
    print("Please input the bug id")
    exit()


BUGZILLA_HOME='http://bugzilla.spreadtrum.com/bugzilla/rest/bug'
API_KEY="xxxx"
Conment = "无效bug关闭"

# first modify to invald
values={
    "api_key":API_KEY,
    "cf_occurence_count":"10%",
    "status":"Invalided",
    'comment':{
        'body':"修改bug状态为无效",
        'is_private':False,
    },
}
r=requests.put("http://bugzilla.spreadtrum.com/bugzilla/rest/bug/"+Bug_ID,json=values)
if(DEBUG==1):
    print(r.text)

#the close it
values={
    "api_key":API_KEY,
    "cf_occurence_count":"10%",
    "status":"Closed",
    'comment':{
        'body':"关闭无效bug",
        'is_private':False,
    },
    "resolution":"FIXED"
}
r=requests.put("http://bugzilla.spreadtrum.com/bugzilla/rest/bug/"+Bug_ID,json=values)
if(DEBUG==1):
    print(r.text)









