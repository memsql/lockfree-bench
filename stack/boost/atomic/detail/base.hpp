#ifndef BOOST_DETAIL_ATOMIC_BASE_HPP
#define BOOST_DETAIL_ATOMIC_BASE_HPP

//  Copyright (c) 2009 Helge Bahmann
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

// Base class definition and fallback implementation.
// To be overridden (through partial specialization) by
// platform implementations.

#include <string.h>

#include "../../memory_order.hpp"

#ifndef DISABLE_ATOMIC_OPERATORS

#define BOOST_ATOMIC_DECLARE_ASSIGNMENT_OPERATORS \
    operator value_type(void) volatile const \
    { \
        return load(memory_order_seq_cst); \
    } \
     \
    this_type & \
    operator=(value_type v) volatile \
    { \
        store(v, memory_order_seq_cst); \
        return *const_cast<this_type *>(this); \
    }

#else

// locked out
//
#define BOOST_ATOMIC_DECLARE_ASSIGNMENT_OPERATORS

#endif

#define BOOST_ATOMIC_DECLARE_BASE_OPERATORS \
    BOOST_ATOMIC_DECLARE_ASSIGNMENT_OPERATORS \
	 \
	bool \
	compare_exchange_strong( \
		value_type & expected, \
		value_type desired, \
		memory_order order = memory_order_seq_cst) volatile \
	{ \
		return compare_exchange_strong(expected, desired, order, calculate_failure_order(order)); \
	} \
	 \
	bool \
	compare_exchange_weak( \
		value_type & expected, \
		value_type desired, \
		memory_order order = memory_order_seq_cst) volatile \
	{ \
		return compare_exchange_weak(expected, desired, order, calculate_failure_order(order)); \
	} \
	 \

#ifndef DISABLE_ATOMIC_OPERATORS

#define BOOST_ATOMIC_DECLARE_ADDITIVE_OPERATORS \
	value_type \
	operator++(int) volatile \
	{ \
		return fetch_add(1); \
	} \
	 \
	value_type \
	operator++(void) volatile \
	{ \
		return fetch_add(1) + 1; \
	} \
	 \
	value_type \
	operator--(int) volatile \
	{ \
		return fetch_sub(1); \
	} \
	 \
	value_type \
	operator--(void) volatile \
	{ \
		return fetch_sub(1) - 1; \
	} \
	 \
	value_type \
	operator+=(difference_type v) volatile \
	{ \
		return fetch_add(v) + v; \
	} \
	 \
	value_type \
	operator-=(difference_type v) volatile \
	{ \
		return fetch_sub(v) - v; \
	} \

#define BOOST_ATOMIC_DECLARE_BIT_OPERATORS \
	value_type \
	operator&=(difference_type v) volatile \
	{ \
		return fetch_and(v) & v; \
	} \
	 \
	value_type \
	operator|=(difference_type v) volatile \
	{ \
		return fetch_or(v) | v; \
	} \
	 \
	value_type \
	operator^=(difference_type v) volatile \
	{ \
		return fetch_xor(v) ^ v; \
	} \

#else

// locked out
//
#define BOOST_ATOMIC_DECLARE_ADDITIVE_OPERATORS
#define BOOST_ATOMIC_DECLARE_BIT_OPERATORS

#endif

#define BOOST_ATOMIC_DECLARE_POINTER_OPERATORS \
	BOOST_ATOMIC_DECLARE_BASE_OPERATORS \
	BOOST_ATOMIC_DECLARE_ADDITIVE_OPERATORS \

#define BOOST_ATOMIC_DECLARE_INTEGRAL_OPERATORS \
	BOOST_ATOMIC_DECLARE_BASE_OPERATORS \
	BOOST_ATOMIC_DECLARE_ADDITIVE_OPERATORS \
	BOOST_ATOMIC_DECLARE_BIT_OPERATORS \

namespace boost {
namespace detail {
namespace atomic {

static inline memory_order
calculate_failure_order(memory_order order)
{
	switch(order) {
		case memory_order_acq_rel:
			return memory_order_acquire;
		case memory_order_release:
			return memory_order_relaxed;
		default:
			return order;
	}
}

template<typename T, typename C , unsigned int Size>
class base_atomic;

}
}
}

#endif
