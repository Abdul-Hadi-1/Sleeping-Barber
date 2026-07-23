# Sleeping Barber — Linux Kernel System Call

An implementation of the classic Sleeping Barber synchronisation problem written as a **custom system call inside the Linux kernel**, built for the Operating Systems module at the National University of Computer and Emerging Sciences (FAST-NUCES), Karachi.

Rather than solving the problem in userspace, the barber and customers run as **kernel threads** coordinated by **kernel semaphores**, with all output written to the kernel log.

## The problem

A barber shop has one barber, one barber's chair, and a waiting room with a limited number of seats. If there are no customers, the barber sleeps. A customer who arrives to find the barber asleep wakes him; a customer who arrives while the barber is busy takes a waiting-room seat, or leaves if every seat is taken. The challenge is coordinating arrivals and the barber without deadlock or lost wake-ups.

This implementation runs 1 barber, 5 customers, and 3 waiting-room seats.

## How it works

Four kernel semaphores coordinate the threads:

| Semaphore | Initial value | Purpose |
|---|---|---|
| `waiting` | 3 (number of chairs) | Counting semaphore limiting occupancy of the waiting room |
| `barber_Chair` | 1 | Mutual exclusion over the barber's chair — one customer at a time |
| `barber_sleeping` | 1 | Signals the barber to wake when a customer arrives |
| `barber_seatbelt` | 1 | Holds the customer in the chair until the haircut is finished |

Each participant runs as its own kernel thread, created with `kthread_create` and started with `wake_up_process`. The barber thread loops, blocking on `barber_sleeping` until woken, servicing a customer, then signalling completion through `barber_seatbelt`. Customer threads acquire a waiting-room seat, then the barber's chair, wake the barber, and wait to be released.

## Implementation details

- **Custom system call** — exposed as `asmlinkage long sys_SleepingBarber(void)`, added to the kernel source and syscall table, requiring a full kernel rebuild
- **Kernel threads** — `kthread_create` / `wake_up_process`, not userspace pthreads
- **Kernel semaphores** — `struct semaphore` with `sema_init`, `down()` and `up()`
- **Kernel logging** — all trace output via `printk`, readable with `dmesg`
- **Timing** — `msleep` simulates travel time, haircut duration, and arrival staggering

## Building and running

This is kernel source, not a userspace program — it cannot be compiled with `gcc` on its own.

1. Place the source in the Linux kernel tree and register the call in the syscall table
2. Add the prototype to the kernel headers
3. Rebuild and install the kernel, then reboot into it
4. Invoke it from a small userspace program:

```c
#include <unistd.h>
#include <sys/syscall.h>

int main(void) {
    syscall(<SYSCALL_NUMBER>);   // number assigned in the syscall table
    return 0;
}
```

5. View the output:

```bash
dmesg | tail -40
```

Developed and tested on Ubuntu Linux.

## Notes

University project for the Operating Systems module; I was elected team leader.
