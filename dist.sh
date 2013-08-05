#!/bin/bash
find . -name ".svn" | xargs rm -rf
find . -name "svn-*" | xargs rm -rf
find . -name "*~"|xargs rm -f
find . -name "*.bak"|xargs rm -f
find . -name "*.o"|xargs rm -f
find . -name "*.so"|xargs rm -f
find . -name "*.a" | xargs rm -f
find . -name "errs" | xargs rm -f
find . -type f -perm /u+x -exec ./test.sh {} ";" -print | xargs rm -f
