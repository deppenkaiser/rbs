/* rbs_sm_test.c — Regressionstest für die rbs_sm-State-Machine.
 * Validiert: Regel+Effekt-Runde pro Tick, Marker-Routing auf die Handler,
 * das Konsumieren des Markers als Fortschrittsmechanik und den Abbruch
 * beim Endzustand (Handler liefert false).
 * Stil: assert-basiert, kein externes Framework.
 */
#include "rbs.h"
#include "rbs_sm.h"

#include <assert.h>
#include <stdio.h>

enum token
{
	N_PAY = -3,
	N_WET = -2,
	N_RAIN = -1,
	Z = 0,
	RAIN = 1,
	WET = 2,
	ADULT = 3,
	PAY = 4,
	TN
};

enum value
{
	MONEY,
	VALUE_COUNT
};

static int calls_wet = 0;
static int calls_adult = 0;

/* RAIN -> WET (Wetter) und WET -> PAY (Fahrtkosten) */
struct rbs_term if_rain[] =
{
	{ .comparison = false, .fact_enum = RAIN },
	{ .comparison = false, .fact_enum = Z }
};
enum token then_rain[] = { WET, Z };

struct rbs_term if_wet[] =
{
	{ .comparison = false, .fact_enum = WET },
	{ .comparison = false, .fact_enum = Z }
};
enum token then_wet[] = { PAY, Z };

struct rbs_rule rules[] =
{
	{ if_rain, then_rain },
	{ if_wet, then_wet },
};

struct rbs_effect effects[] =
{
	{ .trigger_fact_enum = PAY, .value_enum = MONEY, .op = SUB, .operand = 10 }
};

static bool _sm_handle_wet(sm_state_t next_state, void* user_data)
{
	rbs_sm_t fsm = (rbs_sm_t) user_data;
	calls_wet++;

	/* Konsumieren: ohne das Loeschen re-feriert RAIN->WET im naechsten
	 * Tick und die Schleife haengt dauerhaft am WET-Marker. */
	rbs_set_fact(fsm->rbs->facts, rbs_invert_token(RAIN));
	rbs_set_fact(fsm->rbs->facts, rbs_invert_token(WET));

	return rbs_sm_advance(fsm, next_state);
}

static bool _sm_handle_adult(sm_state_t next_state, void* user_data)
{
	(void) next_state;
	(void) user_data;
	calls_adult++;
	return false; /* Endzustand */
}

struct rbs_sm_slot slots[] =
{
	{ .fact = WET,   .handler = _sm_handle_wet },
	{ .fact = ADULT, .handler = _sm_handle_adult },
};

int main(void)
{
	struct rbs rbs =
	{
		.facts = rbs_create_facts_buffer(TN),
		.memory = rbs_create_memory_buffer(VALUE_COUNT)
	};
	assert(rbs.facts != NULL);
	assert(rbs.memory != NULL);

	rbs_initialize_facts(rbs.facts, TN);
	rbs_initialize_memory(rbs.memory, VALUE_COUNT);
	rbs.memory[MONEY] = 100;

	rbs_set_fact(rbs.facts, RAIN);
	rbs_set_fact(rbs.facts, ADULT);

	struct rbs_sm fsm = {0};
	rbs_sm_init(&fsm, &rbs, rules, 2, effects, 1, slots, 2);
	rbs_sm_run(&fsm);

	/* Tick 1: RAIN->WET, WET->PAY, Effekt zahlt (100 -> 90), Marker WET.
	 * Tick 2: Marker ADULT -> Endzustand. */
	assert(fsm.ticks == 2);
	assert(calls_wet == 1);
	assert(calls_adult == 1);
	assert(rbs.memory[MONEY] == 90.0);
	assert(rbs_is_fact(rbs.facts, ADULT));
	assert(!rbs_is_fact(rbs.facts, WET));
	assert(!rbs_is_fact(rbs.facts, RAIN));

	rbs_destroy_memory_buffer(&rbs.memory);
	rbs_destroy_facts_buffer(&rbs.facts);

	printf("rbs_sm_test: ALLE TESTS OK\n");
	return 0;
}