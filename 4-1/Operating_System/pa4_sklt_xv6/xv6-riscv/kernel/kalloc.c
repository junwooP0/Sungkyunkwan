// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"


void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.


#define BLKS_PER_PG (PGSIZE/BSIZE) // 페이지 당 블록 수
#define SWAP_PAGES (SWAPMAX / BLKS_PER_PG) // 스왑 영역이 담을 수 있는 페이지 수

int swap_used[SWAP_PAGES]; // block 관리: 1은 사용 중, 0은 사용 가능

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

// pa4: struct for page control
struct page pages[PHYSTOP/PGSIZE];
struct page *page_lru_head;
int num_free_pages;
int num_lru_pages;



void
kinit()
{
  initlock(&kmem.lock, "kmem");

  // 1) lru head 선언
  page_lru_head = &pages[0];
  // lru 초기화
  page_lru_head->next = page_lru_head;
  page_lru_head->prev = page_lru_head;

  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  // lru 리스트에서 페이지 제거
  int idx = ((uint64)pa - KERNBASE) / PGSIZE;
  lru_remove(&pages[idx]);
  num_free_pages++;

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);

  // 페이지 구조체 초기화
  pages[idx].pagetable = 0;
  pages[idx].vaddr     = 0;
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
// pa4: kalloc function
void *
kalloc(void) {
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if (r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(!r){
    r = (struct run *)swap(); // 스왑
    if(r == 0){
      // 실패 시 OOM 출력
      printf("error: out of memory\n");
      return 0;
    }
  }

  if (r) {
      //page 상태 초기화
      memset((char*)r, 0, PGSIZE);
      int idx = ((uint64)r - KERNBASE) / PGSIZE;
      pages[idx].next = 0;
      pages[idx].prev = 0;
      pages[idx].pagetable = 0;
      pages[idx].vaddr = 0;
  }
  return (void*)r;
}



// lru 리스트에서 페이지 제거
void
lru_remove(struct page *p) {

  if (p == page_lru_head) return;

  if (p->next && p->prev) {
    p->prev->next = p->next;
    p->next->prev = p->prev;
    p->next = p->prev = 0;
    num_lru_pages--;
  }
}

// lru 리스트에 페이지 추가
void
lru_add(struct page *p) {
  if(p == page_lru_head) return;

  if (p->next || p->prev) lru_remove(p); //이미 있으면 제거

  p->next = page_lru_head;
  p->prev = page_lru_head->prev;
  page_lru_head->prev->next = p;
  page_lru_head->prev = p;
  num_lru_pages++;
}


// victim page 선택
struct page*
choose_victim(void) {
  struct page *p = page_lru_head->next;
  
  // LRU 리스트 돌기
  while (p != page_lru_head) {
    pte_t *entry = walk(p->pagetable, (uint64)p->vaddr, 0); // PTE 찾기
    if (entry) {
      if (*entry & PTE_A) { // 접근 비트가 1인 경우
        *entry &= ~PTE_A;
        p = p->next;
        continue;
      }
    }
    return p; // 접근 비트가 0인 경우 희생
  }
  return 0;
}

// 스왑 아웃을 위한 페이지 할당
void *
swap(void)
{
  struct page *victim = choose_victim();

  uint64 va  = (uint64)victim->vaddr;  // 가상 주소
  pte_t *pte = walk(victim->pagetable, va, 0);

  uint64 pa = PTE2PA(*pte); // 물리 주소

  //스왑 블록 번호 할당
  int blkno = -1;
  for (int b = 0; b < SWAP_PAGES; b++) {
      if (swap_used[b] == 0) {
          swap_used[b] = 1;
          blkno = b;
          break;
      }
  }
  if (blkno < 0) {
      blkno = 0; // 빈 공간 없으면 0번 사용
  }
  swapwrite(va, blkno); // 스왑 영역에 기록

  // PTE 갱신
  uint64 flags = PTE_FLAGS(*pte) & ~PTE_V;
  *pte = ((uint64)blkno << 10) | flags;

  sfence_vma(victim->pagetable, va);   // TLB 플러시

  lru_remove(victim);

  return (void*)(pa);
}
