#include <logging/logging.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include "rbs/rbs.h"

int32_t rbs_invert_token(int32_t token)
{
    return token * -1;
}

int32_t* rbs_create_facts_buffer(uint32_t token_count)
{
    return malloc(token_count * sizeof(int32_t));
}

memory_t rbs_create_memory_buffer(uint32_t value_count)
{
    return malloc(value_count * sizeof(double));
}

void rbs_destroy_facts_buffer(int32_t** ptoken)
{
    if (*ptoken != NULL)
    {
        free(*ptoken);
        *ptoken = NULL;
    }
}

void rbs_destroy_memory_buffer(memory_t* pmemory)
{
    if (*pmemory != NULL)
    {
        free(*pmemory);
        *pmemory = NULL;
    }
}

void rbs_initialize_facts(int32_t* facts, uint32_t token_count)
{
    if (facts != NULL)
    {
        for (uint32_t i = 1; i < token_count; ++i)
        {
            facts[i - 1] = rbs_invert_token(i);
        }
    }
}

bool rbs_is_fact(int32_t* facts, uint32_t token_count, int32_t fact)
{
    uint32_t magnitude = fact < 0 ? (uint32_t) -fact : (uint32_t) fact;
    bool valid = (facts != NULL) && (fact != 0) && (magnitude < token_count);
    return valid && (facts[magnitude - 1] == fact);
}

void rbs_set_fact(int32_t* facts, uint32_t token_count, int32_t fact)
{
    uint32_t magnitude = fact < 0 ? (uint32_t) -fact : (uint32_t) fact;
    bool valid = (facts != NULL) && (fact != 0) && (magnitude < token_count);

    if (valid)
    {
        facts[magnitude - 1] = fact;
    }
}

void rbs_set_fact_named(struct rbs* rbs, int32_t fact)
{
    if (rbs != NULL)
    {
        rbs_set_fact(rbs->facts, rbs->token_count, fact);

        uint32_t magnitude = fact < 0 ? (uint32_t) -fact : (uint32_t) fact;
        bool valid = (rbs->facts != NULL) && (fact != 0) && (magnitude < rbs->token_count);

        const char* name = NULL;
        if (valid && rbs->fact_names && rbs->fact_names_count > 0 &&
            magnitude > 0 && magnitude <= rbs->fact_names_count)
        {
            name = rbs->fact_names[magnitude - 1];
        }

        char buf[160];
        int pos = 0;
        if (name && name[0] != '\0')
        {
            pos = snprintf(buf, sizeof(buf), "rbs_set_fact: %s", name);
        }
        else
        {
            pos = snprintf(buf, sizeof(buf), "rbs_set_fact: Fakt %d", fact);
        }
        snprintf(buf + pos, sizeof(buf) - pos, " (%s)",
                 fact > 0 ? "aktiv" : "negiert");
        logging_log_message(buf);
    }
}

void rbs_initialize_memory(memory_t memory, uint32_t value_count)
{
    if (memory != NULL)
    {
        memset(memory, 0, value_count * sizeof(double));
    }
}

bool rbs_compare(memory_t memory, uint32_t value_count, int32_t value, enum operation op, double operand)
{
    bool valid = (memory != NULL) && (value >= 0) && ((uint32_t) value < value_count);
    double lhs = valid ? memory[(size_t) value] : 0.0;
    int32_t op_value = op;

    bool result = false;
    switch (op_value)
    {
        case EQ: result = lhs == operand; break;
        case NE: result = lhs != operand; break;
        case LT: result = lhs < operand; break;
        case LE: result = lhs <= operand; break;
        case GT: result = lhs > operand; break;
        case GE: result = lhs >= operand; break;
        default: result = false; break;
    }
    return valid && result;
}

bool rbs_term_is_true(struct rbs* rbs, rbs_term_t term)
{
    bool result = false;
    if (rbs != NULL && term != NULL)
    {
        if (term->comparison)
        {
            result = rbs_compare(rbs->memory, rbs->value_count, term->value_enum, term->op, term->operand);
        }
        else
        {
            result = rbs_is_fact(rbs->facts, rbs->token_count, term->fact_enum);
        }
    }
    return result;
}

static void set_fact_if_changed(int32_t* facts, uint32_t token_count, int32_t fact, int32_t* changed, size_t* changed_count)
{
    uint32_t magnitude = fact < 0 ? (uint32_t) -fact : (uint32_t) fact;
    bool valid = (facts != NULL) && (fact != 0) &&
                 (magnitude < token_count) && (magnitude != 0);

    if (valid)
    {
        uint32_t idx = magnitude - 1;
        if (facts[idx] != fact)
        {
            facts[idx] = fact;
            if (changed && changed_count && *changed_count < 256)
            {
                changed[*changed_count] = fact;
                (*changed_count)++;
            }
        }
    }
}

