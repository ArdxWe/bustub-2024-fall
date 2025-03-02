//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hyperloglog_presto.cpp
//
// Identification: src/primer/hyperloglog_presto.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/hyperloglog_presto.h"
#include <bitset>
#include <cstdint>

namespace bustub {

/** @brief Parameterized constructor. */
template <typename KeyType>
HyperLogLogPresto<KeyType>::HyperLogLogPresto(int16_t n_leading_bits)
    : dense_bucket_(static_cast<int>(std::pow(2, n_leading_bits)), 0), cardinality_(0), bits_(n_leading_bits) {}

/** @brief Element is added for HLL calculation. */
template <typename KeyType>
auto HyperLogLogPresto<KeyType>::AddElem(KeyType val) -> void {
  /** @TODO(student) Implement this function! */
  hash_t hash = CalculateHash(val);
  std::bitset<BITSET_CAPACITY> binary(hash);

  int bucket_index = 0;

  // compute bucket index.
  int p = 0;
  for (int i = BITSET_CAPACITY - bits_; i < BITSET_CAPACITY; i++) {
    if (binary[i]) {
      bucket_index += static_cast<int>(pow(2, p));
    }
    p++;
  }

  assert(bucket_index >= 0 && bucket_index < static_cast<int>(dense_bucket_.size()));

  uint64_t zeros_count = 0;
  for (int i = 0; i < BITSET_CAPACITY - bits_; i++) {
    if (!binary[i]) {
      zeros_count++;
    } else {
      break;
    }
  }

  zeros_count = zeros_count % (1 << 7);

  auto old_value = dense_bucket_[bucket_index].to_ulong() + overflow_bucket_[bucket_index].to_ulong() * (1 << 4);
  if (zeros_count > old_value) {
    uint64_t overflow = 0;
    uint64_t dense = 0;
    overflow = zeros_count >> 4;
    dense = zeros_count % (1 << 4);

    dense_bucket_[bucket_index] = std::bitset<DENSE_BUCKET_SIZE>(dense);
    overflow_bucket_[bucket_index] = overflow;
  }
}

/** @brief Function to compute cardinality. */
template <typename T>
auto HyperLogLogPresto<T>::ComputeCardinality() -> void {
  /** @TODO(student) Implement this function! */
  double res = 0;
  for (int i = 0; i < static_cast<int>(dense_bucket_.size()); i++) {
    res += 1 / (pow(2, dense_bucket_[i].to_ulong() + (overflow_bucket_[i].to_ulong() << 4)));
  }

  cardinality_ = CONSTANT * dense_bucket_.size() * dense_bucket_.size() / res;
}

template class HyperLogLogPresto<int64_t>;
template class HyperLogLogPresto<std::string>;
}  // namespace bustub
