/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
boot_paddr   = 0x007c00;		/* boot offset */
kernel_paddr = 0x100000;
kernel_vaddr = 0xffffffff00100000;

kernel_pos   = 0x001000;		/* physical position in output */
/* kernel_off   = - kernel_vaddr + kernel_pos + boot_paddr; */

page_table_paddr   = 0x00200000;
kernel_stack_paddr = 0x005fffff;
kernel_size = SIZEOF(.kernel);

user_vaddr = 0x100000;

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

	.user user_vaddr : AT(LOADADDR(.kernel) + SIZEOF(.kernel))
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
