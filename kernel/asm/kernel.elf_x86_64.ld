/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
ENTRY(_start)
SECTIONS
{
	. = 0x100000;

	.setup :
	{
		*(.setup)
	}

	.text ALIGN (4K):
	{
		*(.text)
	}

	.data ALIGN (4K) :
	{
		*(.data)
		*(.rodata*)
	}

	.bss ALIGN (4K) :
	{
		*(COMMON*)
		*(.bss*)
	}
}
