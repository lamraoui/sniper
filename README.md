.

## Overview

**SNIPER** (SNIPER is Not an Imperative Errors Repairer) is a tool to automatically localize faults in imperative programs, such as ANSI C. SNIPER makes use of the LLVM compiler infrastructure for processing input programs, and uses Yices 1 as a backend SMT solver. 
The architecture of SNIPER is based on the LLVM compiler infrastructure to facilitate 
the handling of programs to be analyzed. 
Because SNIPER includes in itself the 
LLVM and Yices 1 libraries, the tool can be considered as standalone. 
The advantage of a standalone tool is that we have full control at each stage. 
The control can be on the manipulation of the LLVM’s IR 
or on the manipulation of the Yices’ context. This makes it 
possible for SNIPER to collect several information leading the fault localization 
to be precise and extensive. 


More details on the architecture of SNIPER and can be found in [Publications](#publications).


## Installation


### Download SNIPER


Click [here](https://bitbucket.org/lamraous/hosobe-project/get/b654a4ad5bfb.zip) to download **SNIPER** zip package and extract it to `sniper` on your computer. 
Alternatively, you can checkout the last version of sniper by running the command: 
```c
git clone https://lamraous@bitbucket.org/lamraous/hosobe-project.git
```

### Installation on Mac OS/Linux

Before proceeding to the following steps, please make sure that you have `gcc 4.8` installed on your computer and correctly installed `llvm 3.3` and `yices 1.0.39`.  
To install `gcc 4.8`:  
For Ubuntu, simply run `sudo apt-get install gcc 4.8`.  
For Mac, simply run `sudo port install gcc 4.8`.  
For `llvm 3.3` installation on MacOS/Linux, please visit [here](http://llvm.org). Note that **SNIPER** requires LLVM to be compiled with the build mode `Release+Debug+Asserts`.
For `yices 1.0.39` installation on MacOS/Linux, please visit [here](http://yices.csl.sri.com).

1. Open a terminal, cd to `sniper`. 
2. Run the script `./configure`. This will check that all requirements are fulfilled and configure autotools to compile **SNIPER**. 
3. Run `make -j x` with x the number of CPU cores available in your computer. This will compile **SNIPER**, this can take awhile. 
4. Run `make check -j x`. This will execute unit tests to check if the compilation went well. 
[comment]: <> (5. Run `make test-suite`. This will execute regression tests.)


### Installation on Windows

**SNIPER** is currently not supported on Windows.
Contributions to port **SNIPER** on Windows are welcome. 


## Example

This example demonstrates a simple fault localization using **SNIPER**. 
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

To analyse this function with **SNIPER** run the command:
```c
./run foo ./examples/bekkouche/Maxmin.c 1
```

Output:
```
[miaou /sniper] ./run foo ./examples/bekkouche/MinmaxKO.c 1
Bitecode generated
NDEBUG is not defined. Assert enabled.

Loading bitcode file
Bitcode Loading Time: 1 ms
Bitcode file loaded
Launching frontend
Frontend Execution Time: 1 ms
Launching backend



=== [Concolic Profiler] terminated ===
Collected program inputs (0 + 2 + 0):

Concolic Execution Time: 18 ms
Trace Formula Encoding Time: 0 ms
=================================================
Running AllDiagnosis algorithm [NOCOMB][FFTF][Line-lvl]

   number of hard constraints  15
   number of soft constraints  10
   number of error-in. inputs  2


-- Error-inducing Input: (and (= in1 0) (= in2 -1) (= in3 0))
-- Error-inducing Input: (and (= in1 0) (= in2 -1) (= in3 1))
[▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒] 100.00 %
Nb calls solver  : 10

MCSes size: 2

Run AllDiagnosis Time: 9 ms
==============================================
Time: 29 ms

real	0m0.043s
user	0m0.030s
sys	0m0.008s
```

## Help

./src/sniper --help



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

