#! /usr/bin/make

# WARNING:
# only add extensions here that are either present in the kernel, or whose
# header files are present in the include/linux directory of this iptables
# package (HW)
#
PF_EXT_SLIB:=ah addrtype conntrack ecn icmp iprange owner policy realm recent tos ttl unclean CLUSTERIP DNAT ECN LOG MASQUERADE MIRROR NETMAP REDIRECT REJECT SAME SNAT TOS TTL ULOG time domain IPCRACK WIFIREJECT
PF6_EXT_SLIB:=ah dst eui64 frag hbh hl icmp6 ipv6header mh owner policy rt HL LOG REJECT
PFX_EXT_SLIB:=connbytes connmark connlimit comment dccp dscp esp hashlimit helper length limit mac mark multiport pkttype quota sctp state statistic standard string tcp tcpmss u32 udp CLASSIFY CONNMARK DSCP MARK NFLOG NFQUEUE NOTRACK TCPMSS TRACE HTTPCRACK TCPOPTION TCPWINDOW HTTPREDIRECT UTTLOG UTTHOSTSTA UTTINDENTIFY UTTPOLICY UTTWEBIND UTTEXCEPTQQ UTTEXCEPTMSN UTTEXCEPTALIIM uttdev uttsniper UTTNETSNIPER wifiMatch

PF_EXT_SELINUX_SLIB:=
PF6_EXT_SELINUX_SLIB:=
PFX_EXT_SELINUX_SLIB:=CONNSECMARK SECMARK

ifeq ($(DO_SELINUX), 1)
PF_EXT_SE_SLIB:=$(PF_EXT_SELINUX_SLIB)
PF6_EXT_SE_SLIB:=$(PF_EXT_SELINUX_SLIB)
PFX_EXT_SE_SLIB:=$(PF_EXT_SELINUX_SLIB)
endif

# Optionals
PF_EXT_SLIB_OPTS:=$(foreach T,$(wildcard extensions/.*-test),$(shell KERNEL_DIR=$(KERNEL_DIR) $(T)))
PF6_EXT_SLIB_OPTS:=$(foreach T,$(wildcard extensions/.*-test6),$(shell KERNEL_DIR=$(KERNEL_DIR) $(T)))
PFX_EXT_SLIB_OPTS:=$(foreach T,$(wildcard extensions/.*-testx),$(shell KERNEL_DIR=$(KERNEL_DIR) $(T)))

PF_EXT_ALL_SLIB:=$(PF_EXT_SLIB) $(PF_EXT_SE_SLIB) $(PF_EXT_SLIB_OPTS)
PF6_EXT_ALL_SLIB:=$(PF6_EXT_SLIB) $(PF6_EXT_SE_SLIB) $(PF6_EXT_SLIB_OPTS)
PFX_EXT_ALL_SLIB:=$(PFX_EXT_SLIB) $(PFX_EXT_SE_SLIB) $(PFX_EXT_SLIB_OPTS)

PF_EXT_MAN_ALL_MATCHES:=$(foreach T,$(PF_EXT_ALL_SLIB),$(shell test -f extensions/libipt_$(T).man && grep -q register_match extensions/libipt_$(T).c  && echo $(T)))
PF_EXT_MAN_ALL_TARGETS:=$(foreach T,$(PF_EXT_ALL_SLIB),$(shell test -f extensions/libipt_$(T).man && grep -q register_target extensions/libipt_$(T).c && echo $(T)))
PF6_EXT_MAN_ALL_MATCHES:=$(foreach T,$(PF6_EXT_ALL_SLIB),$(shell test -f extensions/libip6t_$(T).man && grep -q register_match6 extensions/libip6t_$(T).c  && echo $(T)))
PF6_EXT_MAN_ALL_TARGETS:=$(foreach T,$(PF6_EXT_ALL_SLIB),$(shell test -f extensions/libip6t_$(T).man && grep -q register_target6 extensions/libip6t_$(T).c && echo $(T)))

