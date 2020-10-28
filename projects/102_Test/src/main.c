#include <stdint.h>
#include "gpio.h"
#include "interrupt.h"
#include "log.h"
#include "soft_timer.h"
#include "wait.h"

typedef struct Counters {
  uint8_t counter_a;
  uint8_t counter_b;
} Counters;

static void prv_counter_timeout(SoftTimerId timer_id, void *context) {
  Counters *counter = context;

  counter->counter_a++;
  LOG_DEBUG("Counter A: %u\n", (counter->counter_a));

  if ((counter->counter_a) % 2 == 0) {
    counter->counter_b++;
    LOG_DEBUG("Counter B: %u\n", (counter->counter_b));
  }

  soft_timer_start_seconds(0.5, prv_counter_timeout, counter, NULL);
}

int main(void) {
  interrupt_init();
  soft_timer_init();

  struct Counters counter;

  LOG_DEBUG("%u %u", counter.counter_a, counter.counter_b);

  soft_timer_start_seconds(0.5, prv_counter_timeout, &counter, NULL);

  while (true) {
    wait();
  }

  return 0;
}