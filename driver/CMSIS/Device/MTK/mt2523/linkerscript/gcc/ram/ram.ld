/********************************************************  
memory map 
  Start Address	Size	CM4 view Devices
  	0x0000_0000	64MB	EMI	
	0x0400_0000	32KB	CM4 TCM / cache	
	0x0400_8000	128KB	CM4 TCM	
	0x0402_8000	40KB	Reserved	
	0x0403_2000	824KB	Reserved
	0x0410_0000	1MB	Boot ROM CM4
	0x0420_0000	62MB	Reserved
	0x0800_0000	128MB	SFC0
 	0x1000_0000	128MB	SW virtual memory management
	0x1800_0000	128MB	SW virtual memory management
*********************************************************/
 
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
 
/* Memory Spaces Definitions */
MEMORY
{
    RAM (rwx)             : ORIGIN = 0x00000000, LENGTH = 4096K    
    VRAM(rwx)             : ORIGIN = 0x10000000, LENGTH = 4096K   
    TCM (rwx)             : ORIGIN = 0x04008000, LENGTH = 128K
}
 
 /* Highest address of the stack */
_stack_end = ORIGIN(TCM) + LENGTH(TCM);    /* end of TCM */

/* Generate a link error if stack don't fit into TCM */
_stack_size = 0x1000; /* required amount of stack 4KB*/

/* stack start */
_stack_start = _stack_end - _stack_size;
 
 


/* Entry Point */
ENTRY(Reset_Handler)

SECTIONS
{
    
    .ram_text :
    {
        Image$$RAM_TEXT$$Base = .;
        _ram_code_start = .;
        
        KEEP(*(.isr_vector))

        KEEP(*(.reset_handler))
        KEEP(*(.init))
        KEEP(*(.fini))
        
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
		
        *(.ram_code)
        *(.ram_rodata)
        
        
        Image$$RAM_TEXT$$Limit = .;
        _ram_code_end = .;
    } > RAM  AT> RAM
    
    . = ALIGN(4);

    .noncached_data :
    {
        
        Image$$NONCACHED_DATA$$Base = .;
        _ram_noncached_rw_start = .;
        
        *(.noncached_rwdata)
        
        
        Image$$NONCACHED_DATA$$Limit = .;
        _ram_noncached_rw_end = .; 
        Image$$NONCACHED_ZI$$Base = .;
        _ram_noncached_zi_start = .;
        
        *(.noncached_zidata)
        
        
        Image$$NONCACHED_ZI$$Limit = .;
        _ram_noncached_zi_end = .;
       
    } > RAM  AT> RAM
    
    . = ALIGN(4);
    . = . + ORIGIN(VRAM);
    .cached_data . :
    {       
        __data_start__ = .;
        _data_start = .;
        Image$$CACHED_DATA$$RW$$Base = .;
     
 
        *(.data)
        *(.data*)
        
        . = ALIGN(4);
        /* preinit data */
        PROVIDE (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE (__init_array_end = .);
        
        . = ALIGN(4);
        /* finit data */
        PROVIDE (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE (__fini_array_end = .);
        
        _data_end = .;
       __data_end__ = .;
       
       
       Image$$CACHED_DATA$$RW$$Limit = .;
    
        
        _bss_start = .;
        __bss_start__ = .;
        Image$$CACHED_DATA$$ZI$$Base = .;
        
        *(.bss)
        *(.bss*)
        *(COMMON)
        
        . = ALIGN(4);
        _bss_end = .;
        __bss_end__ = .;
        Image$$CACHED_DATA$$ZI$$Limit = .;
        
    } > VRAM  AT> RAM
    
    ASSERT(_bss_end <= (ORIGIN(VRAM) + LENGTH(VRAM)),"RAM expired")
    
    . = ALIGN(4);
    
    .tcm :
    {
        
        _tcm_text_start = .;
        Image$$TCM$$RO$$Base = .;
        
        *(.tcm_code)
        *(.tcm_rodata)
        Image$$TCM$$RO$$Limit = .;
        Image$$TCM$$RW$$Base = .;
        
        *(.tcm_rwdata)
        
        Image$$TCM$$RW$$Limit = .;
        _tcm_text_end = .;
        _tcm_zi_start = .;
        Image$$TCM$$ZI$$Base = .;
        
        *(.tcm_zidata)
        
        _tcm_zi_end = .;
        Image$$TCM$$ZI$$Limit = .;
        
        
    }> TCM  AT> RAM
    
    /* use to check if the stack exceeds the total TCM size*/
    .stack :
    {
        . = ALIGN(4);
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _stack_size;
        . = ALIGN(4);
    } > TCM

    Image$$STACK$$ZI$$Base = _stack_end - _stack_size;
    Image$$STACK$$ZI$$Limit = _stack_end;
    
    /*extern linker symbol for region init */
    Load$$RAM_TEXT$$Base = LOADADDR(.ram_text);
    Load$$NONCACHED_DATA$$Base = LOADADDR(.noncached_data);
    Load$$CACHED_DATA$$Base = LOADADDR(.cached_data);
    Load$$TCM$$Base = LOADADDR(.tcm);
   
}
