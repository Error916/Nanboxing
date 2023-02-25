#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

void print_bits(uint8_t *bytes, size_t byte_size) {
	for (int i = (int)byte_size - 1; i >= 0; --i) {
		uint8_t byte = bytes[i];
		for (int j = 7; j >= 0; --j) {
			printf("%d", !!(byte & (1 << j)));
		}
		printf(" ");
	}
	printf("\n");
}

#define INSPECT_VALUE(type, value, label)			\
	{							\
		type name = (value);				\
		printf("%s =\n\t", label);			\
		print_bits((uint8_t*)&name, sizeof(name));	\
	}							\

#define EXP_MASK (((1LL << 11LL) - 1LL) << 52LL)
#define FRACTION_MASK ((1LL << 52LL) - 1LL)
#define SIGN_MASK (1LL << 63LL)
#define TYPE_MASK (((1LL << 4LL) - 1LL) << 48LL)
#define VALUE_MASK ((1LL << 48LL) - 1LL)

// We have 3 bit to define types
// so we can store 8 custom types
#define TYPE(n) ((1LL << 3LL) + n)

inline double mk_inf(void) {
	uint64_t y = EXP_MASK;
	return *(double*)&y;
}

inline double set_type(double x, uint64_t type) {
	uint64_t y = *(uint64_t*)&x;
	y = (y & (~TYPE_MASK)) | (((TYPE_MASK >> 48LL) & type) << 48LL);
	return *(double*)&y;
}

inline uint64_t get_type(double x) {
	uint64_t y = *(uint64_t*)&x;
	return (y & TYPE_MASK) >> 48LL;
}

inline double set_value(double x, uint64_t value) {
	uint64_t y = *(uint64_t*)&x;
	y = (y & (~VALUE_MASK)) | (value & VALUE_MASK);
	return *(double*)&y;
}

inline uint64_t get_value(double x) {
	uint64_t y = *(uint64_t*)&x;
	return (y & VALUE_MASK);
}

inline int is_inf(double x) {
	uint64_t y = *(uint64_t*)&x;
	return ((y & EXP_MASK) == EXP_MASK) && ((y & FRACTION_MASK) == 0);
}

inline int is_nan(double x) {
	uint64_t y = *(uint64_t*)&x;
	return ((y & EXP_MASK) == EXP_MASK) && ((y & FRACTION_MASK) > 0);
}

#define DOUBLE_TYPE 0
#define INTEGER_TYPE 1
#define POINTER_TYPE 2

inline int is_double(double x) {
	return !is_nan(x);
}

inline int is_integer(double x) {
	return is_nan(x) && get_type(x) == TYPE(INTEGER_TYPE);
}

inline int is_pointer(double x) {
	return is_nan(x) && get_type(x) == TYPE(POINTER_TYPE);
}

inline double as_double(double x) {
	return x;
}

inline uint64_t as_integer(double x) {
	return get_value(x);
}

inline void *as_pointer(double x) {
	return (void*)get_value(x);
}

inline double box_double(double x) {
	return x;
}

inline double box_integer(uint64_t x) {
	return set_value(set_type(mk_inf(), TYPE(INTEGER_TYPE)), x);
}

inline double box_pointer(void *x) {
	return set_value(set_type(mk_inf(), TYPE(POINTER_TYPE)), (uint64_t)x);
}

int main(void) {
	double pi = 3.14159265359;

	assert(pi 		== as_double(box_double(pi)));
	assert(12345678LL 	== as_integer(box_integer(12345678LL)));
	assert(&pi 		== as_pointer(box_pointer(&pi)));

	printf("OK\n");

	return 0;
}
