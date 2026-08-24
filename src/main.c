
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum token
{
	N_START = -1,
	ZERO = 0,
	START = 1,
	TOKEN_COUNT
}* token_t;

typedef struct rbs
{
	token_t facts;
}* rbs_t;

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

token_t rbs_create_facts_buffer()
{
	return malloc(rbs_calculate_token_count() * sizeof(enum token));
}

void rbs_destroy_facts_buffer(token_t* ptoken)
{
	if (*ptoken != NULL)
	{
		free(*ptoken);
		*ptoken = NULL;
	}
}

void rbs_initialize_facts(token_t facts)
{
	for (size_t i = 1; i < TOKEN_COUNT; ++i)
	{
		facts[i - 1] = rbs_invert_token(i);
	}
}

int main()
{
	struct rbs rbs = 
	{
		.facts = rbs_create_facts_buffer()
	};

	rbs_initialize_facts(rbs.facts);

	rbs_destroy_facts_buffer(&rbs.facts);

	return 0;
}
