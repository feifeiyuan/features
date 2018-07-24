#!/usr/bin/env Python
# coding=utf-8
import re ;
import sys
from collections import OrderedDict
DEBUG = 1
suspend_devices_list = ["dpm_prepare\[\d*\]","dpm_suspend\[\d*\]","dpm_suspend_late\[\d*\]","dpm_suspend_noirq\[\d*\]"]
resume_device_list = ["dpm_resume\[\d*\]","dpm_complete\[\d*\]","dpm_resume_noirq\[\d*\]","dpm_resume_early\[\d*\]"]
dic_keyword = {"suspend_enter\[\d*\]": "", "sync_filesystems\[\d*\]": "", "freeze_processes\[\d*\]": "", \
               "dpm_prepare\[\d*\]": "", "dpm_suspend\[\d*\]": "", "dpm_suspend_late\[\d*\]": "",
               "dpm_suspend_noirq\[\d*\]": "", \
               "CPU_OFF\[\d*\]": "", "syscore_suspend\[\d*\]": "", "machine_suspend\[\d*\]": "", \
               "syscore_resume\[\d*\]": "", "CPU_ON\[\d*\]": "", "dpm_resume\[\d*\]": "", \
               "dpm_complete\[\d*\]": "", "dpm_resume_noirq\[\d*\]": "", "dpm_resume_early\[\d*\]": "", \
               "resume_console\[\d*\]": "", "thaw_processes\[\d*\]": ""}
dic_sleep_resume_border = {"sleep_begin": "suspend_enter\[\d*\] begin", \
                           "sleep_end": "machine_suspend\[\d*\] begin", \
                           "resume_begin": "machine_suspend\[\d*\] end", \
                           "resume_end": "thaw_processes\[\d*\] end"}

class debug_log:
    def debug_list(self,list):
        if(DEBUG==1):
            for x in list:
                print(x)

class match_begin_end:
    def __init__(self, dic_keyword_begin,dic_keyword_begin_flag, dic_keyword_end,dic_keyword_end_flag,dic_end_time,content, result_list):
        self.dic_keyword_begin =dic_keyword_begin;
        self.dic_keyword_begin_flag = dic_keyword_begin_flag;
        self.dic_keyword_end = dic_keyword_end;
        self.dic_keyword_end_flag = dic_keyword_end_flag;
        self.content = content;
        self.result_list = result_list;
        self.dic_end_time = dic_end_time;
    def match_begin(self):
        for x in  self.dic_keyword_begin:
            if (re.findall ( x, self.content )):
                self.result_list.append(self.content)
                if(self.dic_keyword_begin_flag[x+" flag"] == "1" ):
                    index = self.result_list.index(self.dic_keyword_begin[x])
                    self.result_list.pop(index)
                self.dic_keyword_begin[x] = self.content
                self.dic_keyword_begin_flag[x+" flag"] = "1"
                self.dic_keyword_end_flag[x.replace ( " begin", " end flag")] = "0"
                continue;
    def match_end(self,list_valid):
        for x in self.dic_keyword_end:
            if (re.findall ( x, self.content )):
                self.result_list.append ( self.content )
                self.dic_keyword_end[x] = self.content
                if (self.dic_keyword_end_flag[x + " flag"] == "1"):
                    index = self.result_list.index ( self.dic_keyword_end[x] )
                    self.result_list.pop ( index )

                if(self.dic_keyword_end[x] in self.result_list):
                    # calculate start and end time
                    if(self.dic_keyword_begin[x.replace(" end"," begin")] ==""):# only end not start
                        index = self.result_list.index ( self.dic_keyword_end[x] )
                        self.result_list.pop ( index )
                    else: # match end and start cal time
                        start_time = ''.join ( re.findall ( "\d*.\d{6}", self.dic_keyword_begin[x.replace(" end"," begin")]))
                        end_time = ''.join ( re.findall ( "\d*.\d{6}", self.content ))
                        space_time = '{:.3f}'.format ( (float ( end_time ) - float ( start_time )) * 1000)
                        self.dic_end_time[self.content] = str ( space_time ) + "ms\n"
                        list_valid.append(self.dic_keyword_begin[x.replace(" end"," begin")])
                        list_valid.append(self.content)

                self.dic_keyword_end_flag[x + " flag"] = "1"
                self.dic_keyword_begin_flag[x.replace ( " end", " begin flag" )] = "0"
                continue

