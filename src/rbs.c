#include <logging/logging.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include "rbs.h"

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
    if (facts == NULL)
    {
        return;
    }

    for (uint32_t i = 1; i < token_count; ++i)
    {
        facts[i - 1] = rbs_invert_token(i);
    }
}

bool rbs_is_fact(int32_t* facts, uint32_t token_count, int32_t fact)
{
    if (facts == NULL || fact == 0)
    {
        return false;
    }

    uint32_t magnitude = fact < 0 ? (uint32_t) -fact : (uint32_t) fact;
    if (magnitude >= token_count)
    {
        return false;
    }

    bool is_positiv = fact > 0;
    return facts[magnitude - 1] == fact;
}

void rbs_set_fact(int32_t* facts, uint32_t token_count, int32_t fact)
{
    if (facts == NULL || fact == 0)
    {
        return;
    }

    uint32_t magnitude = fact < 0 ? (uint32_t) -fact : (uint32_t) fact;
    if (magnitude >= token_count)
    {
        return;
    }

    facts[magnitude - 1] = fact;
}

void rbs_initialize_memory(memory_t memory, uint32_t value_count)
{
    if (memory == NULL)
    {
        return;
    }

    memset(memory, 0, value_count * sizeof(double));
}

bool rbs_compare(memory_t memory, uint32_t value_count, int32_t value, enum operation op, double operand)
{
    if (memory == NULL || value < 0 || (uint32_t) value >= value_count)
    {
        return false;
    }

    double lhs = memory[(size_t) value];
    int32_t op_value = op;

    switch (op_value)
    {
        case EQ: return lhs == operand;
        case NE: return lhs != operand;
        case LT: return lhs < operand;
        case LE: return lhs <= operand;
        case GT: return lhs > operand;
        case GE: return lhs >= operand;
        default: return false;
    }
}

bool rbs_term_is_true(struct rbs* rbs, rbs_term_t term)
{
    if (rbs == NULL || term == NULL)
    {
        return false;
    }

    if (term->comparison)
    {
        return rbs_compare(rbs->memory, rbs->value_count, term->value_enum, term->op, term->operand);
    }
    return rbs_is_fact(rbs->facts, rbs->token_count, term->fact_enum);
}

static void set_fact_if_changed(int32_t* facts, uint32_t token_count, int32_t fact, int32_t* changed, size_t* changed_count)
{
    if (facts == NULL || fact == 0)
    {
        return;
    }
    uint32_t magnitude = fact < 0 ? (uint32_t) -fact : (uint32_t) fact;
    if (magnitude >= token_count || magnitude == 0)
    {
        return;
    }
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

void rbs_step(struct rbs* rbs, const rbs_rule_t rules, size_t rule_count,
              const rbs_effect_t effects, size_t effect_count)
{
    if (rbs == NULL || rbs->facts == NULL || rbs->memory == NULL)
    {
        return;
    }
    if ((rule_count > 0 && rules == NULL) || (effect_count > 0 && effects == NULL))
    {
        return;
    }

    bool matched[rule_count > 0 ? rule_count : 1];
    bool fired[effect_count > 0 ? effect_count : 1];
    double results[effect_count > 0 ? effect_count : 1];

    for (size_t i = 0; i < rule_count; ++i)
    {
        const rbs_rule_t rule = &rules[i];
        matched[i] = true;

        for (rbs_term_t term = rule->if_terms; !(term->fact_enum == 0 && !term->comparison); ++term)
        {
            if (!rbs_term_is_true(rbs, term))
            {
                matched[i] = false;
                break;
            }
        }
    }

    for (size_t i = 0; i < effect_count; ++i)
    {
        const rbs_effect_t effect = &effects[i];
        fired[i] = rbs_is_fact(rbs->facts, rbs->token_count, effect->trigger_fact_enum);
        results[i] = 0.0;

        if (!fired[i] || effect->value_enum < 0 || (uint32_t) effect->value_enum >= rbs->value_count)
        {
            fired[i] = false;
            continue;
        }

        double result = rbs->memory[(size_t) effect->value_enum];
        switch (effect->op)
        {
            case ADD: result += effect->operand; break;
            case SUB: result -= effect->operand; break;
            case MUL: result *= effect->operand; break;
            case DIV:
                if (effect->operand == 0.0)
                {
                    fired[i] = false;
                    continue;
                }
                result /= effect->operand;
                break;
            default: break;
        }
        results[i] = result;
    }

    int32_t changed_facts[256];
    size_t changed_count = 0;

    for (size_t i = 0; i < rule_count; ++i)
    {
        if (!matched[i])
        {
            continue;
        }
        for (const int32_t* fact = rules[i].then_facts; *fact != 0; ++fact)
        {
            set_fact_if_changed(rbs->facts, rbs->token_count, *fact, changed_facts, &changed_count);
        }
    }

    for (size_t i = 0; i < effect_count; ++i)
    {
        if (!fired[i])
        {
            continue;
        }
        rbs->memory[(size_t) effects[i].value_enum] = results[i];
        int32_t inv = rbs_invert_token(effects[i].trigger_fact_enum);
        set_fact_if_changed(rbs->facts, rbs->token_count, inv, changed_facts, &changed_count);
    }

    {
        char buf[512];
        int pos = snprintf(buf, sizeof(buf), "rbs_step end");
        for (size_t i = 0; i < changed_count; ++i)
        {
            int32_t f = changed_facts[i];
            const char* name = NULL;
            uint32_t magnitude = f < 0 ? (uint32_t)-f : (uint32_t)f;
            if (rbs && rbs->fact_names && magnitude > 0 && magnitude <= rbs->token_count)
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
}
