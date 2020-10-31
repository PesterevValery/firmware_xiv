#include "adc.h"

#ifndef MPXE
#include <stddef.h>
#include "log.h"

#include "interrupt.h"
#include "soft_timer.h"
// x86 implementation very similar to STM32F0 implementation.
// adc_read_raw should always return 4090.
// Vdda locked at 3300 mV.
// adc_read_converted should always return close to 2V
// temperature reading always returns 293 kelvin.

#define ADC_RETURNED_VOLTAGE_RAW 2500
#define ADC_CONTINUOUS_CB_FREQ_MS 50
#define ADC_TEMP_RETURN 293
#define ADC_VDDA_RETURN 3300

typedef struct AdcInterrupt {
  AdcCallback callback;
  AdcPinCallback pin_callback;
  void *context;
  uint16_t reading;
} AdcInterrupt;

static AdcInterrupt s_adc_interrupts[NUM_ADC_CHANNELS];

static bool s_active_channels[NUM_ADC_CHANNELS];

static uint16_t prv_get_temp(uint16_t reading) {
  return ADC_TEMP_RETURN;
}

static uint16_t prv_get_vdda(uint16_t reading) {
  return ADC_VDDA_RETURN;
}

static void prv_reset_channel(AdcChannel channel) {
  s_adc_interrupts[channel].callback = NULL;
  s_adc_interrupts[channel].pin_callback = NULL;
  s_adc_interrupts[channel].context = NULL;
  s_adc_interrupts[channel].reading = 0;
}

static StatusCode prv_check_channel_valid_and_enabled(AdcChannel adc_channel) {
  if (adc_channel >= NUM_ADC_CHANNELS) {
    return status_code(STATUS_CODE_INVALID_ARGS);
  }
  if (!s_active_channels[adc_channel]) {
    return status_code(STATUS_CODE_EMPTY);
  }
  return STATUS_CODE_OK;
}

static GpioAddress prv_channel_to_gpio(uint8_t adc_channel) {
  GpioAddress address;
  if (adc_channel >= 8 && adc_channel < 10) {
    address.port = GPIO_PORT_B;
    address.pin = adc_channel - 8;
  } else if (adc_channel >= 10 && adc_channel < ADC_CHANNEL_TEMP) {
    address.port = GPIO_PORT_C;
    address.pin = adc_channel - 10;
  } else {
    address.port = GPIO_PORT_A;
    address.pin = adc_channel;
  }
  return address;
}

static void prv_periodic_continous_cb(SoftTimerId id, void *context) {
  for (AdcChannel i = 0; i < NUM_ADC_CHANNELS; i++) {
    if (s_adc_interrupts[i].callback != NULL) {
      s_adc_interrupts[i].callback(i, s_adc_interrupts[i].context);
    } else if (s_adc_interrupts[i].pin_callback != NULL) {
      s_adc_interrupts[i].pin_callback(prv_channel_to_gpio(i), s_adc_interrupts[i].context);
    }
  }
  soft_timer_start_millis(ADC_CONTINUOUS_CB_FREQ_MS, prv_periodic_continous_cb, NULL, NULL);
}

void adc_init(AdcMode adc_mode) {
  interrupt_init();
  soft_timer_init();
  if (adc_mode == ADC_MODE_CONTINUOUS) {
    soft_timer_start_millis(ADC_CONTINUOUS_CB_FREQ_MS, prv_periodic_continous_cb, NULL, NULL);
  }
  for (size_t i = 0; i < NUM_ADC_CHANNELS; ++i) {
    prv_reset_channel(i);
  }
  adc_set_channel(ADC_CHANNEL_REF, true);
}

StatusCode adc_set_channel(AdcChannel adc_channel, bool new_state) {
  if (adc_channel >= NUM_ADC_CHANNELS) {
    return status_code(STATUS_CODE_INVALID_ARGS);
  }
  s_active_channels[adc_channel] = new_state;
  return STATUS_CODE_OK;
}

StatusCode adc_get_channel(GpioAddress address, AdcChannel *adc_channel) {
  *adc_channel = address.pin;
  switch (address.port) {
    case GPIO_PORT_A:
      if (address.pin > 7) {
        return status_code(STATUS_CODE_INVALID_ARGS);
      }
      break;
    case GPIO_PORT_B:
      if (address.pin > 1) {
        return status_code(STATUS_CODE_INVALID_ARGS);
      }
      *adc_channel += 8;
      break;
    case GPIO_PORT_C:
      if (address.pin > 5) {
        return status_code(STATUS_CODE_INVALID_ARGS);
      }
      *adc_channel += 10;
      break;
  }
  if (*adc_channel > ADC_CHANNEL_15) {
    return status_code(STATUS_CODE_INVALID_ARGS);
  }
  return STATUS_CODE_OK;
}

