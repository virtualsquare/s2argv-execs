s2argv-execs
============

s2argv converts a command string into an argv array for execv, execvp, execvpe. 
execs is like execv taking a string instead of an argv.
Similarly execsp and execspe are the counterpart of execvp and execvpe, respectively, using command strings.

Use cases are included in the man pages.

To install this library:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

Copyright Renzo Davoli 2014-2020, renzo@cs.unibo.it
