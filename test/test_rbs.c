/* rbs_test.c — Regressionstest für den rbs-Kern.
 * Validiert die Fakt-Indizierung (Off-by-one-Regression), positive/
 * negative Zustände und die Grenz-Magnituden des Puffers.
 * Stil: assert-basiert, kein externes Framework, int32-Stil wie rbs.c.
 */
#include "rbs.h"

#include <assert.h>
#include <stdio.h>

enum token
{
	N_T3 = -3,
	N_T2 = -2,
	N_T1 = -1,
	Z = 0,
	T1 = 1,
	T2 = 2,
	T3 = 3,
	TN
};

int main(void)
{
	int32_t* facts = rbs_create_facts_buffer(TN);
	assert(facts != NULL);
	rbs_initialize_facts(facts, TN);

	/* Initial: jeder Fakt ist NOT (false), d.h. Slot i-1 = -i. */
	assert(!rbs_is_fact(facts, T1));
	assert(!rbs_is_fact(facts, T2));
	assert(!rbs_is_fact(facts, T3));
	assert(rbs_is_fact(facts, N_T1));
	assert(rbs_is_fact(facts, N_T2));
	assert(rbs_is_fact(facts, N_T3));

	/* Setzen und erkennen (Regression auf Off-by-one rbs_is_fact). */
	rbs_set_fact(facts, T2);
	assert(rbs_is_fact(facts, T2));
	assert(!rbs_is_fact(facts, N_T2));

	rbs_set_fact(facts, N_T3);
	assert(rbs_is_fact(facts, N_T3));
	assert(!rbs_is_fact(facts, T3));

	/* Grenz-Magnituden: kleinster (1 -> Slot 0) und groesster (TN-1). */
	rbs_set_fact(facts, T1);
	assert(rbs_is_fact(facts, T1));

	rbs_set_fact(facts, T3);
	assert(rbs_is_fact(facts, T3));
	assert(rbs_invert_token(T3) == N_T3);

	rbs_destroy_facts_buffer(&facts);
	assert(facts == NULL);

	printf("rbs_test: ALLE TESTS OK\n");
	return 0;
}
