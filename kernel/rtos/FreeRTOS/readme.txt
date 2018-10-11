Brief:          This module is freertos.
Usage:          GCC:  In project makefile
                        LIBS += $(OUTPATH)/librtos.a
                        MODULE_PATH += $(KRL_OS_PATH)
                KEIL: 
                        Add to project: /kernel/rtos/FreeRTOS/Source/event_groups.c
                        Add to project: /kernel/rtos/FreeRTOS/Source/list.c
                        Add to project: /kernel/rtos/FreeRTOS/Source/queue.c
                        Add to project: /kernel/rtos/FreeRTOS/Source/tasks.c
                        Add to project: /kernel/rtos/FreeRTOS/Source/timers.c
                        Add to project: /kernel/rtos/FreeRTOS/Source/portable/MemMang/heap_4.c
                        Add to project: /kernel/rtos/FreeRTOS/Source/portable/RVDS/ARM_CM4F/port.c
                        C/C++ Include Paths: kernel/rtos/FreeRTOS/Source/include /kernel/rtos/FreeRTOS/Source/portable/RVDS/ARM_CM4F
                IAR:  
                        Add to project: /kernel/rtos/FreeRTOS/Source/event_groups.c
                        Add to project: /kernel/rtos/FreeRTOS/Source/list.c
                        Add to project: /kernel/rtos/FreeRTOS/Source/queue.c
                        Add to project: /kernel/rtos/FreeRTOS/Source/tasks.c
                        Add to project: /kernel/rtos/FreeRTOS/Source/timers.c
                        Add to project: /kernel/rtos/FreeRTOS/Source/portable/MemMang/heap_4.c
                        Add to project: /kernel/rtos/FreeRTOS/Source/portable/IAR/ARM_CM4F/port.c
                        C/C++ Include Paths: kernel/rtos/FreeRTOS/Source/include /kernel/rtos/FreeRTOS/Source/portable/RVDS/ARM_CM4F                       
Dependency:     None
Notice:         If configCHECK_FOR_STACK_OVERFLOW defines to 1 or 2, a more file is needed:  kernel\service\src\os_port_callback.c
Relative doc:   Please refer to freeRTOS website for more information
Example project:project/mt2523_hdk/freertos_thread_creation, and project/mt7687_hdk/freertos_thread_creation
