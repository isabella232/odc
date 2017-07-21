#!/bin/bash

set -uex

# A unique working directory

wd=$(pwd)
test_wd=$(pwd)/test_odb_374

mkdir ${test_wd}
cd ${test_wd}

# Test doing a replace

odb mdset "expver='    0001'" ../2000010106.odb temporary1.odb

# And go back to where we were

odb mdset "expver='0018    '" temporary1.odb temporary2.odb

# And check that we made a change, and reverted it

odb compare ../2000010106.odb temporary1.odb

odb compare ../2000010106.odb temporary2.odb && exit -1

# Clean up

cd ${wd}
rm -rf ${test_wd}
