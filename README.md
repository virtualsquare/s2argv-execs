s2argv-execs
============

s2argv converts a command string into an argv array for execv, execvp, execvpe. 
execs is like execv taking a string instead of an argv.
Similarly execsp and execspe are the counterpart of execvp and execvpe, respectively, using command strings.

Use cases are included in the man pages.

To install this library:
```
$ autoreconf -if
$ ./configure
$ make
$ sudo make install
```

To create the .deb package:
```
$ git clone https://github.com/rd235/s2argv-execs.git
$ cd s2argv-execs/
$ ./create_deb.sh
```
Note that by doing so binary packages will be created in the parent directory.


Copyright Renzo Davoli 2014, renzo@cs.unibo.it
