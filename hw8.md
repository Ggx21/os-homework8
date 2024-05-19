# 作业8

郭高旭 2021010803

仓库地址：[os-homework8](https://github.com/Ggx21/os-homework8)

## 测试方法

我选择了posix库。

```bash
g++ -o process_switch process_switch.cpp
g++ -pthread -o thread_switch thread_switch.cpp
python3 test.py
```

## 测试结果

|   Process Count   | Process Avg Time (ms) | Process Jitter (ms)  | Process Avg Memory (KB)  |   Thread Count   | Thread Avg Time (ms) | Thread Jitter (ms)  | Thread Avg Memory (KB)  |
|-------------------|-----------------------|----------------------|--------------------------|------------------|----------------------|---------------------|-------------------------|
|        10         |         3.684         |        0.367         |          3980.8          |        10        |         1.964        |        0.180        |         4108.8          |
|        50         |        10.977         |        0.188         |          3980.8          |        50        |         2.693        |        0.119        |         4147.2          |
|       100         |        20.419         |        0.386         |          3968.0          |       100        |         3.569        |        0.080        |         4211.2          |
|       200         |        38.172         |        0.901         |          3955.2          |       200        |         5.752        |        0.476        |         5056.0          |
|       500         |        92.383         |        1.962         |          3968.0          |       500        |        10.723        |        0.304        |         7449.6          |
|      1000         |       179.196         |        2.157         |          3955.2          |      1000        |        19.881        |        0.695        |        11584.0          |

## 测试结果分析

1. **Process Switch vs. Thread Switch**:
   - 通过比较进程切换和线程切换的平均时间和内存使用情况，可以看出线程切换的平均时间要比进程切换的平均时间低，这表明线程切换的开销比进程切换小。
   - 然而，线程切换的内存使用量似乎要高于进程切换，这可能是由于线程共享了更多的资源，如堆栈空间。

2. **影响因素分析**:
   - 随着进程或线程数量的增加，平均时间呈现出明显的增长趋势。这表明在大规模并发的情况下，系统的性能可能会受到较大的影响。
   - 随着进程或线程数量的增加，时间的波动（jitter）也会增加，这表明系统的稳定性可能会下降。

3. **内存使用量**:
   - 随着进程或线程数量的增加，内存使用量也呈现出增加的趋势。这可能是由于每个进程或线程需要分配的资源增加，导致总体内存使用量的增加。

## POSIX库源码分析

### 线程控制块（TCB）数据结构

在`pthread.h`中，并没有直接给出TCB的数据结构定义，因为TCB是操作系统内核的一部分，通常不会在用户空间的库中定义。不过可以看到一些与线程创建和管理相关的数据结构，例如`pthread_t`和`pthread_attr_t`。

```c
/* Data structure for thread identifiers.  */
typedef unsigned long int pthread_t;

/* Data structure for attributes of threads.  */
typedef struct __pthread_attr_s
{
  int detachstate;
  struct sched_param schedparam;
  int inheritsched;
  int scope;
  size_t guardsize;
  int stackaddr_set;
  void *stackaddr;
  size_t stacksize;
} pthread_attr_t;
```

`pthread_t`是线程标识符，用于唯一标识一个线程。`pthread_attr_t`结构体则包含了线程的属性，如分离状态（detachstate）、调度参数（schedparam）、继承调度策略（inheritsched）、作用域（scope）、守护页大小（guardsize）、栈地址（stackaddr）和栈大小（stacksize）等。

### 线程切换过程

```c
/* Wait for thread TH to terminate.  */
extern int pthread_join(pthread_t __th, void** __thread_return);

/* Indicate that the thread TH is never to be joined with PTHREAD_JOIN.
   The resources of TH will therefore be freed immediately when it
   terminates, instead of waiting for another thread to perform PTHREAD_JOIN
   on it.  */
extern int pthread_detach(pthread_t __th);
```

`pthread_join`函数用于等待一个线程终止，而`pthread_detach`则用于标记一个线程，使其在终止时立即释放资源，而不是等待其他线程通过`pthread_join`来回收资源。这些操作可能会触发线程切换，尤其是当一个线程终止或被阻塞时，调度器会选择另一个线程来运行。

线程切换通常发生在以下情况下：

- 当前线程执行完毕（通过`pthread_exit`）。
- 当前线程被阻塞（例如等待锁或I/O操作）。
- 当前线程的执行时间片用完。
- 调度器根据优先级或其他调度策略决定切换线程。

在内核中，线程切换涉及到保存当前线程的状态（如寄存器、栈指针等）到TCB中，并从另一个线程的TCB中恢复其状态。

由于线程切换是由操作系统内核管理的，具体的实现细节会依赖于具体的操作系统。`pthread.h`提供的是用户空间的接口，用于与内核中的线程管理机制交互。