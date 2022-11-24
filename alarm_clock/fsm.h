#ifndef FSM_H
#define FSM_H

#include <stdint.h>
#include <stdio.h>

enum reserved_fsm_event_type {
	/* Reserved event types */
	FSM_EVENT_ENTRY = 0,
	FSM_EVENT_EXIT,
	FSM_EVENT_INIT,
	/* First application event, to be used as starting value for application
	 * specific event enum */
	FSM_EVENT_APPLICATION,
};

enum fsm_handler_rc {
	FSM_RC_HANDLED,
	FSM_RC_IGNORED,
	FSM_RC_TRANSITION,
};

struct fsm_event {
	uint32_t type;
};

typedef enum fsm_handler_rc (*fsm_event_handler_t)(void *fsm, struct fsm_event const *event);
typedef char *(*fsm_state_name_getter)(const fsm_event_handler_t state);
typedef char *(*fsm_event_name_getter)(const struct fsm_event *event);

extern const struct fsm_event fsm_entry_event;
extern const struct fsm_event fsm_exit_event;
extern const struct fsm_event fsm_init_event;

#define FSM_NAME_MAX_LEN 16

struct fsm {
	fsm_event_handler_t state;

	/* Tracing utilities */
	fsm_event_name_getter event_name_getter;
	fsm_state_name_getter state_name_getter;
	int                   trace_transitions;
	int                   trace_events;

	char name[FSM_NAME_MAX_LEN + 1];
};

static inline enum fsm_handler_rc transition_guard(struct fsm *fsm, fsm_event_handler_t target)
{
	fsm->state = target;
	return FSM_RC_TRANSITION;
}

#define FSM_HANDLED()          (FSM_RC_HANDLED)
#define FSM_IGNORED()          (FSM_RC_IGNORED)
#define FSM_TRANSITION(target) transition_guard(&fsm->fsm, (fsm_event_handler_t) (target))

void fsm_init(struct fsm             *fsm,
              char                   *name,
              fsm_event_handler_t     init_pseudo_state,
              struct fsm_event const *init_event);
void fsm_dispatch(struct fsm *fsm, struct fsm_event const *event);

#endif /* FSM_H */
