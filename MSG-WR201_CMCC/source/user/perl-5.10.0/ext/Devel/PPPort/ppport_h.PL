################################################################################
#
#  ppport_h.PL -- generate ppport.h
#
################################################################################
#
#  $Revision: 1.1 $
#  $Author: yu.dehai $
#  $Date: 2012/09/27 21:24:29 $
#
################################################################################
#
#  Version 3.x, Copyright (C) 2004-2006, Marcus Holland-Moritz.
#  Version 2.x, Copyright (C) 2001, Paul Marquess.
#  Version 1.x, Copyright (C) 1999, Kenneth Albanowski.
#
#  This program is free software; you can redistribute it and/or
#  modify it under the same terms as Perl itself.
#
################################################################################

package Devel::PPPort;
require "PPPort.pm";
rename 'ppport.h', 'ppport.old' if -f 'ppport.h';
unlink "ppport.old" if WriteFile("ppport.h") && -f 'ppport.h';
