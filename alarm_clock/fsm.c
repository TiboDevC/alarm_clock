#include <assert.h>
#include <string.h>

#include <Arduino.h>

#include "fsm.h"

#include "debug.h"

const struct fsm_event fsm_entry_event = {.type = FSM_EVENT_ENTRY};
const struct fsm_event fsm_exit_event  = {.type = FSM_EVENT_EXIT};
const struct fsm_event fsm_init_event  = {.type = FSM_EVENT_INIT};

static void fsm_trace_transition(const struct fsm         *fsm,
                                 const fsm_event_handler_t source,
                                 const fsm_event_handler_t target)
{
	if (fsm->trace_transitions == 0 || fsm->state_name_getter == NULL) {
		return;
	}
	debug("[fsm] ");
	debug(fsm->state_name_getter(source));
	debug(" -> ");
	debug(fsm->state_name_getter(target));
	debug("\n");
}

static void fsm_trace_event(const struct fsm *fsm, const struct fsm_event *event)
{
	if (fsm->trace_events == 0 || fsm->event_name_getter == NULL || fsm->state_name_getter == NULL) {
		return;
	}

	debug("[fsm] Dispatch event ");
	debug(fsm->event_name_getter(event));
	debug(" to state handler ");
	debug(fsm->state_name_getter(fsm->state));
	debug("\n");
}

void fsm_init(struct fsm               *fsm,
              char                     *name,
              const fsm_event_handler_t init_pseudo_state,
              struct fsm_event const   *init_event)
{
	/* Initialize tracing utilities and configuration */
	strncpy(fsm->name, name, FSM_NAME_MAX_LEN);
	fsm->name[FSM_NAME_MAX_LEN] = '\0';

	debug("Init FSM\n");

	fsm->state_name_getter = NULL;
	fsm->trace_transitions = 0;
	fsm->event_name_getter = NULL;
	fsm->trace_events      = 0;

	/* Set initial pseudo state which should always transition upon getting
	 * the FSM_EVENT_INIT event type. */
	fsm->state = init_pseudo_state;

	/* Execute initial transition from initial pseudo state. */
	(*fsm->state)(fsm, init_event);
	/* Enter default state set by the initial pseudo state event handler. */
	(*fsm->state)(fsm, &fsm_entry_event);

	debug("END init FSM\n");
}

static void _assert_if_transition(enum fsm_handler_rc rc)
{
	if (FSM_RC_TRANSITION == rc) {
		debug("[fsm] Forbidden transition\n");
		/* Transition are forbidden during those events
		 * - fsm_exit_event
		 * - fsm_skip_step
		 * - fsm_entry_event
		 */
		assert(0);
	}
}

void fsm_dispatch(struct fsm *fsm, struct fsm_event const *event)
{
	enum fsm_handler_rc rc;
	fsm_event_handler_t prev_state = fsm->state;

	fsm_trace_event(fsm, event);
	rc = (*fsm->state)(fsm, event);

	if (FSM_RC_TRANSITION == rc) {
		fsm_trace_transition(fsm, prev_state, fsm->state);
		/* We are transitioning from one state to another. Send EXIT event
		 * to source state, and ENTRY event to target state. */
		_assert_if_transition((*prev_state)(fsm, &fsm_exit_event));
		_assert_if_transition((*fsm->state)(fsm, &fsm_entry_event));
	}
}
