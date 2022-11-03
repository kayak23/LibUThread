# USER-LEVEL THREAD LIBRARY
Authors: , Rafael Millan

## Abstract
Our User-Level Thread Libray provides a complete interface for
applications to create and run independent threads concurrently at user-level

Our Library allows the user to:
- Create new threads.
- User Round-Robin to schedule threads with a 100Hz timer.
	- Preemtion is used to provide an interrupt based scheduler.
- Syncronize resource usage among threads with semaphores.

## queue API
We implemented our queue using a doubuly linked list. We defined a `queue`
structure as a linked list of `q_node` data structures with a void pointer
containing the node data. Additionally, each node contains pointer to its next
and previous element in the queue. In this case, each node prepresents a thread
in our program. Our `queue` struct keeps track of the head and tail pointers of
the queue, allowinf us to implement the `enqueue` and `dequeue` operations
in O(1). Error handling ensures that valid arguments are passed into the queue
library funrctions. Enqueuing or dequeueing as well as deleting into a queue
that does not exists or using invalid or `NULL` data will report a failure.
Additionally an error occurs when calling `queue_destroy()` on a non-empty
queue and `queue_iterate()` reports an error if it receives a non-existent
queue or a `NULL` function.

## uthread API

## semaphore API
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
semaphore count. The use of semaphore allows our thread library to syncronize
thread operations and allows us to keep track of resources in order to
guarantee mutual exclusion when it comes to resource allocation and usage.
There is a corner case where TA calls `sem_down()` on a semaphore with a count
of 0, and gets blocked. TB calls `sem_up()` on the same semaphore, and awakens
TA. However, before TA can run again, TC calls `sem_down()` on the semaphore
and 'snatches' the newly available resource. TA is not reawakened until TC
releases the resource with `sem_up()`. This can lead to TA being starved if the
semaphore resource is always getting 'snaTched' before A can execute. 

## preemption
