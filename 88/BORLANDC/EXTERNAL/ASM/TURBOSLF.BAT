@echo off
rem Batch file to build SPITBOL Load Function using Borland Turbo Assembler
tasm /l /e %1
tlink /t /i %1.obj float.obj,%1.slf
del %1.obj
del %1.map
