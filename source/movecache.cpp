/**
 * The MoveCache class stores known board states and the moves
 * made from them in order to re-use previous computed moves
 * without the computational overhead
 *
 */

#include <movecache.h>
#include <stdio.h>  // for snprintf(...)

#include <algorithm>
#include <array>
#include <iterator>
#include <memory>
#include <mutex>

namespace chess {
    using std::array;
    using std::get;
    using std::lock_guard;
    using std::make_unique;
    using std::mutex;
    using std::transform;

    MoveCache::MoveCache() {
        num_offered = 0;
        num_entries = 0;
        num_lookups = 0;
        num_changed = 0;
        num_found = 0;
        pCacheMutex = make_unique<mutex>();
    }

    /* static */
    string MoveCache::createKey(Board const& board) {
        static array<char const, 7> const b = {'.', 'p', 'n', 'b', 'r', 'q', 'k'};
        static array<char const, 7> const w = {'.', 'P', 'N', 'B', 'R', 'Q', 'K'};
        static array<array<char const, 7>, 2> const symbols{b, w};
        array<char, BOARD_SIZE + 1> key{};

        transform(begin(board.board), end(board.board), begin(key), [](Piece const& piece) -> char {
            return symbols[getSide(piece)][getType(piece)];
        });
        key.at(BOARD_SIZE) = 0;
        return string(begin(key), end(key));
    }

    void MoveCache::offer(Board const& board, Move const& move, Color const side, int const value,
                          int const movesExamined) {
        if (!move.isValid(board)) return;
        ++num_offered;
        string const key = createKey(board);

        lock_guard<mutex> guard(*pCacheMutex);
        EntryFindType entry = getEntry(board, side);
        if (!get<0>(entry)) {
            ++num_entries;
            cache[side][key] = Entry(move, movesExamined, value);
        } else {
            Entry& best = get<1>(entry);
            if ((side == White && value > best.getValue())
                || (side == Black && value < best.getValue())) {
                best.move = move;
                best.setValue(value);
                best.movesExamined += movesExamined;
                cache[side][key] = best;
                ++num_changed;
            }
        }
    }

    Entry MoveCache::lookup(Board const& board, Color const side) {
        lock_guard<mutex> guard(*pCacheMutex);
        ++num_lookups;
        EntryFindType entry = getEntry(board, side);
        if (get<0>(entry)) ++num_found;
        return get<1>(entry);
    }

    double MoveCache::getRisk(Board const& board, Color const side) {
        lock_guard<mutex> guard(*pCacheMutex);
        EntryFindType entry = getEntry(board, side);
        return get<0>(entry) ? get<1>(entry).getRisk() : 1.0;
    }

    void MoveCache::increaseMoveUsedCount(Board const& board, Color const side) {
        lock_guard<mutex> guard(*pCacheMutex);
        EntryFindType entry = getEntry(board, side);
        if (get<0>(entry)) {
            Entry& found = get<1>(entry);
            found.numRetries++;
        }
    }

    void MoveCache::increaseMoveImprovedCount(Board const& board, Color const side) {
        lock_guard<mutex> guard(*pCacheMutex);
        EntryFindType entry = getEntry(board, side);
        if (get<0>(entry)) {
            Entry& found = get<1>(entry);
            found.numBetter++;
        }
    }

    void MoveCache::showMetrics() const {
        using std::cout, std::endl;
        char buff[256] = "0.00 %";
        if (num_lookups > 0) {
            double pctUsed = (double(num_found) / double(num_lookups)) * 100.0;
            snprintf(buff, sizeof(buff), "%-.4g %%", pctUsed);
        }

        cout << "Lookups : " << addCommas(num_lookups) << endl;
        cout << "Found   : " << addCommas(num_found) << endl;
        cout << "Used    : " << buff << endl;
        cout << "Offered : " << addCommas(num_offered) << endl;
        cout << "Entries : " << addCommas(num_entries) << endl;
        cout << "Changed : " << addCommas(num_changed) << endl;
    }

}  // namespace chess
