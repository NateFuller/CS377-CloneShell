/******************************************************************
 * Project : Closh                                                        
 * Author :  Nathan Fuller (UMASS ID: 26506886)
 * Author :  Daniel Laizer (UMASS ID: 27616321)         
*******************************************************************/

README.txt -- closh.c -- 02/15/2015
====================================================================
Outline of Work

1. Cloned project and placed into a private repo to work separately
2. Tackled sequential execvp calls of closh without timeouts
3. Experiemented with timeouts in sequential portion
4. Implemented timeouts/alarm/signal in sequential portion -- finished

5. Began concurrent/parallel portion of project without timeouts
6. Debugged...
7. Debugged...
8. Debugged...
9. Finally got it to work concurrently without timers.
10. Repeated steps 6-8 above.
11. Got timeouts to work by adding the second for-loop in order to wait for 
zombie children (and reap them).
	11a. We were facing a problem where the parent process was terminating
	before all children
12. Cleaned up/commented code
13. Wrote this document

====================================================================
====================================================================
Design Choices

1. Split into halves: sequential and parallel. We've had experience working
with sequential processes before so we figured splitting up would allow us 
to work on the sequential portion and then move to parallel later.

2. For the parallel portion: chose to store forked child pids into an array
for the parent to reference afterwards in order to reap any zombie-processes.

3. Moved the signal() call to outside of the while loop since it was going
to be shared by both the sequential and parallel portions.

4. Used a global pid_t variable for the signal handler to explicitly kill that
process once the alarm goes off.
