#include "rbs.h"

#include <stdio.h>

typedef enum token
{
	N_PAY = -7,
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
	PAY = 7,
	TOKEN_COUNT
}* token_t;

typedef enum value
{
	AGE,
	MONEY,
	SPEED,
	VALUE_COUNT
}* value_t;

struct rbs_term if_weather[] =
{
	{ .comparison = false, .fact_enum = RAIN },
	{ .comparison = false, .fact_enum = CLOUDY },
	{ .comparison = false, .fact_enum = ZERO }
};
enum token then_weather[] = { WET, ZERO };

struct rbs_term if_wet[] =
{
	{ .comparison = false, .fact_enum = WET },
	{ .comparison = false, .fact_enum = ZERO }
};
enum token then_wet[] = { UMBRELLA, ZERO };

struct rbs_term if_adult[] =
{
	{ .comparison = true, .value_enum = AGE, .op = GT, .operand = 18 },
	{ .comparison = false, .fact_enum = ZERO }
};
enum token then_adult[] = { ADULT, PAY, ZERO };

struct rbs_rule rules[] =
{
	{ if_weather, then_weather },
	{ if_wet, then_wet },
	{ if_adult, then_adult }
};

struct rbs_effect effects[] =
{
	{ .trigger_fact_enum = PAY, .value_enum = MONEY, .op = SUB, .operand = 10 }
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
	rbs.memory[MONEY] = 100;

	rbs_set_fact(rbs.facts, RAIN);
	rbs_set_fact(rbs.facts, CLOUDY);

	size_t rule_count = sizeof(rules) / sizeof(rules[0]);
	rbs_fire(&rbs, rules, rule_count);

	size_t effect_count = sizeof(effects) / sizeof(effects[0]);
	rbs_apply_effects(&rbs, effects, effect_count);

	printf("WET:      %s\n", rbs_is_fact(rbs.facts, WET) ? "true" : "false");
	printf("UMBRELLA: %s\n", rbs_is_fact(rbs.facts, UMBRELLA) ? "true" : "false");
	printf("NOT WET:  %s\n", rbs_is_fact(rbs.facts, N_WET) ? "true" : "false");
	printf("ADULT:    %s\n", rbs_is_fact(rbs.facts, ADULT) ? "true" : "false");
	printf("MONEY:    %.0f\n", rbs.memory[MONEY]);

	rbs_destroy_facts_buffer(&rbs.facts);
	rbs_destroy_memory_buffer(&rbs.memory);

	return 0;
}