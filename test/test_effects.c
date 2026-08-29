/* rbs_effects_test.c — Regressions-Test fuer Schritt-Effekte (rbs_step).
 * Validiert: alle Effekte eines gemeinsamen Triggers feuern, Trigger wird
 * genau einmal invertiert, Ergebnisse stammen vom unveraenderten Basis-Stand
 * (Snapshots, keine Sequenz-Kette), und nach dem Konsumieren feuert nichts
 * mehr.
 * Stil: assert-basiert, kein externes Framework.
 */
#include "rbs.h"
#include <assert.h>
#include <stdio.h>

enum token { N_T1 = -1, Z = 0, T1 = 1, TN };
enum value { M0, M1, M2, M3, VC };

int main(void) {
    int32_t* facts = rbs_create_facts_buffer(TN);
    memory_t mem = rbs_create_memory_buffer(VC);
    rbs_initialize_facts(facts, TN);
    rbs_initialize_memory(mem, VC);

    mem[M0] = 10.0;
    mem[M1] = 10.0;
    mem[M2] = 10.0;
    mem[M3] = 10.0;

    rbs_set_fact(facts, TN, T1);

    struct rbs_effect effects[5] = {
        { T1, M0, ADD, 5.0 },
        { T1, M1, SUB, 3.0 },
        { T1, M2, MUL, 2.0 },
        { T1, M3, DIV, 4.0 },
        { T1, M2, MUL, 3.0 }
    };

    struct rbs r = { .facts = facts, .token_count = TN, .memory = mem, .value_count = VC };

    /* Ein Schritt: alle Effekte rechnen vom unveraenderten Basis-Stand
     * (M2: einmal 10*2=20 und einmal 10*3=30, zuletzt geschrieben -> 30),
     * der gemeinsame Trigger wird genau einmal invertiert (N_T1). */
    rbs_step(&r, NULL, 0, effects, 5);

    assert(mem[M0] == 15.0);
    assert(mem[M1] == 7.0);
    assert(mem[M2] == 30.0);
    assert(mem[M3] == 2.5);

    assert(!rbs_is_fact(facts, TN, T1));
    assert(rbs_is_fact(facts, TN, N_T1));

    /* Nach dem Konsumieren feuert im naechsten Schritt kein Effekt mehr. */
    rbs_step(&r, NULL, 0, effects, 5);
    assert(mem[M0] == 15.0);
    assert(mem[M1] == 7.0);
    assert(mem[M2] == 30.0);
    assert(mem[M3] == 2.5);

    printf("rbs_effects_test: ALLE TESTS OK\n");

    rbs_destroy_memory_buffer(&mem);
    rbs_destroy_facts_buffer(&facts);
    return 0;
}