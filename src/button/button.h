#ifndef ALARM_CLOCK_BUTTON_H
#define ALARM_CLOCK_BUTTON_H

#include <stdint.h>

#define BUTTON_ID_SHIFT     ALARM_CLOCK_FSM_EVT_SHT
#define BUTTON_ID_SIZE      4
#define BUTTON_ID_MSK       (0x0F)
#define BUTTON_ACTION_SHIFT (BUTTON_ID_SHIFT + BUTTON_ID_SIZE)
#define BUTTON_ACTION_SIZE  2
#define BUTTON_ACTION_MSK   (0x01)
#define BUTTON_COUNT_SHIFT  (BUTTON_ACTION_SHIFT + BUTTON_ACTION_SIZE)
#define BUTTON_COUNT_MSK    (0x3FF)

enum button_action_t { SHORT_PRESS, LONG_PRESS };

enum button_id_t {
	B_MENU_CLOCK,
	B_MENU_SETTINGS,
	B_DAY_MONDAY,
	B_DAY_TUESDAY,
	B_DAY_WEDNESDAY,
	B_DAY_THURSDAY,
	B_DAY_FRIDAY,
	B_DAY_SATURDAY,
	B_DAY_SUNDAY,
//	B_HOUR_INC,
	B_HOUR_DEC,
	B_MIN_INC,
	B_MIN_DEC,
	B_LAST_BUTTON,
};

struct button_evt_t {
	uint8_t button_id;
	uint8_t action;
	uint8_t push_count;
};

void init_buttons(void);
void button_start_polling(void);
void button_stop_polling(void);

#endif /* ALARM_CLOCK_BUTTON_H */
