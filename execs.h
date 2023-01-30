/*
 * s2argv: convert strings to argv
 * Copyright (C) 2014 Renzo Davoli. University of Bologna. <renzo@cs.unibo.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef S2ARGV_H
#define S2ARGV_H
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#define EXECS_SOVERSION 1

extern char **environ;

/* This header file declares all the functions defined in
	 the libexecs and libeexecs libraries.
	 libeexecs is a minimal subset of the libexecs library designed
	 for embedded systems with strict memory requirements.
	 It implements only the execs* functions. Programs using libexecs
	 can also use the esystem (a.k.a. system_eexecsp) inline function */

/* fork security: if defined this function gets called for each child process created
	 by this library. If it returns a non zero value it fails and exec is aborted */
/* it can be used to drop privileges such as capabilities */
extern int (* execs_fork_security)(void *execs_fork_security_arg);
extern void *execs_fork_security_arg;

/***************** library functions defined both in libexecs and in libeexecs ********/

/* execs is like execv: argv is computed by parsing args */
/* execsp is like execvp: argv is computed by parsing args,
	 argv[0] is the executable file to be searched for along $PATH */
/* execse and execspe permit the specification of the environment
	 (as in execve or execvpe) */
/* execs, execse, execsp and execspe do not require dynamic allocation *but*
	 require an extra copy of args on the stack */
/* in all eexecs* functions, the string args is modified
	 (no extra copies on the stack, args is parsed on itself): */
#define EXECS_NOSEQ 0x1
#define EXECS_NOVAR 0x2

int _execs_common(const char *path, const char *args, char *const envp[], char *buf, int flags);

#define execs(path, args) _execs_common((path),(args),environ,NULL,EXECS_NOSEQ)
#define execse(path, args, env) _execs_common((path),(args),(env),NULL,EXECS_NOSEQ)
#define execsp(args) _execs_common(NULL,(args),environ,NULL,EXECS_NOSEQ)
#define execspe(args,env) _execs_common(NULL,(args),(env),NULL,EXECS_NOSEQ)

#define eexecs(path, args) _execs_common((path),(args),environ,(args),EXECS_NOSEQ)
#define eexecse(path, args, env) _execs_common((path),(args),(env),(args),EXECS_NOSEQ)
#define eexecsp(args) _execs_common(NULL,(args),environ,(args),EXECS_NOSEQ)
#define eexecspe(args,env) _execs_common(NULL,(args),(env),(args),EXECS_NOSEQ)

static inline int system_eexecsp(const char *command) {
	int status;
	pid_t pid;
	switch (pid=fork()) {
		case -1:
			return -1;
		case 0:
			if (__builtin_expect(execs_fork_security == NULL || execs_fork_security(execs_fork_security_arg) == 0, 1))
				_execs_common(NULL, (char *) command, environ, (char *) command, 0);
			_exit(127);
		default:
			waitpid(pid,&status,0);
			return status;
	}
}

#define esystem(cmd) system_eexecsp(cmd)

/******** library functions defined in libexecs only (not in libeexec) ********/

int _system_common(const char *path, const char *command, int redir[3], int flags);

/* system_safe requires the absolute path of the command */
/* system_execs executes the program whose path has been passed as its first arg. */
#define system_safe(cmd)                  _system_common("",(cmd),NULL,EXECS_NOSEQ | EXECS_NOVAR)

#define system_execs(path,cmd)            _system_common((path),(cmd),NULL,EXECS_NOSEQ)
#define system_execsp(cmd)                _system_common(NULL,(cmd),NULL,EXECS_NOSEQ)
#define system_execsa(cmd)                _system_common("",(cmd),NULL,EXECS_NOSEQ)
#define system_execsr(path,command,redir) _system_common((path),(cmd),(redir),EXECS_NOSEQ)
#define system_execsrp(cmd,redir)         _system_common(NULL,(cmd),(redir),EXECS_NOSEQ)
#define system_execsra(cmd,redir)         _system_common("",(cmd),(redir),EXECS_NOSEQ)

