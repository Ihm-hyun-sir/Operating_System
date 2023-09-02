// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "proc.h"


void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld
char bitmap[4096];

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
} kmem;

struct page pages[PHYSTOP/PGSIZE];
struct page *page_lru_head;
struct spinlock lru_lock;
int num_free_pages;
int num_lru_pages;

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
    kfree(p);
}
//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
  {
	  panic("kfree");
  }

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.

int
find_bit(char num)
{
        int shift = 0x1;
	int count = 0;
        while(count<8)
        {
                if(num & shift)
                {
			shift = shift << 1;
			count++;
		}
		else
			break;
        }
	if(count ==8)
		count--;
	return count;
}
int
swap_out(void)
{
	int reference_bit = 0;
        pte_t *pte;
        int i = 0;
	int index = 0;
	if(!num_lru_pages)
	{
		cprintf("Out of Memory \n");
		return 0;
	}
        while(1)
        {
                pte = walkpgdir(page_lru_head->pgdir , (void*)page_lru_head->vaddr , 0 );
		reference_bit = (*pte)&PTE_A;
                if(reference_bit)
                {
                        *pte &= ~PTE_A;
                        page_lru_head = page_lru_head->next;
                }
                else
                {
                        for(i = 0; i < 4096 ; i++)
                        {
                                if(bitmap[i] != 0xFFFFFFFF)
                                        break;
                        }
			if(i == 4096)
			{
				return 0;
			}
                        index = i*8 + find_bit(bitmap[i]);
			bitmap[i] = bitmap[i] | (1 << index%8);
			swapwrite(page_lru_head->vaddr , index );
			kfree(P2V(PTE_ADDR(*pte)));
			delete_lru_list(page_lru_head);
			uint flags = PTE_FLAGS(*pte);
			uint front_pte = (uint)index << 12;
			*pte = 0;
			*pte = front_pte|flags;
			*pte &= ~PTE_P;
			break;	
                }

        }
        return 1;
}

char*
kalloc(void)
{
  struct run *r;

try_again:
  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(!r)
  {
	release(&kmem.lock);
  	if(swap_out())
  		goto try_again;
  }
  if(r)
    kmem.freelist = r->next; 
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

struct page* 
allocpage()
{
	for(int i = 0 ; i < PHYSTOP/PGSIZE ; i++)
	{
		if(pages[i].state == 0)
		{
			pages[i].state = 1;
			return &pages[i];
		}
	}
	return 0;
}

void
append_lru_list(uint va, pde_t* pgdir)
{
	struct page* newpage = allocpage();
	if(newpage == 0)
                cprintf("error\n");
	newpage->vaddr = (char*)va;
	newpage->pgdir = pgdir;
	num_lru_pages++;
	if(num_lru_pages == 1)
	{
		page_lru_head = newpage;
		page_lru_head -> prev = newpage;
		page_lru_head -> next = newpage;
		newpage->next = page_lru_head;
		newpage->prev = page_lru_head;
		return;
	}
	newpage->prev = page_lru_head->prev;
	newpage->next = page_lru_head;
	page_lru_head -> prev -> next = newpage;
	page_lru_head -> prev = newpage;
	return;
}
void
delete_lru_list(struct page* erase)
{
        erase->prev->next = erase->next;
        erase->next->prev = erase->prev;
        if(erase == page_lru_head)
                page_lru_head = erase->next;

        erase->state = 0;
        erase->next = 0;
        erase->prev = 0;
        erase->vaddr =0;
        erase->pgdir=0;

        num_lru_pages--;
}
int
swap_in(pde_t* pgdir,uint vaddr)
{
	uint va = PGROUNDDOWN(vaddr);
	pde_t *pde;
	pde = &pgdir[PDX(va)];
	if(*pde & PTE_P)
	{
		pte_t* pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
		pte_t* pte = &pgtab[PTX(va)];
		uint index =(uint)*pte >> 12;
		uint mod = index%8;
		uint test = 1 << mod;
		if( bitmap[index/8]&test )
		{
			if(bitmap[index/8] == 0xFFFFFFFF)
				bitmap[index/8] = 0xFF;
			char* v = kalloc();
			swapread(v ,index );
			uint flags = PTE_FLAGS(*pte);
			*pte = 0;
			*pte =  (uint)V2P(v)| flags  | PTE_P;
			append_lru_list(va,pgdir);
			bitmap[index/8] &= ~test;
		}
		else
			return 0;
	}
	else
		return 0;
	return  1;

}