PF_EXT_MAN_ALL_MATCHES+=$(foreach T,$(PFX_EXT_ALL_SLIB),$(shell test -f extensions/libipt_$(T).man && grep -q xtables_register_match extensions/libxt_$(T).c  && echo $(T)))
PF_EXT_MAN_ALL_TARGETS+=$(foreach T,$(PFX_EXT_ALL_SLIB),$(shell test -f extensions/libipt_$(T).man && grep -q xtables_register_target extensions/libxt_$(T).c && echo $(T)))
PF6_EXT_MAN_ALL_MATCHES+=$(foreach T,$(PFX_EXT_ALL_SLIB),$(shell test -f extensions/libip6t_$(T).man && grep -q xtables_register_match extensions/libxt_$(T).c  && echo $(T)))
PF6_EXT_MAN_ALL_TARGETS+=$(foreach T,$(PFX_EXT_ALL_SLIB),$(shell test -f extensions/libip6t_$(T).man && grep -q xtables_register_target extensions/libxt_$(T).c && echo $(T)))

PF_EXT_MAN_MATCHES:=$(filter $(PF_EXT_SLIB), $(PF_EXT_MAN_ALL_MATCHES))
PF_EXT_MAN_MATCHES+=$(filter $(PFX_EXT_SLIB), $(PF_EXT_MAN_ALL_MATCHES))
PF_EXT_MAN_MATCHES+=$(filter $(PF_EXT_SE_SLIB), $(PF_EXT_MAN_ALL_MATCHES))
PF_EXT_MAN_MATCHES+=$(filter $(PFX_EXT_SE_SLIB), $(PF_EXT_MAN_ALL_MATCHES))

PF_EXT_MAN_TARGETS:=$(filter $(PF_EXT_SLIB), $(PF_EXT_MAN_ALL_TARGETS))
PF_EXT_MAN_TARGETS+=$(filter $(PFX_EXT_SLIB), $(PF_EXT_MAN_ALL_TARGETS))
PF_EXT_MAN_TARGETS+=$(filter $(PF_EXT_SE_SLIB), $(PF_EXT_MAN_ALL_TARGETS))
PF_EXT_MAN_TARGETS+=$(filter $(PFX_EXT_SE_SLIB), $(PF_EXT_MAN_ALL_TARGETS))

PF_EXT_MAN_EXTRA_MATCHES:=$(filter-out $(PF_EXT_MAN_MATCHES), $(PF_EXT_MAN_ALL_MATCHES))
PF_EXT_MAN_EXTRA_TARGETS:=$(filter-out $(PF_EXT_MAN_TARGETS), $(PF_EXT_MAN_ALL_TARGETS))

PF6_EXT_MAN_MATCHES:=$(filter $(PF6_EXT_SLIB), $(PF6_EXT_MAN_ALL_MATCHES))
PF6_EXT_MAN_MATCHES+=$(filter $(PFX_EXT_SLIB), $(PF6_EXT_MAN_ALL_MATCHES))
PF6_EXT_MAN_MATCHES+=$(filter $(PF6_EXT_SE_SLIB), $(PF6_EXT_MAN_ALL_MATCHES))
PF6_EXT_MAN_MATCHES+=$(filter $(PFX_EXT_SE_SLIB), $(PF6_EXT_MAN_ALL_MATCHES))

PF6_EXT_MAN_TARGETS:=$(filter $(PF6_EXT_SLIB), $(PF6_EXT_MAN_ALL_TARGETS))
PF6_EXT_MAN_TARGETS+=$(filter $(PFX_EXT_SLIB), $(PF6_EXT_MAN_ALL_TARGETS))
PF6_EXT_MAN_TARGETS+=$(filter $(PF6_EXT_SE_SLIB), $(PF6_EXT_MAN_ALL_TARGETS))
PF6_EXT_MAN_TARGETS+=$(filter $(PFX_EXT_SE_SLIB), $(PF6_EXT_MAN_ALL_TARGETS))

PF6_EXT_MAN_EXTRA_MATCHES:=$(filter-out $(PF6_EXT_MAN_MATCHES), $(PF6_EXT_MAN_ALL_MATCHES))
PF6_EXT_MAN_EXTRA_TARGETS:=$(filter-out $(PF6_EXT_MAN_TARGETS), $(PF6_EXT_MAN_ALL_TARGETS))

