<<<<<<< HEAD
#!/bin/csh -f

# svn $Id: check_nc.sh 937 2019-01-28 06:13:04Z arango $
=======
#!/bin/bash
#
# svn $Id$
>>>>>>> master
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
# Copyright (c) 2002-2020 The ROMS/TOMS Group                           :::
#   Licensed under a MIT/X style license                                :::
#   See License_ROMS.txt                                                :::
#:::::::::::::::::::::::::::::::::::::::::::::::::::::: David Robertson :::
#                                                                       :::
# ROMS NetCDF file checker between simulations:                         :::
#                                                                       :::
# This script compares the binary differences of all ROMS/TOMS output   :::
# NetCDF files between two simulations of the same application.  The    :::
# DEBUGGING and POSITIVE_ZERO options need to be activated to avoid     :::
# time marks in output NetCDF files and other header information.       :::
#                                                                       :::
# Usage:                                                                :::
#                                                                       :::
#    check_nc.sh <dir1> <dir2>                                          :::
#                                                                       :::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

dir1=$1
dir2=$2
diffs=0

# diff the file pairs one by one and increment $diffs if necessary.
#
# $? stores the exit code of the previous command, in this case diff.
# If $? is non-zero it means that differences were found so we should
# increment $diffs.

for file in $dir1/*.nc
do
  f=`basename $file`
  diff -q ${dir1}/${f} ${dir2}/${f} >& /dev/null
  if [ $? -ne 0 ]; then
    echo "${dir1}/${f} and ${dir2}/${f} differ!"
    let "diffs+=1"
  fi
done

# Exit and set exit code to $diffs so we can sum total differences
# (hopefully 0) in the calling script.

exit ${diffs}

