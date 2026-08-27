
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <memory.h>

typedef enum token
{
	N_UMBRELLA = -5,
	N_WET = -4,
	N_CLOUDY = -3,
	N_RAIN = -2,
	N_START = -1,
	ZERO = 0,
	START = 1,
	RAIN = 2,
	CLOUDY = 3,
	WET = 4,
	UMBRELLA = 5,
	TOKEN_COUNT
}* token_t;

typedef enum value
{
	AGE,
	MONEY,
	SPEED,
	VALUE_COUNT
}* value_t;

typedef double* memory_t;

typedef struct rbs
{
	token_t facts;
	memory_t memory;
}* rbs_t;

typedef struct rbs_rule
{
	const enum token* if_facts;
	const enum token* then_facts;
} rbs_rule_t;

bool rbs_check_token_is_true(enum token token)
{
	bool is_true = false;
	int32_t token_value = token;
	
	if (token_value > 0)
	{
		is_true = true;
	}
	else if (token_value < 0)
	{
		is_true = false;
	}

	return is_true;
}

enum token rbs_invert_token(enum token token)
{
	int32_t token_value = token;
	return (enum token) token_value * -1;
}

uint32_t rbs_calculate_token_count()
{
	return TOKEN_COUNT * 2 - 1;
}

uint32_t rbs_calculate_value_count()
{
	return VALUE_COUNT;
}

token_t rbs_create_facts_buffer()
{
	return malloc(rbs_calculate_token_count() * sizeof(enum token));
}

memory_t rbs_create_memory_buffer()
{
	return malloc(rbs_calculate_value_count() * sizeof(double));
}

void rbs_destroy_facts_buffer(token_t* ptoken)
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

void rbs_initialize_facts(token_t facts)
{
	for (size_t i = 1; i < TOKEN_COUNT; ++i)
	{
		facts[i - 1] = rbs_invert_token(i);
	}
}

bool rbs_is_fact(token_t facts, enum token fact)
{
	int32_t token_value = fact;
	uint32_t magnitude = token_value < 0 ? (uint32_t) -token_value : (uint32_t) token_value;

	return facts[magnitude - 1] == fact;
}

void rbs_set_fact(token_t facts, enum token fact)
{
	int32_t token_value = fact;
	uint32_t magnitude = token_value < 0 ? (uint32_t) -token_value : (uint32_t) token_value;

	facts[magnitude - 1] = fact;
}

void rbs_initialize_memory(memory_t memory)
{
	memset(memory, 0, rbs_calculate_value_count() * sizeof(double));
}

void rbs_fire(struct rbs* rbs, const rbs_rule_t* rules, size_t rule_count)
{
	for (size_t i = 0; i < rule_count; ++i)
	{
		const rbs_rule_t* rule = &rules[i];
		bool matches = true;

		for (const enum token* fact = rule->if_facts; *fact != ZERO; ++fact)
		{
			if (!rbs_is_fact(rbs->facts, *fact))
			{
				matches = false;
				break;
			}
		}

		if (matches)
		{
			for (const enum token* fact = rule->then_facts; *fact != ZERO; ++fact)
			{
				rbs_set_fact(rbs->facts, *fact);
			}
		}
	}
}

const enum token if_weather[] = { RAIN, CLOUDY, ZERO };
const enum token then_weather[] = { WET, ZERO };
const enum token if_wet[] = { WET, ZERO };
const enum token then_wet[] = { UMBRELLA, ZERO };

const rbs_rule_t rules[] =
{
	{ if_weather, then_weather },
	{ if_wet, then_wet }
};

int main()
{
	struct rbs rbs =
	{
		.facts = rbs_create_facts_buffer(),
		.memory = rbs_create_memory_buffer()
	};

	rbs_initialize_facts(rbs.facts);
	rbs_initialize_memory(rbs.memory);

	rbs_set_fact(rbs.facts, RAIN);
	rbs_set_fact(rbs.facts, CLOUDY);

	size_t rule_count = sizeof(rules) / sizeof(rules[0]);
	rbs_fire(&rbs, rules, rule_count);

	printf("WET:      %s\n", rbs_is_fact(rbs.facts, WET) ? "true" : "false");
	printf("UMBRELLA: %s\n", rbs_is_fact(rbs.facts, UMBRELLA) ? "true" : "false");
	printf("NOT WET:  %s\n", rbs_is_fact(rbs.facts, N_WET) ? "true" : "false");

	rbs_destroy_facts_buffer(&rbs.facts);
	rbs_destroy_memory_buffer(&rbs.memory);

	return 0;
}
