#!/bin/sh

#这个文件用于更改ANBOT2A、APV2B、APV2C、APV2D、APV3  避障参数文件和机器人模型URDF文件


echo "*************************************************************************************"
echo "该脚本用于更改ANBOT2A、APV2B、APV2C、APV2D、APV2E、APVZNJ、APV3A、APVM、APVS 避障参数文件和机器人模型URDF文件"
echo "*************************************************************************************"


OBSTACLE_DIR="/home/anbot/system/bin/src/anbot/navigation-indigo-devel/anbot_obstacle/param"
URDF_DIR="/home/anbot/system/bin/src/anbot/anbot_model_config/urdf/mobile_base"

BASE_OBSTACLE_FILE="anbot_obstacle"
BASE_URDF_FILE="anbot_base.urdf.xacro"



#$1 为源文件   $2为目标文件
replace_file()
{
     if [ ! -f $1  ];then
	echo   $1  not exist,operation fail,quit
     exit -1
     fi

     if [ ! -f $2 ];then
       cp  $1  $2 
     else
       rm  $2
       cp  $1  $2
     fi     			
}



#$1  为所选型号
copy_ob_urdf_file()
{
    echo "你选择的是 $1"

    cd  $OBSTACLE_DIR
    if [ $? != 0 ];then
    echo "fail,error code:2"
        exit 2
    fi

    #echo "copy ${BASE_OBSTACLE_FILE}_$1.yaml  to   ${BASE_OBSTACLE_FILE}.yaml   "
    replace_file  ${BASE_OBSTACLE_FILE}_$1.yaml   ${BASE_OBSTACLE_FILE}.yaml     
    if [ $? != 0 ];then
        echo "fail,error code:3"
        exit 3
    fi

		
    cd  $URDF_DIR
    if [ $? != 0 ];then
        echo "fail,error code:4"
        exit 4
    fi

    #echo "copy ${BASE_URDF_FILE}.$1  to  ${BASE_URDF_FILE} "
    replace_file    ${BASE_URDF_FILE}.$1	${BASE_URDF_FILE}
    if [ $? != 0 ];then
        echo "fail,error code:5"
	exit 5
    fi	


    echo "success"
}





echo ""
echo "请输入需要选择的型号"
echo "1 :ANBOT2A"
echo "2 :APV2B"
echo "3 :APV2C"
echo "4 :APV2D(ultrasound radar)"
echo "5 :APV2E(ultrasound radar + millimeter wave radar + single-point laser)"
echo "6 :APV3A"
echo "7 :APVM"
echo "8 :APVS"
echo "9 :APVZNJ(custom-made for znj)"
echo "10 :QQB1"
echo "11 :TSL"
echo "12 :APV4"
echo "13 :WS(wu shuang liu lun di pan)"
echo "14 :XS(xin song  xiao fang ji qi ren)"
echo "15 :APVS_D"
echo "16 :DELIVERY"
echo "17 :STERILIZER"
echo "18 :Opti"
echo "其它任意键:退出"
echo ""




read   TYPE
case  "$TYPE" in
       1) copy_ob_urdf_file  ANBOT2A;;
       2) copy_ob_urdf_file  APV2B;;
       3) copy_ob_urdf_file  APV2C;;
       4) copy_ob_urdf_file  APV2D;;
       5) copy_ob_urdf_file  APV2E;;
       6) copy_ob_urdf_file  APV3A;;
       7) copy_ob_urdf_file  APVM;;
       8) copy_ob_urdf_file  APVS;;
       9) copy_ob_urdf_file  APVZNJ;;
       10) copy_ob_urdf_file  QQB1;;
       11) copy_ob_urdf_file  TSL;;
       12) copy_ob_urdf_file  APV4;;
       13) copy_ob_urdf_file  WS;;
       14) copy_ob_urdf_file  XS;;
       15) copy_ob_urdf_file  APVS_D;;
       16) copy_ob_urdf_file  DELIVERY;;
       17) copy_ob_urdf_file  STERILIZER;;
       18) copy_ob_urdf_file  Opti;;
       *) echo  "退出";;
esac





