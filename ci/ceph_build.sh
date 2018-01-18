#!/bin/bash

workspace=/root
build_dir=$workspace/stor4nfv
ceph_src_dir=$build_dir/src/ceph

function show_stage {
    echo
    echo $1
    echo
}

function ceph_build_validate {
    show_stage "validate"
    if [[ -z "$@" ]]; then
        echo "usage: ${0} output_dir pkgtype"
	exit 1
    fi
    output_dir="$1"
    pkgtype="$2"
    if [ ! -d ${output_dir} -o ! -w ${output_dir} ] ; then
        echo "${0}: Output directory '${output_dir}' does not exist or cannot be written"
        exit 1
    fi
    if [ ! -d ${ceph_src_dir} ] ; then
        echo "${0}: Directory '${ceph_src_dir}' does not exist, run this script from the root of stor4nfv source tree"
        exit 1
    fi
    echo
    echo "Build"
    echo
}

# Build ceph
show_stage "compile"
# TODO: use code inside stor4nfv
cd $workspace
git clone https://github.com/ceph/ceph.git
cd ceph
git reset --hard v12.2.2
git submodule update --init --recursive
./install-deps.sh
./do_cmake.sh
cd build
make

ceph_rpm_build() {
    show_stage "ceph rpm build"
}

ceph_deb_build() {
    cd $workspace/ceph
    dpkg-buildpackage
    if [ ${?} -ne 0 ] ; then
        echo "${0}: ceph build failed"
        exit 1
    fi
}

if [ $pkgtype == "centos" ];then
   ceph_rpm_build
elif [ $pkgtype == "ubuntu" ];then
   ceph_deb_build
   latest_ceph_build=`ls -rt $workspace | tail -1`
   cp $workspace/$latest_ceph_build $build_dir/build_output
fi
