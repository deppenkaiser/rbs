/* rbs_sm_test.c — Regressionstest für die rbs_sm-State-Machine.
 * Validiert die Schritt-Semantik: eine Regel sieht im laufenden Schritt
 * nicht, was eine andere Regel im selben Schritt ableitet (neue Fakten
 * wirken erst im naechsten Schritt), Marker-Routing auf die Handler, das
 * Konsumieren als Fortschrittsmechanik und den Abbruch beim Endzustand
 * (Handler liefert false).
 * Stil: assert-basiert, kein externes Framework.
 */
#include "rbs.h"
#include "rbs_sm.h"

#include <assert.h>
#include <stdio.h>

enum token
{
	N_PAY = -4,
	N_WET = -3,
	N_RAIN = -2,
	N_ADULT = -1,
	Z = 0,
	RAIN = 1,
	WET = 2,
	ADULT = 3,
	PAY = 4,
	TN
};

enum value
{
	AGE,
	MONEY,
	VALUE_COUNT
};

static int calls_wet = 0;
static int calls_adult = 0;

/* RAIN -> WET (Wetter) und AGE>18 -> ADULT + PAY (Erwachsener bezahlt) */
struct rbs_term if_rain[] =
{
	{ .comparison = false, .fact_enum = RAIN },
	{ .comparison = false, .fact_enum = Z }
};
enum token then_rain[] = { WET, Z };

struct rbs_term if_adult[] =
{
	{ .comparison = true, .value_enum = AGE, .op = GT, .operand = 18 },
	{ .comparison = false, .fact_enum = Z }
};
enum token then_adult[] = { ADULT, PAY, Z };

struct rbs_rule rules[] =
{
	{ if_rain, then_rain },
	{ if_adult, then_adult },
};

struct rbs_effect effects[] =
{
	{ .trigger_fact_enum = PAY, .value_enum = MONEY, .op = SUB, .operand = 10 }
};

static bool _sm_handle_wet(sm_state_t next_state, void* user_data)
{
	rbs_sm_t fsm = (rbs_sm_t) user_data;
	calls_wet++;

	/* Schritt-Semantik: PAY ist zwar im Schritt abgeleitet und in der
	 * Faktenbasis, aber der Effekt darf erst im NAECHSTEN Schritt zahlen,
	 * wenn PAY aktiv ist. Deshalb ist MONEY hier noch 100. */
	assert(fsm->rbs->memory[MONEY] == 100.0);

	/* Konsumieren: ohne das Loeschen re-feriert RAIN->WET im naechsten
	 * Schritt und die Schleife haengt dauerhaft am WET-Marker. */
	rbs_set_fact(fsm->rbs->facts, fsm->rbs->token_count, rbs_invert_token(RAIN));
	rbs_set_fact(fsm->rbs->facts, fsm->rbs->token_count, rbs_invert_token(WET));

	return rbs_sm_advance(fsm, next_state);
}

static bool _sm_handle_adult(sm_state_t next_state, void* user_data)
{
	rbs_sm_t fsm = (rbs_sm_t) user_data;
	(void) next_state;
	calls_adult++;

	assert(fsm->rbs->memory[MONEY] == 90.0);
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
		.token_count = TN,
		.memory = rbs_create_memory_buffer(VALUE_COUNT),
		.value_count = VALUE_COUNT
	};
	assert(rbs.facts != NULL);
	assert(rbs.memory != NULL);

	rbs_initialize_facts(rbs.facts, TN);
	rbs_initialize_memory(rbs.memory, VALUE_COUNT);
	rbs.memory[AGE] = 20;
	rbs.memory[MONEY] = 100;

	rbs_set_fact(rbs.facts, rbs.token_count, RAIN);

	struct rbs_sm fsm = {0};
	rbs_sm_init(&fsm, &rbs, rules, 2, effects, 1, slots, 2);
	rbs_sm_run(&fsm);

	/* Schritt 1: RAIN->WET und AGE>18->ADULT+PAY werden ABGELEITET
	 * (sichtbar ab Schritt 2), der Effekt zahlt noch nicht (MONEY 100).
	 * Router: WET -> Handler, konsumiert RAIN+WET.
	 * Schritt 2: PAY ist aktiv -> Effekt zahlt 100 -> 90. Router: ADULT
	 * -> Endzustand. */
	assert(fsm.ticks == 2);
	assert(calls_wet == 1);
	assert(calls_adult == 1);
	assert(rbs.memory[MONEY] == 90.0);
	assert(rbs_is_fact(rbs.facts, rbs.token_count, ADULT));
	assert(!rbs_is_fact(rbs.facts, rbs.token_count, WET));
	assert(!rbs_is_fact(rbs.facts, rbs.token_count, RAIN));
	assert(!rbs_is_fact(rbs.facts, rbs.token_count, PAY));

	rbs_destroy_memory_buffer(&rbs.memory);
	rbs_destroy_facts_buffer(&rbs.facts);

	printf("rbs_sm_test: ALLE TESTS OK\n");
	return 0;
}