/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
ENTRY(_start)
SECTIONS
{
	. = 0x100000;

	.setup :
	{
		*(.setup)
	}

	.text :
	{
		*(.text)
	}

	.data :
	{
		*(.data)
		*(.rodata*)
	}

	.bss :
	{
		*(COMMON*)
		*(.bss*)
	}
}
