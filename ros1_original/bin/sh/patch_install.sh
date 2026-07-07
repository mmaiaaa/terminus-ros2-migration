#!/bin/bash
#  用于安装补丁包,补丁文包需为zip格式
# usage:
#  1 将本脚本放置到~/system/bin/sh文件夹中
#  2 $cd ~/system/bin/sh; chmod a+x ./*
#  3 执行"$./patch_install.sh [patch.zip文件路径]"或者"$bash patch_install.sh [patch.zip文件路径]"安装补丁
#    暂不支持 ../*这类相对路径
#  4 保险起见,手动执行"$chmod -R a+x ~/system"添加执行权限
#  5 启动主控 "$cd ~/system/bin;./start.sh"

patch_file=$1
work_path=`pwd` # 记录当前工作路径

# 关闭主控防止文件操作失败
cd /home/anbot/system/bin
echo "switch work path:`pwd`"
sh stop.sh

# check file name format
case $patch_file in
  /[^.]*\.zip)
    echo "input absolute path /*.zip "
    patch_path=$patch_file ;;
#  ~/[^.]*\.zip)
#    echo "input absolute path ~/*.zip"
#    patch_path=$patch_file ;;
  \./[^.]*\.zip)
    echo "input relative path ./*.zip"
    patch_path=$work_path/${patch_file#\./} ;;
  [^.]*\.zip)
    echo "input relative path *.zip  "
    patch_path=$work_path/$patch_file ;;
  *)
    echo "input invalid file name."
    exit 1 ;;
esac
echo "patch file absolute path:$patch_path"

# check the file if exist
if [ ! -f $patch_path ]; then
  echo "error: the patch file is not exist."
  exit 2
else
  read -p "confirm the patch file is right?(y/n)" input
  if [ $input != "y" ]; then
    exit 2
  fi
fi

# 解压补丁包到临时文件夹
patch_tmp=/home/anbot/tmp
if_patch_tmp_exist=0
echo ""
echo "start extract file:$patch_path to $patch_tmp"
if [ -d $patch_tmp ]; then
  if_patch_tmp_exist=1
else
  echo "create tmp directory:$patch_tmp"
  mkdir -p $patch_tmp
fi
unzip $patch_path -d $patch_tmp
if [ $? != 0 ]; then
  echo "unzip the file failed."
  if [ $if_patch_tmp_exist -eq 0 ]; then # 删除临时文件夹
    rm -rf $patch_tmp
  else
    rm -rf "$patch_tmp/patch"
  fi
  exit 3
fi

# check patch_config.txt
cd "$patch_tmp/patch"
echo ""
echo "check patch_config.txt..."
patch_config="patch_config.txt"
if [ ! -f $patch_config ]; then
  echo "patch_config.txt is not exist."
  exit 4
fi

# check platform
platform=`arch`
case $platform in
  x86_64)
    platform="x86-64"
    echo "platform $platform"  ;;
  aarch64)
    echo "platform $platform"  ;;
  *)
    echo "platform error"
    exit 5;;
esac
cd "$patch_tmp/patch"
file_list=(`ls|grep -vE "tmate|\."`) # tmate_x86 tmate_arm64
for file in ${file_list[@]}; do
  r=`file $file`
  n=`echo $r|grep ELF|wc -l`
  if [ $n -eq 1 ]; then
    n=`echo $r|grep $platform|wc -l`
    if [ $n -ne 1 ];then
      echo "the file $file don't match the platform."
      exit 6
    fi
  fi
done

# 安装补丁:从patch文件夹拷贝文件到system/bin中的对应目录
echo ""
echo "start install patch..."
base_path="/home/anbot"
count_succ=0
count_fail=0
cat $patch_config | grep -vE "#|^$" | while read -r line; do
  echo "---------------------"
  echo "update file:$line"
  file_name=${line##*/}
  source=$file_name
  target="$base_path/$line"
  target_bak="${target}.bak"

  if [ -f $target ]; then   # 判断patch中文件和system/bin中的文件是否一致,md5相同则放弃更新
    source_md5=`md5sum $source`; source_md5=${source_md5%% *}
    target_md5=`md5sum $target`; target_md5=${target_md5%% *}
    if [ $source_md5 == $target_md5 ]; then
      echo "the target file is the same as the patch file."
    else
      mv $target $target_bak # 备份原文件
      cp $source $target
      if [ $? -eq 0 ]; then
        count_succ=$[1+count_succ]
      else
        count_fail=$[1+count_fail]
      fi
    fi
  else
    cp $source $target
    if [ $? -eq 0 ]; then
      count_succ=$[1+count_succ]
    else
      count_fail=$[1+count_fail]
    fi
  fi
  echo "success:$count_succ failed:$count_fail"

  sleep 0.2s
done

# 删除临时文件夹
echo ""
echo "install the patch end."
if [ $if_patch_tmp_exist -eq 0 ]; then
  echo "delete $patch_tmp"
  rm -rf $patch_tmp
else
  echo "delete $patch_tmp/patch"
  rm -rf "$patch_tmp/patch"
fi

# 添加执行权限
cd ~
chmod -R a+x system
#sh start.sh &

exit 0
