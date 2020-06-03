/**
 * The MoveCache class stores known board states and the moves
 * made from them in order to re-use previous computed moves
 * without the computational overhead
 *
 */

#include <movecache.h>
namespace chess {
  map<string, MoveCache::Entry> MoveCache::cache;

  int MoveCache::num_offered;
  int MoveCache::num_entries;
  int MoveCache::num_lookups;
  int MoveCache::num_changed;
  int MoveCache::num_found;

  string MoveCache::createKey(const Board& board) {
    char key[BOARD_SIZE + 1]{};
    for (auto i = 0; i < BOARD_SIZE; ++i) {
      unsigned int p = board.board[i];
      key[i] = symbols[getSide(p)][getType(p)];
    }
    key[BOARD_SIZE] = 0;
    return string(key);
  }

  Move MoveCache::lookup(const Board& board) {
    string key = createKey(board);
    ++num_lookups;

    if (cache.find(key) == cache.end()) return Move();
    ++num_found;
    return cache[key].move;
  }

  void MoveCache::offer(const Board& board, Move& move) {
    ++num_offered;

    if (!move.isValid(board)) return;
    string key = createKey(board);
    if (cache.find(key) == cache.end()) {
      ++num_entries;
      cache[key] = Entry(move);
      return;
    }
    Entry hit = cache[key];
    if (move.getValue() > hit.move.getValue()) {
      ++num_changed;
      hit.move = move;
      hit.changed++;
      hit.hit++;
      cache[key] = hit;
    }
  }
}  // namespace chess
