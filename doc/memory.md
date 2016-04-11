## Memory Layout

### Virtual Memory

Memory is mapped in 2MiB pages.

| Start  | Stop | Purpose |
| --- |:--- | :--- |
| ``0000 0000 0000 0000`` | ``0000 7fff ffff ffff`` | Application space
| ``0000 8000 0000 0000`` | ``ffff 7fff ffff ffff`` | Memory hole from [48:63] sign extension
| ``ffff 8000 0000 0000`` | ``ffff ffff ffff ffff`` | Canonical upper half
| ``ffff 8000 0000 0000`` | ``ffff ff7e ffff ffff`` | Reserved
| ``ffff ff7f 0000 0000`` | ``ffff fffe ffff ffff`` | Reserved
| ``ffff ffff 0000 0000`` | ``ffff ffff 2000 0000`` | Kernel code


  PM4L[000]->PDP[000]->PD[000]: 0x0000000000000000
  PM4L[511]->PDP[511]->PD[508]: 0x0000000000000000



### Physical Memory

The first 512 of the image is the boot sector and is loaded by the 

These values are held in the linker script

| Start  | Stop | Page | Variable Name | Purpose |
| --- |:--- | :--- | :--- | :--- |
| ``0000 0000`` | ``0000 7bff`` | 1 | ``...`` | ...
| ``0000 7c00`` | ``0000 7dff`` | 1 | ``boot_paddr`` | Boot sector 
| ``0000 7e00`` | ``0007 ffff`` | 1 | ``boot_paddr + kernel_pos`` | Setup code, loaded by Boot sector
| ``0010 0000`` | ``001f ffff`` | 1 | ``kernel_paddr`` | Kernel code, data
| ``0020 0000`` | ``0020 0fff`` | 2 | ``kdata_base`` | kdata (see kdata.h)
| ``0020 1000`` | ``0020 1fff`` | 2 | ``idt_paddr`` | System 64-bit IDT (256*16) bytes
| ``0020 2000`` | ``009f ffff`` | 2 | ``percpu_paddr`` | percpu structures
| ``0020 2000`` | ``0020 2fff`` | 2 | ``percpu_tbl_paddr`` | percpu allocation table
| ``0020 3000`` | ``0020 33ff`` | 2 | ``gdt_paddr`` |  64-bit GDT 
| ``0020 3400`` | ``003f ffff`` | 2 | ``...`` |  percpu stacks, lapic regs, etc
| ``0040 0000`` | ``005f ffff`` | 3 | ``page_table_paddr`` | Page table
| ``0060 0000`` | ``.... ....`` | 4 | ``user_paddr`` | Application code, data

### Disk Image Layout