def find_sleep_resume_log(file_path):
    dic_keyword_begin = {}
    dic_keyword_begin_flag = {}
    dic_keyword_end = {}
    dic_keyword_end_flag = {}
    dic_end_time = OrderedDict();
    list_valid = []
    match_log_list = []
    ret_list = []
    for x in dic_keyword:
        dic_keyword_begin[x+" begin"] = dic_keyword[x];
        dic_keyword_begin_flag[x+" begin flag"] = dic_keyword[x];
        dic_keyword_end[x + " end"] = dic_keyword[x];
        dic_keyword_end_flag[x+" end flag"] = dic_keyword[x];
    try:
        with open(file_path,"r",encoding="UTF-8",errors='ignore') as ftrace_file:
            while True:
                content = ftrace_file.readline()
                if not content:
                    break
                suspend_match_begin_end_obj = match_begin_end(dic_keyword_begin,dic_keyword_begin_flag,dic_keyword_end,\
                                                              dic_keyword_end_flag,dic_end_time,content,match_log_list)
                suspend_match_begin_end_obj.match_begin ()
                suspend_match_begin_end_obj.match_end (list_valid)

            for key in match_log_list:
                if(key in list_valid):
                    if(key in dic_end_time.keys() and not re.findall("machine_suspend\[\d*\] end",key)):
                        ret_list.append("%-100s" % (key.replace("\n",""))+dic_end_time[key])
                    else:
                        ret_list.append(key)
    except Exception:
        print("there is some error during open "+file_path)
    return ret_list

class find_border:

    def __init__(self,match_sleep_resume,dic_temp,dic_group_device,dic_group_cpu,dic_group_device_flag,dic_group_cpu_flag,list_group_total,content,sleep_resume_flag):
        self.match_sleep_resume = match_sleep_resume
        self.dic_temp = dic_temp
        self.dic_group_device = dic_group_device
        self.dic_group_cpu = dic_group_cpu
        self.dic_group_device_flag = dic_group_device_flag
        self.dic_group_cpu_flag = dic_group_cpu_flag
        self.list_group_total = list_group_total
        self.content =  content
        if (sleep_resume_flag == 0):
            self.begin = dic_sleep_resume_border["sleep_begin"]
            self.end = dic_sleep_resume_border["sleep_end"]
            self.tag = "sleep"
        else:
            self.begin = dic_sleep_resume_border["resume_begin"]
            self.end = dic_sleep_resume_border["resume_end"]
            self.tag = "resume"

    def match_border(self):
        if (re.findall ( self.begin, self.content )):
            self.dic_temp["start_time"] = ''.join ( re.findall ( "\d*.\d{6}", self.content) ) # start_time
            self.dic_temp["flag"] = 1
            self.dic_temp["times"] = self.dic_temp["times"] + 1
        if (re.findall ( self.end, self.content ) and self.dic_temp["flag"] is 1):
            self.dic_temp["temp_list"].append ( self.content+'\n' )
            end_time = ''.join ( re.findall ( "\d*.\d{6}", self.content) )
            space_time = '{:.3f}'.format ( (float ( end_time ) - float (self.dic_temp["start_time"] )) * 1000)
            self.match_sleep_resume[str ( self.dic_temp["times"]) +" "+self.tag+":  "+str(space_time)+"ms"] = self.dic_temp["temp_list"] #获取对应的key
            self.dic_temp["temp_list"] = []
            self.dic_temp["flag"] = 0
            return 1
        if (self.dic_temp["flag"] == 1):
            self.dic_temp["temp_list"].append (self.content)
            if(re.findall("dpm_prepare\[\d*\]",self.content) or
                   re.findall ( "dpm_suspend\[\d*\]", self.content ) or
                   re.findall ( "dpm_suspend_late\[\d*\]", self.content ) or
                   re.findall ( "dpm_suspend_noirq\[\d*\]", self.content ) or
                   re.findall("dpm_resume\[\d*\]",self.content) or
                   re.findall ( "dpm_complete\[\d*\]", self.content ) or
                   re.findall ( "dpm_resume_noirq\[\d*\]", self.content ) or
                   re.findall ( "dpm_resume_early\[\d*\]", self.content )
               ):
                self.dic_group_device["temp_group_list"].append ( self.content )
                self.dic_group_device_flag["flag"] = 1
            else:
                self.dic_group_device_flag["flag"] = 0
                if(self.dic_group_device["temp_group_list"] != []):
                    self.list_group_total.append ( self.dic_group_device["temp_group_list"] )
                    self.dic_group_device["temp_group_list"] = []

            if(re.findall("CPU_OFF\[\d*\]",self.content) or re.findall("CPU_ON\[\d*\]",self.content)):
                self.dic_group_cpu["temp_group_list"].append(self.content)
                self.dic_group_cpu_flag["flag"] = 1
            elif(self.dic_group_cpu_flag["flag"] == 1):
                self.dic_group_cpu_flag["flag"] = 0
                self.list_group_total.append(self.dic_group_cpu["temp_group_list"])
                self.dic_group_cpu["temp_group_list"] = []


