#!/bin/sh

stp_input_file=(
./trafficnew/2d_mesh_8x8/fpppp_2d_mesh_8x8.stp
./trafficnew/2d_mesh_8x8/h264dh_2d_mesh_8x8.stp
./trafficnew/2d_mesh_8x8/h264dl_2d_mesh_8x8.stp
./trafficnew/2d_mesh_8x8/robot_2d_mesh_8x8.stp
./trafficnew/2d_mesh_8x8/sample_2d_mesh_8x8.stp
./trafficnew/2d_mesh_8x8/satell_2d_mesh_8x8.stp
./trafficnew/2d_mesh_8x8/sparse_2d_mesh_8x8.stp
)

NOC_WIDTH=8
NOC_HEIGHT=8
NOC_DEPTH=1
EXECUTION_ITERATION=20
START_ITERATION=1
END_ITERATION=9
program_id=0
token_size_scale=(1 16 8 8 16 1 16);

delta=0.31
point_temp=55
L_MR_passive=0.005

routing_algo=3
i=5

rm ./data/*

for((i=0;i<=6;i++));
do
./3D_OMESH ${stp_input_file[$i]} \
	${NOC_WIDTH} ${NOC_HEIGHT} ${NOC_DEPTH} \
    ${EXECUTION_ITERATION} ${START_ITERATION} ${END_ITERATION} \
    ${program_id} ${token_size_scale[$i]} $i ${routing_algo}
done



