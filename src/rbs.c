#include <stdlib.h>
#include <memory.h>
#include "rbs.h"

bool rbs_check_token_is_true(int32_t token)
{
	bool is_true = false;

	if (token > 0)
	{
		is_true = true;
	}
	else if (token < 0)
	{
		is_true = false;
	}

	return is_true;
}

int32_t rbs_invert_token(int32_t token)
{
	return token * -1;
}

int32_t* rbs_create_facts_buffer(uint32_t token_count)
{
	return malloc((token_count * 2 - 1) * sizeof(int32_t));
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
	for (uint32_t i = 1; i < token_count; ++i)
	{
		facts[i - 1] = rbs_invert_token(i);
	}
}

bool rbs_is_fact(int32_t* facts, int32_t fact)
{
	uint32_t magnitude = fact < 0 ? (uint32_t) -fact : (uint32_t) fact;

	return facts[magnitude - 1] == fact;
}

void rbs_set_fact(int32_t* facts, int32_t fact)
{
	uint32_t magnitude = fact < 0 ? (uint32_t) -fact : (uint32_t) fact;

	facts[magnitude - 1] = fact;
}

void rbs_initialize_memory(memory_t memory, uint32_t value_count)
{
	memset(memory, 0, value_count * sizeof(double));
}

bool rbs_compare(memory_t memory, int32_t value, enum operation op, double operand)
{
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
	if (term.comparison)
	{
		return rbs_compare(rbs->memory, term.value, term.op, term.operand);
	}
	return rbs_is_fact(rbs->facts, term.fact);
}

void rbs_fire(struct rbs* rbs, const rbs_rule_t* rules, size_t rule_count)
{
	for (size_t i = 0; i < rule_count; ++i)
	{
		const rbs_rule_t* rule = &rules[i];
		bool matches = true;

		for (const rbs_term_t* term = rule->if_terms; !(term->fact == 0 && !term->comparison); ++term)
		{
			if (!rbs_term_is_true(rbs, *term))
			{
				matches = false;
				break;
			}
		}

		if (matches)
		{
			for (const int32_t* fact = rule->then_facts; *fact != 0; ++fact)
			{
				rbs_set_fact(rbs->facts, *fact);
			}
		}
	}
}