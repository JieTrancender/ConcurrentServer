一、同一进程中的线程共享如下内容：

  1 全局变量
  
  2 堆数据
  
  3 打开的文件描述符
  
  4 当前工作目录
  
  5 用户及用户组ID
  
二、每一个线程具有独立的内容：

 1 线程ID
 
 2 堆栈
 
 3 errno变量
 
 4 优先级
 
三、线程函数调用

  1 pthread_create()函数 - 创建线程
  
    函数原型：
    
      #include <pthread.h>
      
      int pthread_create(pthread_t *tid, const pthread_attr_t *attr, void *(*func)(void *), void *arg);
      
      第一个参数为指向线程标识符的指针
      
      第二个参数用来设置线程属性
      
      第三个参数是线程运行函数的起始地址
      
      第四个参数是运行函数的参数
      
      成功时返回 0， 出错时返回错误码
      
  2 pthread_join()函数 - 挂起当前线程直到所等待的线程结束，与进程的waitpid()函数功能类似，是一个线程阻塞函数
  
    函数原型：
    
      #include <pthread.h>
      
      int pthread_join(pthread_t thread, void **retval);
      
      第一个参数为被等待的线程标识符
      
      第二个参数为一个用户自定义指针，可以用来储存被等待线程的返回值
      
      成功时返回 0，出错时返回错误码：EINVAL表示线程ID无效，ESRCH表示无法找到相应ID的线程
      
    注：一个线程不能被多个线程等待
    
  3 pthread_detach()函数 - 将一个线程设置为“可分离”的
  
    函数原型：
    
      #include <pthread.h>
      
      int pthread_detach(pthread_t tid);
      
      参数为被设置为“可分离”的线程ID
      
      成功返回 0，出错时返回错误码
      
  4 pthread_exit()函数 - 终止当前线程，并返回状态值，如果当前线程是可联合的，其退出状态将保留供pthread_join()函数调用
  
    函数原型：
    
      #include <pthread.h>
      
      void pthread_exit(void *status);
      
      参数status指向线程的退出状态
      
    注：如果main函数返回，或者当前进程中，任一线程调用了exit()函数，將终止进程中所有线程
    
  5 pthread_self()函数 - 返回当前线程的ID，类似进程中的getpid()函数
  
    函数原型：
    
      #include <pthread.h>
      
      pthread_t pthread_self(void");
      
      返回当前线程的ID
      
  6 pthread_cancel()函数 - 终止指定线程的执行
  
    函数原型：
    
      #include <pthread.h>
      
      int pthread_cancel(pthread_t tid);
      
      参数为被终止线程的ID
      
      成功时返回 0，出错时返回错误码
      
四、给新线程传递参数

  1 传递参数的普通方法
  
    用于线程产生的pthread_create()函数只能允许传递给执行函数一个参数，所以，当需要传递多个数据时，应先将这些数据封装在一个结构中，再将该结构传递给执行的函数
    
  2 通过指针传递参数
  
    在C语言中，传递给函数的参数是被拷贝到函数的执行堆栈中，可利用这一特点使新线程获得参数拷贝，主线程将要传递的数据转换成通用指针类型，然后传递给新线程，新线程再将接收的参数转换成原数据类型
    
  3 通过分配arg的空间传递参数
  
    主线程先为每个新线程分配储存arg的空间，再将arg传递给新线程，新线程使用完后释放arg空间
