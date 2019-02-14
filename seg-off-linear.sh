#!/bin/bash - 
#===============================================================================
#
#          FILE: seg-off-linear.sh
# 
#         USAGE: ./seg-off-linear.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Norman Back (), 
#  ORGANIZATION: 
#       CREATED: 14/02/19 08:01
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

# Params: loadaddr seg:offset


if [ "$#" -ne 2 ] ; then
	echo "Usage"
	echo "$0 loadaddr seg:offset startaddr"
	exit 1
fi

SEG=$(echo $2 |cut -d: -f1 )
OFF=$(echo $2 |cut -d: -f2 )
START=$1


printf "0x0%x\n" $(( 0x${SEG}0 + 0x${OFF} + 0x${START} ))

