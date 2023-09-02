#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
int
sys_getpname(void)
{
	int pid;
	
	if(argint(0, &pid) < 0)
		return -1;
	return getpname(pid);
}
int
sys_ps(void)
{
	int pid;

	if(argint(0, &pid) < 0) //xv6에서 자체 개발한 함수 선언한 함수의 파라미터를 넘겨준다 0으로 첫번째, 1로 두번째 파라미터를 pid에 넘겨주는 형태
		return -1;
	ps(pid);
	return 1;
}


int
sys_getnice(void)
{
	int pid;

	if(argint(0, &pid) < 0)
		return -1;
	return getnice(pid);
}

int
sys_setnice(void)
{
	int pid;
	int value;

	if(argint(0, &pid) < 0 || argint(1,&value) < 0)
		return -1;
	return setnice(pid,value);
}

int
sys_mmap(void)
{

	int lenth, prot, flags, fd ,offset;
	int addr;

	if(argint(0, &addr) < 0 || argint(1, &lenth) <0 || argint(2,&prot) <0 || argint(3,&flags) <0 )
		return -1;
	if(argint(4,&fd) < 0)
		fd = -1;
	if(argint(5,&offset)<0)
		offset = 0;
	return mmap((uint)addr,lenth,prot,flags,fd,offset);
}
int
sys_munmap(void)
{
	int addr;

	if(argint(0, &addr) < 0)
		return -1;
	
	return munmap(addr);
}
int
sys_freemem(void)
{
	return freemem();
}