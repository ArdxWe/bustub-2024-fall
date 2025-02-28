//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hyperloglog.cpp
//
// Identification: src/primer/hyperloglog.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <bitset>
#include <cassert>
#include <cmath>

#include "primer/hyperloglog.h"

namespace bustub {

/** @brief Parameterized constructor. */
template <typename KeyType>
HyperLogLog<KeyType>::HyperLogLog(int16_t n_bits)
    : cardinality_(0), buckets_(static_cast<int64_t>(pow(2, n_bits)), 0), bits_{n_bits} {}

/**
 * @brief Function that computes binary.
 *
 * @param[in] hash
 * @returns binary of a given hash
 */
template <typename KeyType>
auto HyperLogLog<KeyType>::ComputeBinary(const hash_t &hash) const -> std::bitset<BITSET_CAPACITY> {
  /** @TODO(student) Implement this function! */
  return std::bitset<BITSET_CAPACITY>{hash};
}

/**
 * @brief Function that computes leading zeros.
 *
 * @param[in] bset - binary values of a given bitset
 * @returns leading zeros of given binary set
 */
template <typename KeyType>
auto HyperLogLog<KeyType>::PositionOfLeftmostOne(const std::bitset<BITSET_CAPACITY> &bset) const -> uint64_t {
  /** @TODO(student) Implement this function! */
  uint64_t p = 1;

  for (int i = BITSET_CAPACITY - bits_-1; i >= 0; i--) {
    if (bset[i]) {
      return p;
    }
    p++;
  }

  return p;
}

/**
 * @brief Adds a value into the HyperLogLog.
 *
 * @param[in] val - value that's added into hyperloglog
 */
template <typename KeyType>
auto HyperLogLog<KeyType>::AddElem(KeyType val) -> void {
  /** @TODO(student) Implement this function! */
  hash_t hash = CalculateHash(val);
  std::bitset<BITSET_CAPACITY> binary = ComputeBinary(hash);

  int bucket_index = 0;

  // compute bucket index.
  int p = 0;
  for (int i = BITSET_CAPACITY - bits_; i < BITSET_CAPACITY; i++) {
    if (binary[i]) {
      bucket_index += static_cast<int>(pow(2, p));
    }
    p++;
  }

  assert(bucket_index >= 0 && bucket_index < static_cast<int>(buckets_.size()));


  uint64_t pos = PositionOfLeftmostOne(binary);
  buckets_[bucket_index] = std::max(static_cast<uint64_t>(buckets_[bucket_index]), pos);
}

/**
 * @brief Function that computes cardinality.
 */
template <typename KeyType>
auto HyperLogLog<KeyType>::ComputeCardinality() -> void {
  /** @TODO(student) Implement this function! */
  double res = 0;
  for (int i = 0; i < static_cast<int>(buckets_.size()); i++) {
    res += 1 / (pow(2, buckets_[i]));
  }

  cardinality_ = CONSTANT * buckets_.size() * buckets_.size() / res;
}

template class HyperLogLog<int64_t>;
template class HyperLogLog<std::string>;

}  // namespace bustub
