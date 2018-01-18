#!/bin/bash

type=$1

tmp_build_dir=/root/stor4nfv
build_dir=/opt/stor4nfv
tmp_output_dir=$tmp_build_dir/build_output
output_dir=$build_dir/build_output
cp -r $build_dir $tmp_build_dir

cd $tmp_build_dir
# Build ceph packages
./ci/ceph_build.sh build_output $type

if [ $type == "centos" ];then
   # Move Ceph Rpm builds from tmp_output_dir to output_dir
   mv $tmp_output_dir/*.rpm $output_dir
elif [ $type == "ubuntu" ];then
   # Move Ceph Debian builds from tmp_output_dir to output_dir
   mv $tmp_output_dir/*.deb $output_dir
fi
