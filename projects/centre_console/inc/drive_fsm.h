#pragma once

// This FSM is for handling the drive states of the car, in summary, to enter the
// drive state we need to close the motor controller relays, and also tell motor
// controller interface that we're entering the drive state. To enter parking
// we need to make sure that the ebrake is applied. This module uses relay_tx,
// and also ebrake_tx to ensure these transitions happen.

// Requires CAN, GPIO, soft timers, event queue, and interrupts to be initialized.

#include "ebrake_tx.h"
#include "fsm.h"
#include "mci_output_tx.h"
#include "relay_tx.h"
#include "status.h"
#include "stdbool.h"

typedef enum {
  DRIVE_STATE_NEUTRAL = 0,
  DRIVE_STATE_PARKING,
  DRIVE_STATE_DRIVE,
  DRIVE_STATE_REVERSE,
  NUM_DRIVE_STATES
} DriveState;

typedef struct DriveFsmStorage {
  Fsm drive_fsm;
  DriveState destination;
  RelayTxStorage relay_storage;
  EbrakeTxStorage ebrake_storage;
  MciOutputTxStorage mci_output_storage;
} DriveFsmStorage;

StatusCode drive_fsm_init(DriveFsmStorage *storage);

bool drive_fsm_process_event(DriveFsmStorage *storage, Event *e);

DriveState *drive_fsm_get_global_state(DriveFsmStorage *storage);
