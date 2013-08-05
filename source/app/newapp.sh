#!/bin/bash
. ~/.profile

a=`pwd`
mkdir $1
cd $1
cp ../$2/*ake* .
for i in *ake*
do
    cat $i | sed -e "s/$2/$1/g" > make.tmp
    echo mv -f make.tmp $i
    mv -f make.tmp $i
    echo aenf $i
    aenf $i
done

cd $a

mkdir ../../binaries/linux_x/float/app/$1
cd ../../binaries/linux_x/float/app/$1
cp ../$2/*ake* .
chmod +w *
rm make.tmp
for i in *ake*
do
    cat $i | sed -e "s/$2/$1/g" > make.tmp
    echo mv -f make.tmp $i
    mv -f make.tmp $i
    echo aenf $i
    aenf $i
done
cd $a

mkdir ../../binaries/linux_x/fixed/app/$1
cd ../../binaries/linux_x/fixed/app/$1
cp ../$2/*ake* .
chmod +w *
rm make.tmp
for i in *ake*
do
    cat $i | sed -e "s/$2/$1/g" > make.tmp
    echo mv -f make.tmp $i
    mv -f make.tmp $i
    echo aenf $i
    aenf $i
done
cd $a

mkdir ../../binaries/cygwin/float/app/$1
cd ../../binaries/cygwin/float/app/$1
cp ../$2/*ake* .
chmod +w *
rm make.tmp
for i in *ake*
do
    cat $i | sed -e "s/$2/$1/g" > make.tmp
    echo mv -f make.tmp $i
    mv -f make.tmp $i
    echo aenf $i
    aenf $i
done
cd $a

mkdir ../../binaries/cygwin/fixed/app/$1
cd ../../binaries/cygwin/fixed/app/$1
cp ../$2/*ake* .
chmod +w *
rm make.tmp
for i in *ake*
do
    cat $i | sed -e "s/$2/$1/g" > make.tmp
    echo mv -f make.tmp $i
    mv -f make.tmp $i
    echo aenf $i
    aenf $i
done
cd $a