StatusCode adc_register_callback(AdcChannel adc_channel, AdcCallback callback, void *context) {
  status_ok_or_return(prv_check_channel_valid_and_enabled(adc_channel));
  prv_reset_channel(adc_channel);
  s_adc_interrupts[adc_channel].callback = callback;
  s_adc_interrupts[adc_channel].context = context;
  return STATUS_CODE_OK;
}

StatusCode adc_read_raw(AdcChannel adc_channel, uint16_t *reading) {
  status_ok_or_return(prv_check_channel_valid_and_enabled(adc_channel));
  s_adc_interrupts[adc_channel].reading = ADC_RETURNED_VOLTAGE_RAW;
  *reading = s_adc_interrupts[adc_channel].reading;

  // this section mimics the IRQ handler
  if (s_adc_interrupts[adc_channel].callback != NULL) {
    s_adc_interrupts[adc_channel].callback(adc_channel, s_adc_interrupts[adc_channel].context);
  } else if (s_adc_interrupts[adc_channel].pin_callback != NULL) {
    s_adc_interrupts[adc_channel].pin_callback(prv_channel_to_gpio(adc_channel),
                                               s_adc_interrupts[adc_channel].context);
  }

  return STATUS_CODE_OK;
}

StatusCode adc_read_converted(AdcChannel adc_channel, uint16_t *reading) {
  status_ok_or_return(prv_check_channel_valid_and_enabled(adc_channel));
  uint16_t adc_reading = 0;
  adc_read_raw(adc_channel, &adc_reading);
  switch (adc_channel) {
    case ADC_CHANNEL_TEMP:
      *reading = prv_get_temp(adc_reading);
      return STATUS_CODE_OK;

    case ADC_CHANNEL_REF:
      *reading = prv_get_vdda(adc_reading);
      return STATUS_CODE_OK;

    case ADC_CHANNEL_BAT:
      adc_reading *= 2;
      break;

    default:
      break;
  }

  uint16_t vdda;
  adc_read_converted(ADC_CHANNEL_REF, &vdda);
  *reading = (adc_reading * vdda) / 4095;

  return STATUS_CODE_OK;
}

#else  // #ifndef MPXE
#include <stddef.h>
#include <stdlib.h>

#include "adc.pb-c.h"
#include "interrupt.h"
#include "log.h"
#include "soft_timer.h"
#include "status.h"
#include "store.h"
#include "stores.pb-c.h"

#define ADC_RETURNED_VOLTAGE_RAW 2500
#define ADC_CONTINUOUS_CB_FREQ_MS 50
#define ADC_TEMP_RETURN 293
#define ADC_VDDA_RETURN 3300

typedef struct AdcInterrupt {
  AdcCallback callback;
  void *context;
  uint16_t reading;
} AdcInterrupt;

static MxAdcStore s_store = MX_ADC_STORE__INIT;

static AdcInterrupt s_adc_interrupts[NUM_ADC_CHANNELS];

static bool s_active_channels[NUM_ADC_CHANNELS];

static void prv_export() {
  store_export(MX_STORE_TYPE__ADC, &s_store, NULL);
}

static void update_store(ProtobufCBinaryData msg_buf, ProtobufCBinaryData mask_buf) {
  MxAdcStore *msg = mx_adc_store__unpack(NULL, msg_buf.len, msg_buf.data);
  MxAdcStore *mask = mx_adc_store__unpack(NULL, mask_buf.len, mask_buf.data);

  for (uint16_t i = 0; i < mask->n_reading; i++) {
    if (mask->reading[i] != 0) {
      s_store.reading[i] = msg->reading[i];
    }
  }
  mx_adc_store__free_unpacked(msg, NULL);
  mx_adc_store__free_unpacked(mask, NULL);
  prv_export();
}

static uint16_t prv_get_temp(uint16_t reading) {
  return ADC_TEMP_RETURN;
}

static uint16_t prv_get_vdda(uint16_t reading) {
  return ADC_VDDA_RETURN;
}
static void prv_periodic_continous_cb(SoftTimerId id, void *context) {
  for (uint8_t i = 0; i < NUM_ADC_CHANNELS; i++) {
    if (s_adc_interrupts[i].callback != NULL) {
      s_adc_interrupts[i].callback(i, s_adc_interrupts[i].context);
    }
  }
  soft_timer_start_millis(ADC_CONTINUOUS_CB_FREQ_MS, prv_periodic_continous_cb, NULL, NULL);
}

