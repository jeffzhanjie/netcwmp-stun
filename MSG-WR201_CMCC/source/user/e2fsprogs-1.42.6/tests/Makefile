#
# Makefile for the tests directory
#

srcdir = .
top_srcdir = ..

top_builddir = ..
my_dir = tests
INSTALL = /usr/bin/install -c

# Beginning of file MCONFIG

all::

check::

SHELL = /bin/sh

COMPRESS_EXT = gz bz2 bz Z

prefix = /usr/local
root_prefix = /usr/local
exec_prefix = ${prefix}
root_bindir = ${exec_prefix}/bin
root_sbindir = ${exec_prefix}/sbin
root_libdir = ${exec_prefix}/lib
datarootdir = ${prefix}/share
bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libdir = ${exec_prefix}/lib
datadir= ${datarootdir}
localedir = $(datadir)/locale
root_sysconfdir= ${prefix}/etc
includedir = ${prefix}/include
mandir = ${datarootdir}/man
man1dir = $(mandir)/man1
man3dir = $(mandir)/man3
man5dir = $(mandir)/man5
man8dir = $(mandir)/man8
infodir = ${datarootdir}/info
datadir = ${datarootdir}



 V =
 ifeq ($(strip $(V)),)
 #  E = @echo
 #  ES = echo
 #  Q = @
    E = @echo
    ES = echo
    Q = @
 else
    E = @\#
    ES = \#
    Q =
 endif

# E = @echo
# ES = echo
# Q = @

CC = /data3/wang.bingrong/brwang/x86/backfire/staging_dir/toolchain-i386_gcc-4.1.2_uClibc-0.9.30.1/usr/bin/i486-openwrt-linux-uclibc-gcc
BUILD_CC = gcc
CFLAGS = -g -O2
CPPFLAGS = -I. -I$(top_builddir)/lib -I$(top_srcdir)/lib -I$(top_builddir)/intl -I$(top_srcdir)/intl
ALL_CFLAGS = $(CPPFLAGS) $(CFLAGS)
LDFLAGS = 
ALL_LDFLAGS = $(LDFLAGS) 
LDFLAGS_STATIC = $(LDFLAGS) -static
BUILD_CFLAGS = 
BUILD_LDFLAGS = 
RDYNAMIC = -rdynamic
LINK_BUILD_FLAGS = 
LINK_INSTALL_FLAGS = -f
RM = /bin/rm
LN = /bin/ln
LN_S = ln -s
MV = /bin/mv
CP = /bin/cp
CHMOD = /bin/chmod
AR = ar
AWK = gawk
SED = /bin/sed
PERL = /usr/bin/perl
RANLIB = ranlib
STRIP = strip
LD = $(PURE) /data3/wang.bingrong/brwang/x86/backfire/staging_dir/toolchain-i386_gcc-4.1.2_uClibc-0.9.30.1/usr/bin/i486-openwrt-linux-uclibc-gcc
ARUPD = $(AR) r
ARGEN = $(AR) rc
LDCONFIG = /sbin/ldconfig
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL}
MKINSTALLDIRS = $(top_builddir)/config/mkinstalldirs

#
# Library definitions
#
LIB = $(top_builddir)/lib
LIBSS = $(LIB)/libss.a  -ldl
LIBCOM_ERR = $(LIB)/libcom_err.a  -lpthread
LIBE2P = $(LIB)/libe2p.a
LIBEXT2FS = $(LIB)/libext2fs.a
LIBUUID = $(LIB)/libuuid.a 
LIBQUOTA = $(LIB)/libquota.a
LIBBLKID = $(LIB)/libblkid.a  $(LIBUUID)
LIBINTL = ${top_builddir}/intl/libintl.a 
DEPLIBSS = $(LIB)/libss.a
DEPLIBCOM_ERR = $(LIB)/libcom_err.a
DEPLIBUUID = $(LIB)/libuuid.a
DEPLIBQUOTA = $(LIB)/libquota.a
DEPLIBBLKID = $(LIB)/libblkid.a  $(DEPLIBUUID)

STATIC_LIBSS = $(LIB)/libss.a -ldl
STATIC_LIBCOM_ERR = $(LIB)/libcom_err.a -lpthread
STATIC_LIBE2P = $(LIB)/libe2p.a
STATIC_LIBEXT2FS = $(LIB)/libext2fs.a
STATIC_LIBUUID = $(LIB)/libuuid.a 
STATIC_LIBQUOTA = $(LIB)/libquota.a
STATIC_LIBBLKID = $(LIB)/libblkid.a $(STATIC_LIBUUID)
DEPSTATIC_LIBSS = $(LIB)/libss.a
DEPSTATIC_LIBCOM_ERR = $(LIB)/libcom_err.a
DEPSTATIC_LIBUUID = $(LIB)/libuuid.a
DEPSTATIC_LIBQUOTA = $(LIB)/libquota.a
DEPSTATIC_LIBBLKID = $(LIB)/libblkid.a $(DEPSTATIC_LIBUUID)

