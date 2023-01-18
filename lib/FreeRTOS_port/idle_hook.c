#if 0
// this is referring to the loop function of your arduino project
extern void __attribute__((weak)) idle_task(void);

void vApplicationIdleHook(void)
{
	idle_task(); // will use your projects loop function as the rtos idle loop
}
#endif
