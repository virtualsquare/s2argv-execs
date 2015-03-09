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
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <s2argv.h>
#include <unistd.h>
//#define  S2ARGVTEST

#define END 0
#define SPACE 1
#define CHAR 2
#define SINGLEQUOTE 3
#define DOUBLEQUOTE 4
#define ESCAPE 5
#define DOUBLEESC 6
#define NSTATES (DOUBLEESC+1)

#define NEWARG 1
#define CHARCOPY 2
#define ENDARG 4

char nextstate[NSTATES][NSTATES-1]= {
	{END,0,0,0,0,0},
	{END,SPACE,CHAR,SINGLEQUOTE,DOUBLEQUOTE,ESCAPE},
	{END,SPACE,CHAR,SINGLEQUOTE,DOUBLEQUOTE,ESCAPE},
	{END,SINGLEQUOTE,SINGLEQUOTE,CHAR,SINGLEQUOTE,SINGLEQUOTE},
	{END,DOUBLEQUOTE,DOUBLEQUOTE,DOUBLEQUOTE,CHAR,DOUBLEESC},
	{END,CHAR,CHAR,CHAR,CHAR,CHAR},
	{END,DOUBLEQUOTE,DOUBLEQUOTE,DOUBLEQUOTE,DOUBLEQUOTE,DOUBLEQUOTE}};

char action[NSTATES][NSTATES-1]= {
	{0,0,NEWARG|CHARCOPY,NEWARG,NEWARG,NEWARG},
	{0,0,NEWARG|CHARCOPY,NEWARG,NEWARG,NEWARG},
	{ENDARG,ENDARG,CHARCOPY,0,0,0},
	{ENDARG,CHARCOPY,CHARCOPY,0,CHARCOPY,CHARCOPY},
	{ENDARG,CHARCOPY,CHARCOPY,CHARCOPY,0,0},
	{0,CHARCOPY,CHARCOPY,CHARCOPY,CHARCOPY,CHARCOPY},
	{0,CHARCOPY,CHARCOPY,CHARCOPY,CHARCOPY,CHARCOPY}};

static int args_fsa(const char *args, char **argv, char *buf)
{
	int state=SPACE;
	int argc=0;
	char *thisarg=NULL;
	for (;state != END;args++) {
		int this;
		switch (*args) {
			case 0:
				this=END;
				break;
			case ' ':
			case '\t':
			case '\n':
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
		if (argv) {
			if (action[state][this] & NEWARG)
				thisarg=buf;
			if (action[state][this] & CHARCOPY)
				*buf++=*args;
			if (action[state][this] & ENDARG) {
				*buf++=0;
				*argv++=thisarg;
			}
		} 
		if (action[state][this] & ENDARG)
			argc++;
		//printf("%s %d->%d\n",args,state,nextstate[state][this]);
		state=nextstate[state][this];
	}
	if (argv)
		*argv=0;
	return argc;
}

#ifndef NOCOPY_ONLY

char **s2argv(const char *args, int *pargc)
{
	int argc=args_fsa(args,NULL,NULL);
	char buf[strlen(args)+1];
	char **argv=calloc(argc+1,sizeof(char *));
	if (argv) {
		int i;
		args_fsa(args,argv,buf);
		for (i=0; i<argc; i++)
			argv[i]=strdup(argv[i]);
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
#endif

int execs_common(const char *path, const char *args, char *const envp[], char *buf)
{
	int argc=args_fsa(args,NULL,NULL);
	char *argv[argc+1];
	args_fsa(args,argv,buf);
	if (path)
		return execve(path, argv, envp);
	else
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
			execsp_nocopy(buf);
			exit(-1);
		} else {
			int status;
			wait(&status);
		}
	}
}
#endif