void adc_init(AdcMode adc_mode) {
  StoreFuncs funcs = {
    (GetPackedSizeFunc)mx_adc_store__get_packed_size,
    (PackFunc)mx_adc_store__pack,
    (UnpackFunc)mx_adc_store__unpack,
    (FreeUnpackedFunc)mx_adc_store__free_unpacked,
    (UpdateStoreFunc)update_store,
  };
  s_store.n_reading = NUM_ADC_CHANNELS;
  s_store.reading = malloc(NUM_ADC_CHANNELS * sizeof(uint32_t));

  store_register(MX_STORE_TYPE__ADC, funcs, &s_store, NULL);

  interrupt_init();
  soft_timer_init();
  if (adc_mode == ADC_MODE_CONTINUOUS) {
    soft_timer_start_millis(ADC_CONTINUOUS_CB_FREQ_MS, prv_periodic_continous_cb, NULL, NULL);
  }

  adc_set_channel(ADC_CHANNEL_REF, true);
}

StatusCode adc_set_channel(AdcChannel adc_channel, bool new_state) {
  if (adc_channel >= NUM_ADC_CHANNELS) {
    return status_code(STATUS_CODE_INVALID_ARGS);
  }
  s_active_channels[adc_channel] = new_state;

  prv_export();
  return STATUS_CODE_OK;
}

StatusCode adc_get_channel(GpioAddress address, AdcChannel *adc_channel) {  // no change?
  *adc_channel = address.pin;

  switch (address.port) {
    case GPIO_PORT_A:
      if (address.pin > 7) {
        return status_code(STATUS_CODE_INVALID_ARGS);
      }
      break;
    case GPIO_PORT_B:
      if (address.pin > 1) {
        return status_code(STATUS_CODE_INVALID_ARGS);
      }
      *adc_channel += 8;
      break;
    case GPIO_PORT_C:
      if (address.pin > 5) {
        return status_code(STATUS_CODE_INVALID_ARGS);
      }
      *adc_channel += 10;
      break;
  }

  if (*adc_channel > ADC_CHANNEL_15) {
    return status_code(STATUS_CODE_INVALID_ARGS);
  }
  return STATUS_CODE_OK;
}

StatusCode adc_register_callback(AdcChannel adc_channel, AdcCallback callback,
                                 void *context) {  // no change?
  if (adc_channel >= NUM_ADC_CHANNELS) {
    return status_code(STATUS_CODE_INVALID_ARGS);
  }
  if (s_active_channels[adc_channel] != true) {
    return status_code(STATUS_CODE_EMPTY);
  }

  s_adc_interrupts[adc_channel].callback = callback;
  s_adc_interrupts[adc_channel].context = context;

  return STATUS_CODE_OK;
}

StatusCode adc_read_raw(AdcChannel adc_channel, uint16_t *reading) {
  if (adc_channel >= NUM_ADC_CHANNELS) {
    return status_code(STATUS_CODE_INVALID_ARGS);
  }
  if (s_active_channels[adc_channel] != true) {
    return status_code(STATUS_CODE_EMPTY);
  }

  s_adc_interrupts[adc_channel].reading = ADC_RETURNED_VOLTAGE_RAW;
  s_store.reading[adc_channel] = ADC_RETURNED_VOLTAGE_RAW;
  *reading = s_adc_interrupts[adc_channel].reading;

  if (s_adc_interrupts[adc_channel].callback != NULL) {
    s_adc_interrupts[adc_channel].callback(adc_channel, s_adc_interrupts[adc_channel].context);
  }
  prv_export();

  return STATUS_CODE_OK;
}

StatusCode adc_read_converted(AdcChannel adc_channel, uint16_t *reading) {
  if (adc_channel >= NUM_ADC_CHANNELS) {
    return status_code(STATUS_CODE_INVALID_ARGS);
  }
  if (s_active_channels[adc_channel] != true) {
    return status_code(STATUS_CODE_EMPTY);
  }

  uint16_t adc_reading = 0;
  adc_read_raw(adc_channel, &adc_reading);
  switch (adc_channel) {
    case ADC_CHANNEL_TEMP:
      *reading = prv_get_temp(adc_reading);
      return STATUS_CODE_OK;

    case ADC_CHANNEL_REF:
      *reading = prv_get_vdda(adc_reading);
      return STATUS_CODE_OK;

    case ADC_CHANNEL_BAT:
      adc_reading *= 2;
      break;

    default:
      break;
  }

  uint16_t vdda;
  adc_read_converted(ADC_CHANNEL_REF, &vdda);
  *reading = (adc_reading * vdda) / 4095;

  s_store.reading[adc_channel] = *reading;
  prv_export();

  return STATUS_CODE_OK;
}

#endif  // #ifndef MPXE
