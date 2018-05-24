#!/bin/bash

recipe=$1
ceph_dir=$PWD/src/ceph

if [ -n "$1" ]; then
    echo "recipe file: $1"
else
    echo "must supply one recipe file"
    exit
fi

if [ ! -f "$recipe" ]; then
    echo "recipe file $recipe doesn't exist"
    exit
fi

source $recipe

echo $PROJECT
echo $SUMMARY
echo $BRANCH
echo $REPO
echo $OPTION

do_patch() {
    echo ""
    echo "$PROJECT do_patch"
    cd $ceph_dir
    if [ -d "$PROJECT" ]; then
        rm -rf $PROJECT
    fi
    git clone -b $BRANCH $REPO $PROJECT
    cd $PROJECT
    for patch in ${SOURCES[@]}
    do
        echo ""
        echo $patch
        if [ ! -f "$ceph_dir/$patch" ]; then
            echo "$patch doesn't exit"
        fi
        check_results=`patch -p1 < $ceph_dir/$patch | grep FAILED`
        echo "command (patch -p1 < $ceph_dir/$patch) results are:"
        echo "$check_results"
        if [[ $check_results =~ "FAILED" ]]; then
            echo "$patch could not be applied successfully"
            exit
        fi
    done
}

do_patch
