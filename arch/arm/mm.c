/*******************************************************************
 * Copyright 2021-2080 evilbinary
 * 作者: evilbinary on 01/01/20
 * 邮箱: rootdebug@163.com
 ********************************************************************/
#include "../mm.h"

#include "../cpu.h"

static mem_block_t* block_head = NULL;
static mem_block_t* block_tail = NULL;
static mem_block_t* blocks = NULL;

extern boot_info_t* boot_info;


void map_page(void* physaddr, void* virtualaddr, u32 flags) {
  
}

void mm_init() {

  // mm init
  // mm_alloc_init();

  mm_test();
}

void mm_alloc_init(){
  memory_info_t* first_mem = (memory_info_t*)&boot_info->memory[0];
  mem_block_t* blocks = (mem_block_t*)first_mem->base+0x10;
  u32 size = sizeof(mem_block_t) * boot_info->memory_number;
  for (int i = 0; i < boot_info->memory_number; i++) {
    memory_info_t* mem = (memory_info_t*)&boot_info->memory[i];
    if(mem->type!=1){
      continue;
    }
    // skip
    u32 addr=mem->base;
    u32 len=mem->length;
    if(0x500>mem->base&& mem->base<0x4000){
      addr=0x4000;
      len=mem->base-addr;
    }
    blocks[i].addr = addr;
    blocks[i].size = len;
    blocks[i].type = MEM_FREE;
    blocks[i].next = NULL;
  }
  blocks[0].addr = blocks[0].addr + size;
  blocks[0].size = blocks[0].size - size;
  for (int i = 0; i < (boot_info->memory_number - 1); i++) {
    blocks[i].next = &blocks[i + 1];
    if(i>0){
      blocks[i].prev=&blocks[i - 1];
    }else{
      blocks[i].prev=NULL;
    }
  }
  block_head=blocks;
}

void* mm_alloc(size_t size) {
  mem_block_t* p = block_head;
  for (; p != NULL; p = p->next) {
    if ((p->type == MEM_FREE)) {
      if (p->size == size) {
        p->type = MEM_USED;
        return p->addr;
      } else if ((size+sizeof(mem_block_t)) < p->size) {
        mem_block_t* b=(mem_block_t*)p->addr;
        u32 rest=p->size-size-sizeof(mem_block_t);
        b->size=size;
        b->addr=b+sizeof(mem_block_t);
        b->type=MEM_USED;
        mem_block_t* prev=p->prev;
        if(prev!=NULL){
          prev->next=b;
        }else{
          block_head=b;
        }
        p=b->addr+size;
        p->size=rest;
        p->addr=p+sizeof(mem_block_t);
        p->prev=b;
        p->type=MEM_FREE;
        b->next=p;
        return b->addr;
      }
    }
  }
  kprintf("erro malloc end");
  return NULL;
}

void mm_free(void* addr) {
  mem_block_t* p = block_head;
  for (; p != NULL; p = p->next) {
    if (p->addr == addr) {
      p->type = MEM_FREE;
      // todo merging mem
      break;
    }
  }
}

u32 mm_get_size(void* addr) {
  mem_block_t* p = block_head;
  for (; p != NULL; p = p->next) {
    if (p->addr == addr) {
      return p->size;
    }
  }
}

void mm_test(){
    // map_page(0x90000,0x600000,3);
    // u32* addr=mm_alloc(256);
    // *addr=0x123456;
}