PROFILED_LIBSS = $(LIB)/libss.a -ldl
PROFILED_LIBCOM_ERR = $(LIB)/libcom_err.a -lpthread
PROFILED_LIBE2P = $(LIB)/libe2p.a
PROFILED_LIBEXT2FS = $(LIB)/libext2fs.a
PROFILED_LIBUUID = $(LIB)/libuuid.a 
PROFILED_LIBQUOTA = $(LIB)/libquota.a
PROFILED_LIBBLKID = $(LIB)/libblkid.a $(PROFILED_LIBUUID)
DEPPROFILED_LIBSS = $(LIB)/libss.a
DEPPROFILED_LIBCOM_ERR = $(LIB)/libcom_err.a
DEPPROFILED_LIBUUID = $(LIB)/libuuid.a
DEPPROFILED_LIBQUOTA = $(LIB)/libquota.a
DEPPROFILED_LIBBLKID = $(LIB)/libblkid.a $(DEPPROFILED_LIBUUID)

#
# A fast substitution command for fixing up man pages, shell scripts, etc.
#
SUBST_CONF=$(top_builddir)/util/subst.conf
SUBSTITUTE= $(top_builddir)/util/subst -f $(SUBST_CONF)
SUBSTITUTE_UPTIME= $(top_builddir)/util/subst -t -f $(SUBST_CONF)
DEP_SUBSTITUTE= $(top_builddir)/util/subst $(SUBST_CONF)

$(top_builddir)/util/subst:
	cd $(top_builddir)/util ; $(MAKE) subst

#
# Script for installing symlinks (for shared libraries)
#
$(top_builddir)/util/install-symlink: $(top_srcdir)/util/install-symlink.in \
			      $(top_builddir)/config.status
	cd $(top_builddir); CONFIG_FILES=util/install-symlink ./config.status
	chmod +x $(top_builddir)/util/install-symlink

$(top_builddir)/util/symlinks:
	cd $(top_builddir)/util ; $(MAKE) symlinks

INSTALL_SYMLINK = /bin/sh $(top_builddir)/util/install-symlink \
			 \
			--symlinks=$(top_builddir)/util/symlinks
DEP_INSTALL_SYMLINK = $(top_builddir)/util/install-symlink \
			$(top_builddir)/util/symlinks

#
# Warning flags
#
# Run make gcc-wall to do a build with warning messages.
#
#
WFLAGS=		-std=c99 -D_XOPEN_SOURCE=600 -D_GNU_SOURCE \
			-pedantic $(WFLAGS_EXTRA) \
			-Wall -W -Wwrite-strings -Wpointer-arith \
			-Wcast-qual -Wcast-align -Wno-variadic-macros \
			-Wstrict-prototypes -Wmissing-prototypes \
			-Wformat-security  -Wformat-nonliteral \
			-Wmissing-format-attribute -O2 -Wstrict-aliasing \
			-Wnested-externs -Winline -DNO_INLINE_FUNCS -Wshadow \
			-UENABLE_NLS

gcc-wall-new:
	(make CFLAGS="-g -O2 $(WFLAGS)" > /dev/null) 2>&1 | sed -f $(top_srcdir)/util/gcc-wall-cleanup 

gcc-wall:
	make clean > /dev/null
	make gcc-wall-new

#
# Installation user and groups
#
BINGRP=		bin
BINOWN=		bin
BINMODE=	555
INCGRP=		bin
INCOWN=		bin
INCMODE=	444
LIBOWN=		bin
LIBGRP=		bin
LIBMODE=	444
MANGRP=		bin
MANOWN=		bin
MANMODE=	444

#
# Autoconf magic...
#

DEP_LIB_MAKEFILES = $(top_srcdir)/lib/Makefile.library \
	$(top_srcdir)/lib/Makefile.elf-lib  \
	$(top_srcdir)/lib/Makefile.bsd-lib $(top_srcdir)/lib/Makefile.darwin-lib \
	$(top_srcdir)/lib/Makefile.solaris-lib $(top_srcdir)/lib/Makefile.checker \
	$(top_srcdir)/lib/Makefile.profile

$(top_builddir)/config.status: $(top_srcdir)/configure
	cd $(top_builddir); ./config.status --recheck

$(top_builddir)/MCONFIG: $(top_srcdir)/MCONFIG.in $(top_builddir)/config.status
	cd $(top_builddir); CONFIG_FILES=MCONFIG ./config.status

$(top_builddir)/lib/config.h: $(top_srcdir)/lib/config.h.in \
			      $(top_builddir)/config.status
	cd $(top_builddir); CONFIG_FILES=lib/config.h ./config.status

$(top_builddir)/lib/dirpaths.h: $(DEP_SUBSTITUTE) $(top_srcdir)/lib/dirpaths.h.in
	$(E) "	SUBST $@"
	$(Q) $(SUBSTITUTE) $(top_srcdir)/lib/dirpaths.h.in $@

$(top_builddir)/lib/substitute_sh: $(top_srcdir)/lib/substitute_sh.in \
		$(top_builddir)/config.status
	cd $(top_builddir); CONFIG_FILES=lib/substitute_sh ./config.status

