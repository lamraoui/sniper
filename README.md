.

## Overview

**SNIPER** (SNIPER is Not an Imperative Errors Repairer) is a tool to automatically localize faults in imperative programs, such as ANSI C. 
SNIPER makes use of the LLVM compiler infrastructure for processing input programs, and uses Yices 1 as a backend SMT solver. 
The architecture of SNIPER is based on the LLVM compiler infrastructure to facilitate the handling of programs to be analyzed. 

**SNIPER** can also be viewed as a framework on top of what it is possible to implement various backends. For example, in the current version, **SNIPER** implements a backend to automatically localize faults (see SniperBackend). Other methods to analyse computer software can be implemented as well, witout much effort. 

More details on SNIPER and its the architecture can be found in [Publications](#publications).


## Installation


### Download SNIPER


Click [here](https://bitbucket.org/lamraous/hosobe-project/get/b654a4ad5bfb.zip) to download **SNIPER** zip package and extract it to `sniper` on your computer. 
Alternatively, you can checkout the last version of sniper by running the command: 
```c
git clone https://lamraous@bitbucket.org/lamraous/hosobe-project.git
```

### Installation on Mac OS/Linux

Before proceeding to the following steps, please make sure that you have `gcc >4.8` installed on your computer and correctly installed `clang 3.3`, `llvm 3.3` and `yices 1.0.39`.  
To install `gcc 4.8`:  
For Ubuntu, simply run `sudo apt-get install gcc 4.8`.  
For Mac, simply run `sudo port install gcc 4.8`.  
For `clang 3.3` and `llvm 3.3` installation on MacOS/Linux, please visit [here](http://llvm.org). 
Note that **SNIPER** requires LLVM to be compiled with the build mode `Release+Debug+Asserts`. You can check this by running the command `llvm-config --build-mode`.
Also, please note that **SNIPER** will most likely not compile or work with a different version of Clang or LLVM. 
For `yices 1.0.39` installation on MacOS/Linux, please visit [here](http://yices.csl.sri.com).

To compile **SNIPER** follow the steps below.
1. Open a terminal, cd to `sniper`. 
2. Run the script `./autogen.sh`.
3. Run the script `./configure`. This will check that all requirements are fulfilled and configure autotools to compile **SNIPER**. 
4. Run `make -j x` with x the number of CPU cores available in your computer. This will compile **SNIPER**, this can take awhile. 
5. Run `make check -j x`. This will execute unit tests to check if the compilation went well. 
[comment]: <> (5. Run `make test-suite`. This will execute regression tests.)
6. Run `make install`. This will install **SNIPER**'s binaries on your computer. 


### Installation on Windows

**SNIPER** is currently not supported on Windows.
Contributions to port **SNIPER** on Windows are welcome. 


## Example

The example below demonstrates a simple fault localization using **SNIPER**. 
This example uses a small function, which contains an error in the assignment of line 12.

```c
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
	assert(least <= most);
}
```

To analyse this function with **SNIPER** you first need to compile the function into an LLVM bitecode. To do so, run the following command:
```c
clang ./examples/bekkouche_benchmark/MinmaxKO.c -S -emit-llvm -O0 -gline-tables-only -o ./examples/bekkouche_benchmark/MinmaxKO.bc
```
Then, run **SNIPER** on the previously generated bitecode:
```c
sniper -v -ce -function foo -cfile ./examples/bekkouche_benchmark/MinmaxKO.c ./examples/bekkouche_benchmark/MinmaxKO.bc
```
This command will execute **SNIPER** with Concolic execution and FFTF.

The result of the analysis is ouput as below.
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

`sniper --help`


## Experiments

	# Granularity Level Experiment
	The script `sniper/examples/tcas_benchmark/tcas-granularity-runall.sh` can be 
	used to run SNIPER on TCAS programs with different granularity levels.



## Limitations

Supported: subset of ANSI-C, handeling of loops/recursion in currenlty broken,
array and pointer are partially handled, global variables, 


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

## Support

If you have questions about **SNIPER**, please contact [simohamed.lamraoui AT gmail DOT com].

