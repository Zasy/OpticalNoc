#!/bin/sh
INJECTION_RATE=0.1

TIME=1000
NOC_WIDTH=8
NOC_HEIGHT=8
NOC_DEPTH=1

# ./3d_omesh $INJECTION_RATE $NOC_WIDTH $NOC_HEIGHT $NOC_DEPTH $TIME 0 0 temperature/8x8center_block.steady

chip_temp_files=(
./temperature/8x8center_block.steady
./temperature/8x8corner_block.steady
./temperature/8x8narrow_strait.steady
)

for((m=0; m<=3; m++)); # 设置 traffic
do

for((n=0; n<=2; n++)); # 设置 温度分布
do

for((j=5; j<=6; j++)); # 设置 路由
do

for((i=1; i<=30; i+=3)); # 设置 Injection_Rate
do
     INJECTION_RATE=`echo "scale=2;${i}/100.0"|bc`
     ./3d_omesh ${INJECTION_RATE} ${NOC_WIDTH} ${NOC_HEIGHT} ${NOC_DEPTH} ${TIME} ${j} ${m} ${chip_temp_files[$n]}

done
done
done
done