ifneq ($(DO_SELINUX), 1)
PF_EXT_MAN_EXTRA_MATCHES:=$(filter-out $(PF_EXT_SELINUX_SLIB), $(PF_EXT_MAN_EXTRA_MATCHES))
PF_EXT_MAN_EXTRA_TARGETS:=$(filter-out $(PF_EXT_SELINUX_SLIB), $(PF_EXT_MAN_EXTRA_TARGETS))
PF_EXT_MAN_EXTRA_MATCHES:=$(filter-out $(PFX_EXT_SELINUX_SLIB), $(PF_EXT_MAN_EXTRA_MATCHES))
PF_EXT_MAN_EXTRA_TARGETS:=$(filter-out $(PFX_EXT_SELINUX_SLIB), $(PF_EXT_MAN_EXTRA_TARGETS))

PF6_EXT_MAN_EXTRA_MATCHES:=$(filter-out $(PF6_EXT_SELINUX_SLIB), $(PF6_EXT_MAN_EXTRA_MATCHES))
PF6_EXT_MAN_EXTRA_TARGETS:=$(filter-out $(PF6_EXT_SELINUX_SLIB), $(PF6_EXT_MAN_EXTRA_TARGETS))
PF6_EXT_MAN_EXTRA_MATCHES:=$(filter-out $(PFX_EXT_SELINUX_SLIB), $(PF6_EXT_MAN_EXTRA_MATCHES))
PF6_EXT_MAN_EXTRA_TARGETS:=$(filter-out $(PFX_EXT_SELINUX_SLIB), $(PF6_EXT_MAN_EXTRA_TARGETS))
endif

allman:
	@echo ALL_SLIB: $(PF_EXT_ALL_SLIB)
	@echo ALL_MATCH: $(PF_EXT_MAN_ALL_MATCHES)
	@echo ALL_TARGET: $(PF_EXT_MAN_ALL_TARGETS)

PF_EXT_SLIB+=$(PF_EXT_SLIB_OPTS)
PF6_EXT_SLIB+=$(PF6_EXT_SLIB_OPTS)
PFX_EXT_SLIB+=$(PFX_EXT_SLIB_OPTS)

# for RT2880/3052 kernel 2.6
PF_EXT_SLIB+=layer7 webstr

ifeq ($(CONFIG_USER_IPTABLES_IPSET),y)
PF_EXT_SLIB+=set
PF_EXT_SLIB+=SET
endif

OPTIONALS+=$(patsubst %,XT:%,$(PFX_EXT_SLIB_OPTS))
OPTIONALS+=$(patsubst %,IPv4:%,$(PF_EXT_SLIB_OPTS))
OPTIONALS+=$(patsubst %,IPv6:%,$(PF6_EXT_SLIB_OPTS))

ifndef NO_SHARED_LIBS
SHARED_LIBS+=$(foreach T,$(PF_EXT_SLIB),extensions/libipt_$(T).so)
SHARED_SE_LIBS+=$(foreach T,$(PF_EXT_SE_SLIB),extensions/libipt_$(T).so)
EXTRA_INSTALLS+=$(foreach T, $(PF_EXT_SLIB), $(DEST_IPT_LIBDIR)/libipt_$(T).so)
EXTRA_INSTALLS+=$(foreach T, $(PF_EXT_SE_SLIB), $(DEST_IPT_LIBDIR)/libipt_$(T).so)

ifeq ($(DO_IPV6), 1)
SHARED_LIBS+=$(foreach T,$(PF6_EXT_SLIB),extensions/libip6t_$(T).so)
SHARED_SE_LIBS+=$(foreach T,$(PF6_EXT_SE_SLIB),extensions/libip6t_$(T).so)
EXTRA_INSTALLS+=$(foreach T, $(PF6_EXT_SLIB), $(DEST_IPT_LIBDIR)/libip6t_$(T).so)
EXTRA_INSTALLS+=$(foreach T, $(PF6_EXT_SE_SLIB), $(DEST_IPT_LIBDIR)/libip6t_$(T).so)
endif

