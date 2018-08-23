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
import logging

from xtesting.core import feature

logger = logging.getLogger(__name__)


class OsStor4nfvTest(feature.Feature):

    def execute(self):
        logger.info("Stor4NFV OpenStack test suite")
        self.result = 100
        return feature.Feature.EX_OK
