/********************************************************  
memory map 
  Start Address	Size	CM4 view Devices
  0x0000_0000	64MB	EMI	
	0x0400_0000	32KB	CM4 TCM / cache	
	0x0400_8000	64KB	CM4 TCM	
	0x0401_8000	32KB	Reserved	
	0x0402_0000	896KB	Reserved	
	0x0410_0000	1MB	Boot ROM CM4
	0x0420_0000	384KB	SYSRAM
	0x0430_0000	8KB	Retention SRAM
	0x0440_0000	1MB	WIFI ROM
	0x0800_0000	128MB	SFC0
 	0x1000_0000	128MB	SW virtual memory management
	0x1800_0000	128MB	SW virtual memory management
*********************************************************/
 
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
 
/* Memory Spaces Definitions:4M FLASH,4M PSRAM,384k SYSRAM */
MEMORY
{

    TCM (rwx)             : ORIGIN = 0x04008000, LENGTH = 64K
    SYSRAM (rwx)          : ORIGIN = 0x04200000, LENGTH = 359K
    WIFI_DATA(rwx)        : ORIGIN = 0x04259C00, LENGTH = 25K
    VSYSRAM (rwx)         : ORIGIN = 0x14200000, LENGTH = 359K      
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
    . = ORIGIN(ROM_RTOS);
    .text :
    {
        _text_start = .;
        Image$$TEXT$$Base = .;
        
        KEEP(*(.reset_handler))
        KEEP(*(.init))
        KEEP(*(.fini))
        
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        
         . = ALIGN(4);
      __boson_data_lmastart = .;
        KEEP(*(.boson_data))
         . = ALIGN(4);
        Image$$TEXT$$Limit = .;
        _text_end = .;
        
    } > SYSRAM  AT> SYSRAM
   

    . = ALIGN(32);
    _ram_code_load = LOADADDR(.cached_ram_text);

    .cached_ram_text :
    {
        Image$$CACHED_RAM_TEXT$$Base = .;
        _ram_code_start = .;

        *(.ram_code)
        *(.ram_rodata)
        
        
        Image$$CACHED_RAM_TEXT$$Limit = .;
        _ram_code_end = .;
    } > VRAM   AT> SYSRAM
  

    . = ALIGN(4);
    _data_load = LOADADDR(.cached_ram_data);

     .cached_ram_data :
    {       
        _data_start = .;
        Image$$CACHED_RAM_DATA$$RW$$Base = .;
        
        
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
       Image$$CACHED_RAM_DATA$$RW$$Limit = .;
    
        
    } > VRAM  AT> SYSRAM
    
    
    . = ALIGN(4);
    .bss (NOLOAD) : 
    {
          _bss_start = .;
        Image$$CACHED_RAM_DATA$$ZI$$Base = .;
        
        *(.bss)
        *(.bss*)
        *(COMMON)
        
        . = ALIGN(4);
        _bss_end = .;
        Image$$CACHED_RAM_DATA$$ZI$$Limit = .;
        
    } > VRAM  AT> SYSRAM
    
     
       . = ALIGN(32);
     _ram_noncached_rw_load = LOADADDR(.noncached_ram_data);
     . = . - ORIGIN(VRAM);

    .noncached_ram_data . :
    {
        Image$$NONCACHED_RAM_DATA$$Base = .;
        _ram_noncached_rw_start = .;
        
        *(.noncached_ram_rwdata)
        
        
        Image$$NONCACHED_RAM_DATA$$Limit = .;
        _ram_noncached_rw_end = .; 
    } > RAM  AT> SYSRAM
     
    . = ALIGN(4);
    .noncached_ram_bss . (NOLOAD) :
    {                
        Image$$NONCACHED_RAM_ZI$$Base = .;
        _ram_noncached_zi_start = .;
        
        *(.noncached_ram_zidata)
        
        
        Image$$NONCACHED_RAM_ZI$$Limit = .;
        _ram_noncached_zi_end = .;
       
    } > RAM  AT> SYSRAM
    
    ASSERT(_ram_noncached_zi_end <= (ORIGIN(RAM) + LENGTH(RAM)),"PSRAM expired")

     . = ALIGN(32);
    _sysram_code_load = LOADADDR(.sysram_text);

    .sysram_text :
    {
        Image$$CACHED_SYSRAM_TEXT$$Base = .;
        _sysram_code_start = .;
        

        *(.sysram_code)
        *(.sysram_rodata)
        
        
        Image$$CACHED_SYSRAM_TEXT$$Limit = .;
        _sysram_code_end = .;
    } > VSYSRAM   AT> SYSRAM
    
    . = ALIGN(4);
      _cached_sysram_data_load = LOADADDR(.cached_sysram_data);

     .cached_sysram_data  :
    {       
        _cached_sysram_data_start = .;
        Image$$CACHED_SYSRAM_DATA$$RW$$Base = .;

        *(.cached_sysram_rwdata)

        _cached_sysram_data_end = .;          
       Image$$CACHED_SYSRAM_DATA$$RW$$Limit = .;
    
        
    } > VSYSRAM  AT> SYSRAM
    

    . = ALIGN(4);
    .cached_sysram_bss (NOLOAD) : 
    {
        _cached_sysram_bss_start = .;
        Image$$CACHED_SYSRAM_DATA$$ZI$$Base = .;
        
       *(.cached_sysram_zidata)
        
        . = ALIGN(4);
        _cached_sysram_bss_end = .;
        Image$$CACHED_SYSRAM_DATA$$ZI$$Limit = .;
        
    } > VSYSRAM  AT> SYSRAM
    
     
       . = ALIGN(32);
    _noncached_sysram_rw_load = LOADADDR(.noncached_sysram_data);
      . = . - ORIGIN(VSYSRAM) + ORIGIN(SYSRAM);

    .noncached_sysram_data . :
    {
        Image$$NONCACHED_SYSRAM_DATA$$Base = .;
        _noncached_sysram_rw_start = .;
        *(.noncached_sysram_rwdata)
        
        
        Image$$NONCACHED_SYSRAM_DATA$$Limit = .;
        _noncached_sysram_rw_end = .; 
    } > SYSRAM  AT> SYSRAM
    
    
    . = ALIGN(4);
    .noncached_sysram_bss . (NOLOAD) :
    {                
        Image$$NONCACHED_SYSRAM_ZI$$Base = .;
        _noncached_sysram_zi_start = .;
        *(.noncached_sysram_zidata)
        
        
        Image$$NONCACHED_SYSRAM_ZI$$Limit = .;
        _noncached_sysram_zi_end = .;
    } > SYSRAM  AT> SYSRAM
    
    ASSERT(_noncached_sysram_zi_end <= (ORIGIN(SYSRAM) + LENGTH(SYSRAM)),"SYSRAM expired")
  
   . = ALIGN(4);
    _tcm_text_load = LOADADDR(.tcm);

    .tcm :
    {
        
        _tcm_text_start = .;
        Image$$TCM$$RO$$Base = .;
        Image$$VECTOR$$TABLE$$Base = .;
         KEEP(*(.isr_vector))
        *(.tcm_code)
        *(.tcm_rodata)
        Image$$TCM$$RO$$Limit = .;
        Image$$TCM$$RW$$Base = .;
        
        *(.tcm_rwdata)
        . = ALIGN(4);
        Image$$TCM$$RW$$Limit = .;
        _tcm_text_end = .;
    }> TCM  AT> SYSRAM

    . = ALIGN(4);
    .tcm_bss (NOLOAD) :
    {
        _tcm_zi_start = .;
        Image$$TCM$$ZI$$Base = .;
        
        *(.tcm_zidata)
        
        _tcm_zi_end = .;
        Image$$TCM$$ZI$$Limit = .;
        
        
    }> TCM AT> SYSRAM  
    
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
    
    /* provide nvdm start and length for NVDM management */
    Image$$NVDM$$ZI$$Base = ORIGIN(ROM_NVDM_RESERVED);
    Image$$NVDM$$ZI$$Length = LENGTH(ROM_NVDM_RESERVED);
    
    
}