$(top_builddir)/util/subst.conf: $(top_srcdir)/util/subst.conf.in \
		$(top_builddir)/config.status
	cd $(top_builddir); CONFIG_FILES=util/subst.conf ./config.status

Makefile: $(srcdir)/Makefile.in $(top_builddir)/MCONFIG \
		$(DEP_MAKEFILE) $(top_builddir)/config.status
	cd $(top_builddir); CONFIG_FILES=$(my_dir)/Makefile ./config.status

#$(top_srcdir)/configure: $(top_srcdir)/configure.in
#	cd $(top_srcdir) && autoheader && autoconf

#
# Make depend magic...
#

.depend: Makefile $(SRCS) $(top_srcdir)/depfix.sed $(top_srcdir)/wordwrap.pl
	if test -n "$(SRCS)" ; then \
		$(CC) -M $(ALL_CFLAGS) $(SRCS) | \
			$(SED) -f $(top_srcdir)/depfix.sed \
			    -e 's; $(srcdir)/; $$(srcdir)/;g' \
			    -e 's; $(top_srcdir)/; $$(top_srcdir)/;g' \
			    -e 's; $(top_builddir)/; $$(top_builddir)/;g' \
			    -e 's; \./; ;g' \
			    -e '/^#/d' \
			    -e '/^ *\\$$/d' | \
			$(PERL) $(top_srcdir)/wordwrap.pl > .depend; \
	else :; fi

depend:: .depend
	if test -n "$(SRCS)" ; then \
		sed -e '/^# +++ Dependency line eater +++/,$$d' \
			< $(srcdir)/Makefile.in | cat - .depend \
			> $(srcdir)/Makefile.in.new; \
	if cmp -s $(srcdir)/Makefile.in $(srcdir)/Makefile.in.new ; then \
		$(RM) $(srcdir)/Makefile.in.new ; \
	else \
		$(MV) $(srcdir)/Makefile.in $(srcdir)/Makefile.in.old; \
		$(MV) $(srcdir)/Makefile.in.new $(srcdir)/Makefile.in; \
	fi ; else :; fi

# End of file MCONFIG

all::  test_one test_script

test_one: $(srcdir)/test_one.in Makefile mke2fs.conf
	@echo "Creating test_one script..."
	@echo "#!/bin/sh" > test_one
	@echo "HTREE=y" >> test_one
	@echo "SRCDIR=." >> test_one
	@echo "DIFF_OPTS=-u" >> test_one
	@cat $(srcdir)/test_one.in >> test_one
	@chmod +x test_one

test_script: test_one test_script.in Makefile mke2fs.conf
	@echo "Creating test_script..."
	@echo "#!/bin/sh" > test_script
	@echo "SRCDIR=." >> test_script
	@cat $(srcdir)/test_script.in >> test_script
	@chmod +x test_script

mke2fs.conf: $(srcdir)/mke2fs.conf.in
	$(CP) $(srcdir)/mke2fs.conf.in mke2fs.conf

.PHONY : test_pre test_post check always_run

TESTS=$(wildcard $(srcdir)/[a-z]_*)
$(TESTS):: test_one always_run
	@./test_one $@

test_pre:
	@$(RM) -f *.failed
	@echo "Running e2fsprogs test suite..."
	@echo " "

test_post: test_pre $(TESTS)
	@$(srcdir)/test_post

check:: test_pre test_post test_script

check-failed: $(basename $(wildcard *.failed))
	@$(srcdir)/test_post


TDIR=f_testnew
# Target which creates a new testcase to simplify adding new regression tests.
testnew:
	@echo "Creating a new e2fsck testcase in ${TDIR}"
	@mkdir -p ${TDIR}
	dd if=/dev/zero of=${TDIR}/image bs=1k count=8k
	$(top_srcdir)/misc/mke2fs -j -F -N 256 ${TDIR}/image
	@echo "new test description" > ${TDIR}/name
	@echo; echo; echo "New test filesystem at ${TDIR}/image."
	@echo "Now, break the filesystem as appropriate, and run 'make testend'"

EXPECT1=${TDIR}/expect.1
EXPECT2=${TDIR}/expect.2
# Target which generates the expect files for the new testcase.
testend: test_one ${TDIR}/image
	gzip -9 ${TDIR}/image
	@OUT1=${EXPECT1} OUT2=${EXPECT2} ./test_one ${TDIR}
	@echo; echo; echo "*** output from first e2fsck run (${EXPECT1}) ***"
	@cat ${EXPECT1}
	@echo "*** output from second e2fsck run (${EXPECT2}) ***"
	@cat ${EXPECT2}
	@echo "*** end of e2fsck output ***"
	@echo; echo "Hopefully e2fsck now fixes this problem properly."
	@echo "If all is well, edit ${TDIR}/name and rename ${TDIR}."

clean::
	$(RM) -f *~ *.log *.new *.failed *.ok *.tmp test_one test_script mke2fs.conf

distclean:: clean
	$(RM) -f Makefile
	$(RM) -rf ${TDIR}
