#!/bin/sh

#
# $Id: foreach-ref-console.sh,v 1.1.1.1 2011/08/17 11:10:43 brwang Exp $
#
# use this script to run some script on each reference output script.
# It uses the foreach_ref_console function in functions.sh to do its
# work.
# 
# Start this from testing/*, listing all the test names (directories),
# space-separated. The script to run is the first argument.
#
# The script will be provided with three arguments -
#    1) the name of the test
#    2) the name of the console# which is either "", east or west
#    3) and the file where the reference console should be placed.
#
# The current working directory is *NOT* changed before the script is ran.
#

. ../utils/functions.sh

foreach_ref_console $@


# $Log: foreach-ref-console.sh,v $
# Revision 1.1.1.1  2011/08/17 11:10:43  brwang
# 3520G start
#
# Revision 1.1.1.1  2011/03/15 05:24:28  lei.minghuan
# import ralink
#
# Revision 1.1.1.1  2009/12/14 12:44:47  brwang
# ralink wlan
#
# Revision 1.1.1.1  2007-10-08 08:01:05  steven
#
#
# Revision 1.1  2002/10/10 16:09:59  mcr
# 	refactored update-ref-console to use new foreach_ref_console
# 	function in functions.sh, added a simple way to invoke it,
# 	and also used it in repatch-console.sh.
#
# Revision 1.1  2002/09/20 17:05:18  rgb
# # This script is to update the console reference output from the last
# # run of tests, for a list of tests (directories) supplied on the
# # command line, space delimited.
#
#
