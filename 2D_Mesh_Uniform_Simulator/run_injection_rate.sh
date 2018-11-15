#!/bin/sh
INJECTION_RATE=0.3
NOC_WIDTH=8
NOC_HEIGHT=8
NOC_DEPTH=2

TIME=1000000
NOC_WIDTH=4
NOC_HEIGHT=4
NOC_DEPTH=2
for((i=1; i<=99; i+=3));
do
    INJECTION_RATE=`echo "scale=2;${i}/100.0"|bc`
    ./3d_omesh ${i} ${NOC_WIDTH} ${NOC_HEIGHT} ${NOC_DEPTH} ${TIME}
done

TIME=1000000
NOC_WIDTH=8
NOC_HEIGHT=8
NOC_DEPTH=2

for((i=1; i<=99; i+=3));
do
    INJECTION_RATE=`echo "scale=2;${i}/100.0"|bc`
    ./3d_omesh ${i} ${NOC_WIDTH} ${NOC_HEIGHT} ${NOC_DEPTH} ${TIME}
done