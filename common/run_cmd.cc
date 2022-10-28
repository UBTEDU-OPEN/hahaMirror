#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "logging.h"

static void waitChildCb(int signo) {
  int status;
  while (waitpid(-1, &status, WNOHANG) > 0)
    ;
}

void runCmd(std::string cmd) {
  pid_t ret;

  signal(SIGCHLD, waitChildCb);
  if ((ret = fork()) < 0) {
    LOG_ERROR("fork fail. ErrNo[{}], ErrMsg[{}]\n", errno, strerror(errno));
  } else if (0 == ret) {
    execl("/bin/sh", "sh", "-c", cmd.c_str(), NULL);
    exit(0);
    //  可以结束掉子进程,那么程序将不会再运行最后一行的printf,原因在于:fork之后的代码父子进程都可见都会执行,通过if可以控制父子进程进行执行不同的内容,原理在于fork不同于其他函数他返回两个值,给父进程返回的是子进程的pid,给子进程自己返回的是0,失败返回
    //  -1,于是当在if中碰到exit自然子进程就结束了
  } else {
    // LOG_DEBUG("** parent process run. pid:[{}], ppid:[{}],ret:[{}]  **\n",
    // getpid(), getppid(), ret);
  }
}
