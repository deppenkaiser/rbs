#include "rbs.h"
#include <string.h>  // für memset

void rbs_init(RBSContext* ctx) {
    memset(ctx->facts, 0, sizeof(ctx->facts));
    memset(ctx->values, 0, sizeof(ctx->values));
}

// Hilfsfunktion: Prüft, ob ein Literal wahr ist
static bool is_literal_true(RBSContext* ctx, Literal lit, const RBSCallbacks* callbacks) {
    bool token_value = false;
    
    // 1. Prüfe Fakten
    if (ctx->facts[lit.token]) {
        token_value = true;
    }
    
    // 2. Prüfe Input-Callback (überschreibt Fakten!)
    if (callbacks && callbacks->calculateInput) {
        token_value = callbacks->calculateInput(ctx, lit.token);
    }
    
    // 3. Negation anwenden
    return lit.negated ? !token_value : token_value;
}

// Hilfsfunktion: Prüft, ob eine Expression wahr ist (ODER)
static bool is_expression_true(RBSContext* ctx, Expression expr, const RBSCallbacks* callbacks) {
    for (size_t i = 0; i < expr.count; i++) {
        if (is_literal_true(ctx, expr.literals[i], callbacks)) {
            return true;  // Einer reicht
        }
    }
    return false;  // Keiner war wahr
}

// Hilfsfunktion: Prüft, ob eine Regel wahr ist (UND)
static bool is_rule_true(RBSContext* ctx, Rule rule, const RBSCallbacks* callbacks) {
    for (size_t i = 0; i < rule.count; i++) {
        if (!is_expression_true(ctx, rule.expressions[i], callbacks)) {
            return false;  // Eine Expression war falsch
        }
    }
    return true;  // Alle Expressionen waren wahr
}

// Hilfsfunktion: Setzt ein Literal in den Fakten
static void apply_literal(RBSContext* ctx, Literal lit, const RBSCallbacks* callbacks) {
    if (lit.negated) {
        // Negiertes Literal → aus Fakten entfernen
        ctx->facts[lit.token] = false;
    } else {
        // Nicht-negiertes Literal → zu Fakten hinzufügen
        ctx->facts[lit.token] = true;
    }
    
    // Output-Callback aufrufen (für Berechnungen wie COUNTER-Update)
    if (callbacks && callbacks->calculateOutput) {
        callbacks->calculateOutput(ctx, lit.token);
    }
}

bool rbs_execute(RBSContext* ctx, 
                 const Action* actions, 
                 size_t action_count,
                 const RBSCallbacks* callbacks) {
    bool any_fired = false;
    
    for (size_t i = 0; i < action_count; i++) {
        const Action* action = &actions[i];
        
        // Prüfe, ob die Regel wahr ist
        if (is_rule_true(ctx, action->rule, callbacks)) {
            // Regel feuert → alle Output-Literale anwenden
            for (size_t j = 0; j < action->output_count; j++) {
                apply_literal(ctx, action->output[j], callbacks);
            }
            any_fired = true;
        }
    }
    
    return any_fired;
}

// ============================================================
// HILFSFUNKTIONEN
// ============================================================

void rbs_set_fact(RBSContext* ctx, int token, bool value) {
    if (token >= 0 && token < MAX_TOKENS) {
        ctx->facts[token] = value;
    }
}

bool rbs_get_fact(const RBSContext* ctx, int token) {
    if (token >= 0 && token < MAX_TOKENS) {
        return ctx->facts[token];
    }
    return false;
}

void rbs_set_uint(RBSContext* ctx, int token, uint32_t value) {
    if (token >= 0 && token < MAX_TOKENS) {
        ctx->values[token].type = VAL_UINT;
        ctx->values[token].value.u = value;
    }
}

void rbs_set_int(RBSContext* ctx, int token, uint32_t value) {
    if (token >= 0 && token < MAX_TOKENS) {
        ctx->values[token].type = VAL_INT;
        ctx->values[token].value.i = (int32_t)value;
    }
}

void rbs_set_float(RBSContext* ctx, int token, float value) {
    if (token >= 0 && token < MAX_TOKENS) {
        ctx->values[token].type = VAL_FLOAT;
        ctx->values[token].value.f = value;
    }
}

uint32_t rbs_get_uint(const RBSContext* ctx, int token) {
    if (token >= 0 && token < MAX_TOKENS && ctx->values[token].type == VAL_UINT) {
        return ctx->values[token].value.u;
    }
    return 0;
}

int32_t rbs_get_int(const RBSContext* ctx, int token) {
    if (token >= 0 && token < MAX_TOKENS && ctx->values[token].type == VAL_INT) {
        return ctx->values[token].value.i;
    }
    return 0;
}

float rbs_get_float(const RBSContext* ctx, int token) {
    if (token >= 0 && token < MAX_TOKENS && ctx->values[token].type == VAL_FLOAT) {
        return ctx->values[token].value.f;
    }
    return 0.0f;
}