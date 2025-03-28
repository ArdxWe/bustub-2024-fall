//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_k_replacer.cpp
//
// Identification: src/buffer/lru_k_replacer.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_k_replacer.h"
#include <cstddef>
#include <optional>
#include <unordered_map>
#include "common/config.h"
#include "common/exception.h"

namespace bustub {

/**
 *
 * TODO(P1): Add implementation
 *
 * @brief a new LRUKReplacer.
 * @param num_frames the maximum number of frames the LRUReplacer will be required to store
 */
LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) : replacer_size_(num_frames), k_(k) {}

/**
 * TODO(P1): Add implementation
 *
 * @brief Find the frame with largest backward k-distance and evict that frame. Only frames
 * that are marked as 'evictable' are candidates for eviction.
 *
 * A frame with less than k historical references is given +inf as its backward k-distance.
 * If multiple frames have inf backward k-distance, then evict frame whose oldest timestamp
 * is furthest in the past.
 *
 * Successful eviction of a frame should decrement the size of replacer and remove the frame's
 * access history.
 *
 * @return true if a frame is evicted successfully, false if no frames can be evicted.
 */
auto LRUKReplacer::Evict() -> std::optional<frame_id_t> {
  if (curr_size_ == 0) {
    return std::nullopt;
  }

  bool has_less_k = false;
  std::unordered_map<frame_id_t, size_t> less;
  std::unordered_map<frame_id_t, size_t> k;

  for (const auto &pair : node_store_) {
    frame_id_t frame_id = pair.first;
    const LRUKNode &node = pair.second;

    if (!node.IsEvictable()) {
      continue;
    }

    if (!node.GetKDistance().has_value()) {
      has_less_k = true;
      less[frame_id] = node.GetLastest();
    } else if (!has_less_k) {
      k[frame_id] = node.GetKDistance().value();
    }
  }

  std::unordered_map<frame_id_t, size_t> x = k;

  if (has_less_k) {
    x = less;
  }

  frame_id_t frame_id;
  size_t min = std::numeric_limits<size_t>::max();

  for (const auto &pair : x) {
    if (pair.second < min) {
      frame_id = pair.first;
      min = pair.second;
    }
  }

  Remove(frame_id);
  return {frame_id};
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Record the event that the given frame id is accessed at current timestamp.
 * Create a new entry for access history if frame id has not been seen before.
 *
 * If frame id is invalid (ie. larger than replacer_size_), throw an exception. You can
 * also use BUSTUB_ASSERT to abort the process if frame id is invalid.
 *
 * @param frame_id id of frame that received a new access.
 * @param access_type type of access that was received. This parameter is only needed for
 * leaderboard tests.
 */
void LRUKReplacer::RecordAccess(frame_id_t frame_id, [[maybe_unused]] AccessType access_type) {
  if (frame_id < 0 || static_cast<size_t>(frame_id) >= replacer_size_) {
    throw Exception{std::string{"invalid frame_id"}};
  }

  if (node_store_.find(frame_id) == node_store_.end()) {
    node_store_[frame_id] = LRUKNode{k_};
  }

  auto now = std::chrono::high_resolution_clock::now();
  auto ts = static_cast<size_t>(std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count());
  node_store_[frame_id].Append(ts);
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Toggle whether a frame is evictable or non-evictable. This function also
 * controls replacer's size. Note that size is equal to number of evictable entries.
 *
 * If a frame was previously evictable and is to be set to non-evictable, then size should
 * decrement. If a frame was previously non-evictable and is to be set to evictable,
 * then size should increment.
 *
 * If frame id is invalid, throw an exception or abort the process.
 *
 * For other scenarios, this function should terminate without modifying anything.
 *
 * @param frame_id id of frame whose 'evictable' status will be modified
 * @param set_evictable whether the given frame is evictable or not
 */
void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
  if (frame_id < 0 || static_cast<size_t>(frame_id) >= replacer_size_) {
    throw Exception{std::string{"invalid frame_id"}};
  }

  if (node_store_.find(frame_id) == node_store_.end()) {
    node_store_[frame_id] = LRUKNode{k_};
  }

  auto &frame_node = node_store_[frame_id];
  if (frame_node.IsEvictable() && !set_evictable) {
    curr_size_--;
    frame_node.SetIsEvictable(false);
  } else if (!frame_node.IsEvictable() && set_evictable) {
    curr_size_++;
    frame_node.SetIsEvictable(set_evictable);
  }
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Remove an evictable frame from replacer, along with its access history.
 * This function should also decrement replacer's size if removal is successful.
 *
 * Note that this is different from evicting a frame, which always remove the frame
 * with largest backward k-distance. This function removes specified frame id,
 * no matter what its backward k-distance is.
 *
 * If Remove is called on a non-evictable frame, throw an exception or abort the
 * process.
 *
 * If specified frame is not found, directly return from this function.
 *
 * @param frame_id id of frame to be removed
 */
void LRUKReplacer::Remove(frame_id_t frame_id) {
  if (frame_id < 0 || static_cast<size_t>(frame_id) >= replacer_size_) {
    throw Exception{std::string{"invalid frame_id"}};
  }

  if (node_store_.find(frame_id) == node_store_.end()) {
    throw Exception{std::string{"invalid frame idaaa"}};
  }

  auto &node = node_store_[frame_id];
  if (!node.IsEvictable()) {
    throw Exception{std::string{"invalid frame id bbb"}};
  }

  node_store_.erase(frame_id);
  curr_size_--;
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Return replacer's size, which tracks the number of evictable frames.
 *
 * @return size_t
 */
auto LRUKReplacer::Size() -> size_t { return curr_size_; }

LRUKNode::LRUKNode(size_t k) : k_(k){};

auto LRUKNode::Append(size_t ts) -> void {
  history_.push_back(ts);
  if (history_.size() > k_) {
    history_.pop_front();
  }
}

auto LRUKNode::GetKDistance() const -> std::optional<size_t> {
  if (history_.size() < k_) {
    return std::nullopt;
  }

  return {history_.front()};
}

auto LRUKNode::GetLastest() const -> size_t { return history_.back(); }

auto LRUKNode::IsEvictable() const -> bool { return is_evictable_; }

auto LRUKNode::SetIsEvictable(bool is_evictable) -> void { is_evictable_ = is_evictable; }

LRUKNode::LRUKNode(const LRUKNode &other) : k_(other.k_) {}

auto LRUKNode::operator=(const LRUKNode &other) -> LRUKNode & {
  this->k_ = other.k_;
  return *this;
}
}  // namespace bustub
