# LIBUTHREAD: A User-level Thread Library
##### Authors: _Ryan Stear, SID: 917196751; Rafael Millan, SID: 915769588_

## Abstract
Our User-Level Thread Library provides a complete interface for
applications to create and run independent threads concurrently at user-level

Our Library allows the user to:
- Create and schedule new threads.
- Use semaphores to synchronize resource management among threads.
- Use Round-Robin preemptive scheduling on threads with 10ms interrupts.

## Implementation

### Queue API
We implemented our queue using a doubly linked list. We defined a `queue`
structure as a linked list of `q_node` data structures with a void pointer
containing the node data. Additionally, each node contains pointer to its next
and previous element in the queue. In this case, each node represents a thread
in our program. Our `queue` struct keeps track of the head and tail pointers of
the queue. This allows us to implement the `enqueue` and `dequeue` operations
in O(1). Error handling ensures that valid arguments are passed into the queue
library funrctions. Enqueuing or dequeueing as well as deleting into a queue
that does not exists or using invalid or `NULL` data will report a failure.
Additionally an error occurs when calling `queue_destroy()` on a non-empty
queue and `queue_iterate()` reports an error if it receives a non-existent
queue or a `NULL` function.

### Uthread API
LIBUTHREAD also includes a thread manager for interacting with the thread
library. The uthread API supports the following interactions by an application
thread with the thread library:
* The application/thread seeks to create and/or run new threads.
* The application/thread seeks to get the current thread.
* The application/thread seeks to change the state of itself or other threads.
* The application/thread seeks to yield its processor time.
In order to manage these interactions, the uthread API organizes each of the
current threads into thread data structures (called struct uthread_tcb and
henceforth referred to simply as 'thread control blocks,' or 'TCBs'; see Notes
on Threads). The thread manager maintains a queue (first in, first out) of
thread control blocks to receive processor time.

##### Notes on Threads
A thread is represented in the uthread API by its TCB. A TCB has the following
data members:
* A uthread_ctx_t data member containing the context of the thread (called
  'context').
* A void pointer data member pointing to the stack for the thread (called
  'stack').
* An integer data member representing the state of the thread (called 'state').
There are four possible states for a thread:
1. T_RUNNING: The thread is currently using the processor.
2. T_READY: The thread is ready to have processor time.
3. T_BLOCKED: The thread is currently blocked and cannot receive more processor
   time.
4. T_EXITED: The thread has finished its designated task and cannot receive
more processor time. Additionally, the thread is subject to deletion.
Exited threads are deleted by the idle thread, which executes the function
queue_iterate(), passing the TCB queue and a unique queue_func_t function
called delete_zombies() as parameters. delete_zombies() deletes a thread if its
status is T_EXITED, and queue_iterate() will execute this function on each
thread in the TCB queue.

##### Creating and Running New Threads
**int uthread_run(bool, uthread_func_t, void*):**
Before the application can interact with the thread library, the application
must start the thread manager by executing this function. The function takes
three input parameters:
* A boolean parameter to enable preemption, henceforth called 'preempt'(see
	Peemption API).
* A unthread_func_t parameter containing the function which the initial thread
  will execute, henceforth called 'func'. (see Notes on Threads)
* A void pointer parameter containing the arguments to pass to the initial
  thread's function specified by 'func', henceforth called 'args'.
Upon calling this function, the thread manager initializes the aforementioned
queue of thread control blocks, initializes the initial thread's thread control
block using the 'func' and 'args' parameters, and initializes an idle thread to
remain in an infinite loop, so long as the TCB queue is nonempty (see
uthread_create() for information on thread creation). If 'preempt' is true,
then preemption is enabled before entering the loop. While in this loop, when
the idle thread is scheduled to execute, it checks for and deletes threads of
status T_EXITED using queue_iterate() (see Notes on Threads) and then yields
its processor time. Once every thread has finished executing, the idle thread
exits the loop, ceases preemption, and returns 0 (RET_SUCCESS). Additionally,
if the TCB queue, initial thread, or idle thread fail to initialize, this
function will immediately return with failure code -1 (RET_FAILURE).

**int uthread_create(uthread_func_t, void*):**
This function creates a new thread and enqueues the new thread at the tail of
the queue. Two parameters are taken as input:
* A uthread_func_t parameter containing the function which the initial thread
  will execute, henceforth called 'func'.
* A void pointer parameter containing the arguments to pass to the initial
  thread's function specified by 'func', henceforth called 'args'.
