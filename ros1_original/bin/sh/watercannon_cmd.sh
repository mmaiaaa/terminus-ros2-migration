
#!/bin/bash

echo  "water cannon  test   start"

while  true
do
    cansend    can0  356#0103000001030002     #水炮上
    sleep    8
    cansend    can0  356#0203000002030003    #水炮下
    sleep    8

   # cansend    can0  356#0103000003030000     #水炮左
   # sleep   10
   # cansend    can0  356#0203000003030000       #水炮右
   # sleep   10
   #   cansend    can0  356#0000000000000002       #开启水炮
   # sleep    1
   # cansend    can0  356#0000000000000003       #关闭水炮
   # sleep    1

done 



