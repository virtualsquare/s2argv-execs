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

static inline int execs(const char *path, const char *args) {
	char buf[strlen(args)+1]; 
	return execs_common(path, args, environ, buf);
}

static inline int execse(const char *path, const char *args, char *const envp[]) {
	char buf[strlen(args)+1]; 
	return execs_common(path, args, envp, buf);
}

static inline int execsp(const char *args) {
	char buf[strlen(args)+1]; 
	return execs_common(NULL, args, environ, buf);
}

static inline int execspe(const char *args, char *const envp[]) {
	char buf[strlen(args)+1]; 
	return execs_common(NULL, args, envp, buf);
}

static inline int execs_nocopy(const char *path, char *args) {
	return execs_common(path, args, environ, args);
}

static inline int execse_nocopy(const char *path, char *args, char *const envp[]) {
	return execs_common(path, args, envp, args);
}

static inline int execsp_nocopy(char *args) {
	return execs_common(NULL, args, environ, args);
}

static inline int execspe_nocopy(char *args, char *const envp[]) {
	return execs_common(NULL, args, envp, args);
}

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
int system_execs(const char *path, const char *command);

static inline int system_noshell(const char *command) {
	return system_execs(NULL, command);
}

/* popen_noshell is an "almost" drop in replacement for popen(3),
	 and pclose_noshell is its counterpart for pclose(3). */
/* popen_execs/pclose_execs do not use $PATH to search the executable file*/
FILE *popen_execs(const char *path, const char *command, const char *type);
int pclose_execs(FILE *stream);

static inline FILE *popen_noshell(const char *command, const char *type) {
	return popen_execs(NULL, command, type);
}

static inline int pclose_noshell(FILE *stream) {
	return pclose_execs(stream);
}

/* run a command in coprocessing mode */
pid_t coprocess_common(const char *path, const char *command,
		char *const argv[], char *const envp[], int pipefd[2]);

static inline pid_t coprocv(const char *path, char *const argv[], int pipefd[2]) {
	return coprocess_common(path, NULL, argv, environ, pipefd);
}

static inline pid_t coprocve(const char *path, char *const argv[], char *const envp[], int pipefd[2]) {
	return coprocess_common(path, NULL, argv, envp, pipefd);
}

static inline pid_t coprocvp(const char *file, char *const argv[], int pipefd[2]) {
	return coprocess_common(NULL, file, argv, environ, pipefd);
}

static inline pid_t coprocvpe(const char *file, char *const argv[],
		char *const envp[], int pipefd[2]) {
	return coprocess_common(NULL, file, argv, envp, pipefd);
}

static inline pid_t coprocs(const char *path, const char *command, int pipefd[2]) {
	return coprocess_common(path, command, NULL, environ, pipefd);
}

static inline pid_t coprocse(const char *path, const char *command, 
		char *const envp[], int pipefd[2]) {
	return coprocess_common(path, command, NULL, envp, pipefd);
}

static inline pid_t coprocsp(const char *command, int pipefd[2]) {
	return coprocess_common(NULL, command, NULL, environ, pipefd);
}

static inline pid_t coprocspe(const char *command, 
		char *const envp[], int pipefd[2]) {
	return coprocess_common(NULL, command, NULL, envp, pipefd);
}

#endif
