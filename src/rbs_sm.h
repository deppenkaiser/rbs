#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sm/sm.h>

#include "rbs.h"

typedef struct rbs_sm_slot
{
	int32_t fact;
	sm_state_handler_t handler;
}* rbs_sm_slot_t;

typedef struct rbs_sm
{
	struct rbs* rbs;
	rbs_rule_t rules;
	size_t rule_count;
	rbs_effect_t effects;
	size_t effect_count;
	rbs_sm_slot_t slots;
	size_t slot_count;
	/* Optional: wird nach jedem Schritt mit der Schrittnummer aufgerufen
	 * (z. B. um die Step-Grenze in der Konsole zu markieren). */
	void (*on_step)(struct rbs_sm* fsm, uint32_t tick);
	/* Optional: Konstruktor/Destruktor der App-FSM. start_handler loesst
	 * die Zustandsschleife aus (genau einmal, bevor der erste Schritt
	 * laeuft); stop_handler wird danach genau einmal gerufen, nachdem die
	 * Schleife terminiert ist (Rueckgabe true/false bzw. kein Marker). */
	sm_state_handler_t start_handler;
	sm_state_handler_t stop_handler;
	uint32_t ticks;
}* rbs_sm_t;

void rbs_sm_init(rbs_sm_t fsm, struct rbs* rbs,
                 const rbs_rule_t rules, size_t rule_count,
                 const rbs_effect_t effects, size_t effect_count,
                 const rbs_sm_slot_t slots, size_t slot_count);

/* Rechnet das RBS einen Schritt voraus (Regeln + Effekte) und schaltet auf
 * den Handler des aktiven Zustands-Markers um. Liefert false, wenn kein
 * Marker gesetzt ist — die State-Machine ist dann beendet. */
bool rbs_sm_advance(rbs_sm_t fsm, sm_state_t current);

/* Blockierend: startet die Zustandsschleife, bis ein Handler false liefert. */
void rbs_sm_run(rbs_sm_t fsm);