SHARED_LIBS+=$(foreach T,$(PFX_EXT_SLIB),extensions/libxt_$(T).so)
SHARED_SE_LIBS+=$(foreach T,$(PFX_EXT_SE_SLIB),extensions/libxt_$(T).so)
EXTRA_INSTALLS+=$(foreach T, $(PFX_EXT_SLIB), $(DEST_IPT_LIBDIR)/libxt_$(T).so)
EXTRA_INSTALLS+=$(foreach T, $(PFX_EXT_SE_SLIB), $(DEST_IPT_LIBDIR)/libxt_$(T).so)

else 	# NO_SHARED_LIBS
EXT_OBJS+=$(foreach T,$(PF_EXT_SLIB),extensions/libipt_$(T).o)
EXT_OBJS+=$(foreach T,$(PF_EXT_SE_SLIB),extensions/libipt_$(T).o)
EXT_OBJS+=$(foreach T,$(PFX_EXT_SLIB),extensions/libxt_$(T).o)
EXT_OBJS+=$(foreach T,$(PFX_EXT_SE_SLIB),extensions/libxt_$(T).o)
EXT_FUNC+=$(foreach T,$(PF_EXT_SLIB),ipt_$(T))
EXT_FUNC+=$(foreach T,$(PF_EXT_SE_SLIB),ipt_$(T))
EXT_FUNC+=$(foreach T,$(PFX_EXT_SLIB),xt_$(T))
EXT_FUNC+=$(foreach T,$(PFX_EXT_SE_SLIB),xt_$(T))
EXT_OBJS+= extensions/initext.o
ifeq ($(DO_IPV6), 1)
EXT6_OBJS+=$(foreach T,$(PF6_EXT_SLIB),extensions/libip6t_$(T).o)
EXT6_OBJS+=$(foreach T,$(PF6_EXT_SE_SLIB),extensions/libip6t_$(T).o)
EXT6_OBJS+=$(foreach T,$(PFX_EXT_SLIB),extensions/libxt_$(T).o)
EXT6_FUNC+=$(foreach T,$(PF6_EXT_SLIB),ip6t_$(T))
EXT6_FUNC+=$(foreach T,$(PF6_EXT_SE_SLIB),ip6t_$(T))
EXT6_FUNC+=$(foreach T,$(PFX_EXT_SLIB),xt_$(T))
EXT6_OBJS+=$(foreach T,$(PFX_EXT_SE_SLIB),extensions/libxt_$(T).o)
EXT6_FUNC+=$(foreach T,$(PFX_EXT_SE_SLIB),xt_$(T))
EXT6_OBJS+= extensions/initext6.o
endif	# DO_IPV6
endif	# NO_SHARED_LIBS

ifndef TOPLEVEL_INCLUDED
local:
	cd .. && $(MAKE) $(SHARED_LIBS) $(SHARED_SE_LIBS)
endif

ifdef NO_SHARED_LIBS
extensions/libext.a: $(EXT_OBJS)
	rm -f $@; ar crv $@ $(EXT_OBJS)

extensions/libext6.a: $(EXT6_OBJS)
	rm -f $@; ar crv $@ $(EXT6_OBJS)

extensions/initext.o: extensions/initext.c
extensions/initext6.o: extensions/initext6.c

extensions/initext.c: extensions/Makefile
	echo "" > $@
	for i in $(EXT_FUNC); do \
		echo "extern void $${i}_init(void);" >> $@; \
	done
	echo "void init_extensions(void) {" >> $@
	for i in $(EXT_FUNC); do \
		echo "	$${i}_init();" >> $@; \
	done
	echo "}" >> $@

extensions/initext6.c: extensions/Makefile
	echo "" > $@
	for i in $(EXT6_FUNC); do \
		echo "extern void $${i}_init(void);" >> $@; \
	done
	echo "void init_extensions(void) {" >> $@
	for i in $(EXT6_FUNC); do \
		echo "	$${i}_init();" >> $@; \
	done
	echo "}" >> $@

extensions/lib%.o: extensions/lib%.c
	$(CC) $(CFLAGS) -D_INIT=$*_init -c -o $@ $<

endif
 
