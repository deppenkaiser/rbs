#include "rbs_sm.h"

void rbs_sm_init(rbs_sm_t fsm, struct rbs* rbs,
                 const rbs_rule_t rules, size_t rule_count,
                 const rbs_effect_t effects, size_t effect_count,
                 const rbs_sm_slot_t slots, size_t slot_count)
{
	fsm->rbs = rbs;
	fsm->rules = rules;
	fsm->rule_count = rule_count;
	fsm->effects = effects;
	fsm->effect_count = effect_count;
	fsm->slots = slots;
	fsm->slot_count = slot_count;
	fsm->ticks = 0;
}

/* Einstieg: laesst das RBS den ersten Zustand bestimmen. */
bool _rbs_sm_entry(sm_state_t current, void* user_data)
{
	return rbs_sm_advance((rbs_sm_t) user_data, current);
}

bool rbs_sm_advance(rbs_sm_t fsm, sm_state_t current)
{
	if (fsm == NULL || current == NULL || fsm->rbs == NULL)
	{
		return false;
	}

	/* Ein Schritt: alle Regeln + Effekte auf derselben Basis, Commit erst
	 * am Schritt-Ende. Das RBS bestimmt so die Ziel-Zustaende. */
	rbs_step(fsm->rbs, fsm->rules, fsm->rule_count,
	         fsm->effects, fsm->effect_count);
	fsm->ticks++;

	if (fsm->on_step != NULL)
	{
		fsm->on_step(fsm, fsm->ticks);
	}

	/* Aktiven Zustand ueber die Faktenbasis ermitteln und umschalten. */
	for (size_t i = 0; i < fsm->slot_count; ++i)
	{
		if (fsm->slots[i].fact != 0 &&
		    rbs_is_fact(fsm->rbs->facts, fsm->rbs->token_count, fsm->slots[i].fact))
		{
			current->state_function = fsm->slots[i].handler;
			return true;
		}
	}

	return false; /* kein Zustands-Marker aktiv -> FSM beendet */
}

void rbs_sm_run(rbs_sm_t fsm)
{
	if (fsm == NULL)
	{
		return;
	}

	struct sm_state init = {.state_function = _rbs_sm_entry};
	sm_run(&init, fsm);
}