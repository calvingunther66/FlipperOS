/* Linker script for FlipperOS Userland Apps (.fos) */

MEMORY
{
    /* Reserve 64KB for Kernel (0x20000000 - 0x20010000) */
    /* Userland starts at 0x20010000, size 192KB (up to 256K total) */
    /* Note: RP2040 has 264KB, slightly more complex, but this is safe */
    RAM (rwx) : ORIGIN = 0x20010000, LENGTH = 192K
}

ENTRY(main)

SECTIONS
{
    .text : {
        *(.text*)
        *(.rodata*)
    } > RAM

    .data : {
        *(.data*)
    } > RAM

    .bss : {
        *(.bss*)
    } > RAM
}