Upon calling this function, a new TCB is allocated for the new thread. The
context and stack data members for this TCB are also allocated. The state of
the thread is then set to T_READY, the thread is enqueued at the tail of the
TCB queue, and the function then returns 0 (RET_SUCCESS). Additionally, if
either the TCB or its data members fail to allocate, the function returns
failure code -1 (RET_FAILURE).

##### Getting the Current Thread
**struct uthread_tcb *uthread_current():**
This function returns a static global data member called 'curr_thread', which
is a pointer to the current thread's TCB.

##### Changing Thread States
**void uthread_exit():**
This function is executed if the current thread completes the function
specified when it was created. The status data member of the current thread's
TCB is set to T_EXITED and the thread yields its processor time.

**void uthread_block():**
This function sets the status data member of the current thread's TCB to
T_BLOCKED and then yields its processor time.
	These test cases perform possible scenario sush as enqueueing and dequeueing many threads within a loop, or modifying the values of the queue with`1queue_iterate()`.
**void uthread_unblock(struct uthread_tcb*):**
This function sets the status data member of the TCB pointed to by the sole
parameter to T_READY.

##### Yielding
**void uthread_yield():**
This function yields the current thread's processor time. Since this is a
critical section of code, preemption is disabled while the code executes. The
status of the current thread is set to T_READY if the current thread is not
blocked or exited, and then is enqueued in the TCB queue. The function then
cycles through the TCB queue until it finds the first thread that is read to
execute, sets its status to T_RUNNING, changes the 'curr_thread' global to
point to this thread, and then performs a context switch to this thread.
Preemption is reenabled just before the context switch occurs.

### Semaphore API
We based our implementation off of the examples shown in class, primarily form
`sem_up()` and `sem_down()`. We define a `semaphore` struct as a data structure
which keeps track of a queue of blocked threads `q_blocked`, as well as an
internal semaphore count. `sem_up()` and `sem_down()` contain critical sections
of code in which they modify `q_blocked`, in order to avoid any data being
modified incorrectly, we enable and disable preemption around the critical
section of code. The implementation of `sem_down()` is as follows, we first
need to make sure that our semaphore exists, then we enter the critical section
and check the current semaphore count. If the count is already at zero, we cant
decrease it any further so the current thread becomes blocked and gets enqueued
to `q_blocked`. If the count is greater than zero, meaning that the semaphore
is available to take, we simply decrease the count and return. For `sem_up()`
we check that the semaphore exists and in the critical section, we unblock or
'wake up' the oldest thread in `q_blocked`. Before we return, we increment the
semaphore count. The use of semaphore allows our thread library to synchronize
thread operations and allows us to keep track of resources in order to
guarantee mutual exclusion when it comes to resource allocation and usage.
There is a corner case where TA calls `sem_down()` on a semaphore with a count
of 0, and gets blocked. TB calls `sem_up()` on the same semaphore, and awakens
TA. However, before TA can run again, TC calls `sem_down()` on the semaphore
and 'snatches' the newly available resource. TA is not reawakened until TC
releases the resource with `sem_up()`.  This can lead to TA being starved if the
semaphore resource is always getting 'snatched' before A can execute, in order to avoid starvation the oldest thread in `q_blocked`  is woken up. 


### Preemption API
Additionally, LIBUTHREAD supports the use of preemptive scheduling to forcibly
yield threads every ten milliseconds. The preemption API includes
implementations for functions to enable, disable, start, and stop the
preemption timer.

##### Starting and Stopping
**void preempt_start(bool):**
If the boolean parameter passed to this function evaluates to false, this
function does nothing. If true, it initializes five global structures: two
timeval structures (included in the header sys/time.h), two sigaction
structures (included in the header signal.h), a sigset_t structure (also
included in the header signal.h), and sets a global boolean called 'enabled' to
true. The initialized timeval structures are used to create a virtual timer
that raises the signal SIGVTALRM every 10ms and to store any former timer
settings. The signal behavior for signal SIGVTALRM is then changed to the
handler alarm_yield() specified by the first sigaction structure, which forces
the current thread to yield, while the old signal behavior is stored in the
second sigaction structure. Additionally, SIGVTALRM is stored in the sigset_t
structure.

**void preempt_stop():**
If 'enabled' is false, this function does nothing. Otherwise, the current timer
is disabled, the default signal behavior and former timer settings are
restored, and each of the five structures allocated in preempt_start() are
freed.

##### Enabling and Disabling
**preempt_disable*():**
If 'enabled' is false, this function does nothing. Otherwise, the signals
within the global sigset_t structure are set to be blocked.
**preempt_enable():**
If 'enabled' is false, this function does nothing. Otherwise, the signals
within the global sigset_t structure are set to be unblocked.