/* system_nosh is an "almost" drop in replacement for system(3).
	 it does not start a shell but it parses the arguments and
	 runs the command */
/* system_execsq* support colon separated sequences of commands */
#define system_nosh(cmd)                  _system_common(NULL,(cmd),NULL,0)
#define system_execsqp(cmd)               _system_common(NULL,(cmd),NULL,0)
#define system_execsqa(cmd)               _system_common("",(cmd),NULL,0)
#define system_execsqrp(cmd,redir)        _system_common(NULL,(cmd),(redir),0)
#define system_execsqra(cmd,redir)        _system_common("",(cmd),(redir),0)

FILE *_popen_common(const char *path, const char *command, const char *type, int flags);
/* popen_execs/pclose_execs do not use $PATH to search the executable file*/
int pclose_execs(FILE *stream);

/* popen_nosh is an "almost" drop in replacement for popen(3),
	 and pclose_nosh is its counterpart for pclose(3). */
#define popen_nosh(cmd, type) _popen_common(NULL, (cmd), (type))
#define pclose_nosh(stream) _popen_common(stream)

#define popen_execs(path, cmd, type) _popen_common(path, (cmd), (type), EXECS_NOSEQ)
#define popen_execsp(cmd, type) _popen_common(NULL, (cmd), (type), EXECS_NOSEQ)
#define pclose_execsp(stream) pclose_execs(stream)

/* run a command in coprocessing mode */
pid_t _coprocess_common(const char *path, const char *command,
		char *const argv[], char *const envp[], int pipefd[2], int flags);

#define coprocv(path, argv, pfd) _coprocess_common((path),NULL,(argv), environ, pfd, EXECS_NOSEQ)
#define coprocve(path, argv, env, pfd) _coprocess_common((path),NULL,(argv), (env), pfd, EXECS_NOSEQ)
#define coprocvp(file, argv, pfd) _coprocess_common(NULL,(file),(argv), environ, pfd, EXECS_NOSEQ)
#define coprocvpe(file, argv, env, pfd) _coprocess_common(NULL,(file),(argv), (env), pfd, EXECS_NOSEQ)

#define coprocs(path, cmd, pfd) _coprocess_common((path),(cmd),NULL, environ, pfd, EXECS_NOSEQ)
#define coprocse(path, cmd, env, pfd) _coprocess_common((path),(cmd),NULL, (env), pfd, EXECS_NOSEQ)
#define coprocsp(cmd, pfd) _coprocess_common(NULL,(cmd),NULL, environ, pfd, EXECS_NOSEQ)
#define coprocspe(cmd, env, pfd) _coprocess_common(NULL,(cmd),NULL, (env), pfd, EXECS_NOSEQ)

/* Low level argc management functions */

/* s2argv parses args.
	 It allocates, initializes and returns an argv array, ready for execv.
	 s2argv is able to parse several commands separated by semicolons (;).
	 The return value is the sequence of all the corresponding argv
	 (each one has a NULL element as its terminator) and one further
	 NULL element terminates the whole sequence.
	 (i.e. this multi-argv has two NULLs in a row at its end).
	 This format is compatible with the standard argv.
 */
char **s2argv(const char *args);

/* s2argv_free deallocates an argv returned by s2argv */
void s2argv_free(char **argv);

/* the sum of s2argc for all commands, including the NULLs */
size_t s2argvlen(char **argv);

/* argc of the (first) command */
/* argv=argv+s2argc(argv)+1 is the next argv */
size_t s2argc(char **argv);

/* var definition function (e.g. s2argv_getvar=getenv)*/
typedef char * (* s2argv_getvar_t) (const char *name);
extern s2argv_getvar_t s2argv_getvar;

/* multi argv. Args can contain several commands semicolon (;) separated.
	 This function parses args and calls f for each command/argv in args.
	 If f returns 0 s2multiargv calls f for the following argv, otherwise
	 returns the non-zero value.
	*/
int s2multiargv(const char *args,
		int (*f)(char **argv, void *opaque), void *opaque, int flags);

#endif
