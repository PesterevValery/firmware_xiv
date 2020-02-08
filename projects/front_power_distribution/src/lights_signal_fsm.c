#include "lights_signal_fsm.h"
#include "front_power_distribution_events.h"

FSM_DECLARE_STATE(state_none);                 // no lights active
FSM_DECLARE_STATE(state_left_signal);          // left signal active
FSM_DECLARE_STATE(state_right_signal);         // right signal active
FSM_DECLARE_STATE(state_hazard_signal);        // hazard active
FSM_DECLARE_STATE(state_hazard_left_signal);   // hazard active and left signal active
FSM_DECLARE_STATE(state_hazard_right_signal);  // hazard active and right signal active

static bool prv_guard_on(const Fsm *fsm, const Event *e, void *context) {
  return e->data == 1;
}

static bool prv_guard_off(const Fsm *fsm, const Event *e, void *context) {
  return e->data == 0;
}

FSM_STATE_TRANSITION(state_none) {
  SignalFsmStorage *storage = fsm->context;
  FSM_ADD_GUARDED_TRANSITION(storage->signal_left_event, &prv_guard_on, state_left_signal);
  FSM_ADD_GUARDED_TRANSITION(storage->signal_right_event, &prv_guard_on, state_right_signal);
  FSM_ADD_GUARDED_TRANSITION(storage->signal_hazard_event, &prv_guard_on, state_hazard_signal);
}

FSM_STATE_TRANSITION(state_left_signal) {
  SignalFsmStorage *storage = fsm->context;
  FSM_ADD_GUARDED_TRANSITION(storage->signal_left_event, &prv_guard_off, state_none);
  FSM_ADD_GUARDED_TRANSITION(storage->signal_right_event, &prv_guard_on, state_right_signal);
  FSM_ADD_GUARDED_TRANSITION(storage->signal_hazard_event, &prv_guard_on, state_hazard_left_signal);
}

FSM_STATE_TRANSITION(state_right_signal) {
  SignalFsmStorage *storage = fsm->context;
  FSM_ADD_GUARDED_TRANSITION(storage->signal_left_event, &prv_guard_on, state_left_signal);
  FSM_ADD_GUARDED_TRANSITION(storage->signal_right_event, &prv_guard_off, state_none);
  FSM_ADD_GUARDED_TRANSITION(storage->signal_hazard_event, &prv_guard_on,
                             state_hazard_right_signal);
}

FSM_STATE_TRANSITION(state_hazard_signal) {
  SignalFsmStorage *storage = fsm->context;
  FSM_ADD_GUARDED_TRANSITION(storage->signal_left_event, &prv_guard_on, state_hazard_left_signal);
  FSM_ADD_GUARDED_TRANSITION(storage->signal_right_event, &prv_guard_on, state_hazard_right_signal);
  FSM_ADD_GUARDED_TRANSITION(storage->signal_hazard_event, &prv_guard_off, state_none);
}

FSM_STATE_TRANSITION(state_hazard_left_signal) {
  SignalFsmStorage *storage = fsm->context;
  FSM_ADD_GUARDED_TRANSITION(storage->signal_left_event, &prv_guard_off, state_hazard_signal);
  FSM_ADD_GUARDED_TRANSITION(storage->signal_right_event, &prv_guard_on, state_hazard_right_signal);
  FSM_ADD_GUARDED_TRANSITION(storage->signal_hazard_event, &prv_guard_off, state_left_signal);
}

FSM_STATE_TRANSITION(state_hazard_right_signal) {
  SignalFsmStorage *storage = fsm->context;
  FSM_ADD_GUARDED_TRANSITION(storage->signal_left_event, &prv_guard_on, state_hazard_left_signal);
  FSM_ADD_GUARDED_TRANSITION(storage->signal_right_event, &prv_guard_off, state_hazard_signal);
  FSM_ADD_GUARDED_TRANSITION(storage->signal_hazard_event, &prv_guard_off, state_right_signal);
}

static void prv_state_none_output(Fsm *fsm, const Event *e, void *context) {
  SignalFsmStorage *storage = context;
  blink_event_generator_stop(&storage->blink_event_generator);
}

static void prv_state_left_signal_output(Fsm *fsm, const Event *e, void *context) {
  SignalFsmStorage *storage = context;
  blink_event_generator_start(&storage->blink_event_generator,
                              FRONT_POWER_DISTRIBUTION_GPIO_EVENT_SIGNAL_LEFT);
}

static void prv_state_right_signal_output(Fsm *fsm, const Event *e, void *context) {
  SignalFsmStorage *storage = context;
  blink_event_generator_start(&storage->blink_event_generator,
                              FRONT_POWER_DISTRIBUTION_GPIO_EVENT_SIGNAL_RIGHT);
}

static void prv_state_hazard_signal_output(Fsm *fsm, const Event *e, void *context) {
  SignalFsmStorage *storage = context;
  blink_event_generator_start(&storage->blink_event_generator,
                              FRONT_POWER_DISTRIBUTION_GPIO_EVENT_SIGNAL_HAZARD);
}

StatusCode lights_signal_fsm_init(SignalFsmStorage *storage, const SignalFsmSettings *settings) {
  storage->signal_left_event = settings->signal_left_event;
  storage->signal_right_event = settings->signal_right_event;
  storage->signal_hazard_event = settings->signal_hazard_event;

  BlinkEventGeneratorSettings blinker_settings = {
    .interval_us = settings->blink_interval_us,
    .default_state = BLINKER_STATE_OFF,  // all lights default to off
  };
  blink_event_generator_init(&storage->blink_event_generator, &blinker_settings);

  fsm_init(&storage->fsm, "Lights Signal FSM", &state_none, storage);
  fsm_state_init(state_none, &prv_state_none_output);
  fsm_state_init(state_left_signal, &prv_state_left_signal_output);
  fsm_state_init(state_right_signal, &prv_state_right_signal_output);
  fsm_state_init(state_hazard_signal, &prv_state_hazard_signal_output);
  fsm_state_init(state_hazard_left_signal, &prv_state_hazard_signal_output);
  fsm_state_init(state_hazard_right_signal, &prv_state_hazard_signal_output);

  return STATUS_CODE_OK;
}

StatusCode lights_signal_fsm_process_event(SignalFsmStorage *storage, const Event *event) {
  fsm_process_event(&storage->fsm, event);
  return STATUS_CODE_OK;
}
