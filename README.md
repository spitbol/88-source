# 88-source
Source distribution of Micro version of SPITBOL-386 with all of its functionality but with a much reduced memory capability.


## Note from Mark Emmer (30 Jun 2015) about the state of this distribution. 

Note that there were two versions. While both were 16-bit 8088 code, one provided 16-bit and the other 32-bit INTEGER data types in SPITBOL.  

The V37 MINIMAL source is from 2009.

The C files contain conditionals for all the other operating systems and architectures 
I did along the way, like SPARC, PowerPC, MIPS, Moto68K, etc. 
I think I recall stripping many of those out in the C files I sent you in 2009 when you first resumed work on this.  

The #define you need for the 8088 version is BCC16, and if that's still in the original files I sent you, 
perhaps you should use those C files instead. 

If anyone actually tries to build this, they will need Borland Turbo C++ 3.x or 4.x, the last versions to support 16-bit object files. 

With a quick search, it appears 3.x is available free online, though there seem to be issues finding the right full-screen DOS box for it to run in. 

The assembler used was tasm that came bundled with Borland C++. I believe the makefile is set up for Intel' 32-bit Code Builder
because I was using it at the time with the 32-bit version of SPITBOL and it had more features than Borland's tmake. 

You need a 32-bit SPITBOL to run the MINIMAL tokenizer and 8088 code generator.
 
With some effort, I'm sure it could be made to work with other 16-bit C compilers and assemblers, but Borland is the only one I tried.  

This baby SPITBOL was pretty robust, and was even used on some 8088 cards plugged into a host chassis.

# SPITBOL-88

This is the source distribution of a "micro" version of Catspaw SPITBOL-386 with all of its 
functionality but with a much reduced memory capability. It 
generates EXE files and is useful in situations where you wish to 
distribute a program that is capable of running on all MS-DOS 
platforms, from 8088 through Pentium.

Although the memory limitations are severe, you'll be surprised 
at how large a program can be accommodated in the 56 KB 
workspace. For example, the large demonstration programs 
KALAH.SPT and ATN.SPT fit comfortably. Programs that accumulate 
data in tables and arrays will be more of a problem, but for 
simple filtering or reformatting applications, SPITBOL-88 should 
prove useful.

By maintaining full functionality with SPITBOL-386, you'll be 
able to move '386 programs directly to SPITBOL-88, with no source 
code changes in most cases.

## Integers

Two versions of SPITBOL-88 are provided: SPITBOLS.EXE and 
SPITBOLL.EXE. They are identical except for the size of integers; 
they provide 16 and 32-bit integers respectively:

SPITBOLS' 16-bit integers are the same as those in Robert 
Dewar's PC-SPITBOL. Values range from -32,768 to +32,767. 
SPITBOLS stands for "SPITBOL Short".

SPITBOLL's 32-bit integers are the same as SPITBOL-80386. 
Values range from -2,147,483,648 to +2,147,483,647. Because the 
8088 is fundamentally a 16-bit machine, integer arithmetic with 
SPITBOLL will be slower than in SPITBOLS. Also, an additional 
two bytes are required to hold each integer value, which may be 
significant in programs that store many integers. SPITBOLL 
stands for "SPITBOL Long".

## Integer Keywords

Both 8088 versions limit the values of integer keywords to 
&MAXLNGTH (see below).

## Real Numbers

SPITBOL-88 provides 64-bit floating point support identical to 
SPITBOL-386, including trigonometric, logarithmic, and 
exponential functions. A software emulator is included, so an 
80x87 numeric coprocessor is not required (but will be used if 
present).

## Memory Usage

SPITBOL-88 uses a small memory model that provides approximately 
56 KB of workspace (for compiled program and data). Default 
memory values are:

|Name               |8088 version  |80386 version|
|:------------------|-------------:|------------:|
|&MAXLNGTH          |9,000 bytes   | 4 megabytes |
|Workspace          |56 kilobytes  |64 megabytes |
|Stack              |2,812 bytes   |32 kilobytes |

The initial stack and &MAXLNGTH values can be adjusted from the 
command line, subject to the overall limitations provided by the 
small memory model. The -s# command line option will change the 
stack size to "#" bytes, while -m# changes the maximum object 
size (&MAXLNGTH). Changing either will affect the workspace size. 
Increasing the stack size will diminish the workspace and 
increase &MAXLNGTH. A smaller stack will enlarge the workspace 
and decrease &MAXLNGTH.

Type SPITBOLS or SPITBOLL at the DOS prompt (no arguments) to 
obtain a complete list of command line options.

## I/O Processing Options

SPITBOL-88 follows the I/O conventions of SPITBOL-386. Options 
formed according to the syntax of Dewar's PC-SPITBOL are not 
valid.

## I/O Record Length

To prevent unnecessary memory thrashing in the small memory 
model, the default record length and buffer size is 512 bytes, 
not 1,024 bytes as is the case in SPITBOL-386. This may be 
increased to 1,024 with the -b and -l I/O options in the INPUT or 
OUTPUT function. See the description of the INPUT funciton in 
manual chapter 19 for more information on these options.

