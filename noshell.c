#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <s2argv.h>

int system_execsr(const char *path, const char *command, int *redir) {
	if (command) {
		int status;
		pid_t pid;
		switch (pid=fork()) {
			case -1:
				return -1;
			case 0:
				if (redir) {
					int i;
					for (i=0; i<3; i++) {
						if (redir[i] >= 0 && redir[i] != i) {
							dup2(redir[i],i);
							close(redir[i]);
						}
					}
				}
				execs(path, command);
				_exit(127);
			default:
				waitpid(pid,&status,0);
				return status;
		}
	} else
		return 1; // for system compatibility
}

pid_t coprocess_common(const char *path, const char *command, 
		char *const argv[], char *const envp[], int pipefd[2]) {
	if (command) {
		int pfd_in[2];
		int pfd_out[2];
		pid_t pid;
		if (pipe(pfd_in) == -1 || pipe(pfd_out) == -1)
			return -1;
		switch (pid=fork()) {
			case -1:
				return -1;
			case 0:
				if (dup2(pfd_in[0],0) == -1 || dup2(pfd_out[1],1) == -1)
					_exit(127);
				close(pfd_in[0]);
				close(pfd_in[1]);
				close(pfd_out[0]);
				close(pfd_out[1]);
				if (argv) {
					if (path)
						execve(command, argv, envp);
					else
						execvpe(path, argv, envp);
				} else 
					execse(path, command, envp);
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

FILE *popen_execs(const char *path, const char *command, const char *type) {
	if ((type[0] == 'r' || type[0] == 'w') && (type[1] == 0 || type[1] == 'e')) {
		int fd[2];
		struct popen_info *new;
		int streamno = (type[0] == 'r') ? STDIN_FILENO : STDOUT_FILENO;
		if (pipe(fd))
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
				dup2(fd[1-streamno],1-streamno);
				close(fd[0]);
				close(fd[1]);
				if (path)
					execs_nocopy(path, (char *) command);
				else
					execsp_nocopy((char *) command);
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

