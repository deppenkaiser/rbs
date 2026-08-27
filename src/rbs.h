#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef enum operation
{
	NONE = 0,
	ADD = 1,
	SUB = 2,
	MUL = 3,
	DIV = 4,
	EQ = 5,
	NE = 6,
	LT = 7,
	LE = 8,
	GT = 9,
	GE = 10,
	OPERATION_COUNT
}* operation_t;

typedef double* memory_t;

typedef struct rbs_term
{
	bool comparison;
	int32_t fact;
	int32_t value;
	enum operation op;
	double operand;
} rbs_term_t;

typedef struct rbs
{
	int32_t* facts;
	memory_t memory;
}* rbs_t;

typedef struct rbs_rule
{
	const rbs_term_t* if_terms;
	const int32_t* then_facts;
} rbs_rule_t;

bool rbs_check_token_is_true(int32_t token);
int32_t rbs_invert_token(int32_t token);
int32_t* rbs_create_facts_buffer(uint32_t token_count);
memory_t rbs_create_memory_buffer(uint32_t value_count);
void rbs_destroy_facts_buffer(int32_t** ptoken);
void rbs_destroy_memory_buffer(memory_t* pmemory);
void rbs_initialize_facts(int32_t* facts, uint32_t token_count);
bool rbs_is_fact(int32_t* facts, int32_t fact);
void rbs_set_fact(int32_t* facts, int32_t fact);
void rbs_initialize_memory(memory_t memory, uint32_t value_count);
bool rbs_compare(memory_t memory, int32_t value, enum operation op, double operand);
bool rbs_term_is_true(struct rbs* rbs, rbs_term_t term);
void rbs_fire(struct rbs* rbs, const rbs_rule_t* rules, size_t rule_count);