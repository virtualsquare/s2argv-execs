/*
 * noshell: system(3) safe and effficient replacements
 * Copyright (C) 2014-2023 Renzo Davoli. University of Bologna. <renzo@cs.unibo.it>
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

#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <execs.h>

struct system_execsq_t {
	const char *path;
	int *redir;
	int flags;
};

static int system_execsq_f(char **argv, void *arg) {
	struct system_execsq_t *v=arg;
	int status;
	pid_t pid;
	switch (pid=fork()) {
		case -1:
			return -1;
		case 0:
			if (__builtin_expect(execs_fork_security && execs_fork_security(execs_fork_security_arg) != 0, 0))
				_exit(127);
			if (v->redir) {
				int i;
				for (i=0; i<3; i++) {
					if (v->redir[i] >= 0 && v->redir[i] != i) {
						dup2(v->redir[i],i);
						close(v->redir[i]);
					}
				}
			}
			if (v->path) {
				if (*v->path)
					execv(v->path, argv);
				else if (argv[0][0] == '/')
					execv(argv[0], argv);
				else
					_exit(127);
			} else
				execvp(argv[0], argv);
			_exit(127);
		default:
			waitpid(pid,&status,0);
			return status;
	}
}

int _system_common(const char *path, const char *command, int redir[3], int flags) {
	struct system_execsq_t seqexec_var={path, redir, flags};
	if (command) {
		int rv = s2multiargv(command, system_execsq_f, &seqexec_var, flags);
		return (rv == -1) ? W_EXITCODE(127, 0) : rv;
	} else
		return 1;
}

pid_t _coprocess_common(const char *path, const char *command,
		char *const argv[], char *const envp[], int pipefd[2], int flags) {
	if (command) {
		int pfd_in[2];
		int pfd_out[2];
		pid_t pid;
		if (pipe2(pfd_in, O_CLOEXEC) == -1 || pipe2(pfd_out, O_CLOEXEC) == -1)
			return -1;
		switch (pid=fork()) {
			case -1:
				return -1;
			case 0:
				if (__builtin_expect(execs_fork_security && execs_fork_security(execs_fork_security_arg) != 0, 0))
					_exit(127);
				if (dup2(pfd_in[0],0) == -1 || dup2(pfd_out[1],1) == -1)
					_exit(127);
				close(pfd_in[0]);
				close(pfd_in[1]);
				close(pfd_out[0]);
				close(pfd_out[1]);
				if (argv) {
					if (path)
						_execs_common(path, (const char *) argv, envp, NULL, flags);
					else
						_execs_common(command, (const char *) argv, envp, NULL, flags);
				} else
					_execs_common(path, command, envp, NULL, flags);
				_exit(127);
			default:
				pipefd[0]=pfd_out[0];
				pipefd[1]=pfd_in[1];
				close(pfd_in[0]);
				close(pfd_out[1]);
				return pid;
		}
	} else
		return 1;
}

struct popen_info {
	FILE *stream;
	pid_t pid;
	struct popen_info *next;
};
static struct popen_info *popen_list;

FILE *_popen_common(const char *path, const char *command, const char *type, int flags) {
	if ((type[0] == 'r' || type[0] == 'w') && (type[1] == 0 || type[1] == 'e')) {
		int fd[2];
		struct popen_info *new;
		int streamno = (type[0] == 'r') ? STDIN_FILENO : STDOUT_FILENO;
		if (pipe2(fd, O_CLOEXEC))
			return NULL;
		if ((new=malloc(sizeof(struct popen_info)))==NULL) {
			errno = ENOMEM;
			return NULL;
		}
		switch (new->pid = fork()) {
			case -1:
				close(fd[0]);
				close(fd[1]);
				free(new);
				return NULL;
			case 0:
				if (__builtin_expect(execs_fork_security && execs_fork_security(execs_fork_security_arg) != 0, 0))
					_exit(127);
				dup2(fd[1-streamno],1-streamno);
				close(fd[0]);
				close(fd[1]);
				if (path)
					_execs_common(path,(char *) command,environ,(char *) command, flags);
				else
					_execs_common(NULL, (char *) command,environ,(char *) command, flags);
				_exit(127);
			default:
				close(fd[1-streamno]);
				if (type[1] == 'e')
					fcntl(fd[streamno], F_SETFD, FD_CLOEXEC);
				if ((new->stream = fdopen(fd[streamno], type)) == NULL) {
					close(streamno);
					free(new);
					return NULL;
				} else {
					new->next = popen_list;
					popen_list = new;
					return new->stream;
				}
		}
	} else {
		errno = EINVAL;
		return NULL;
	}
}

int pclose_execs(FILE *stream) {
	struct popen_info **pprev;
	for (pprev = &popen_list; *pprev != NULL; pprev = &((*pprev)->next)) {
		struct popen_info *curr = *pprev;
		if (curr->stream == stream) {
			int status;
			pid_t waitrv;
			*pprev = curr->next;
			fclose(curr->stream);
			while ((waitrv = waitpid(curr->pid, &status, 0)) == -1 && errno == EINTR)
				;
			free(curr);
			if (waitrv != -1)
				return status;
			else {
				errno = ECHILD;
				return -1;
			}
		}
	}
	errno = EINVAL;
	return -1;
}
