/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
boot_paddr   	   = 0x007c00;		/* boot offset */
kernel_paddr       = 0x100000;
kernel_vaddr_off   = 0xffffffff00000000;
kernel_vaddr 	   = kernel_vaddr_off + kernel_paddr;
kernel_pos   	   = 0x001000;		/* physical position in output */
kernel_stack_paddr = 0x009fffff;
kernel_size 	   = SIZEOF(.kernel);
user_vaddr 	   = 0x100000;

kdata_base	   = 0x00200000;
idt_paddr	   = 0x00201000;
percpu_paddr	   = 0x00202000;
percpu_tbl_paddr   = 0x00202000;
gdt_paddr 	   = 0x00203000;
page_table_paddr   = 0x00400000;
user_paddr	   = 0x00600000;

SECTIONS
{
	.boot-setup boot_paddr :
	{
		*(.boot)
		_setup_paddr = .;
		*(.setup)
	}

	.kernel kernel_vaddr : AT(boot_paddr + kernel_pos)
	{
		*.ko(.text)
		*.ko(.data*)
		*.ko(.rodata*)
		*.ko(COMMON*)
		*.ko(.bss*)
	}

	.user user_vaddr + SIZEOF(.kernel): AT(LOADADDR(.kernel) + SIZEOF(.kernel))
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
