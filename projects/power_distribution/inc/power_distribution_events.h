#pragma once

typedef enum {
  POWER_DISTRIBUTION_CAN_EVENT_RX = 0,
  POWER_DISTRIBUTION_CAN_EVENT_TX,
  POWER_DISTRIBUTION_CAN_EVENT_FAULT,
  NUM_POWER_DISTRIBUTION_CAN_EVENTS,
} PowerDistributionCanEvent;

// Handled by power_distribution_gpio
typedef enum {
  POWER_DISTRIBUTION_GPIO_EVENT_DRIVER_DISPLAY = NUM_POWER_DISTRIBUTION_CAN_EVENTS + 1,
  POWER_DISTRIBUTION_GPIO_EVENT_STEERING,
  POWER_DISTRIBUTION_GPIO_EVENT_CENTRE_CONSOLE,
  POWER_DISTRIBUTION_GPIO_EVENT_DRL,
  POWER_DISTRIBUTION_GPIO_EVENT_PEDAL,
  POWER_DISTRIBUTION_GPIO_EVENT_HORN,
  POWER_DISTRIBUTION_GPIO_EVENT_BRAKE_LIGHT,
  POWER_DISTRIBUTION_GPIO_EVENT_STROBE,
  POWER_DISTRIBUTION_GPIO_EVENT_SIGNAL_LEFT,
  POWER_DISTRIBUTION_GPIO_EVENT_SIGNAL_RIGHT,
  POWER_DISTRIBUTION_GPIO_EVENT_SIGNAL_HAZARD,
  NUM_POWER_DISTRIBUTION_GPIO_EVENTS,
} PowerDistributionGpioEvent;

// Handled by a lights_signal_fsm instance
typedef enum {
  POWER_DISTRIBUTION_SIGNAL_EVENT_LEFT = NUM_POWER_DISTRIBUTION_GPIO_EVENTS + 1,
  POWER_DISTRIBUTION_SIGNAL_EVENT_RIGHT,
  POWER_DISTRIBUTION_SIGNAL_EVENT_HAZARD,
  NUM_POWER_DISTRIBUTION_SIGNAL_EVENTS,
} PowerDistributionSignalEvent;

// TODO(SOFT-138): Add a module to handle sync events
typedef enum {
  POWER_DISTRIBUTION_SYNC_EVENT_LIGHTS = NUM_POWER_DISTRIBUTION_SIGNAL_EVENTS + 1,
  NUM_POWER_DISTRIBUTION_SYNC_EVENTS,
} PowerDistributionSyncEvent;

// Also handled by power_distribution_gpio
typedef enum {
  POWER_DISTRIBUTION_POWER_SEQUENCE_EVENT_TURN_ON_EVERYTHING_MAIN =
      NUM_POWER_DISTRIBUTION_SYNC_EVENTS + 1,
  POWER_DISTRIBUTION_POWER_SEQUENCE_EVENT_TURN_ON_EVERYTHING_AUX,
  POWER_DISTRIBUTION_POWER_SEQUENCE_EVENT_TURN_ON_BMS,
  POWER_DISTRIBUTION_POWER_SEQUENCE_EVENT_TURN_OFF_EVERYTHING,
  NUM_POWER_DISTRIBUTION_POWER_SEQUENCE_EVENTS,
} PowerDistributionPowerSequenceEvent;

// is this even used? we should use it
typedef enum {
  POWER_DISTRIBUTION_CURRENT_SENSE_EVENT_DATA_READY =
      NUM_POWER_DISTRIBUTION_POWER_SEQUENCE_EVENTS + 1,
  NUM_POWER_DISTRIBUTION_CURRENT_SENSE_EVENTS,
} PowerDistributionCurrentSenseEvent;