static void step_match_rules(struct rbs* rbs, const rbs_rule_t rules, size_t rule_count, bool* matched);
static size_t step_evaluate_effects(struct rbs* rbs, const rbs_effect_t effects, size_t effect_count, bool* fired, double* results);
static size_t step_apply_facts(struct rbs* rbs, const rbs_rule_t rules, size_t rule_count, const bool* matched, int32_t* changed_facts);
static void step_apply_effects(struct rbs* rbs, const rbs_effect_t effects, size_t effect_count, const bool* fired, const double* results);
static void step_log(struct rbs* rbs, const int32_t* changed_facts, size_t changed_count);

void rbs_step(struct rbs* rbs, const rbs_rule_t rules, size_t rule_count,
              const rbs_effect_t effects, size_t effect_count)
{
    bool valid = (rbs != NULL) && (rbs->facts != NULL) && (rbs->memory != NULL) &&
                 (rule_count == 0 || rules != NULL) &&
                 (effect_count == 0 || effects != NULL);
    if (valid)
    {
        bool matched[rule_count > 0 ? rule_count : 1];
        bool fired[effect_count > 0 ? effect_count : 1];
        double results[effect_count > 0 ? effect_count : 1];
        int32_t changed_facts[256];

        step_match_rules(rbs, rules, rule_count, matched);
        step_evaluate_effects(rbs, effects, effect_count, fired, results);
        size_t changed_count = step_apply_facts(rbs, rules, rule_count, matched, changed_facts);
        step_apply_effects(rbs, effects, effect_count, fired, results);
        step_log(rbs, changed_facts, changed_count);
    }
}

static void step_match_rules(struct rbs* rbs, const rbs_rule_t rules, size_t rule_count,
                             bool* matched)
{
    for (size_t i = 0; i < rule_count; ++i)
    {
        matched[i] = true;
        for (rbs_term_t term = rules[i].if_terms; !(term->fact_enum == 0 && !term->comparison); ++term)
        {
            if (!rbs_term_is_true(rbs, term))
            {
                matched[i] = false;
                break;
            }
        }
    }
}

static size_t step_evaluate_effects(struct rbs* rbs, const rbs_effect_t effects, size_t effect_count,
                                    bool* fired, double* results)
{
    size_t valid_count = 0;
    for (size_t i = 0; i < effect_count; ++i)
    {
        const rbs_effect_t effect = &effects[i];
        double result = 0.0;
        bool ok = rbs_is_fact(rbs->facts, rbs->token_count, effect->trigger_fact_enum) &&
                  effect->value_enum >= 0 && (uint32_t) effect->value_enum < rbs->value_count;
        if (ok)
        {
            result = rbs->memory[(size_t) effect->value_enum];
            switch (effect->op)
            {
                case ADD: result += effect->operand; break;
                case SUB: result -= effect->operand; break;
                case MUL: result *= effect->operand; break;
                case DIV:
                    if (effect->operand != 0.0)
                    {
                        result /= effect->operand;
                    }
                    else
                    {
                        ok = false;
                    }
                    break;
                default: break;
            }
        }
        fired[i] = ok;
        results[i] = result;
        if (ok)
        {
            valid_count++;
        }
    }
    return valid_count;
}

static size_t step_apply_facts(struct rbs* rbs, const rbs_rule_t rules, size_t rule_count,
                               const bool* matched, int32_t* changed_facts)
{
    size_t changed_count = 0;
    for (size_t i = 0; i < rule_count; ++i)
    {
        const int32_t* fact_list = NULL;
        if (matched[i])
        {
            fact_list = rules[i].then_facts;
        }
        else if (rules[i].else_facts != NULL)
        {
            fact_list = rules[i].else_facts;
        }
        if (fact_list)
        {
            for (const int32_t* fact = fact_list; *fact != 0; ++fact)
            {
                set_fact_if_changed(rbs->facts, rbs->token_count, *fact, changed_facts, &changed_count);
            }
        }
    }
    return changed_count;
}

static void step_apply_effects(struct rbs* rbs, const rbs_effect_t effects, size_t effect_count,
                               const bool* fired, const double* results)
{
    for (size_t i = 0; i < effect_count; ++i)
    {
        if (fired[i])
        {
            rbs->memory[(size_t) effects[i].value_enum] = results[i];
        }
    }
}

static void step_log(struct rbs* rbs, const int32_t* changed_facts, size_t changed_count)
{
    char buf[512];
    int pos = snprintf(buf, sizeof(buf), "rbs_step end");
    for (size_t i = 0; i < changed_count; ++i)
    {
        int32_t f = changed_facts[i];
        const char* name = NULL;
        uint32_t magnitude = f < 0 ? (uint32_t)-f : (uint32_t)f;
        if (rbs && rbs->fact_names && rbs->fact_names_count > 0 &&
            magnitude > 0 && magnitude <= rbs->fact_names_count)
        {
            name = rbs->fact_names[magnitude - 1];
        }
        if (name && name[0] != '\0')
        {
            pos += snprintf(buf + pos, sizeof(buf) - pos, " %s", name);
        }
        else
        {
            pos += snprintf(buf + pos, sizeof(buf) - pos, " %d", f);
        }
    }
    logging_log_message(buf);
}
