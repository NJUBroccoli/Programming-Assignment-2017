#include "common.h"
#include "syscall.h"
#include "string.h"

extern uint32_t _end;
extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern void ramdisk_write(const void *buf, off_t offset, size_t len);
int i;

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);
  	
//  printf("eax:0x%x;ecx:0x%x;edx:0x%x;ebx:0x%x;esi:0x%x;edi:0x%x;esp:0x%x;ebp:0x%x\n",r->eax,r->ecx,r->edx,r->ebx,r->esi,r->edi,r->esp,r->ebp);

  switch (a[0]) {
	case SYS_none: SYSCALL_ARG1(r)=1; break;
	case SYS_exit: //printf("eax:0x%x; ecx:0x%x; edx:0x%x; ebx:0x%x\n",r->eax,r->ecx,r->edx,r->ebx);
				   _halt(SYSCALL_ARG2(r));break;
	case SYS_write: //printf("eax:0x%x; ecx:0x%x; edx:0x%x; ebx:0x%x\n",r->eax,r->ecx,r->edx,r->ebx);break;
				   if (SYSCALL_ARG2(r)==1 || SYSCALL_ARG2(r)==2){
					   SYSCALL_ARG1(r)=0;
					   for (; SYSCALL_ARG1(r) < SYSCALL_ARG4(r); SYSCALL_ARG1(r)++){
						   char *tmp=(char*)(SYSCALL_ARG3(r));
						   _putc(tmp[SYSCALL_ARG1(r)]);
					   }
				   }
				   break;
	case SYS_brk:  _heap.end = (void*)SYSCALL_ARG2(r);//printf("end:0x%x\n",_heap.end); 
				   SYSCALL_ARG1(r)=0; break;

	case SYS_open: for (i=0;i<NR_FILES;i++){
					   if (strcmp((char*)SYSCALL_ARG2(r),file_table[i].name)==0)
					     SYSCALL_ARG1(r)=i;
					     break;
				   }
				   if (i==NR_FILES) assertion(0);
				   break;
	case SYS_close:SYSCALL_ARG1(r)=0; 
				   break;
	case SYS_read: if (SYSCALL_ARG4(r)<=file_table[SYSCALL_ARG1(r)].size){
					  ramdisk_read(SYSCALL_ARG3(r),file_table[SYSCALL_ARG1(r)].disk_offset,SYSCALL_ARG4(r));
				   }
				   else
					  ramdisk_read(SYSCALL_ARG3(r),file_table[SYSCALL_ARG1(r)].disk_offset,file_table[SYSCALL_ARG1(r)].size);
				   SYSCALL_ARG1(r)=SYSCALL_ARG4(r);
				   break;
	case SYS_write:
				   
				   break;
	case SYS_lseek:break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
