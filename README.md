# SNIPER Documentation

## Overview

**SNIPER** (SNIPER is Not an Imperative Errors Repairer) is a tool to automatically localize faults in imperative programs, such as ANSI C. 
SNIPER makes use of the LLVM compiler infrastructure for processing input programs, and uses Yices 1 as a backend SMT solver. 
The architecture of SNIPER is based on the LLVM compiler infrastructure to facilitate the handling of programs to be analyzed. 

**SNIPER** can also be viewed as a framework on top of what it is possible to implement various backends. For example, in the current version, **SNIPER** implements a backend to automatically localize faults (see SniperBackend). Other methods to analyze computer programs can be implemented as well, without much effort. 

More details on SNIPER and its the architecture can be found in [Publications](#publications).


## Installation


### Download SNIPER

Click [here](https://github.com/lamraoui/sniper/archive/master.zip) to download **SNIPER** zip package and extract it to `sniper` on your computer. 
Alternatively, you can checkout the last version of sniper by running the command: 
```c
git clone https://github.com/lamraoui/sniper.git
```

### Installation on Mac OS/Linux

Before proceeding to the installation of **SNIPER**, please make sure that you have `gcc >4.8` installed on your computer and correctly installed `clang 3.3`, `llvm 3.3` and `yices 1.0.40`.  
Note that **SNIPER** requires LLVM to be compiled with the build mode `Release+Debug+Asserts`. You can check this by running the command `llvm-config --build-mode`.
Also, please note that **SNIPER** will most likely not compile or work with a different version of Clang or LLVM. 

If you do not have these tools already installed on your computer, 
please find below the steps to configure and install them.


#### GCC

To install `gcc 4.8`:  
For Ubuntu, simply run `sudo apt-get install gcc 4.8`
For Mac, simply run `sudo port install gcc 4.8` or 
via Homebrew by running the following commands:

1. brew tap homebrew/versions
2. brew install gcc48


#### Yices API

To install `yices 1.0.40`: 

1. Download Yices API 1 [here](http://yices.csl.sri.com/old/download-yices1.shtml).
2. Open a terminal, untar the file: `tar xvzf yices-1.0.40-x86_64-apple-darwin10.8.0-static-gmp.tar.gz`.
3. `cd yices-1.0.40`
4. `sudo cp lib/libyices.a /usr/lib/libyices.a`
5. `sudo cp include/yices_c.h /usr/include/yices_c.h`
6. `sudo cp include/yicesl_c.h /usr/include/yicesl_c.h`


#### Clang and LLVM


To install `clang 3.3`:

1. Open a terminal, an run `sudo port install clang-3.3`
2. Check Clang version: `clang --version`.

To build and install `llvm 3.3` follow the steps below. 
The build uses [CMake](http://llvm.org/docs/CMake.html). Although the build is known to work with CMake >= 2.8.8, we recommend CMake >= v3.2.

1. Download LLVM source code [here](http://llvm.org/releases/download.html#3.3).
2. Open a terminal, untar the file: `tar xvzf llvm-3.3.src.tar.gz`.
3. Move to the LLVM source code directory: `cd llvm-3.3.src`
4. `mkdir build`
5. `cd build`
6. Generate build targets for LLVM: `cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DLLVM_ENABLE_ASSERTIONS=On ../`
7. Run `make -j x` with x the number of CPU cores available in your computer.
8. Run `make check-all -j x` to run the regression tests and ensure everything is in working order.
9. Finally, run `make install` to install LLVM's binaries and libraries on your computer.  


#### SNIPER

To compile **SNIPER** follow the steps below.

1. Open a terminal, cd to `sniper`. 
2. Run the script `./autogen.sh`
3. Run the script `./configure`. This will check that all requirements are fulfilled and configure autotools to compile **SNIPER**. 
4. Run `make -j x` with x the number of CPU cores available in your computer. This will compile **SNIPER**, this can take awhile. 
5. Run `make check -j x`. This will execute unit tests to check if the compilation went well. 
[comment]: <> (5. Run `make test-suite`. This will execute regression tests.)
6. Run `make install`. This will install **SNIPER**'s binaries on your computer. 
7. Finally, run `sudo cp includes/sniper/sniper.h /usr/include/sniper/sniper.h`.


### Installation on Windows

**SNIPER** is currently not supported on Windows.
Contributions to port **SNIPER** on Windows are welcome. 


## Example

The example below demonstrates a simple fault localization using **SNIPER**. 
This example uses a small function, which contains an error in the assignment of line 12.

Note the include definition in line 1. It includes **SNIPER**'s special functions, such as the ones to define a pre-condition (`sniper_assume`)
or a post-condition (`sniper_assert`). 

```c
#include <sniper/sniper.h>

void foo(int in1,int in2,int in3){
	int least;
	int most;
	least = in1;
	most = in1;
	if (most < in2){
		most = in2;
	}
	if (most < in3){
		most = in3;
	}
	if (least > in2){ 
		most = in2; // bug: most = in2 instead of least = in2
	}
	if (least > in3){ 
		least = in3; 
	}
	sniper_assert(least <= most);
}
```

To analyze this function with **SNIPER** you first need to compile the function into an LLVM bitecode. To do so, run the following command:
```c
clang ./examples/bekkouche_benchmark/MinmaxKO.c -S -emit-llvm -O0 -gline-tables-only -o ./examples/bekkouche_benchmark/MinmaxKO.bc
```
Then, run **SNIPER** on the previously generated bitecode:
```c
sniper -v -ce -function foo -cfile ./examples/bekkouche_benchmark/MinmaxKO.c ./examples/bekkouche_benchmark/MinmaxKO.bc
```
This command will execute **SNIPER** with Concolic execution as test-cases generator and the Full Flow-sensitive Trace Formula (FFTF) as program encoding.

The result of the analysis should be output as below.
```c
=================================================
Running AllDiagnosis algorithm [NOCOMB][FFTF][Line-lvl]

   number of hard constraints  15
   number of soft constraints  10
   number of error-in. inputs  2


[▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒] 100.00 %

MCSes size: 2
MCSes (not combined):
{{{(= _least in1)}, {(= _most3 in2)}, {(= tmp10 (> _least in2))}, {(= tmp13 (> _least in3)), (= _least4 in3)}}
, {{(= _least in1)}, {(= _most3 in2)}, {(= tmp10 (> _least in2))}, {(= tmp13 (> _least in3)), (= _least4 in3)}}
}
==============================================
```


## Help

Command-line options to run **SNIPER** can be displayed with the command `sniper --help`.


## Documentation

The documentation of **SNIPER** can be generated with the following commands:

1. Run `cd docs` to move to the documentation folder.
2. Run `doxygen doxygen.cfg.in` to generate the documentation.
3. Open `docs/html/index.html` to see the documentation.


## Experiments

### TCAS Benchmark


### Bekkouche Benchmark



### Granularity Level Experiment

One of the experiment we performed is the evaluation of **SNIPER** on TCAS programs 
with different granularity levels. To do the experiment run 
the script `sniper/examples/tcas_benchmark/tcas-granularity-runall.sh` .



## Limitations

The present version of **SNIPER** is rather young and needs much improvements. 
Currently, **SNIPER** can analyze programs that use a subset of ANSI-C. 
The handling of loops and recursion in currently broken and needs to be fixed. 
Global variables, arrays and pointers are partially handled. 

- Unit tests

- Regression tests


<a name="publications"></a>
## Publications

Si-Mohamed Lamraoui. 
*Formula-based Fault Localization for Imperative Programs with Multiple Faults.*
Supervisor: Shin Nakajima. PhD Thesis, 2016.

Si-Mohamed Lamraoui and Shin Nakajima. 
*A Formula-based Approach for Automatic Fault Localization of Imperative Programs.*
In Journal of Information Processing, 24(1):pages 88–98, January 2016.

Si-Mohamed Lamraoui, Shin Nakajima, and Hiroshi Hosobe. 
*Hardened Flow-sensitive Trace %Formula for Fault Localization.* 
In Proc. ICECCS’15, pages 50–59, 2015.

Clàudio Belo Loureno, Si-Mohamed Lamraoui, Shin Nakajima, and Jorge Sousa Pinto. 
*Studying Verification Conditions for Imperative Programs.* 
In Proc. AVoCS 2015.

Si-Mohamed Lamraoui and Shin Nakajima. 
*A Formula-based Approach for Automatic Fault Localization of Imperative Programs.* 
In Proc. ICFEM’14, pages 251–266, 2014.

Si-Mohamed Lamraoui and Shin Nakajima. 
*SNIPER: A Tool for Automatically Localizing Errors in Imperative Programs.* 
In Proc. SES2013, 2013.

Si-Mohamed Lamraoui and Shin Nakajima. 
*Automated Error Localization with Weighted Partial Maximum Satisfiability.* 
In IEICE Technical Report, SS2013-13, 2013.

Si-Mohamed Lamraoui and Shin Nakajima. 
*SNIPER: An LLVM-based Automatic Fault Localization Tool for Imperative Programs.* 
In IEICE Technical Report, SS2015-15, 2015.

## Licence

**SNIPER** is open source software. You may freely distribute it under the terms of
the license agreement found in LICENSE.txt.

## Support

If you have questions about **SNIPER**, please contact [simohamed.lamraoui AT gmail DOT com].