## SET function Support

The SET function provides random-access positioning within a 
file. In addition to the standard SET function arguments 
documented in the SPITBOL-386 manual, SPITBOL-88 supports most of 
the SET argument forms of Dewar's PC-SPITBOL. This is essential 
for SPITBOLS, because there is no other way to set file positions 
larger than 32,767. Specifically, the forms supported are:

|Call                     |Position |
|:------------------------|:--------|
|SET(file, 'P', m)        |Position to file offset m. |
|SET(file, 'H', m)        |Position to m*32768 + (current position mod 32768).|
|SET(file, 'R', m)        |Position to current position plus or minus m.| 
|SET(file, 'C', m)        |Change record length of binary file to m.|
|SET(file, 'D')           |Delete record is not supported.|

While these new SET functions are now available in all versions 
of Catspaw SPITBOL, they are not needed in any of the 32-bit 
versions, including SPITBOLL.

## HOST Functions

SPITBOL-88 provides identical HOST functions (including screen, 
keyboard, and BIOS calls) to SPITBOL-386. The numbering of some 
host functions differ between Dewar's PC-SPITBOL and SPITBOL-88:

|Function                      |PC-SPITBOL    |SPITBOL-88|
|:-----------------------------|:-------------|:---------|
|Obtain parameter string       |HOST(1)       | HOST(0)|
|Obtain Shell variable         |HOST(2, S)    | HOST(4, S)|
|Sound note                    |HOST(3, I1, I2)| HOST(200, I1, I2)|
|Play string                   |HOST(4, S)    | HOST(201, S)|

The include files PCHOST.INC and HOST.INC provide two different 
version-independent ways of accessing these functions.

## External Functions

SPITBOL-88 supports runtime-loadable external functions. Calling 
arguments and methods are similar to SPITBOL-386, except that 
string lengths are 16-bits, and pointers use the conventional 
8088 16:16 segment:offset notation.

For convenience, arguments specified as INTEGER in the LOAD call 
are passed to external functions as 32-bit long integers, 
regardless of which 8088 version is running. Thus the same 
external function can be used with both SPITBOLS and SPITBOLL. 
However, if an argument is passed as unconverted, a pointer to 
the integer block is passed, and either a 16-bit or 32-bit 
integer will be there, depending upon the version. The LOGIC.ASM 
file shows how to determine which version is running.

External functions must be COM files, ORG'd to location 100H. 
Assembly-language examples are provided in sub-directory 
EXTERNAL\ASM. With limitations, external functions can be written 
in C as well. See sub-directory EXTERNAL\C and the READ.ME file 
there for more information.

Floating point instructions may be used if the /e (emulated) 
option is used with Borland's Turbo Assembler, or the -f option 
(the default) with Borland C++ 3.0. Such functions must be linked 
with FLOAT.OBJ (provided) to produce the memory fixups needed to 
produce floating point emulator opcodes.


## Utility Functions

Several utility functions are provided in directory EXTERNAL\ASM. 
See comments at the beginning of each file for usage information. 
Some have associated include files to simplify usage.

* CVTNS    Convert numeric to 2, 4, or 8 byte string. Useful 
for outputting the internal binary form of integer or real values.
*    CVTSN    Inverse of CVTNS.
*    FILENAME Perform wildcard filename lookup in directory.
*    LOGIC    Perform a variety of logical and base-conversion operations.
*    LTRIM    Trim the left side of string.
*    PATHNAME Returns the file and pathname associated with and I/O channel.
*    TESTEF?  Test functions that show how different types of results can be returned.

In directory EXTERNAL\C:

*    REALCVT  Provides real-to-string conversion with precision and format controls similar to SNOBOL4+.

## Save Files, Executable Files

Save files (execution snapshots of SPITBOL's workspace) may be 
created prior to execution with the -y command-line option, or 
during execution with EXIT(-3,filename) or EXIT(-4,filename).

Executable files (full, stand-alone EXE files) may be created 
prior to execution with the -w command-line option, or during 
execution with EXIT(+3,filename) or EXIT(+4,filename).
Use +3 or -3 with EXIT() to terminate execution after writing the 
file. +4 or -4 will continue execution after writing the file 
(but with all files except INPUT, OUTPUT, and TERMINAL closed), 
and may be used to periodically checkpoint a long-running 
program.

It is now possible to EXIT to a filename that is the same as the 
file being executed. That is, an EXE file can recreate itself as 
it acquires and "remembers" additional data.

## Control-C Interrupts

SPITBOL's SETEXIT function can be used to trap Control-C 
interrupts. The &ERRTYPE code for such an interrupt is 320.

## Time Keeping

The TIME function returns version-dependent values. SPITBOLS' 
TIME function returns execution time in deciseconds (tenths of a 
second). SPITBOLL's TIME function returns execution time in 
milliseconds.

Compilation and runtime statistics (-c and -x command-line 
options) are in deciseconds for SPITBOLS, in milliseconds for 
SPITBOLL. Both express storage usage in words, where a word is 
defined to be 2 bytes. The COLLECT() function returns available 
storage in words as well.

