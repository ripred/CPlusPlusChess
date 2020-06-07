/**
 * The MoveCache class stores known board states and the moves
 * made from them in order to re-use previous computed moves
 * without the computational overhead
 *
 */

#include <movecache.h>

#include <array>

namespace chess {
  using std::array;

  map<unsigned int, map<string, MoveCache::Entry>> MoveCache::cache;
  mutex MoveCache::cacheMutex;

  int MoveCache::num_offered;
  int MoveCache::num_entries;
  int MoveCache::num_lookups;
  int MoveCache::num_changed;
  int MoveCache::num_found;

  string MoveCache::createKey(const Board &board) {
    static array<char const, 7> const b = {' ', 'p', 'n', 'b', 'r', 'q', 'k'};
    static array<char const, 7> const w = {' ', 'P', 'N', 'B', 'R', 'Q', 'K'};
    static array<array<char const, 7>, 2> const symbols{b, w};
    array<char, BOARD_SIZE + 1> key{};
    key.fill(' ');
    key.at(BOARD_SIZE) = 0;

    for (size_t i = 0; i < BOARD_SIZE; ++i) {
      Piece p = board.board[i];
      key[i] = symbols[getSide(p)][getType(p)];
    }
    return string(begin(key), end(key));
  }

  MoveCache::Entry MoveCache::lookup(const Board &board, unsigned int side) {
    string key = createKey(board);
    ++num_lookups;

    std::lock_guard<std::mutex> guard(MoveCache::cacheMutex);

    auto const iterToSideMap = cache.find(side);
    if (iterToSideMap == cache.end()) return Entry();

    auto const &sideMapRef = (*iterToSideMap).second;
    auto const iterToEntry = sideMapRef.find(key);

    if (iterToEntry == sideMapRef.end()) return Entry();

    ++num_found;
    return (*iterToEntry).second;
  }

  void MoveCache::offer(const Board &board, Move &move, unsigned int side, int movesExamined) {
    ++num_offered;
    if (!move.isValid(board)) return;

    string key;
    MoveCacheType::iterator cacheMapIter;
    SideMapType::iterator entryMapIter;

    std::lock_guard<std::mutex> guard(cacheMutex);

    cacheMapIter = cache.find(side);
    if (cacheMapIter == cache.end()) {
      ++num_entries;
      cache[side][key] = Entry(move, movesExamined);
      return;
    }

    SideMapType &sideMap = (*cacheMapIter).second;
    key = createKey(board);
    entryMapIter = sideMap.find(key);
    if (entryMapIter == sideMap.end()) {
      ++num_entries;
      cache[side][key] = Entry(move, movesExamined);
      return;
    }

    Entry hit = cache[side][key];
    if (move.getValue() > hit.move.getValue()) {
      ++num_changed;
      hit.move = move;
      hit.changed++;
      hit.hit++;
      cache[side][key] = hit;
    }
  }

  void MoveCache::showMetrics() {
    using std::cout, std::endl;
    cout << "Offered: " << addCommas(num_offered) << endl;
    cout << "Lookups: " << addCommas(num_lookups) << endl;
    cout << "Changed: " << addCommas(num_changed) << endl;
    cout << "Entries: " << addCommas(num_entries) << endl;
    cout << "Found  : " << addCommas(num_found) << endl;
  }

}  // namespace chess
