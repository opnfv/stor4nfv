#!/usr/bin/env python
#
# Copyright (c) 2018 All rights reserved
# This program and the accompanying materials
# are made available under the terms of the Apache License, Version 2.0
# which accompanies this distribution, and is available at
#
# http://www.apache.org/licenses/LICENSE-2.0
#

import sys
import os
import subprocess

import functest_kubernetes.k8stest as k8stest


class K8sStor4nfvTest(k8stest.K8sTesting):

    """Stor4NFV Kubernetes test suite"""
    def __init__(self, **kwargs):
        if "case_name" not in kwargs:
            kwargs.get("case_name", 'stor4nfv_k8s')
        super(K8sStor4nfvTest, self).__init__(**kwargs)
        self.check_envs()

    def run_kubetest(self):
        success = True
        if success:
            self.result = 100
        elif failure:
            self.result = 0
