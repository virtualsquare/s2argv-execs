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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "s2argv.h"
//#define  S2ARGVTEST

#define SPACE 0
#define CHAR 1
#define SINGLEQUOTE 2
#define DOUBLEQUOTE 3
#define ESCAPE 4
#define NSTATES (ESCAPE+1)

#define NEWARG 1
#define CHARCOPY 2
#define ENDARG 4

char nextstate[NSTATES][NSTATES]= {
	{SPACE,CHAR,SINGLEQUOTE,DOUBLEQUOTE,ESCAPE},
	{SPACE,CHAR,SINGLEQUOTE,DOUBLEQUOTE,ESCAPE},
	{SINGLEQUOTE,SINGLEQUOTE,CHAR,SINGLEQUOTE,SINGLEQUOTE},
	{DOUBLEQUOTE,DOUBLEQUOTE,DOUBLEQUOTE,CHAR,DOUBLEQUOTE},
	{CHAR,CHAR,CHAR,CHAR,CHAR}};

char action[NSTATES][NSTATES]= {
	{0,NEWARG|CHARCOPY,NEWARG,NEWARG,NEWARG},
	{ENDARG,CHARCOPY,0,0,0},
	{CHARCOPY,CHARCOPY,0,CHARCOPY,CHARCOPY},
	{CHARCOPY,CHARCOPY,CHARCOPY,0,CHARCOPY},
	{CHARCOPY,CHARCOPY,CHARCOPY,CHARCOPY,CHARCOPY}};

static void args_fsa_s2argv(int argc, char *arg, char *arg_in_s, void *opaque)
{
	char **argv=opaque;
	argv[argc]=strdup(arg);
}

static void args_fsa_execs(int argc, char *arg, char *arg_in_s, void *opaque)
{
	char **argv=opaque;
	argv[argc]=arg_in_s;
	strcpy(arg_in_s, arg);
}

static int args_fsa(char *args, 
		void (*fsa_argf)(int argc, char *arg, char *arg_in_s, void *opaque),
		void *opaque)
{
	char thisarg[strlen(args)+1]; // each arg is no longer than the entire string!
	char *pthisarg;
	char *pthiss;
	int argc=0;
	int state=SPACE;
	int len=strlen(args);
	int i;
	int quote;
	for (i=0; i<=len; i++) {
		int this;
		switch (args[i]) {
			case ' ':
			case '\t':
			case '\n':
			case 0:
				this=SPACE;
				break;
			case '\'':
				this=SINGLEQUOTE;
				break;
			case '"':
				this=DOUBLEQUOTE;
				break;
			case '\\':
				this=ESCAPE;
				break;
			default:
				this=CHAR;
		}
		if (action[state][this] & NEWARG) {
			pthisarg=thisarg;
			pthiss=&args[i];
		}
		if (action[state][this] & CHARCOPY)
			*pthisarg++=args[i];
		if (action[state][this] & ENDARG) {
			*pthisarg=0;
			if (fsa_argf)
				fsa_argf(argc,thisarg,pthiss,opaque);
			//printf("%d %s\n",argc,thisarg);
			argc++;
		}
		state=nextstate[state][this];
	}
	return argc;
}

char **s2argv(const char *args, int *pargc)
{
	int argc=args_fsa((char *)args,NULL,NULL);
	char **argv=calloc(argc+1,sizeof(char *));
	if (argv) {
		args_fsa((char *)args,args_fsa_s2argv,argv);
		if (pargc)
			*pargc=argc;
	}
	return argv;
}

void s2argv_free(char **argv)
{
	char **scan;
	for (scan=argv; *scan != (char *)0; scan++)
		free(*scan);
	free(argv);
}

int execs(const char *path, char *args)
{
	int argc=args_fsa(args,NULL,NULL);
	char *argv[argc+1];
	args_fsa(args,args_fsa_execs,argv);
	argv[argc]=(char *)0;
	return execv(path, argv);
}

int execsp(char *args)
{
	int argc=args_fsa(args,NULL,NULL);
	char *argv[argc+1];
	args_fsa(args,args_fsa_execs,argv);
	argv[argc]=(char *)0;
	return execvp(argv[0], argv);
}

int execspe(char *args, char *const envp[])
{
	int argc=args_fsa(args,NULL,NULL);
	char *argv[argc+1];
	args_fsa(args,args_fsa_execs,argv);
	argv[argc]=(char *)0;
	return execvpe(argv[0], argv, envp);
}

#ifdef S2ARGVTEST

static printargv(int argc, char **argv)
{
	printf("argc=%d\n",argc);
	argc=0;
	for(;*argv!=0;argv++,argc++)
		printf("argv[%d]=\"%s\"\n",argc,*argv);
}

main()
{
	char buf[1024];
	while (1) {
		char **myargv;
		int myargc;
		fgets(buf,1024,stdin);
		myargv=s2argv(buf,&myargc);
		printargv(myargc,myargv);
		s2argv_free(myargv);
		myargv=s2argv(buf,NULL);
		printargv(0,myargv);
		s2argv_free(myargv);
		if (fork()==0) {
			execsp(buf);
			exit(-1);
		} else {
			int status;
			wait(&status);
		}

	}
}
#endif
