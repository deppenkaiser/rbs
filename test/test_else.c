/* rbs_else_test.c — Regressionstest für die else-Zweige (rbs_rule.else_facts).
 * Validiert: eine Regel setzt bei nicht zutreffendem if-Zweig die else-Fakten
 * (typisch der rueckwaertige Reset), und Fakten persistieren bis zum
 * expliziten Zuruecksetzen (kein Auto-Konsum).
 * Stil: assert-basiert, kein externes Framework.
 */
#include <rbs/rbs.h>
#include <assert.h>
#include <stdio.h>

enum token
{
	N_UMBRELLA = -2,
	N_WET = -1,
	Z = 0,
	WET = 1,
	UMBRELLA = 2,
	TN
};

enum value { VC };

int main(void)
{
	int32_t* facts = rbs_create_facts_buffer(TN);
	memory_t mem = rbs_create_memory_buffer(VC);
	rbs_initialize_facts(facts, TN);
	rbs_initialize_memory(mem, VC);

	struct rbs_term if_wet[] =
	{
		{ .comparison = false, .fact_enum = WET },
		{ .comparison = false, .fact_enum = Z }
	};
	enum token then_wet[] = { UMBRELLA, Z };
	enum token else_wet[] = { N_UMBRELLA, Z };

	struct rbs_rule rules[] = { { if_wet, then_wet, else_wet } };
	struct rbs r = { .facts = facts, .token_count = TN, .memory = mem, .value_count = VC, .fact_names = NULL };

	/* Start: WET nicht aktiv -> else-Zweig setzt N_UMBRELLA (bleibt negiert). */
	rbs_step(&r, rules, 1, NULL, 0);
	assert(!rbs_is_fact(facts, TN, UMBRELLA));
	assert(rbs_is_fact(facts, TN, N_UMBRELLA));

	/* Regen startet: WET aktiv -> then-Zweig setzt UMBRELLA. */
	rbs_set_fact(facts, TN, WET);
	rbs_step(&r, rules, 1, NULL, 0);
	assert(rbs_is_fact(facts, TN, UMBRELLA));

	/* UMBRELLA persistiert (kein Auto-Konsum): ein weiterer Schritt ohne
	 * Aenderung laesst UMBRELLA aktiv. */
	rbs_step(&r, rules, 1, NULL, 0);
	assert(rbs_is_fact(facts, TN, UMBRELLA));

	/* Regen zu Ende: WET explizit zuruecksetzen -> else-Zweig setzt
	 * N_UMBRELLA und damit UMBRELLA zurueck. */
	rbs_set_fact(facts, TN, N_WET);
	rbs_step(&r, rules, 1, NULL, 0);
	assert(!rbs_is_fact(facts, TN, UMBRELLA));
	assert(rbs_is_fact(facts, TN, N_UMBRELLA));

	/* Regel ohne else_facts bleibt zulaessig (then-only, kein Crash). */
	struct rbs_rule then_only[] = { { if_wet, then_wet, NULL } };
	rbs_set_fact(facts, TN, WET);
	rbs_step(&r, then_only, 1, NULL, 0);
	assert(rbs_is_fact(facts, TN, UMBRELLA));

	rbs_destroy_memory_buffer(&mem);
	rbs_destroy_facts_buffer(&facts);
	return 0;
}
