/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
boot_paddr   = 0x007c00;		/* boot offset */
kernel_vaddr = 0x100000;		/* virtual memory address of main() */
kernel_pos   = 0x001000;		/* physical position in output */
kernel_off   = - kernel_vaddr + kernel_pos + boot_paddr;
SECTIONS
{
	. = boot_paddr;

	.boot :
	{
		*(.boot)
	}

	.setup :
	{
		_setup_paddr = ADDR(.setup);
		*(.setup)
	}

	. = kernel_vaddr;
	.text : AT(ADDR(.text) + kernel_off)
	{
		*(.text)
	}
 
	.data : AT(ADDR(.data) + kernel_off)
	{
		*(.data)
		*(.rodata*)
	}

	.bss : AT(ADDR(.bss) + kernel_off)
	{
		*(COMMON*)
		*(.bss*)
	}

	/DISCARD/ :
	{
		*(.eh_frame)
		*(.comment)
	}
}
