#include "rbs.h"
#include "rbs_sm.h"

#include <stdio.h>
#include <logging/logging.h>

typedef enum token
{
	N_PAY = -7,
	N_ADULT = -6,
	N_UMBRELLA = -5,
	N_WET = -4,
	N_CLOUDY = -3,
	N_RAIN = -2,
	N_START = -1,
	ZERO = 0,
	START = 1,
	RAIN = 2,
	CLOUDY = 3,
	WET = 4,
	UMBRELLA = 5,
	ADULT = 6,
	PAY = 7,
	TOKEN_COUNT
}* token_t;

typedef enum value
{
	AGE,
	MONEY,
	VALUE_COUNT
}* value_t;

struct rbs_term if_weather[] =
{
	{ .comparison = false, .fact_enum = RAIN },
	{ .comparison = false, .fact_enum = CLOUDY },
	{ .comparison = false, .fact_enum = ZERO }
};
enum token then_weather[] = { WET, ZERO };

struct rbs_term if_wet[] =
{
	{ .comparison = false, .fact_enum = WET },
	{ .comparison = false, .fact_enum = ZERO }
};
enum token then_wet[] = { UMBRELLA, ZERO };

struct rbs_term if_adult[] =
{
	{ .comparison = true, .value_enum = AGE, .op = GT, .operand = 18 },
	{ .comparison = false, .fact_enum = ZERO }
};
enum token then_adult[] = { ADULT, PAY, ZERO };

struct rbs_rule rules[] =
{
	{ if_weather, then_weather },
	{ if_wet, then_wet },
	{ if_adult, then_adult }
};

struct rbs_effect effects[] =
{
	{ .trigger_fact_enum = PAY, .value_enum = MONEY, .op = SUB, .operand = 10 }
};

static bool _app_handle_wet(sm_state_t next_state, void* user_data)
{
	rbs_sm_t fsm = (rbs_sm_t) user_data;

	printf("WET:      true (Regen, Schirm noetig)\n");
	printf("MONEY:    %.0f\n", fsm->rbs->memory[MONEY]);

	/* Konsumieren: Regen stoppen und WET-Fakt loeschen, sonst re-feriert die
	 * Wetterregel WET im naechsten Tick und die Schleife haengt hier. */
	rbs_set_fact(fsm->rbs->facts, fsm->rbs->token_count, rbs_invert_token(RAIN));
	rbs_set_fact(fsm->rbs->facts, fsm->rbs->token_count, rbs_invert_token(WET));

	return rbs_sm_advance(fsm, next_state);
}

static void _app_on_step(rbs_sm_t fsm, uint32_t tick)
{
	(void) fsm;
	printf("--- Step %u ---\n", tick);
}

static bool _app_handle_adult(sm_state_t next_state, void* user_data)
{
	rbs_sm_t fsm = (rbs_sm_t) user_data;

	printf("ADULT:    true (erwachsen -> es wird bezahlt)\n");
	printf("MONEY:    %.0f\n", fsm->rbs->memory[MONEY]);

	/* Endzustand: FSM beenden. */
	return false;
}

struct rbs_sm_slot slots[] =
{
	{ .fact = WET,   .handler = _app_handle_wet },
	{ .fact = ADULT, .handler = _app_handle_adult },
};

int main()
{
	logging_log_message("rbs main start");
	struct rbs rbs =
	{
		.facts = rbs_create_facts_buffer(TOKEN_COUNT),
		.token_count = TOKEN_COUNT,
		.memory = rbs_create_memory_buffer(VALUE_COUNT),
		.value_count = VALUE_COUNT
	};

	rbs_initialize_facts(rbs.facts, TOKEN_COUNT);
	rbs_initialize_memory(rbs.memory, VALUE_COUNT);

	rbs.memory[AGE] = 20;
	rbs.memory[MONEY] = 100;

	rbs_set_fact(rbs.facts, rbs.token_count, RAIN);
	rbs_set_fact(rbs.facts, rbs.token_count, CLOUDY);

	struct rbs_sm fsm =
	{
		.rbs = &rbs,
		.rules = rules,
		.rule_count = sizeof(rules) / sizeof(rules[0]),
		.effects = effects,
		.effect_count = sizeof(effects) / sizeof(effects[0]),
		.slots = slots,
		.slot_count = sizeof(slots) / sizeof(slots[0]),
		.on_step = _app_on_step,
	};

	rbs_sm_run(&fsm);

rbs_destroy_facts_buffer(&rbs.facts);
rbs_destroy_memory_buffer(&rbs.memory);

	logging_log_message("rbs main end");
	return 0;
}