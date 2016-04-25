/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
boot_paddr   	   = 0x007c00;		/* boot offset */
kernel_paddr       = 0x100000;
kernel_vaddr_off   = 0xffffffff00000000;
kernel_vaddr 	   = kernel_vaddr_off + kernel_paddr;
kernel_pos   	   = 0x002000;		/* physical position in output */
/* kernel_stack_paddr = 0xa00000; */
kernel_size 	   = SIZEOF(.kernel);
user_vaddr 	   = 0x600000;
user_size	   = SIZEOF(.user);
user_pos 	   = kernel_pos + _user_start;
kdata_base	   = 0x00200000;
idt_paddr	   = 0x00201000;
percpu_paddr	   = 0x00203000;
percpu_tbl_paddr   = 0x00203000;
gdt_paddr 	   = 0x00204000;
page_table_paddr   = 0x00400000;
page_table_size	   = 0x00200000;
user_paddr	   = page_table_paddr + page_table_size;

cpu_stack_size     = 0x400;

SECTIONS
{
	.boot-setup boot_paddr :
	{
		*(.boot)
		setup_pos = .;
		*(.setup)
	}

	.kernel kernel_vaddr : AT(boot_paddr + kernel_pos)
	{
		*.ko(.text)
		*.ko(.data*)
		*.ko(.rodata*)
		*.ko(COMMON*)
		*.ko(.bss*)
		/*
		 * We need to align the start of user to 512 so that we can load
		 * it cleanly as an ATA LSB 
		 */
		_user_start = ALIGN(512) - kernel_vaddr;
	}

	.user user_vaddr : AT(LOADADDR(.kernel) + _user_start)
	{
		*.o(.text)
		*.o(.data*)
		*.o(.rodata*)
		*.o(COMMON*)
		*.o(.bss*)
	}

	/DISCARD/ :
	{
		*(.eh_frame)
		*(.comment)
	}
}
