
global get_apic_base_msr
global cpu_spin_here
	
bits 64
section .text

cpu_spin_here:
	jmp $
