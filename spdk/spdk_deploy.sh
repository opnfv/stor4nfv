#!/bin/sh

ceph_dir=$HOME/ceph
pushd $ceph_dir
git clone https://github.com/spdk/spdk.git

spdk_dir=$ceph_dir/spdk
pushd $spdk_dir
git submodule update --init

SYSTEM=`uname -s`

if [ -s /etc/redhat-release ]; then
        pushd $spdk_dir/dpdk
        make install T=x86_64-native-linuxapp-gcc DESTDIR=./
        pushd $spdk_dir
        ./configure --with-dpdk=./dpdk/x86_64-native-linuxapp-gcc
        make
elif [ -f /etc/debian_version ]; then
        pushd $spdk_dir/dpdk
        make install T=x86_64-native-linuxapp-gcc DESTDIR=./
        pushd $spdk_dir
        ./configure --with-dpdk=./dpdk/x86_64-native-linuxapp-gcc
        make
elif [ $SYSTEM = "FreeBSD" ] ; then
        pushd $spdk_dir/dpdk
        gmake install T=x86_64-native-linuxapp-gcc DESTDIR=./
        pushd $spdk_dir/dpdk
        ./configure --with-dpdk=./dpdk/x86_64-native-linuxapp-gcc
        gmake
fi