#pragma once

#include <stdbool.h>

#include "can_msg.h"

// For setting the CAN device
typedef enum {
  SYSTEM_CAN_DEVICE_RESERVED = 0,
  SYSTEM_CAN_DEVICE_BMS_CARRIER = 1,
  SYSTEM_CAN_DEVICE_CENTRE_CONSOLE = 2,
  SYSTEM_CAN_DEVICE_POWER_DISTRIBUTION_REAR = 3,
  SYSTEM_CAN_DEVICE_POWER_DISTRIBUTION_FRONT = 4,
  SYSTEM_CAN_DEVICE_MOTOR_CONTROLLER = 5,
  SYSTEM_CAN_DEVICE_PEDAL = 6,
  SYSTEM_CAN_DEVICE_STEERING = 7,
  SYSTEM_CAN_DEVICE_SOLAR = 8,
  SYSTEM_CAN_DEVICE_CHARGER = 10,
  SYSTEM_CAN_DEVICE_IMU = 11,
  NUM_SYSTEM_CAN_DEVICES = 11
} SystemCanDevice;

// For setting the CAN message ID
typedef enum {
  SYSTEM_CAN_MESSAGE_BPS_HEARTBEAT = 0,
  SYSTEM_CAN_MESSAGE_SET_RELAY_STATES = 1,
  SYSTEM_CAN_MESSAGE_GET_RELAY_STATES = 2,
  SYSTEM_CAN_MESSAGE_POWERTRAIN_HEARTBEAT = 3,
  SYSTEM_CAN_MESSAGE_GET_AUX_STATUS = 4,
  SYSTEM_CAN_MESSAGE_FAULT_SEQUENCE = 5,
  SYSTEM_CAN_MESSAGE_POWER_ON_MAIN_SEQUENCE = 6,
  SYSTEM_CAN_MESSAGE_POWER_OFF_SEQUENCE = 7,
  SYSTEM_CAN_MESSAGE_POWER_ON_AUX_SEQUENCE = 8,
  SYSTEM_CAN_MESSAGE_DRIVE_OUTPUT = 9,
  SYSTEM_CAN_MESSAGE_SET_EBRAKE_STATE = 10,
  SYSTEM_CAN_MESSAGE_OVUV_DCDC_AUX = 16,
  SYSTEM_CAN_MESSAGE_MC_ERROR_LIMITS = 17,
  SYSTEM_CAN_MESSAGE_PEDAL_OUTPUT = 18,
  SYSTEM_CAN_MESSAGE_CRUISE_TARGET = 19,
  SYSTEM_CAN_MESSAGE_BRAKE = 20,
  SYSTEM_CAN_MESSAGE_FRONT_POWER = 21,
  SYSTEM_CAN_MESSAGE_DRIVE_STATE = 22,
  SYSTEM_CAN_MESSAGE_LIGHTS_SYNC = 23,
  SYSTEM_CAN_MESSAGE_LIGHTS = 24,
  SYSTEM_CAN_MESSAGE_HORN = 25,
  SYSTEM_CAN_MESSAGE_GET_CHARGER_CONNECTION_STATE = 26,
  SYSTEM_CAN_MESSAGE_SET_CHARGER_RELAY = 27,
  SYSTEM_CAN_MESSAGE_BEGIN_PRECHARGE = 28,
  SYSTEM_CAN_MESSAGE_PRECHARGE_COMPLETED = 29,
  SYSTEM_CAN_MESSAGE_HAZARD = 30,
  SYSTEM_CAN_MESSAGE_BATTERY_VT = 32,
  SYSTEM_CAN_MESSAGE_BATTERY_AGGREGATE_VC = 33,
  SYSTEM_CAN_MESSAGE_MOTOR_CONTROLLER_VC = 35,
  SYSTEM_CAN_MESSAGE_MOTOR_VELOCITY = 36,
  SYSTEM_CAN_MESSAGE_MOTOR_DEBUG = 37,
  SYSTEM_CAN_MESSAGE_MOTOR_TEMPS = 38,
  SYSTEM_CAN_MESSAGE_MOTOR_AMP_HR = 39,
  SYSTEM_CAN_MESSAGE_ODOMETER = 40,
  SYSTEM_CAN_MESSAGE_AUX_DCDC_VC = 43,
  SYSTEM_CAN_MESSAGE_DCDC_TEMPS = 44,
  SYSTEM_CAN_MESSAGE_SOLAR_DATA_FRONT = 45,
  SYSTEM_CAN_MESSAGE_CHARGER_INFO = 47,
  SYSTEM_CAN_MESSAGE_LINEAR_ACCELERATION = 51,
  SYSTEM_CAN_MESSAGE_ANGULAR_ROTATION = 52,
  NUM_SYSTEM_CAN_MESSAGES = 40
} SystemCanMessage;
