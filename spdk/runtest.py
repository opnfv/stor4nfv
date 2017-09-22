#! /usr/bin/env python

import os
import sys


def run_test(test_path):
    os.system(test_path)


if __name__ == "__main__":
    py_file = sys.argv[0]
    py_path = os.path.abspath(os.path.dirname(py_file))
    test_path = os.path.join(py_path, 'unittest.sh')
    run_test(test_path)
