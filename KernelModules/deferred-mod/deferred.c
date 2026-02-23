#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/timer.h>

#define TIMER_TIMEOUT 2
#define MSG "INCOMING INCOMING"

typedef unsigned long ul;

static struct timer_list timer;

// timer callback
static void timer_callback(struct timer_list* timer) {
  pr_info("The message is: %s\n", MSG);
  mod_timer(timer, jiffies + TIMER_TIMEOUT * HZ);
}


static int timer_init(void) {
  pr_info("[LOG] timer module loaded\n");

  timer_setup(&timer, timer_callback, 0);
  mod_timer(&timer, jiffies + TIMER_TIMEOUT * HZ);
  
  return 0;
}

static void timer_exit(void) {
  del_timer_sync(&timer);
  pr_info("[LOG] timer_exit module has exited\n");
}

MODULE_DESCRIPTION("Exploring device driver deferred work from a handler");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(timer_init);
module_exit(timer_exit);
