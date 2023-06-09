#ifndef __RISCV_H_
#define __RISCV_H_

#include "types.h"

#define KERNBASE 0x80200000L
#define PHYSTOP (KERNBASE + 128*1024*1024 - 0x00200000L)

#define TRAMPOLINE (MAXVA - PGSIZE)

#define PGSIZE 4096 // bytes per page
#define PGSHIFT 12  // bits of offset within a page

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))

#define SSTATUS_SPP (1L << 8)  // Previous mode, 1=Supervisor, 0=User
#define SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#define SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#define SSTATUS_SIE (1L << 1)  // Supervisor Interrupt Enable
#define SSTATUS_UIE (1L << 0)  // User Interrupt Enable

#define SIE_SEIE (1L << 9) // external
#define SIE_STIE (1L << 5) // timer
#define SIE_SSIE (1L << 1) // software


#define PTE_V (1L << 0) // valid
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4) // user can access


// shift a physical address to the right place for a PTE.
#define PA2PTE(pa) ((((uint64)pa) >> 12) << 10)

#define PTE2PA(pte) (((pte) >> 10) << 12)

#define PTE_FLAGS(pte) ((pte) & 0x3FF)

// extract the three 9-bit page table indices from a virtual address.
#define PXMASK          0x1FF // 9 bits
#define PXSHIFT(level)  (PGSHIFT+(9*(level)))
#define PX(level, va) ((((uint64) (va)) >> PXSHIFT(level)) & PXMASK)

#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))  


#define SATP_SV39 (8L << 60)

#define MAKE_SATP(pagetable) (SATP_SV39 | (((uint64)pagetable) >> 12))

typedef uint64 pte_t;
typedef uint64* pagetable_t ;

static inline void 
w_satp(uint64 x)
{
  asm volatile("csrw satp, %0" : : "r" (x));
}


static inline void
sfence_vma()
{
  // the zero, zero means flush all TLB entries.
  asm volatile("sfence.vma zero, zero");
}

static inline uint64
r_sie()
{
  uint64 x;
  asm volatile("csrr %0, sie" : "=r" (x) );
  return x;
}

static inline void 
w_sie(uint64 x)
{
  asm volatile("csrw sie, %0" : : "r" (x));
}


static inline uint64 
r_tp(){
    uint64 x;
    asm volatile(
        "mv %0 ,tp\n"
        :"=r" (x)
    );
    return x;
}

static inline uint64
r_sstatus(){
    uint64 x;
    asm volatile(
        "csrr %0 ,sstatus\n"
        :"=r"(x)
    );
    return x;
}

static inline void 
w_sstatus(uint64 x ){
    asm volatile(
        "csrw sstatus , %0\n"
        :
        :"r"(x)
    );
}

static inline void
intr_on(){
    w_sstatus(r_sstatus() | SSTATUS_SIE);
}

static inline void
intr_off(){
    w_sstatus(r_sstatus() & ~SSTATUS_SIE);
}

static inline int
intr_get(){
    uint64 x = r_sstatus();
    return (x & SSTATUS_SIE) != 0;
}

static inline void
set_spie(){
    //uint64 x = r_sstatus();
    w_sstatus(r_sstatus() | SSTATUS_SPIE);
}

static inline void 
w_stvec(uint64 x){
    asm volatile(
        "csrw stvec , %0\n"
        :
        :"r" (x) 
    );
}

static inline void
w_mtvec(uint64 x){
    asm volatile(
        "csrw mtvec , %0\n"
        :
        :"r"(x)
    );
}

static inline void
w_sscratch(uint64 x){
    asm volatile(
        "csrw sscratch, %0 \n"
        :
        :"r" (x)
    );

}
static inline uint64 
r_sscratch(){
    uint64 x;
    asm volatile(
        "csrr %0,sscratch \n"
        :"=r" (x)
    );
    return x;
}

static inline uint64
r_scause(){
    uint64 x;
    asm volatile(
        "csrr %0, scause \n"
        :"=r" (x)
    );
    return x;
}

static inline uint64
r_stval(){
    uint64 x;
    asm volatile(
        "csrr %0, stval \n"
        :"=r" (x)
    );
    return x;
}

static inline uint64
r_sepc(){
    uint64 x;
    asm volatile(
        "csrr %0 ,sepc \n"
        :"=r" (x)
    );
    return x;
}
static inline void
w_sepc(uint64 x){
    asm volatile(
        "csrw sepc, %0 \n"
        :
        :"r" (x)
    );
} 

static inline uint64 
r_time(){
    uint64 x;
    asm volatile(
        "csrr %0,time\n"
        :"=r"(x)
    );
    return x;
}
#endif 