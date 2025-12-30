#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "sleeplock.h"
#include "fs.h" 
#include "file.h"

struct spinlock tickslock;
uint ticks;

extern char trampoline[], uservec[], userret[];

extern struct mmap_area map[MAX_MMAP_AREA];

// in kernelvec.S, calls kerneltrap().
void kernelvec();

extern int devintr();

void
trapinit(void)
{
  initlock(&tickslock, "time");
}

// set up to take exceptions and traps while in the kernel.
void
trapinithart(void)
{
  w_stvec((uint64)kernelvec);
}


//page fault 발생 시 호출
int
page_fault_handler(uint64 va, int is_write)
{
    struct proc *p = myproc();
    uint64 page = PGROUNDDOWN(va); // va를 해당 페이지의 시작 주소로 내림

    int idx = -1;

    // 프로세스 매핑 찾기
    for (int i = 0; i < MAX_MMAP_AREA; i++) {
      if (map[i].used == 0) continue;
      if (map[i].p != p) continue;
      uint64 start = map[i].addr;
      uint64 end   = start + map[i].length;
      if (va < start || va >= end) continue;
      idx = i; // 매핑 찾음
      break;
    }
    if (idx == -1) return -1;  // 매핑 실패

    int prot_r = 0;
    int prot_w = 0;
    if (map[idx].prot & PROT_READ)  prot_r = 1;
    if (map[idx].prot & PROT_WRITE) prot_w = 1;

    // 쓰기 금지 검사
    if (is_write && prot_w == 0) return -1;

    char *mem = kalloc();
    if (mem == 0){
        map[idx].used = 0;
        return -1;
    }
    memset(mem, 0, PGSIZE);

    // 페이지 테이블 매핑
    int perm = PTE_V | PTE_U; 
    if (prot_r != 0) perm |= PTE_R;
    if (prot_w != 0) perm |= PTE_W;
    int check = mappages(p->pagetable, page, PGSIZE, (uint64)mem, perm);
    if (check != 0) return -1;

    // 파일 매핑인 경우 디스크에서 사용자한테 데이터 복사
    if (map[idx].f) readi(map[idx].f -> ip, 0, (uint64)mem, map[idx].f->off, PGSIZE);

    return 1;
}

//
// handle an interrupt, exception, or system call from user space.
// called from trampoline.S
//
void
usertrap(void)
{
  int which_dev = 0;

  if((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");

  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  w_stvec((uint64)kernelvec);

  struct proc *p = myproc();
  
  // save user program counter.
  p->trapframe->epc = r_sepc();
  
  if(r_scause() == 8){
    // system call

    if(killed(p))
      exit(-1);

    // sepc points to the ecall instruction,
    // but we want to return to the next instruction.
    p->trapframe->epc += 4;

    // an interrupt will change sepc, scause, and sstatus,
    // so enable only now that we're done with those registers.
    intr_on();

    syscall();
  } else if (r_scause() == 13 || r_scause() == 15) {
    uint64 va = r_stval();
    int is_write = (r_scause() == 15);
    int r = page_fault_handler(va, is_write);
    if (r != 1) setkilled(p); 
    else {
      usertrapret();
      return;
    }
  }

   else if((which_dev = devintr()) != 0){
    // ok
  } else {
    printf("usertrap(): unexpected scause 0x%lx pid=%d\n", r_scause(), p->pid);
    printf("            sepc=0x%lx stval=0x%lx\n", r_sepc(), r_stval());
    setkilled(p);
  }

  if(killed(p))
    exit(-1);

  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2)
    yield();

  usertrapret();
}

//
// return to user space
//
void
usertrapret(void)
{
  struct proc *p = myproc();

  // we're about to switch the destination of traps from
  // kerneltrap() to usertrap(), so turn off interrupts until
  // we're back in user space, where usertrap() is correct.
  intr_off();

  // send syscalls, interrupts, and exceptions to uservec in trampoline.S
  uint64 trampoline_uservec = TRAMPOLINE + (uservec - trampoline);
  w_stvec(trampoline_uservec);

  // set up trapframe values that uservec will need when
  // the process next traps into the kernel.
  p->trapframe->kernel_satp = r_satp();         // kernel page table
  p->trapframe->kernel_sp = p->kstack + PGSIZE; // process's kernel stack
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp();         // hartid for cpuid()

  // set up the registers that trampoline.S's sret will use
  // to get to user space.
  
  // set S Previous Privilege mode to User.
  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
  x |= SSTATUS_SPIE; // enable interrupts in user mode
  w_sstatus(x);

  // set S Exception Program Counter to the saved user pc.
  w_sepc(p->trapframe->epc);

  // tell trampoline.S the user page table to switch to.
  uint64 satp = MAKE_SATP(p->pagetable);

  // jump to userret in trampoline.S at the top of memory, which 
  // switches to the user page table, restores user registers,
  // and switches to user mode with sret.
  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
  ((void (*)(uint64))trampoline_userret)(satp);
}

// interrupts and exceptions from kernel code go here via kernelvec,
// on whatever the current kernel stack is.
void 
kerneltrap()
{
  int which_dev = 0;
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();
  
  if((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");
  if(intr_get() != 0)
    panic("kerneltrap: interrupts enabled");

  if((which_dev = devintr()) == 0){
    // interrupt or trap from an unknown source
    printf("scause=0x%lx sepc=0x%lx stval=0x%lx\n", scause, r_sepc(), r_stval());
    panic("kerneltrap");
  }

  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2 && myproc() != 0)
    yield();

  // the yield() may have caused some traps to occur,
  // so restore trap registers for use by kernelvec.S's sepc instruction.
  w_sepc(sepc);
  w_sstatus(sstatus);
}

void
clockintr()
{
  if(cpuid() == 0){
    acquire(&tickslock);
    ticks++;
    wakeup(&ticks);
    release(&tickslock);
  }

  // 현재 실행 중인 프로세스 업데이트
  struct proc *p = myproc();
  if(p != 0 && p->state == RUNNING) {

    // 틱 단위 runtime
    p->runtime += 1000;

    // vruntime 계산
    p->vruntime += (1000 * 1024) / p->weight;
    // run되면서 타임 슬라이스 감소
    p->timeslice = p->timeslice - 1000;

    // 타임 마감 된 경우
    if(p->timeslice == 0) {
      // 새로운 vdeadline 계산, 타임 슬라이스 초기화
      p->vdeadline = p->vruntime + (5000 * 1024) / p->weight;
      p->timeslice = 5000;
      yield();
    }

  }

  // ask for the next timer interrupt. this also clears
  // the interrupt request. 1000000 is about a tenth
  // of a second.
  w_stimecmp(r_time() + 100000); //과제에 따라 수정
}

// check if it's an external interrupt or software interrupt,
// and handle it.
// returns 2 if timer interrupt,
// 1 if other device,
// 0 if not recognized.
int
devintr()
{
  uint64 scause = r_scause();

  if(scause == 0x8000000000000009L){
    // this is a supervisor external interrupt, via PLIC.

    // irq indicates which device interrupted.
    int irq = plic_claim();

    if(irq == UART0_IRQ){
      uartintr();
    } else if(irq == VIRTIO0_IRQ){
      virtio_disk_intr();
    } else if(irq){
      printf("unexpected interrupt irq=%d\n", irq);
    }

    // the PLIC allows each device to raise at most one
    // interrupt at a time; tell the PLIC the device is
    // now allowed to interrupt again.
    if(irq)
      plic_complete(irq);

    return 1;
  } else if(scause == 0x8000000000000005L){
    // timer interrupt.
    clockintr();
    return 2;
  } else {
    return 0;
  }
}

