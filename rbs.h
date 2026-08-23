#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// ============================================================
// 1. LITERAL: Ein Token mit optionaler Negation
// ============================================================
typedef struct {
    int token;      // Token-ID (z.B. 0, 1, 2, ...)
    bool negated;   // true = NICHT token
} Literal;

// ============================================================
// 2. EXPRESSION: ODER-Verknüpfung von Literalen
//    (wahr, wenn mindestens ein Literal wahr ist)
// ============================================================
typedef struct {
    Literal* literals;
    size_t count;
} Expression;

// ============================================================
// 3. RULE: UND-Verknüpfung von Expressionen
//    (wahr, wenn ALLE Expressionen wahr sind)
// ============================================================
typedef struct {
    Expression* expressions;
    size_t count;
} Rule;

// ============================================================
// 4. ACTION: Wenn Rule wahr → dann setze Output-Literale
// ============================================================
typedef struct {
    Rule rule;
    Literal* output;
    size_t output_count;
} Action;

// ============================================================
// 5. FAKTEN & WERTE
// ============================================================
#define MAX_TOKENS 64

typedef enum {
    VAL_UINT,
    VAL_INT,
    VAL_FLOAT
} ValueType;

typedef struct {
    ValueType type;
    union {
        uint32_t u;
        int32_t i;
        float f;
    } value;
} TokenValue;

typedef struct {
    bool facts[MAX_TOKENS];          // true = Token ist in Fakten
    TokenValue values[MAX_TOKENS];   // Werte für Tokens (optional)
} RBSContext;

// ============================================================
// 6. CALLBACKS für benutzerdefinierte Logik
// ============================================================
typedef struct {
    // Wird aufgerufen, wenn ein Token in einer Regel geprüft wird
    // Rückgabe: true = Token ist wahr (zusätzlich zu den Fakten)
    bool (*calculateInput)(RBSContext* ctx, int token);
    
    // Wird aufgerufen, wenn ein Token als Output gesetzt wird
    void (*calculateOutput)(RBSContext* ctx, int token);
} RBSCallbacks;

// ============================================================
// 7. HAUPT-FUNKTIONEN
// ============================================================

// Initialisiert den Kontext (alle Fakten = false, Werte = 0)
void rbs_init(RBSContext* ctx);

// Führt einen Zyklus aus
// Gibt true zurück, wenn mindestens eine Regel gefeuert hat
bool rbs_execute(RBSContext* ctx, 
                 const Action* actions, 
                 size_t action_count,
                 const RBSCallbacks* callbacks);

// Hilfsfunktionen zum Setzen/Lesen von Fakten
void rbs_set_fact(RBSContext* ctx, int token, bool value);
bool rbs_get_fact(const RBSContext* ctx, int token);

// Hilfsfunktionen für Werte
void rbs_set_uint(RBSContext* ctx, int token, uint32_t value);
void rbs_set_int(RBSContext* ctx, int token, int32_t value);
void rbs_set_float(RBSContext* ctx, int token, float value);
uint32_t rbs_get_uint(const RBSContext* ctx, int token);
int32_t rbs_get_int(const RBSContext* ctx, int token);
float rbs_get_float(const RBSContext* ctx, int token);