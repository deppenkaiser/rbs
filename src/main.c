#include <stdio.h>
#include "rbs.h"

typedef enum token
{
	N_ADULT = -6,
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
	ADULT = 6,
	TOKEN_COUNT
}* token_t;

typedef enum value
{
	AGE,
	MONEY,
	SPEED,
	VALUE_COUNT
}* value_t;

const rbs_term_t if_weather[] =
{
	{ .comparison = false, .fact = RAIN },
	{ .comparison = false, .fact = CLOUDY },
	{ .comparison = false, .fact = ZERO }
};
const int32_t then_weather[] = { WET, ZERO };
const rbs_term_t if_wet[] =
{
	{ .comparison = false, .fact = WET },
	{ .comparison = false, .fact = ZERO }
};
const int32_t then_wet[] = { UMBRELLA, ZERO };
const rbs_term_t if_adult[] =
{
	{ .comparison = true, .value = AGE, .op = GT, .operand = 18 },
	{ .comparison = false, .fact = ZERO }
};
const int32_t then_adult[] = { ADULT, ZERO };

const rbs_rule_t rules[] =
{
	{ if_weather, then_weather },
	{ if_wet, then_wet },
	{ if_adult, then_adult }
};

int main()
{
	struct rbs rbs =
	{
		.facts = rbs_create_facts_buffer(TOKEN_COUNT),
		.memory = rbs_create_memory_buffer(VALUE_COUNT)
	};

	rbs_initialize_facts(rbs.facts, TOKEN_COUNT);
	rbs_initialize_memory(rbs.memory, VALUE_COUNT);

	rbs.memory[AGE] = 20;

	rbs_set_fact(rbs.facts, RAIN);
	rbs_set_fact(rbs.facts, CLOUDY);

	size_t rule_count = sizeof(rules) / sizeof(rules[0]);
	rbs_fire(&rbs, rules, rule_count);

	printf("WET:      %s\n", rbs_is_fact(rbs.facts, WET) ? "true" : "false");
	printf("UMBRELLA: %s\n", rbs_is_fact(rbs.facts, UMBRELLA) ? "true" : "false");
	printf("NOT WET:  %s\n", rbs_is_fact(rbs.facts, N_WET) ? "true" : "false");
	printf("ADULT:    %s\n", rbs_is_fact(rbs.facts, ADULT) ? "true" : "false");

	rbs_destroy_facts_buffer(&rbs.facts);
	rbs_destroy_memory_buffer(&rbs.memory);

	return 0;
}