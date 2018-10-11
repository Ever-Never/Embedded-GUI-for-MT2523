#ifndef TASK_CALL_H
#define TASK_CALL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (* TaskPointer)(void);
#if 0
#define TASK_CREATE_INIT(task,stack_size,parameter,priority,task_handle)    \
                                                                            \
                                void task##Create(void)                     \
                                {                                           \
                                    xTaskCreate(                            \
                                                 task,                      \
                                                #task,                      \
                                                 stack_size,                \
                                                 parameter,                 \
                                                 priority,                  \
                                                 NULL);                     \
                                }                                           \
                                                                            \
                                const TaskPointer g##task##Alloc            \
                                                                            \
                                __attribute__((section(".task_table")))     \
                                                                            \
                                = task##Create;   
#endif
    
#ifdef __cplusplus
}
#endif    
    
#endif
