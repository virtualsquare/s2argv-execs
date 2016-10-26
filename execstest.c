
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
#include <unistd.h>
#include <execs.h>

static void printargv(char **argv)
{
	for(;*argv!=0;argv++) {
		int argc=0;
		for(;*argv!=0;argv++,argc++)
			printf("argv[%d]=\"%s\"\n",argc,*argv);
		argc++;
	}
}

static int print1argv(char **argv, void *useless) 
{
	int argc=0;
	for(;*argv!=0;argv++,argc++)
		printf("argv[%d]=\"%s\"\n",argc,*argv);
	return 0;
}

int main()
{
	char buf[1024];
	s2argv_getvar=getenv;
	while (1) {
		char **myargv;
		if (fgets(buf,1024,stdin) == NULL)
			return 0;
		buf[strlen(buf)-1]=0;
		myargv=s2argv(buf);
		printargv(myargv);
		printf("len %zd argc %zd\n", s2argvlen(myargv), s2argc(myargv));
		s2argv_free(myargv);
		s2multiargv(buf, print1argv, NULL);
		if (fork()==0) {
			eexecsp(buf);
			exit(-1);
		} else {
			int status;
			wait(&status);
		}
	}
}