EXTRAS += extensions/libipt_targets.man
extensions/libipt_targets.man: $(patsubst %,extensions/libipt_%.man,$(PF_EXT_MAN_ALL_TARGETS))
	@for ext in $(PF_EXT_MAN_TARGETS); do \
	    echo ".SS $$ext" ;\
	    cat extensions/libipt_$$ext.man ;\
	done >extensions/libipt_targets.man
	@if [ -n "$(PF_EXT_MAN_EXTRA_TARGETS)" ]; then \
	    extra="$(PF_EXT_MAN_EXTRA_TARGETS)" ;\
	    for ext in $${extra:-""}; do \
		echo ".SS $$ext (not supported, see Patch-O-Matic)" ;\
		cat extensions/libipt_$$ext.man ;\
	    done ;\
       	fi >>extensions/libipt_targets.man

EXTRAS += extensions/libipt_matches.man
extensions/libipt_matches.man: $(patsubst %,extensions/libipt_%.man,$(PF_EXT_MAN_ALL_MATCHES))
	@for ext in $(PF_EXT_MAN_MATCHES); do \
	    echo ".SS $$ext" ;\
	    cat extensions/libipt_$$ext.man ;\
	done >extensions/libipt_matches.man
	@if [ -n "$(PF_EXT_MAN_EXTRA_MATCHES)" ]; then \
	    extra="$(PF_EXT_MAN_EXTRA_MATCHES)" ;\
	    for ext in $${extra:-""}; do \
		echo ".SS $$ext (not supported, see Patch-O-Matic)" ;\
		cat extensions/libipt_$$ext.man ;\
	    done ;\
       	fi >>extensions/libipt_matches.man

EXTRAS += extensions/libip6t_targets.man
extensions/libip6t_targets.man: $(patsubst %, extensions/libip6t_%.man, $(PF6_EXT_MAN_ALL_TARGETS))
	@for ext in $(PF6_EXT_MAN_TARGETS); do \
	    echo ".SS $$ext" ;\
	    cat extensions/libip6t_$$ext.man ;\
	done >extensions/libip6t_targets.man
	@if [ -n "$(PF6_EXT_MAN_EXTRA_TARGETS)" ]; then \
	    extra="$(PF6_EXT_MAN_EXTRA_TARGETS)" ;\
	    for ext in $${extra:-""}; do \
		echo ".SS $$ext (not supported, see Patch-O-Matic)" ;\
		cat extensions/libip6t_$$ext.man ;\
	    done ;\
       	fi >>extensions/libip6t_targets.man

EXTRAS += extensions/libip6t_matches.man
extensions/libip6t_matches.man: $(patsubst %, extensions/libip6t_%.man, $(PF6_EXT_MAN_ALL_MATCHES))
	@for ext in $(PF6_EXT_MAN_MATCHES); do \
	    echo ".SS $$ext" ;\
	    cat extensions/libip6t_$$ext.man ;\
	done >extensions/libip6t_matches.man
	@if [ -n "$(PF6_EXT_MAN_EXTRA_MATCHES)" ]; then \
	    extra="$(PF6_EXT_MAN_EXTRA_MATCHES)" ;\
	    for ext in $${extra:-""}; do \
		echo ".SS $$ext (not supported, see Patch-O-Matic)" ;\
		cat extensions/libip6t_$$ext.man ;\
	    done ;\
       	fi >>extensions/libip6t_matches.man

$(DEST_IPT_LIBDIR)/libipt_%.so: extensions/libipt_%.so
	@[ -d $(DEST_IPT_LIBDIR)/ ] || mkdir -p $(DEST_IPT_LIBDIR)/
	cp $< $@

$(DEST_IPT_LIBDIR)/libip6t_%.so: extensions/libip6t_%.so
	@[ -d $(DEST_IPT_LIBDIR)/ ] || mkdir -p $(DEST_IPT_LIBDIR)/
	cp $< $@

$(DEST_IPT_LIBDIR)/libxt_%.so: extensions/libxt_%.so
	@[ -d $(DEST_IPT_LIBDIR)/ ] || mkdir -p $(DEST_IPT_LIBDIR)/
	cp $< $@
