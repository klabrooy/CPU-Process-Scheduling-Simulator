# CPU-Process-Scheduling-Simulator
Round Robin scheduling with worst, first and best fit swapping algorithms

## Introduction
A simulator written in C which takes process of different sizes; loads them into memory when
required, using one of three different algorithms and when needed, swaps processes out to create a
sufficiently large hole for a new process to come into memory. It also takes care of scheduling processes
currently in memory using a Round Robin algorithm.

The algorithms to be used for placing a process in memory are:
* First fit: First fit starts searching the free list from the beginning (highest address), and uses the
first hole large enough to satisfy the request. If the hole is larger than necessary, it is split, with
the process occupying the higher address range portion of the hole and the remainder being put
on the free list.
* Best fit: Chooses the smallest hole from the free list that will satisfy the request. If multiple
holes meet this criterion, choose the highest address one in the free list. If the hole is larger than
necessary, it is split, with the process occupying the higher address range portion of the hole and
the remainder being put on the free list.
* Worst Fit: Chooses the largest hole from the free list that will satisfy the request. If multiple
holes meet this criterion, choose the highest address one in the free list. If the hole is larger than
necessary, it is split, with the process occupying the higher address range portion of the hole and
the remainder being put on the free list.


## Usage

Run the following on the command line:

      
      ./swap -f <filename> -a <algorithm-name:first/best/worst> -m <size-of-main-memory-in-MB> -l <quantum-length-in-seconds>
      

The input file (see input.txt) describes a list of processes that are to be created.

Each line of the text file should contain: "<time-created> <process-id> <memory-size> <job-time>"

## Task
COMP30023: Computer Systems: Project 1
