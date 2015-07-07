@echo off
rem Batch file for creating SPITBOL external functions with Borland C++ 3.0.
rem
rem Usage: cslftiny name <args>
rem where:
rem  name is the name of the C program to compile, without the .c extension.
rem  <args> are up to three optional arguments to the compiler, such as -O2.
rem
rem Note the use of the Tiny model to produce a COM file.  The execution
rem model is not quite correct, because DS!=SS when called from SPITBOL.
rem
bcc -mt! -c %2 %3 %4 %1.c
tlink /t /m /s /i tiny.obj %1.obj float.obj, %1.slf, %1.map, extrnlib.lib cs.lib maths.lib
