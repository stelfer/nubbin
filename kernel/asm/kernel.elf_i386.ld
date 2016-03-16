
OUTPUT_FORMAT("elf32-i386")
ENTRY(_start)

SECTIONS
{
	. = 0x100000;

	.multiboot :
	{
		*(.multiboot)
	}

	.setup :
	{
		*(.setup)
	}

	. += 0xC0000000;

	.text : AT(ADDR(.text) - 0xC0000000)
	{
		*(.text)
	}

	.data ALIGN (4K) : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
		*(.rodata*)
	}

	.bss ALIGN (4K) : AT(ADDR(.bss) - 0xC0000000)
	{
		*(COMMON*)
		*(.bss*)
	}
}
