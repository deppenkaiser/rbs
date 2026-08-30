#include <stdio.h>

#include <sm/sm.h>
#include <logging/logging.h>

#include "app.h"

/* --- Handler-Implementierung (Programmlogik der App) --- */

/* Externe Faktenquelle (z. B. Wettersensor / Systemzeit): meldet das Ende
 * des Regens, indem RAIN und CLOUDY negiert werden. Das ist bewusster
 * externer Input — kein Konsum; die Regeln ziehen daraus selbst nach. */
bool app_handle_wet(sm_state_t next_state, void* user_data)
{
	rbs_sm_t fsm = (rbs_sm_t) user_data;

	logging_log_message("wetter: regen klaert auf");
	rbs_set_fact(fsm->rbs->facts, fsm->rbs->token_count, rbs_invert_token(RAIN));
	rbs_set_fact(fsm->rbs->facts, fsm->rbs->token_count, rbs_invert_token(CLOUDY));

	return rbs_sm_advance(fsm, next_state);
}

/* Schritt-Callback: liefert je Schritt eine Zeile (Step-Grenze/Status). */
void app_on_step(rbs_sm_t fsm, uint32_t tick)
{
	rbs_t rbs = fsm->rbs;
	char buf[128];
	(void) tick;
	int pos = snprintf(buf, sizeof(buf), "step %u | UMBRELLA:", (unsigned) tick);
	pos += snprintf(buf + pos, sizeof(buf) - pos, " %s | MONEY:",
	                rbs_is_fact(rbs->facts, rbs->token_count, UMBRELLA) ? "true" : "false");
	snprintf(buf + pos, sizeof(buf) - pos, " %.0f", rbs->memory[MONEY]);
	logging_log_message(buf);
}

/* App-Ende: keine weiteren externen Ereignisse mehr -> FSM beenden. */
bool app_handle_adult(sm_state_t next_state, void* user_data)
{
	rbs_sm_t fsm = (rbs_sm_t) user_data;
	(void) fsm;
	(void) next_state;

	return false;
}

/* Konstruktor (sm-Lebenszyklus-Callback): laeuft ganz am Anfang des
 * Worker-Threads und baut die initiale Welt auf — hier: die externe
 * Ausgangslage (es regnet und ist bewoelkt). Danach uebernimmt die
 * Regel-Engine. Der Konstruktor kann auch Ressourcen/Speicher anlegen. */
callback void sm_on_start(sm_core_t core)
{
	rbs_sm_t fsm = (rbs_sm_t) core->user_data;

	logging_log_message("app: fsm startet (wetter: es regnet und ist bewoelkt)");
	rbs_set_fact(fsm->rbs->facts, fsm->rbs->token_count, RAIN);
	rbs_set_fact(fsm->rbs->facts, fsm->rbs->token_count, CLOUDY);
}

/* Destruktor (sm-Lebenszyklus-Callback): laeuft ganz am Ende des
 * Worker-Threads, nachdem die Schleife terminiert ist. Hier z. B. die
 * Endfakten bilanzieren und Ressourcen/Speicher freigeben. */
callback void sm_on_stop(sm_core_t core)
{
	rbs_sm_t fsm = (rbs_sm_t) core->user_data;

	logging_log_message(rbs_is_fact(fsm->rbs->facts, fsm->rbs->token_count, UMBRELLA) ?
	                    "app: UMBRELLA ist gesetzt (fsm beendet)" :
	                    "app: UMBRELLA NICHT gesetzt");
}

int main()
{
	logging_log_message("rbs main start");
	struct rbs rbs =
	{
		.facts = rbs_create_facts_buffer(TOKEN_COUNT),
		.token_count = TOKEN_COUNT,
		.memory = rbs_create_memory_buffer(VALUE_COUNT),
		.value_count = VALUE_COUNT,
		.fact_names = token_names,
		.fact_names_count = sizeof(token_names) / sizeof(token_names[0])
	};

	rbs_initialize_facts(rbs.facts, TOKEN_COUNT);
	rbs_initialize_memory(rbs.memory, VALUE_COUNT);

	rbs.memory[AGE] = 20;
	rbs.memory[MONEY] = 100;

	/* Die initiale Welt (Regen + Bewoelkung) baut der sm_on_start-Konstruktor
	 * auf. AGE/MONEY sind Konstanten der Sim-Welt und bleiben hier. */

	struct rbs_sm fsm =
	{
		.rbs = &rbs,
		.rules = app_rules,
		.rule_count = sizeof(app_rules) / sizeof(app_rules[0]),
		.effects = app_effects,
		.effect_count = sizeof(app_effects) / sizeof(app_effects[0]),
		.slots = app_slots,
		.slot_count = sizeof(app_slots) / sizeof(app_slots[0]),
		.on_step = app_on_step,
	};

	rbs_sm_run(&fsm);

	rbs_destroy_facts_buffer(&rbs.facts);
	rbs_destroy_memory_buffer(&rbs.memory);

	logging_log_message("rbs main end");
	return 0;
}
