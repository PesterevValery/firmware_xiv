#include <stdint.h>
#include <stdlib.h>

#include "interrupt.h"
#include "log.h"
#include "soft_timer.h"
#include "wait.h"

#define COUNTER_TICK_MS 500

typedef struct Counters {
  uint8_t counter_a;
  uint8_t counter_b;
} Counters;

void priv_timer_callback(SoftTimerId timer_id, void *context) {
  Counters *count = context;
  count->counter_a++;

  LOG_DEBUG("Counter A: %i\n", count->counter_a);

  if ((count->counter_a) % 2 == 0) {
    count->counter_b++;
    LOG_DEBUG("Counter B: %i\n", count->counter_b);
  }

  soft_timer_start_millis(COUNTER_TICK_MS, priv_timer_callback, count, NULL);
}

int main() {
  interrupt_init();
  soft_timer_init();

  Counters count = { 0 };

  soft_timer_start_millis(COUNTER_TICK_MS, priv_timer_callback, &count, NULL);

  while (true) {
    wait();
  }

  return 0;
}
