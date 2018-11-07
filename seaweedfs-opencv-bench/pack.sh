#!/bin/bash

tar cvf - utils concurrentqueue CMakeLists.txt swim.cpp swim2.cpp | gzip > swim.tgz
