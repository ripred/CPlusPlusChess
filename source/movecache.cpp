/**
 * The MoveCache class stores known board states and the moves
 * made from them in order to re-use previous computed moves
 * without the computational overhead
 *
 */

#include <movecache.h>

#include <array>
#include <mutex>

namespace chess {
  using std::array;
  using std::lock_guard;
  using std::make_unique;
  using std::mutex;

  MoveCache::MoveCache() {
    num_offered = 0;
    num_entries = 0;
    num_lookups = 0;
    num_changed = 0;
    num_found = 0;
    pCacheMutex = make_unique<mutex>();
  }

  /* static */
  string MoveCache::createKey(const Board& board) {
    static array<char const, 7> const b = {'.', 'p', 'n', 'b', 'r', 'q', 'k'};
    static array<char const, 7> const w = {'.', 'P', 'N', 'B', 'R', 'Q', 'K'};
    static array<array<char const, 7>, 2> const symbols{b, w};
    array<char, BOARD_SIZE + 1> key{};
    key.fill('.');
    key.at(BOARD_SIZE) = 0;

    for (size_t i = 0; i < BOARD_SIZE; ++i) {
      Piece p = board.board[i];
      key[i] = symbols[getSide(p)][getType(p)];
    }
    return string(begin(key), end(key));
  }

  void MoveCache::offer(Board const& board, Move const& move, Color side, int value,
                        int movesExamined) {
    if (!move.isValid(board)) return;

    string key;
    MoveCacheType::iterator cacheMapIter;
    SideMapType::iterator entryMapIter;

    lock_guard<mutex> guard(*pCacheMutex);
    ++num_offered;

    cacheMapIter = cache.find(side);
    if (cacheMapIter == cache.end()) {
      ++num_entries;
      cache[side][key] = Entry(move, movesExamined, value);
      return;
    }

    key = createKey(board);
    SideMapType& sideMap = (*cacheMapIter).second;

    entryMapIter = sideMap.find(key);
    if (entryMapIter == sideMap.end()) {
      ++num_entries;
      cache[side][key] = Entry(move, movesExamined, value);
      return;
    }

    // get the best move so far for this side and board
    Entry& best = cache[side][key];

    // see if the value param is better and replace this move if so
    if ((side == White && value > best.move.getValue())
        || (side == Black && value < best.move.getValue())) {
      best.move = move;
      best.move.setValue(value);
      best.movesExamined += movesExamined;
      cache[side][key] = best;
      ++num_changed;
    }
  }

  Entry MoveCache::lookup(Board const& board, Color const side) {
    lock_guard<mutex> guard(*pCacheMutex);
    ++num_lookups;

    if (cache.find(side) != cache.end()) {
      string const key = createKey(board);
      if (cache[side].find(key) != cache[side].end()) {
        Entry& entry = cache[side][key];
        if (entry.numRetries != 0) {
          ++num_found;
          return entry;
        }
      }
    }
    return Entry();
  }

  double MoveCache::getRisk(Board const& board, Color const side) {
    lock_guard<mutex> guard(*pCacheMutex);
    if (cache.find(side) != cache.end()) {
      string const key = createKey(board);
      if (cache[side].find(key) != cache[side].end()) {
        Entry const& entry = cache[side][key];
        if (entry.numRetries != 0) {
          return double(entry.numBetter) / double(entry.numRetries);
        }
      }
    }
    return 1.0;
  }

  void MoveCache::increaseMoveUsedCount(Board const& board, Color const side) {
    lock_guard<mutex> guard(*pCacheMutex);
    if (cache.find(side) != cache.end()) {
      string const key = createKey(board);
      if (cache[side].find(key) != cache[side].end()) {
        Entry& entry = cache[side][key];
        entry.numRetries++;
      }
    }
  }

  void MoveCache::increaseMoveImprovedCount(Board const& board, Color const side) {
    lock_guard<mutex> guard(*pCacheMutex);
    if (cache.find(side) != cache.end()) {
      string const key = createKey(board);
      if (cache[side].find(key) != cache[side].end()) {
        Entry& entry = cache[side][key];
        entry.numBetter++;
      }
    }
  }

  void MoveCache::showMetrics() const {
    using std::cout, std::endl;
    cout << "Offered: " << addCommas(num_offered) << endl;
    cout << "Lookups: " << addCommas(num_lookups) << endl;
    cout << "Changed: " << addCommas(num_changed) << endl;
    cout << "Entries: " << addCommas(num_entries) << endl;
    cout << "Found  : " << addCommas(num_found) << endl;
  }

}  // namespace chess
