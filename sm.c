#include "sm.h"
#include <stdio.h>

// ============================================================
// 1. CALLBACKS für berechnete Werte
// ============================================================

bool sm_calculate_input(RBSContext* ctx, int token) {
    // Wird aufgerufen, wenn Token in Regel geprüft wird
    switch (token) {
        case TOKEN_CHECK_STOP:
            // CHECK_STOP ist wahr, wenn COUNTER < 1
            return rbs_get_uint(ctx, TOKEN_COUNTER) < 1;
        default:
            return rbs_get_fact(ctx, token);  // Fallback auf Fakten
    }
}

void sm_calculate_output(RBSContext* ctx, int token) {
    // Wird aufgerufen, wenn Token als Output gesetzt wird
    switch (token) {
        case TOKEN_INITIALIZE:
            // Wenn INITIALIZE gesetzt wird → Counter auf 100
            rbs_set_uint(ctx, TOKEN_COUNTER, 100);
            printf("Counter initialized to 100\n");
            break;
            
        case TOKEN_CALCULATE_COUNTER:
            // Wenn CALCULATE_COUNTER gesetzt wird → Counter - 1
            uint32_t current = rbs_get_uint(ctx, TOKEN_COUNTER);
            rbs_set_uint(ctx, TOKEN_COUNTER, current - 1);
            printf("Counter: %u\n", current - 1);
            break;
    }
}

// ============================================================
// 2. REGELN DEFINIEREN (statische Daten)
// ============================================================

// Regel 0: Wenn NICHT INITIALIZE und NICHT STOP
//         → setze INITIALIZE und CHECK_STOP
static Literal rule0_input_literals[] = {
    {TOKEN_INITIALIZE, true},   // NICHT INITIALIZE
    {TOKEN_STOP, true}          // NICHT STOP
};
static Expression rule0_expressions[] = {
    {rule0_input_literals, 2}   // ODER? Nein, das ist UND!
};
static Rule rule0 = {
    rule0_expressions, 1        // 1 Expression → UND
};
static Literal rule0_output[] = {
    {TOKEN_INITIALIZE, false},  // setze INITIALIZE
    {TOKEN_CHECK_STOP, false}   // setze CHECK_STOP
};

// Regel 1: Wenn INITIALIZE → setze CALCULATE_COUNTER
static Literal rule1_input_literals[] = {
    {TOKEN_INITIALIZE, false}
};
static Expression rule1_expressions[] = {
    {rule1_input_literals, 1}
};
static Rule rule1 = {
    rule1_expressions, 1
};
static Literal rule1_output[] = {
    {TOKEN_CALCULATE_COUNTER, false}
};

// Regel 2: Wenn CHECK_STOP → setze STOP, entferne INITIALIZE, entferne CALCULATE_COUNTER, entferne CHECK_STOP
static Literal rule2_input_literals[] = {
    {TOKEN_CHECK_STOP, false}
};
static Expression rule2_expressions[] = {
    {rule2_input_literals, 1}
};
static Rule rule2 = {
    rule2_expressions, 1
};
static Literal rule2_output[] = {
    {TOKEN_STOP, false},          // setze STOP
    {TOKEN_INITIALIZE, true},     // entferne INITIALIZE
    {TOKEN_CALCULATE_COUNTER, true}, // entferne CALCULATE_COUNTER
    {TOKEN_CHECK_STOP, true}      // entferne CHECK_STOP
};

// Alle Aktionen
static Action actions[] = {
    {rule0, rule0_output, 2},
    {rule1, rule1_output, 1},
    {rule2, rule2_output, 4}
};
static size_t action_count = sizeof(actions) / sizeof(actions[0]);

// ============================================================
// 3. CALLBACK-STRUCT
// ============================================================

static RBSCallbacks callbacks = {
    .calculateInput = sm_calculate_input,
    .calculateOutput = sm_calculate_output
};

// ============================================================
// 4. MAIN
// ============================================================

int main() {
    RBSContext ctx;
    rbs_init(&ctx);
    
    // Initial-Fakten: N_INITIALIZE und N_STOP (in der neuen Logik: negierte Literale)
    // Aber: Wir haben keine N_TOKEN mehr! 
    // Stattdessen setzen wir einfach INITIALIZE und STOP auf false
    rbs_set_fact(&ctx, TOKEN_INITIALIZE, false);
    rbs_set_fact(&ctx, TOKEN_STOP, false);
    // COUNTER initial ist 0 (wird durch INITIALIZE auf 100 gesetzt)
    rbs_set_uint(&ctx, TOKEN_COUNTER, 0);
    
    printf("=== Starting Rule-Based System ===\n");
    
    int cycle = 0;
    while (rbs_execute(&ctx, actions, action_count, &callbacks)) {
        cycle++;
        printf("--- Cycle %d completed ---\n", cycle);
    }
    
    printf("=== System stopped after %d cycles ===\n", cycle);
    printf("Final counter: %u\n", rbs_get_uint(&ctx, TOKEN_COUNTER));
    
    return 0;
}