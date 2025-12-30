#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"


struct cpu cpus[NCPU];

struct proc proc[NPROC];

struct proc *initproc;

int nextpid = 1;
struct spinlock pid_lock;

struct mmap_area map[MAX_MMAP_AREA];

extern void forkret(void);
static void freeproc(struct proc *p);

extern char trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
struct spinlock wait_lock;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
void
proc_mapstacks(pagetable_t kpgtbl)
{
  struct proc *p;
  
  for(p = proc; p < &proc[NPROC]; p++) {
    char *pa = kalloc();
    if(pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((int) (p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
void
procinit(void)
{
  struct proc *p;
  
  initlock(&pid_lock, "nextpid");
  initlock(&wait_lock, "wait_lock");
  for(p = proc; p < &proc[NPROC]; p++) {
      initlock(&p->lock, "proc");
      p->state = UNUSED;
      p->kstack = KSTACK((int) (p - proc));
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
int
cpuid()
{
  int id = r_tp();
  return id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
struct cpu*
mycpu(void)
{
  int id = cpuid();
  struct cpu *c = &cpus[id];
  return c;
}

// Return the current struct proc *, or zero if none.
struct proc*
myproc(void)
{
  push_off();
  struct cpu *c = mycpu();
  struct proc *p = c->proc;
  pop_off();
  return p;
}

int
allocpid()
{
  int pid;
  
  acquire(&pid_lock);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_lock);

  return pid;
}

// nice_to_weight: nice 값에 따른 가중치 하드 코딩
int nice_to_weight[40] = {
  /* 0 ~ 4 */ 88761, 71755, 56483, 46273, 36291,
  /* 5 ~ 9 */ 29154, 23254, 18705, 14949, 11916,
  /* 10 ~ 14 */  9548, 7620, 6100, 4904, 3906,
  /* 15 ~ 19 */  3121, 2501, 1991, 1586, 1277,
  /* 20 ~ 24 */  1024, 820, 655, 526, 423,
  /* 25 ~ 29 */  355, 272, 215, 172, 137,
  /* 30 ~ 34 */  110, 87, 70, 56, 45,
  /* 35 ~ 39 */  36, 29, 23, 18, 15,
};


// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if(p->state == UNUSED) {
      goto found;
    } else {
      release(&p->lock);
    }
  }
  return 0;

found:
  p->pid = allocpid();
  p->state = USED;
  p->nice = 20; // 기본 nice 값
  p->runtime = 0;
  p->vruntime = 0;
  p->vdeadline = p->vruntime + (5000 * 1024) / 1024; // 기본 가상 마감 기한
  p->timeslice = 5; // 기본 타임 슬라이스 값
  p->weight = 1024; // 기본 가중치 값
  p->is_eligible = 0; // eligible 체크용

  // Allocate a trapframe page.
  if((p->trapframe = (struct trapframe *)kalloc()) == 0){
    freeproc(p);
    release(&p->lock);
    return 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  if(p->pagetable == 0){
    freeproc(p);
    release(&p->lock);
    return 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, sizeof(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  return p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
static void
freeproc(struct proc *p)
{
  if(p->trapframe)
    kfree((void*)p->trapframe);
  p->trapframe = 0;
  if(p->pagetable)
    proc_freepagetable(p->pagetable, p->sz);
  p->pagetable = 0;
  p->sz = 0;
  p->pid = 0;
  p->parent = 0;
  p->name[0] = 0;
  p->chan = 0;
  p->killed = 0;
  p->xstate = 0;
  p->state = UNUSED;
}

// Create a user page table for a given process, with no user memory,
// but with trampoline and trapframe pages.
pagetable_t
proc_pagetable(struct proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  if(pagetable == 0)
    return 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  if(mappages(pagetable, TRAMPOLINE, PGSIZE,
              (uint64)trampoline, PTE_R | PTE_X) < 0){
    uvmfree(pagetable, 0);
    return 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  if(mappages(pagetable, TRAPFRAME, PGSIZE,
              (uint64)(p->trapframe), PTE_R | PTE_W) < 0){
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    return 0;
  }

  return pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
void
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmfree(pagetable, sz);
}

// a user program that calls exec("/init")
// assembled from ../user/initcode.S
// od -t xC ../user/initcode
uchar initcode[] = {
  0x17, 0x05, 0x00, 0x00, 0x13, 0x05, 0x45, 0x02,
  0x97, 0x05, 0x00, 0x00, 0x93, 0x85, 0x35, 0x02,
  0x93, 0x08, 0x70, 0x00, 0x73, 0x00, 0x00, 0x00,
  0x93, 0x08, 0x20, 0x00, 0x73, 0x00, 0x00, 0x00,
  0xef, 0xf0, 0x9f, 0xff, 0x2f, 0x69, 0x6e, 0x69,
  0x74, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

// Set up first user process.
void
userinit(void)
{
  struct proc *p;

  p = allocproc();
  initproc = p;
  
  // allocate one user page and copy initcode's instructions
  // and data into it.
  uvmfirst(p->pagetable, initcode, sizeof(initcode));
  p->sz = PGSIZE;

  // prepare for the very first "return" from kernel to user.
  p->trapframe->epc = 0;      // user program counter
  p->trapframe->sp = PGSIZE;  // user stack pointer

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->lock);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint64 sz;
  struct proc *p = myproc();

  sz = p->sz;
  if(n > 0){
    if((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0) {
      return -1;
    }
  } else if(n < 0){
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  return 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *p = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // Copy user memory from parent to child.
  if(uvmcopy(p->pagetable, np->pagetable, p->sz) < 0){
    freeproc(np);
    release(&np->lock);
    return -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  np->nice = p->nice; // 부모의 nice 값을 자식에게 복사
  np->weight = p->weight; // 부모의 weight 값을 자식에게 복사
  np->vruntime = p->vruntime; // 부모의 vruntime 값을 자식에게 복사
  np->runtime = 0;
  np->timeslice = 5;
  np->vdeadline = np->vruntime + (5000 * 1024) / np->weight; // 새로운 vdeadline 계산
  np->is_eligible = 0; // eligible는 재계산해야함

  // increment reference counts on open file descriptors.
  for(i = 0; i < NOFILE; i++)
    if(p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, sizeof(p->name));

  // mmap 영역 복사
  for (int m = 0; m < MAX_MMAP_AREA; m++) {
    // 부모 영역 확인하기
    if (map[m].used == 0 || map[m].p != p) continue;

    for (int i = 0; i < MAX_MMAP_AREA; i++){
      if (map[i].used == 0) break; // 비어있는 공간 찾음
    }

    map[i].f      = map[m].f;
    map[i].addr   = map[m].addr;
    map[i].length = map[m].length;
    map[i].offset = map[m].offset;
    map[i].prot   = map[m].prot;
    map[i].flags  = map[m].flags;
    map[i].p      = np;
    map[i].used   = 1;

    int map_p = 0;
    int prot_r = 0;
    int prot_w = 0;
    if (map[i].flags & MAP_POPULATE) map_p = 1;
    if (map[i].prot & PROT_READ) prot_r = 1;
    if (map[i].prot & PROT_WRITE) prot_w = 1;

    // MAP_POPULATE
    if (map_p == 1) {
        uint64 start = map[i].addr;
        uint64 end   = start + map[i].length;
        
        for (uint64 va = start; va < end; va += PGSIZE) {
          // 부모 PTE 찾기
          pte_t *ppte = walk(p->pagetable, va, 0);

          if (ppte == 0 || (*ppte & PTE_V) == 0)
              continue; // 부모 PTE 못찾음

          uint64 pa = PTE2PA(*ppte); // 부모 물리 주소

            // 새 메모리 할당
          char *mem = kalloc();
          if (mem == 0)
              return 0;

          memmove(mem, (char*)(uint64)pa, PGSIZE); // 부모 내용 복사

          int perm = PTE_V | PTE_U;
          if (prot_r != 0) perm |= PTE_R;
          if (prot_w != 0) perm |= PTE_W;

          int check = mappages(np->pagetable, va, PGSIZE, (uint64)mem, perm);
          
          if (check != 0) return 0;
          }
      }
  }

  pid = np->pid;

  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE;
  release(&np->lock);

  return pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
void
reparent(struct proc *p)
{
  struct proc *pp;

  for(pp = proc; pp < &proc[NPROC]; pp++){
    if(pp->parent == p){
      pp->parent = initproc;
      wakeup(initproc);
    }
  }
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
void
exit(int status)
{
  struct proc *p = myproc();

  if(p == initproc)
    panic("init exiting");

  // Close all open files.
  for(int fd = 0; fd < NOFILE; fd++){
    if(p->ofile[fd]){
      struct file *f = p->ofile[fd];
      fileclose(f);
      p->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(p->cwd);
  end_op();
  p->cwd = 0;

  acquire(&wait_lock);

  // Give any children to init.
  reparent(p);

  // Parent might be sleeping in wait().
  wakeup(p->parent);
  
  acquire(&p->lock);

  p->xstate = status;
  p->state = ZOMBIE;

  release(&wait_lock);

  // 종료전 프로세스 mmap 해제 
  for (int i = 0; i < MAX_MMAP_AREA; i++) {
    if (map[i].used == 1 && map[i].p == p) {
      munmap(map[i].addr);
    }
  }

  // Jump into the scheduler, never to return.
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(uint64 addr)
{
  struct proc *pp;
  int havekids, pid;
  struct proc *p = myproc();

  acquire(&wait_lock);

  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(pp = proc; pp < &proc[NPROC]; pp++){
      if(pp->parent == p){
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->lock);

        havekids = 1;
        if(pp->state == ZOMBIE){
          // Found one.
          pid = pp->pid;
          if(addr != 0 && copyout(p->pagetable, addr, (char *)&pp->xstate,
                                  sizeof(pp->xstate)) < 0) {
            release(&pp->lock);
            release(&wait_lock);
            return -1;
          }
          freeproc(pp);
          release(&pp->lock);
          release(&wait_lock);
          return pid;
        }
        release(&pp->lock);
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || killed(p)){
      release(&wait_lock);
      return -1;
    }
    
    // Wait for a child to exit.
    sleep(p, &wait_lock);  //DOC: wait-sleep
  }
}

// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run.
//  - swtch to start running that process.
//  - eventually that process transfers control
//    via swtch back to the scheduler.
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();

  c->proc = 0;
  for(;;){
    // The most recent process to run may have had interrupts
    // turned off; enable them to avoid a deadlock if all
    // processes are waiting.
    intr_on();

    uint total_weight = 0; //eligible check을 위한 전체 가중치
    uint min_vruntime = 0xFFFFFFFF; //eligible check을 위한 minimum vruntime
    long long left_term = 0; //lag value 대체용
    struct proc *best = 0; //실행될 프로세스
    int fastest = 0; //가장 빨리 끝나는 vdeadline

    //eligible checking
    for(p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      if(p->state == RUNNABLE) {
        if(p->vruntime < min_vruntime) {
          min_vruntime = p->vruntime;
        } //vruntime이 가장 작은 프로세스 찾기
      }
    release(&p->lock);
    }
    if(min_vruntime == 0xFFFFFFFF) {
      continue; //runnable process가 없는 경우
    }

    for(p = proc; p < &proc[NPROC]; p++){
      acquire(&p->lock);
      if(p->state == RUNNABLE) { //runqueue의 프로세스만 고려
        total_weight += p->weight; //전체 가중치 계산
        left_term += (long long)(p->vruntime - min_vruntime) * p->weight; //left term 계산
      }
      release(&p->lock);
    }

    for(p = proc; p < &proc[NPROC]; p++){
      acquire(&p->lock);
      if(p->state == RUNNABLE) { //runqueue의 프로세스
        long long right_term = (long long)(p->vruntime - min_vruntime) * total_weight; //right term 계산
        p->is_eligible = 0;
        if(left_term >= right_term){ //is_eligible
          p->is_eligible = 1;
          if(p->vdeadline < fastest || fastest == 0) {
            fastest = p->vdeadline; //가장 빨리 끝나는 vdeadline 찾기
            best = p; //실행될 프로세스
          }
        }
      }
      release(&p->lock);
    }
    if(best ==0) {
      continue; //runnable process가 없는 경우
    }

    acquire(&best->lock);
    best->state = RUNNING; // best 프로세스를 RUNNING으로
    c->proc = best; // CPU에 프로세스 할당
    swtch(&c->context, &best->context);
    c->proc = 0; // context switch가 끝나면 CPU 할당 해제
    best->is_eligible = 0; // eligible 초기화

    release(&best->lock);
  }
}

// Switch to scheduler.  Must hold only p->lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->noff, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&p->lock))
    panic("sched p->lock");
  if(mycpu()->noff != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;
  swtch(&p->context, &mycpu()->context);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  struct proc *p = myproc();
  acquire(&p->lock);
  p->state = RUNNABLE;
  sched();
  release(&p->lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
void
forkret(void)
{
  static int first = 1;

  // Still holding p->lock from scheduler.
  release(&myproc()->lock);

  if (first) {
    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    fsinit(ROOTDEV);

    first = 0;
    // ensure other cores see first=0.
    __sync_synchronize();
  }

  usertrapret();
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  // Must acquire p->lock in order to
  // change p->state and then call sched.
  // Once we hold p->lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup locks p->lock),
  // so it's okay to release lk.

  acquire(&p->lock);  //DOC: sleeplock1
  release(lk);

  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  release(&p->lock);
  acquire(lk);
}

// Wake up all processes sleeping on chan.
// Must be called without any p->lock.
void
wakeup(void *chan)
{
  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++) {
    if(p != myproc()){
      acquire(&p->lock);
      if(p->state == SLEEPING && p->chan == chan) {
        p->state = RUNNABLE;
        p->timeslice = 5000; // 타임 슬라이스 초기화
        p->vdeadline = p->vruntime + (5000 * 1024) / p->weight; // 새로운 vdeadline 계산
        p->is_eligible = 0; // eligible 재계산
      }
      release(&p->lock);
    }
  }
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
int
kill(int pid)
{
  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++){
    acquire(&p->lock);
    if(p->pid == pid){
      p->killed = 1;
      if(p->state == SLEEPING){
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->lock);
      return 0;
    }
    release(&p->lock);
  }
  return -1;
}

void
setkilled(struct proc *p)
{
  acquire(&p->lock);
  p->killed = 1;
  release(&p->lock);
}

int
killed(struct proc *p)
{
  int k;
  
  acquire(&p->lock);
  k = p->killed;
  release(&p->lock);
  return k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
int
either_copyout(int user_dst, uint64 dst, void *src, uint64 len)
{
  struct proc *p = myproc();
  if(user_dst){
    return copyout(p->pagetable, dst, src, len);
  } else {
    memmove((char *)dst, src, len);
    return 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
int
either_copyin(void *dst, int user_src, uint64 src, uint64 len)
{
  struct proc *p = myproc();
  if(user_src){
    return copyin(p->pagetable, dst, src, len);
  } else {
    memmove(dst, (char*)src, len);
    return 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [USED]      "used",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  struct proc *p;
  char *state;

  printf("\n");
  for(p = proc; p < &proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    printf("%d %s %s", p->pid, state, p->name);
    printf("\n");
  }
}

// getnice: 프로세스의 nice value 반환
int
getnice(int pid)
{
  if (pid <= 0)
    return -1;
    
  struct proc *p;
  int target; // 찾는 프로레스의 nice 값 저장

  for(int i = 0; i < NPROC; i++) {
    p = &proc[i]; // 프로세스에 대한 포인터
    acquire(&p->lock);
    if(p->pid == pid && p->state != UNUSED) {
      target = p->nice;
      release(&p->lock);
      return target;
    }
    release(&p->lock);
  }

  return -1; // 프로세스를 못 찾은 경우
}

// setnice: 프로세스의 nice value 설정
int
setnice(int pid, int value)
{
  if(value < 0 || value > 39)
    return -1; //nice value가 범위를 벗어난 경우

  struct proc *p;
  
  for (int i = 0; i < NPROC; i++) {
    p = &proc[i]; 
    acquire(&p->lock); 

    if (p->pid == pid && p->state != UNUSED) {
      p->nice = value; // nice 값을 value 값으로 설정  
      p->weight = nice_to_weight[value]; // nice 값에 따른 가중치 설정   
      p->vdeadline = p->vruntime + (5000 * 1024) / p->weight; // 새로운 vdeadline 계산
      release(&p->lock); 
      return 0;    
    }
    release(&p->lock);
  }

  return -1;
}


//ps: 프로세스 정보 출력
void
ps(int pid)
{
  struct proc *p;


  static char *states[] = {
    [UNUSED]    = "unused",
    [USED]      = "used",
    [SLEEPING]  = "sleeping",
    [RUNNABLE]  = "runnable",
    [RUNNING]   = "running",
    [ZOMBIE]    = "zombie"
  };

  if(pid == 0) { // 모든 프로세스 정보 출력

    acquire(&tickslock);
    uint current_ticks = ticks; //total tick 계산
    release(&tickslock);

    printf("NAME    PID     STATE          PRIORITY    RUNTIME/WEIGHT       RUNTIME       VRUNTIME       VDEADLINE       IS_ELIGIBLE       TICK     %d\n" , current_ticks * 1000);
    printf("-----------------------------------------------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < NPROC; i++) {
      p = &proc[i]; 
      acquire(&p->lock);
      if (p->state == UNUSED) {
        release(&p->lock);
        continue;
      }
      
      char *state;
      if (p->state >= 0 && p->state < NELEM(states) && states[p->state])
        state = states[p->state]; // 프로세스 상태 저장
      else
        state = "???";
      
      int namelen = strlen(p->name);
      printf("%s", p->name);
      for(int i = 0; i < 8 - namelen; i++) {
        printf(" ");
      }
      int pidlen = 0;
      int temp = p->pid;
      while(temp > 0) {
        temp /= 10;
        pidlen++;
      }
      printf("%d", p->pid);
      for(int i = 0; i < 8 - pidlen; i++) {
          printf(" ");
        }
      int statelen = strlen(state);
      printf("%s", state);
      for(int i = 0; i < 18 - statelen; i++) {
          printf(" ");
        }
      int nicelen = 0;
      temp = p->nice;
      if(p->nice == 0) {
        nicelen = 1;
      }
      else {
        while(temp > 0) {
          temp /= 10;
          nicelen++;
        }
      }
      printf("%d", p->nice);
      for(int i = 0; i < 16 - nicelen; i++) {
        printf(" ");
      }
      int share = p->runtime / p->weight;
      int share_len = 0;
      temp = share;
      if (share == 0) {
        share_len = 1;
      }
      else {
        while(temp > 0) {
          temp /= 10;
          share_len++;
        }
      }
      printf("%d", share);
      for(int i = 0; i < 16 - share_len; i++) {
        printf(" ");
      }
      int run_len = 0;
      temp = p->runtime;
      if(p->runtime == 0) {
        run_len = 1;
      }
      else {
        while(temp > 0) {
          temp /= 10;
          run_len++;
        }
      }
      printf("%d", p->runtime);
      for(int i = 0; i < 16 - run_len; i++) {
        printf(" ");
      }
      int vruntime_len = 0;
      temp = p->vruntime;
      if(p->vruntime == 0) {
        vruntime_len = 1;
      }
      else {
        while(temp > 0) {
          temp /= 10;
          vruntime_len++;
        }
      }
      printf("%d", p->vruntime);
      for(int i = 0; i < 14 - vruntime_len; i++) {
        printf(" ");
      }
      int vdeadline_len = 0;
      temp = p->vdeadline;
      if(p->vdeadline == 0) {
        vdeadline_len = 1;
      }
      else {
        while(temp > 0) {
          temp /= 10;
          vdeadline_len++;
        }
      }
      printf("%d", p->vdeadline);
      for(int i = 0; i < 18 - vdeadline_len; i++) {
        printf(" ");
      }
      if(p->is_eligible == 1) {
        printf("true");
      } else {
        printf("false");
      }
      printf("\n");

      release(&p->lock);
    }
  } else { // 특정 pid의 프로세스 정보 출력
    acquire(&tickslock);
    uint current_ticks = ticks; //total tick 계산
    release(&tickslock);
    
    printf("NAME    PID     STATE          PRIORITY    RUNTIME/WEIGHT       RUNTIME       VRUNTIME       VDEADLINE       IS_ELIGIBLE       TICK     %d\n" , current_ticks * 1000);
    printf("-----------------------------------------------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < NPROC; i++) {
      p = &proc[i];
      acquire(&p->lock);
      if (p->pid == pid && p->state != UNUSED) {
        char *state;
        if (p->state >= 0 && p->state < NELEM(states) && states[p->state])
          state = states[p->state];
        else
          state = "???";

        int namelen = strlen(p->name);
        printf("%s", p->name);
        for(int i = 0; i < 8 - namelen; i++) {
            printf(" ");
          }
        int pidlen = 0;
        int temp = p->pid;
        while(temp > 0) {
          temp /= 10;
          pidlen++;
          }
        printf("%d", p->pid);
        for(int i = 0; i < 8 - pidlen; i++) {
            printf(" ");
          }
          int statelen = strlen(state);
          printf("%s", state);
          for(int i = 0; i < 18 - statelen; i++) {
              printf(" ");
            }
          int nicelen = 0;
          temp = p->nice;
          if(p->nice == 0) {
            nicelen = 1;
          }
          else {
            while(temp > 0) {
              temp /= 10;
              nicelen++;
            }
          }
          printf("%d", p->nice);
          for(int i = 0; i < 16 - nicelen; i++) {
            printf(" ");
          }
          int share = p->runtime / p->weight;
          int share_len = 0;
          temp = share;
          if (share == 0) {
            share_len = 1;
          }
          else {
            while(temp > 0) {
              temp /= 10;
              share_len++;
            }
          }
          printf("%d", share);
          for(int i = 0; i < 16 - share_len; i++) {
            printf(" ");
          }
          int run_len = 0;
          temp = p->runtime;
          if(p->runtime == 0) {
            run_len = 1;
          }
          else {
            while(temp > 0) {
              temp /= 10;
              run_len++;
            }
          }
          printf("%d", p->runtime);
          for(int i = 0; i < 16 - run_len; i++) {
            printf(" ");
          }
          int vruntime_len = 0;
          temp = p->vruntime;
          if(p->vruntime == 0) {
            vruntime_len = 1;
          }
          else {
            while(temp > 0) {
              temp /= 10;
              vruntime_len++;
            }
          }
          printf("%d", p->vruntime);
          for(int i = 0; i < 14 - vruntime_len; i++) {
            printf(" ");
          }
          int vdeadline_len = 0;
          temp = p->vdeadline;
          if(p->vdeadline == 0) {
            vdeadline_len = 1;
          }
          else {
            while(temp > 0) {
              temp /= 10;
              vdeadline_len++;
            }
          }
          printf("%d", p->vdeadline);
          for(int i = 0; i < 18 - vdeadline_len; i++) {
            printf(" ");
          }
          if(p->is_eligible == 1) {
            printf("true");
          } else {
            printf("false");
          }
          printf("\n");
    
          release(&p->lock);
        return;
      }
      release(&p->lock);
    }
  }
}

struct run {
  struct run *next;
};

extern struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;// kalloc.c 전체를 포함하면 오류 발생, kmem 구조체만 선언

// meminfo: 가능한 메모리 바이트로 출력
uint64
meminfo(void)
{
  uint64 remain_mem = 0;
  struct run *free;
  
  acquire(&kmem.lock);
  free = kmem.freelist;
  while(free != 0){
    remain_mem += PGSIZE;
    free = free->next;
  }
  
  release(&kmem.lock);
  
  return remain_mem;
}


// waitpid: 지정한 pid의 자식 프로세스가 종료될 때까지 대기
int
waitpid(int pid)
{
  struct proc *pp; //자식 프로세스
  int foundkid;
  struct proc *p = myproc(); //부모 프로세스
  
  acquire(&wait_lock);  // 부모 자식간의 동기화
  
  for(;;){
    foundkid = 0;
    for(pp = proc; pp < &proc[NPROC]; pp++){
      if(pp->parent == p && pp->pid == pid){
        acquire(&pp->lock);
        foundkid = 1;
        if(pp->state == ZOMBIE){
          freeproc(pp); // 자식 프로세스 메모리 해제
          release(&pp->lock);
          release(&wait_lock);
          return 0;
        }
        release(&pp->lock);
      }
    }
    
    // 자식이 없거나 부모 프로세스 종료됨
    if(!foundkid || killed(p)){
      release(&wait_lock);
      return -1;
    }
    
    // 자식은 존재하지만 아직 종료되지 않은 경우 대기
    sleep(p, &wait_lock);
  }
}

//mmap: 메모리 매핑
uint64
mmap(uint64 addr, int length, int prot, int flags, int fd, int offset)
{
  struct proc *p = myproc();
  struct file *f = 0;
  uint64 start = MMAPBASE + addr;
  int map_a = 0; // MAP_ANONYMOUS
  int map_p = 0; // MAP_POPULATE
  int prot_r = 0; // PROT_READ
  int prot_w = 0; // PROT_WRITE

  // page length 검사
  if (length <= 0) return 0;
  
  if ((length % PGSIZE) != 0) return 0;

  // flag 설정
  if (flags & MAP_ANONYMOUS) map_a = 1;

  if (flags & MAP_POPULATE) map_p = 1;

  // not anonymous but fd is -1
  if (map_a != 1 && fd == -1) return 0;
  
  // prot 설정
  if (prot & PROT_READ) prot_r = 1;
  
  if (prot & PROT_WRITE) prot_w = 1;

  // 파일 열기
  if (fd != -1) f = p->ofile[fd];

  // protection of the file and the prot of the parameter are different
  if (f != 0) {
    if (f->readable == 0 && prot_r != 0) return 0;
    if (f->writable == 0 && prot_w != 0) return 0;
    f = filedup(f);
  }

  int i = 0;
  
  // 빈 슬롯 찾기
  while (i < MAX_MMAP_AREA && map[i].used != 0) i++;

  // 배열 안에 빈 슬롯이 없을 때
  if (i >= MAX_MMAP_AREA) return 0;

  // 빈 슬롯에 할당
  map[i].f = f;
  map[i].addr = start;
  map[i].length = length;
  map[i].offset = offset;
  map[i].prot = prot;
  map[i].flags = flags;
  map[i].p = p;
  map[i].used = 1;

  // MAP_ANONYMOUS 일 때
  if (map_p == 0) return start;

  // MAP_POPULATE 일 때
  if (map_p == 1) {
    if (f) f->off = offset;

    // pysical memory 할당
    uint64 ptr;
    uint64 end = start + length;
    for (ptr = start; ptr < end; ptr += PGSIZE) {
      char *mem = kalloc();
      if (mem == 0) { // 메모리 부족할 때
        map[i].used = 0;
        return 0;
      }
      memset(mem, 0, PGSIZE); // 0으로 메모리 초기화

      // 페이지 테이블 매핑
      int perm = PTE_V | PTE_U;
      if (prot_r != 0) perm |= PTE_R;
      if (prot_w != 0) perm |= PTE_W;

      // 페이지 테이블에 매핑
      int check = mappages(p->pagetable, ptr, PGSIZE, (uint64)mem, perm);

      // 매핑 실패
      if (check != 0) return 0;

      // 페이지 읽기 (실제 데이터 복사)
      if (f) readi(f -> ip, 0, (uint64)mem, (ptr - start) + f->off, PGSIZE);
      
    }
    return start;
  }
  return 0;
}

//munmap: 메모리 매핑 해제
int
munmap(uint64 addr)
{
  struct proc *p = myproc();
  int idx = -1;

  // 프로세스 매핑 찾기
  for (int i = 0; i < MAX_MMAP_AREA; i++) {
    if (map[i].used == 0) continue;
    if (map[i].p != p) continue;
    if (map[i].addr != addr) continue;
    idx = i;
    break;
  }
  if (idx == -1)  
    return -1;

  // 페이지 할당 해제
  for (int off = 0; off < map[idx].length; off += PGSIZE) {
    uint64 va = map[idx].addr + off; // 가상 주소
    uint64 *pte = walk(p->pagetable, va, 0);
    if (pte == 0) continue;
    if ((*pte & PTE_V) == 0) continue; // 페이지가 없는 경우
    if (PTE2PA(*pte) == 0) continue; 

    uint64 pa = PTE2PA(*pte); 

    kfree((void*)pa);
    *pte = 0;
    }


  map[idx].f = 0;
  map[idx].addr = 0;
  map[idx].length = 0;
  map[idx].offset = 0;
  map[idx].prot = 0;
  map[idx].flags = 0;
  map[idx].p = 0;
  map[idx].used = 0;

  return 1;
}

// freemem: 자유 페이지 수 반환
int
freemem(void)
{
  extern int free_pages;  // kalloc.c
  return free_pages;
}


