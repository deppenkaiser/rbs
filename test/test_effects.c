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

    rbs_set_fact(facts, T1);

    struct rbs_effect effects[5] = {
        { T1, M0, ADD, 5.0 },
        { T1, M1, SUB, 3.0 },
        { T1, M2, MUL, 2.0 },
        { T1, M3, DIV, 4.0 },
        { T1, M2, MUL, 3.0 }
    };

    struct rbs r = { facts, mem };
    rbs_apply_effects(&r, effects, 5);

    assert(mem[M0] == 15.0);
    assert(mem[M1] == 7.0);
    assert(mem[M2] == 60.0);
    assert(mem[M3] == 2.5);

    assert(!rbs_is_fact(facts, T1));
    assert(rbs_is_fact(facts, N_T1));

    printf("rbs_effects_test: ALLE TESTS OK\n");

    rbs_destroy_memory_buffer(&mem);
    rbs_destroy_facts_buffer(&facts);
    return 0;
}
