/*******************************************************************
* Copyright 2021-2080 evilbinary
* 作者: evilbinary on 01/01/20
* 邮箱: rootdebug@163.com
********************************************************************/
#include <malloc.h>

//implements 
void *malloc(unsigned long size){
	// return kmalloc(size);
	return NULL;
}

void free(void *addr){
	// kfree(addr);
}
void *realloc (void *block,unsigned long size){
	void *p;
	//vesa_printf(0,300,"realloc %x",block);
	//for(;;);	
	// size+=mem_get_size(block);
	// p=malloc(size);
	// memcpy(p,block);
	// free(block);
	return p;
}
