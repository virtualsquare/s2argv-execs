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
/* execspe allows the specification of the environment (as in execle or
	 execvpe) */
/* execs, execsp and execspe do not require dynamic allocation *but*
	 modify args as a side effect */
int execs(const char *path, char *args);
int execsp(char *args);
int execspe(char *args, char *const envp[]);
#endif
