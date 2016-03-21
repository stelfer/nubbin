/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
ENTRY(pm_start)
SECTIONS
{
	. = 0x1000e8;

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
