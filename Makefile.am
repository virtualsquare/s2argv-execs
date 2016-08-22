lib_LTLIBRARIES = libexecs.la libeexecs.la
noinst_PROGRAMS = execstest

AM_CFLAGS= -Werror -Wall

libexecs_la_SOURCES = execs.c noshell.c
libexecs_la_LDFLAGS = -version-number 1:0:0

libeexecs_la_SOURCES = execs.c
libeexecs_la_CPPFLAGS = -DEEXECS
libeexecs_la_LDFLAGS = -version-number 1:0:0

execstest_SOURCE = exectest.c 
execstest_LDADD = -lexecs

include_HEADERS = execs.h
man_MANS = s2argv.3 execs.3 system_nosh.3 popen_nosh.3 coprocess.3 esystem.3

install-exec-hook:
	ln -sf esystem.3 $(DESTDIR)$(man3dir)/system_eexecsp.3
	ln -sf execs.3 $(DESTDIR)$(man3dir)/execse.3
	ln -sf execs.3 $(DESTDIR)$(man3dir)/execsp.3
	ln -sf execs.3 $(DESTDIR)$(man3dir)/execspe.3
	ln -sf execs.3 $(DESTDIR)$(man3dir)/eexecs.3
	ln -sf execs.3 $(DESTDIR)$(man3dir)/eexecse.3
	ln -sf execs.3 $(DESTDIR)$(man3dir)/eexecsp.3
	ln -sf execs.3 $(DESTDIR)$(man3dir)/eexecspe.3
	ln -sf system_nosh.3 $(DESTDIR)$(man3dir)/system_execs.3
	ln -sf system_nosh.3 $(DESTDIR)$(man3dir)/system_execsp.3
	ln -sf system_nosh.3 $(DESTDIR)$(man3dir)/system_execsa.3
	ln -sf system_nosh.3 $(DESTDIR)$(man3dir)/system_execsr.3
	ln -sf system_nosh.3 $(DESTDIR)$(man3dir)/system_execsrp.3
	ln -sf system_nosh.3 $(DESTDIR)$(man3dir)/system_execsra.3
	ln -sf popen_nosh.3 $(DESTDIR)$(man3dir)/pclose_nosh.3
	ln -sf popen_nosh.3 $(DESTDIR)$(man3dir)/popen_execsp.3
	ln -sf popen_nosh.3 $(DESTDIR)$(man3dir)/pclose_execsp.3
	ln -sf popen_nosh.3 $(DESTDIR)$(man3dir)/popen_execs.3
	ln -sf popen_nosh.3 $(DESTDIR)$(man3dir)/pclose_execs.3
	ln -sf coprocess.3 $(DESTDIR)$(man3dir)/coprocv.3
	ln -sf coprocess.3 $(DESTDIR)$(man3dir)/coprocve.3
	ln -sf coprocess.3 $(DESTDIR)$(man3dir)/coprocvp.3
	ln -sf coprocess.3 $(DESTDIR)$(man3dir)/coprocvpe.3
	ln -sf coprocess.3 $(DESTDIR)$(man3dir)/coprocs.3
	ln -sf coprocess.3 $(DESTDIR)$(man3dir)/coprocse.3
	ln -sf coprocess.3 $(DESTDIR)$(man3dir)/coprocsp.3
	ln -sf coprocess.3 $(DESTDIR)$(man3dir)/coprocspe.3

uninstall-hook:
	rm -f $(DESTDIR)$(man3dir)/system_eexecsp.3
	rm -f $(DESTDIR)$(man3dir)/execse.3
	rm -f $(DESTDIR)$(man3dir)/execsp.3
	rm -f $(DESTDIR)$(man3dir)/execspe.3
	rm -f $(DESTDIR)$(man3dir)/eexecs.3
	rm -f $(DESTDIR)$(man3dir)/eexecse.3
	rm -f $(DESTDIR)$(man3dir)/eexecsp.3
	rm -f $(DESTDIR)$(man3dir)/eexecspe.3
	rm -f $(DESTDIR)$(man3dir)/system_execs.3
	rm -f $(DESTDIR)$(man3dir)/system_execsp.3
	rm -f $(DESTDIR)$(man3dir)/system_execsa.3
	rm -f $(DESTDIR)$(man3dir)/system_execsr.3
	rm -f $(DESTDIR)$(man3dir)/system_execsrp.3
	rm -f $(DESTDIR)$(man3dir)/system_execsra.3
	rm -f $(DESTDIR)$(man3dir)/pclose_nosh.3
	rm -f $(DESTDIR)$(man3dir)/popen_execsp.3
	rm -f $(DESTDIR)$(man3dir)/pclose_execsp.3
	rm -f $(DESTDIR)$(man3dir)/popen_execs.3
	rm -f $(DESTDIR)$(man3dir)/pclose_execs.3
	rm -f $(DESTDIR)$(man3dir)/coprocv.3
	rm -f $(DESTDIR)$(man3dir)/coprocve.3
	rm -f $(DESTDIR)$(man3dir)/coprocvp.3
	rm -f $(DESTDIR)$(man3dir)/coprocvpe.3
	rm -f $(DESTDIR)$(man3dir)/coprocs.3
	rm -f $(DESTDIR)$(man3dir)/coprocse.3
	rm -f $(DESTDIR)$(man3dir)/coprocsp.3
	rm -f $(DESTDIR)$(man3dir)/coprocspe.3

