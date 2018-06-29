TARGET_XML_FLIE="/data/thermalSensorsConfig.xml"
DEST_DIR=/sdcard/result/
FR=${DEST_DIR}check_para_$(date +%Y%m%d-%H-%M-%S).txt

function show_exit_note {
	echo "if you have any questions Please read readme(In the same directory) Priority!!!"
	echo "exit ...."
	exit
}

function determine_exit {
	if [ ! -e $1 ] ; then
		echo "we do not find correct node: "$1
		show_exit_note
	fi
}

function change_xml {
	while read LINE
	do
		local value=`echo $LINE| grep -o -E '<.+>.+</.+>'`
		local name=`echo $value | grep -o -E '</.+>'`
		name=${name#*/}
		name=${name%>*}
		local content=`echo $value | grep -o -E '>.+<'`
		content=" "${content#*>}
		content=${content%<*}
	done  < $TARGET_XML_FLIE
}

function attrget_sensor {  
    ATTR_PAIR=${1#*$2=\"}  
    echo "${ATTR_PAIR%%\"*}"  
}

function analy_xml { 
    local IFS=\>  
	local sensor_name=""
    while read -d \< ENTITY CDATA  
    do  
        local TAG_NAME=${ENTITY%% *}  
		
		if [[ $TAG_NAME == "sensor_name" || $TAG_NAME == "sensor_type" ]] ; then
			sensor_name=`attrget_sensor ${ENTITY#* } "value"` 
		fi
		if [[ $TAG_NAME == "/sensor_name" || $TAG_NAME == "sensor_type" ]] ; then
			echo "\n/*******${sensor_name}*********/" >> $FR
		fi
		
		if [[ $TAG_NAME == "polling_delay" ]] ; then
			local polling_delay=`attrget_sensor ${ENTITY#* } "value"` 
			echo "polling_delay: "$polling_delay >> $FR
		fi
		if [[ $TAG_NAME == "AvgPeriod" ]] ; then
			local AvgPeriod=`attrget_sensor ${ENTITY#* } "value"` 
			echo "AvgPeriod:     "$AvgPeriod >> $FR
		fi
		if [[ $TAG_NAME == "temp" ]] ; then
			local temp=`attrget_sensor ${ENTITY#* } "value"` 
			echo "temp: "$temp >> $FR
		fi
		if [[ $TAG_NAME == "trip" ]] ; then
			local trip_id=`attrget_sensor ${ENTITY#* } "id"` 
			echo "trip_id: "$trip_id >> $FR
		fi
		
		if [[ $TAG_NAME == "perf_trip" ]] ; then
			echo "\n"perf_trip >> $FR
		fi
		if [[ $TAG_NAME == "limit_trip" ]] ; then
			echo "\n"limit_trip >> $FR
		fi
		
		if [[ $TAG_NAME == "zone_level" ]] ; then
			echo "\n"zone_level >> $FR
			local id=`attrget_sensor ${ENTITY#* } "id"`  
			local type=`attrget_sensor ${ENTITY#* } "type"` 
			echo $id | busybox awk '{printf("id:   %-32s",$1)}' >> $FR
			echo "type: "$type >> $FR
		fi
        if [[ $TAG_NAME == "action" ]] ; then  
            local action=`attrget_sensor ${ENTITY#* } "name"`  
			local arg=`attrget_sensor ${ENTITY#* } "arg"`
			if [ "$arg" = "" ] ; then
				arg="no"
			fi
			echo $action | busybox awk '{printf("action: %-30s",$1)}' >> $FR
			echo "arg: "$arg >> $FR
        fi  
    done < $1  
}  

function show_thermal_zone {
	#输出每个thermal_zone×的温度
	local thermal_mode=0;
	for i in `ls -d /sys/class/thermal/thermal_zone*`;
	do
		determine_exit "$i/mode"
		thermal_mode=`cat $i/mode`
		if [ "${thermal_mode}" == "enabled" ] ; then
			determine_exit "$i/temp"
			local temp=`cat $i/temp`
			echo "$i Temperature: $temp" >> "$FR"
		fi
	done
}
function thermal()
{
	echo "\n**************************THERMAL*****************************" >> "$FR"
	show_thermal_zone
	
	# 对thermal.conf文件进行解析
	if [ -e "${TARGET_XML_FLIE}" ] ;  then
		change_xml
		analy_xml ${TARGET_XML_FLIE}
		rm ${TARGET_XML_FLIE}
	else
		echo "\n/system/etc/thermal.conf is not exist" >> "$FR"
		echo "thermal config default" >> "$FR"
	fi
}

thermal