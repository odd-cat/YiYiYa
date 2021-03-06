/*******************************************************************
 * Copyright 2021-2080 evilbinary
 * 作者: evilbinary on 01/01/20
 * 邮箱: rootdebug@163.com
 ********************************************************************/
#include "syscall.h"

#include "device.h"
#include "kernel/stdarg.h"
#include "devcallfn.h"
#include "syscallfn.h"

static void* syscall_table[SYSCALL_NUMBER] = {
    &sys_read, &sys_write, &sys_yeild, &sys_print, &sys_print_at, &sys_ioctl,
    &sys_open, &sys_close, &dev_read,  &dev_write, &dev_ioctl,&sys_exec,&sys_test};

INTERRUPT_SERVICE
void syscall_handler() {
  interrupt_entering_code(ISR_SYSCALL, 0);
  interrupt_process(do_syscall);
  interrupt_exit();
}

extern context_t* current_context;
void syscall_init() { interrutp_regist(ISR_SYSCALL, syscall_handler); }

void* do_syscall(interrupt_context_t* context) {
  // kprintf("syscall %x\n",context.no);
  // current_context->esp0 = context;
  // current_context->esp = context->esp;

  if (context->eax >= 0 && context->eax < SYSCALL_NUMBER) {
    void* fn = syscall_table[context->eax];
    if (fn != NULL) {
      sys_fn_call((context), fn);
      return context->eax;
    } else {
      kprintf("syscall %x not found\n", context->eax);
    }
  }
  return NULL;
}

void* syscall0(u32 num) {
  int ret;
  asm("int %1" : "=a"(ret) : "i"(ISR_SYSCALL), "0"(num));
  return ret;
}

void* syscall1(u32 num, void* arg0) {
  int ret;
  asm volatile("int %1\n" : "=a"(ret) : "i"(ISR_SYSCALL), "0"(num), "b"(arg0));
  return ret;
}
void* syscall2(u32 num, void* arg0, void* arg1) {
  int ret;
  asm volatile("int %1\n"
               : "=a"(ret)
               : "i"(ISR_SYSCALL), "0"(num), "b"(arg0), "c"(arg1));
  return ret;
}
void* syscall3(u32 num, void* arg0, void* arg1, void* arg2) {
  u32 ret = 0;
  asm volatile("int %1\n"
               : "=a"(ret)
               : "i"(ISR_SYSCALL), "0"(num), "b"(arg0), "c"(arg1), "d"(arg2));
  return ret;
}
