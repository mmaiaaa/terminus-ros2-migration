#!/bin/bash
# 用于根据 patch/patch_config.txt文件配置生成补丁包
# 需指定patch文件夹路径,patch_config.txt文件都已经配置好
#usage:
# 假设该脚本位于路径 ~/system/bin/sh
# 可按如下方式执行:
#   1)$bash ~/system/bin/sh/patch_generate.sh [patch路径] 
#   2)$cd ~/system/bin/sh; ./patch_generate.sh [patch路径] 
#   [patch路径]支持"~/system/bin/patch"格式的绝对路径,"./patch"或"patch"格式的相对路径, 暂不支持"../*"这类相对路径

patch_path=$1
work_path=`pwd` # 记录当前工作路径

# 获取输入文件夹的绝对路径
case $patch_path in
  /*  )
    ;;
  \./*)
    patch_path="$work_path/${patch_path#\./}" ;;
  *)
    patch_path="$work_path/$patch_path" ;;
esac
echo "you input path:$patch_path"
read -p "confirm?(y/n)" input
if [ $input != "y" ]; then
  exit 1
fi

# check if the patch directory exist
if [ ! -d $patch_path ]; then
  echo "patch directory is not exist."
  exit 2
fi

# delete old file
cd $patch_path
if [ $? -eq 0 ]; then
  ls | grep -v patch_config | xargs -i rm ./{}
fi
sleep 1s

# check patch_config.txt
echo ""
echo "check patch_config.txt..."
patch_config="$patch_path/patch_config.txt"
if [ ! -f $patch_config ]; then
  echo "patch_config.txt is not exist."
  exit 3
fi

# copy file to patch directory
echo "start copy file..."
base_path="/home/anbot"
count_succ=0
count_fail=0
cat $patch_config | grep -vE "#|^$" | while read -r line; do
  echo "copy file:$line"
  source="$base_path/$line"
  file_name=${line##*/}
  target="$patch_path/$file_name"
  #echo -e "source:$source\ntarget:$target"
  cp $source $target
  if [ $? -eq 0 ]; then
    count_succ=$[1+count_succ]
  else
    count_fail=$[1+count_fail]
  fi
  echo "success:$count_succ failed :$count_fail"
  sleep 0.2s
done

# compress file
flag=0
while [ $flag -eq 0 ]; do
  read -p "compress file name(*.zip):" input
  case $input in
    [^.]*\.zip) flag=1 ;;
    *) echo " file name is not right.";;
  esac
done
cd ${patch_path%/*}
zip -r $input patch
echo "compressed file: ${patch_path%/*}/$input"
