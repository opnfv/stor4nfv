#!/bin/bash
set -ex

sudo apt-get update
sudo apt-get install -y git

git clone https://gerrit.opnfv.org/gerrit/compass4nfv

pushd compass4nfv

CURRENT_DIR=$PWD

export SPDK_VERSION="v1.7.3"
SCENARIO=${SCENARIO:-os-nosdn-nofeature-ha.yml}

./pkgdep.sh

export TAR_URL=file://$CURRENT_DIR/work/building/compass.tar.gz
export DHA=$CURRENT_DIR/deploy/conf/vm_environment/$SCENARIO
export NETWORK=$CURRENT_DIR/deploy/conf/vm_environment/network.yml

./spdk_deploy.sh
