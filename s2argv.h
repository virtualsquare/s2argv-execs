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

extern char **environ;

/* This header file declares all the functions defined in
	 the libs2argv library.  
	 libexecs is a minimal subset of the libs2argv library designed 
	 for embedded systems with strict memory requirements. 
	 It implements only the execs* functions. Programs using libexecs
	 can also use the system_nocopy inline function */

/* s2argv parses args. 
	 It allocates, initializes and returns an argv array, ready for execv. 
	 If pargc is not NULL, *pargc is set to the number of args 
	 (including argv[0], excluding the (char *)0 termination tag, as usual).
 */
char **s2argv(const char *args, int *pargc);

/* s2argv_free deallocates an argv returned by s2argv */
void s2argv_free(char **argv);

/* execs is like execv: argv is computed by parsing args */
/* execsp is like execvp: argv is computed by parsing args,
	 argv[0] is the executable file to be searched for along $PATH */
/* execse and execspe permit the specification of the environment 
	 (as in execve or execvpe) */
/* execs, execse, execsp and execspe do not require dynamic allocation *but*
	 require an extra copy of args on the stack */
/* in all execs*_nocopy functions, the string args is modified 
	 (no extra copies on the stack, args is parsed on itself): */
int execs_common(const char *path, const char *args, char *const envp[], char *buf);

static inline int execse(const char *path, const char *args, char *const envp[]) {
	char buf[strlen(args)+1]; 
	return execs_common(path, args, envp, buf);
}

#define execs(path, args) execse((path),(args),environ)
#define execsp(args) execse(NULL,(args),environ)
#define execspe(args,env) execse(NULL,(args),(env))

#define execs_nocopy(path, args) execs_common((path),(args),environ,(args))
#define execse_nocopy(path, args, env) execs_common((path),(args),(env),(args))
#define execsp_nocopy(args) execs_common(NULL,(args),environ,(args))
#define execspe_nocopy(args,env) execs_common(NULL,(args),(env),(args))

static inline int system_nocopy(const char *command) {
	int status;
	pid_t pid;
	switch (pid=fork()) {
		case -1:
			return -1;
		case 0:
			execs_common(NULL, (char *) command, environ, (char *) command);
			_exit(127);
		default:
			waitpid(pid,&status,0);
			return status;
	}
}

/* system_noshell is an "almost" drop in replacement for system(3).
	 it does not start a shell but it parses the arguments and
	 runs the command */
/* system_execs is similar to system_noshell but instead of searching the
	 executable file along the directories listed in $PATH it starts
	 the program whose path has been passed as its first arg. */
int system_execsr(const char *path, const char *command, int *redir);

#define system_execsrp(cmd,redir) system_execsr(NULL,(cmd),(redir))
#define system_execs(path,cmd) system_execsr((path),(cmd),NULL)
#define system_noshell(cmd) system_execsr(NULL,(cmd),NULL)
#define system_execsp(cmd) system_execsr(NULL,(cmd),NULL)

/* popen_noshell is an "almost" drop in replacement for popen(3),
	 and pclose_noshell is its counterpart for pclose(3). */
/* popen_execs/pclose_execs do not use $PATH to search the executable file*/
FILE *popen_execs(const char *path, const char *command, const char *type);
int pclose_execs(FILE *stream);

#define popen_noshell(cmd, type) popen_execs(NULL, (cmd), (type))
#define popen_execsp(cmd, type) popen_execs(NULL, (cmd), (type))
#define pclose_noshell(stream) pclose_execs(stream)
#define pclose_execsp(stream) pclose_execs(stream)

/* run a command in coprocessing mode */
pid_t coprocess_common(const char *path, const char *command,
		char *const argv[], char *const envp[], int pipefd[2]);

#define coprocv(path, argv, pfd) coprocess_common((path),NULL,(argv), environ, pfd)
#define coprocve(path, argv, env, pfd) coprocess_common((path),NULL,(argv), (env), pfd)
#define coprocvp(file, argv, pfd) coprocess_common(NULL,(file),(argv), environ, pfd)
#define coprocvpe(file, argv, env, pfd) coprocess_common(NULL,(file),(argv), (env), pfd)
#define coprocs(path, cmd, pfd) coprocess_common((path),(cmd),NULL, environ, pfd)
#define coprocse(path, cmd, env, pfd) coprocess_common((path),(cmd),NULL, (env), pfd)
#define coprocsp(cmd, pfd) coprocess_common(NULL,(cmd),NULL, environ, pfd)
#define coprocspe(cmd, env, pfd) coprocess_common(NULL,(cmd),NULL, (env), pfd)

#endif
