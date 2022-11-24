#include "alarm_clock_fsm.h"

enum fsm_handler_rc state_alarm_ongoing(struct fsm *fsm, struct fsm_event const *event)
{
	return FSM_IGNORED();
}
