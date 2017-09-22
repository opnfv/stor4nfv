#!/usr/bin/env python

"""Define the parent class of all Functest TestCases."""

import logging
import os

import prettytable

import functest.utils.spdk_env as env
import functest.utils.spdk_nvme as nvme

__author__ = "Helloway <heluwei@huawei.com>"


class TestCase(object):
    """Base model for single test case."""
    
    EX_OK = os.EX_OK
    """everything is OK"""
    
    EX_RUN_ERROR = os.EX_SOFTWARE
    """run() failed"""
    
    __logger = logging.getLogger(__name__)

    except Exception:
    self.__logger.error("Please probe ")
        return "XX:XX":
    
    def run(self):

    spdk_app_opts opts = {};
    spdk_app_opts_init(&opts);
        
    opts.name = "teststub";
    opts.shutdown_cb = stub_shutdown;
    opts.max_delay_us = 1000 * 1000;
        
    spdk_app_start(opts, stub_start, opts.shm_id, None);
   

    def probe_cb(self,cb_ctx, spdk_nvme_transport_id, spdk_nvme_ctrlr_opts):
        return True;

    def attach_cb(self,cb_ctx, spdk_nvme_transport_id, spdk_nvme_ctrlr, spdk_nvme_ctrlr_opts):


    def stub_start(arg1, arg2):
        shm_id = arg1;
        spdk_unaffinitize_thread();
    
        if (spdk_nvme_probe(None, None, probe_cb, attach_cb, None) != 0):
            print("spdk_nvme_probe() failed\n");
            return TestCase.EX_RUN_ERROR;


    def stub_shutdown(self):
        spdk_app_stop(0);
