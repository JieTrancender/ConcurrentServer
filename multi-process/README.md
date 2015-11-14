一、进程概念与特征

  1 概念
  
    (1) 进程是一个实体，每一个进程都有它自己的地址空间，一般情况下，包括文本区域、数据区域和堆栈。文本区域储存处理器执行的代码;数据区域储存变量和进程执行期间使用的动态分配的内存;堆栈区域储存活动过程调用的指令和本地变量
    
    (2) 进程是一个“执行中的程序”，程序是一个没有生命的实体，只有处理器赋予程序生命时，它才能成为一个活动的实体，我们称其为进程
    
  2 特征
  
    (1)动态性：进程的实质是程序的一次执行过程，进程是动态产生，动态消亡的
    
    (2)并发性：任何进程都可以同其他进程一起并发执行
    
    (3)独立性：进程是一个能独立运行的基本单位，同时也是系统分配资源和调度的独立单位
    
    (4)异步性：由于进程间的相互制约，使进程具有执行的间断性，即进程按各自独立的不可预知的速度向前推进
    
    (5)结构特征：进程由程序、数据和进程控制块三部分组成
    
二、创建进程

  1 fork()函数 - 创建新进程 -- [DemoFirst](demo-first.cpp)
  
    函数原型：
    
      #include <sys/types.h>
      
      #include <unistd.h>
      
      pid_t fork(void);
      
      父进程中返回子进程的进程ID，子进程返回0，出错时返回-1
      
    注：
    
      1) fork执行后，子进程和父进程继续执行fork()函数后的指令。子进程是父进程的副本。子进程拥有父进程的数据空间、堆栈副本，但父、子进程并不共享这些储存空间部分。不过代码段是只读的，则父、子进程共享代码段。如果父、子进程同时对统一文件描述字操作，而没有任何形式的同步，则会出现混乱的状况
      
      2) 父进程中调用fork之前打开的所有描述字在函数fork返回之后子进程回得到一个副本。fork执行后，父、子进程均需要将自己不使用的描述字关闭。防止出现不同步的情况和保证最后能正常关闭描述字
      
  2 vfork()函数 - 创建新进程 -- [DemoSecond](demo-second.cpp)
  
    函数原型：
    
      #include <sys/types.h>
      
      #include <unistd.h>
      
      pid_t vfork(void);
      
      父进程中返回子进程的进程ID，子进程返回0，出错时返回-1
      
    注：
      1) 在BSD3.0中开始出现，主要是为了解决fork昂贵的开销。它是完全共享的创建，新老进程共享同样的资源，完全没有拷贝
      
      2) 两者的区别在于当使用vfork()创建新进程时，父进程暂时阻塞，而子进程则可以借用父进程的地址空间。当子进程推出或调用execve()函数时父进程才继续执行
      
三、终止进程

  1 wait()函数 - 等待一个子进程返回并修改状态 -- [DemoThird](demo-third.cpp)
  
    函数原型：
    
      #include <sys/types.h>
      
      #include <sys/wait.h>
      
      pid_t wait(int *stat_loc);
      
      参数stat_loc存储子进程的终止状态
      
      成功返回子进程的ID;出错返回-1;
      
    注：
      1) 如果没有终止的子进程，但有一个或多个正在执行的子进程，则该函数阻塞，直到有一个子进程终止或者wait被信号中断，wait返回。当系统调用该函数时，如果由一个子进程已经结束，则该系统调用立即返回，并释放子进程所有资源
      
      2) 由于UNIX信号不排队，在SIGCHLD信号同时到来后，信号处理程序中调用了wait函数，其只执行一次，将留下其它的僵尸进程，可以使用waitpid函数解决这个问题
  
  2 waitpid()函数 - 等待指定进程的子进程返回，并修改状态 -- [DemoFourth](demo-fourth.cpp)  [DemoFifth](demo-fifth.cpp)
  
    函数原型：
    
      #include <sys/types.h
      
      #include <sys/wait.h>
      
      pid_t waitpid(pid_t pid, int *stat_loc, int option);
      
      第一个参数pid：-1表示要求直到任何一个子进程的返回状态(等待第一个终止的子进程)，> 0表示要求知道进程号为pid的子进程状态，< 0表示要求直到进程组号为pid的绝对值的子进程状态
      
      第二个参数stat_loc：存储子进程的终止状态
      
      第三个参数options：WUNTRACED表示如果进程进入暂停执行情况则马上返回，但结束状态不予理会，WNOHANG表示如果没有任何已经结束的子进程则马上返回，不予等待
      
      成功返回进程的ID，出错返回-1
      
三、代码实例：

  The first [Demo](demo-first.cpp) with fork function for multi process program .  
  The second [Demo](demo-second.cpp) with vfork function for multi process program .  
  The third [Demo](demo-third.cpp) with wait function for multi process program .
  The fourth [Demo](demo-fourth.cpp) with wait function for multi process program .
  The fifth [Demo](demo-fifth.cpp) with wait function for multi process program .