def match_sleep_resume_border(match_sleep_resume,list_group_total,match_log_list):
    dic_temp_suspend ={"flag":0,"times":0,"start_time":"","temp_list":[]}
    dic_temp_resume = {"flag": 0, "times": 0,"start_time":"", "temp_list": []}
    dic_group_device = {"temp_group_list":[]}
    dic_group_device_flag = {"flag": 0}
    dic_group_cpu = {"temp_group_list": []}
    dic_group_cpu_flag = {"flag": 0}
    dic_total_time = OrderedDict()
    dic_temp_time = {"temp_time":[]}

    for x in match_log_list:
        find_border(match_sleep_resume,dic_temp_suspend,dic_group_device,dic_group_cpu,dic_group_device_flag,dic_group_cpu_flag,list_group_total,x,0).match_border()
        find_border(match_sleep_resume,dic_temp_resume,dic_group_device,dic_group_cpu,dic_group_device_flag,dic_group_cpu_flag,list_group_total,x,1).match_border ()

    for group_list in list_group_total:
        for content in group_list:
            if(re.findall ( "\d*.\d*ms",content)):
                end_time = ''.join ( re.findall ( "\d*.\d*ms",content)).replace("ms","")
                dic_temp_time["temp_time"].append(float(end_time))
        dic_total_time[group_list[0]]= sum(dic_temp_time["temp_time"])
        dic_temp_time["temp_time"] = []
    return dic_total_time

if __name__ == "__main__":

    try:
        ftrace_path = sys.argv[1];
        result_path = sys.argv[2];
    except Exception:
        print("please input correct arg more for readme")
        exit()

    # match *** begin start
    match_log_list  = []
    match_log_list = find_sleep_resume_log(ftrace_path)
    if(not match_log_list):
        exit()
    #debug_log().debug_list(match_log_list)

    #match sleep and resume period
    match_sleep_resume = OrderedDict()
    dic_total_time = OrderedDict()
    list_group_total = []
    dic_total_time = match_sleep_resume_border(match_sleep_resume,list_group_total,match_log_list)

    try:
        with open(result_path, "w", encoding="UTF-8",errors='ignore') as result_file:
            for key in match_sleep_resume:
                result_file.write("第 "+key+'\n')
                for content in match_sleep_resume[key]:
                    for group_key in list_group_total:
                        if(content==group_key[0] ):
                            if(key.find("sleep")!=-1):
                                if(re.findall("CPU_OFF\[\d*\]",content)):
                                    tag = "\ndisable cpu : "
                                else:
                                    tag = "\nsuspend devices : "
                            else:
                                if(re.findall("CPU_ON\[\d*\]",content)):
                                    tag = "\nenable cpu : "
                                else:
                                    tag = "\nresume devices : "
                            result_file.write (tag+str('{:.3f}'.format(dic_total_time[content]))+'ms\n' )
                        elif(content==group_key[len(group_key)-1]):
                            content = content+'\n'
                    result_file.write(content)
    except Exception:
        print ( "there is some error during open " + result_path )



