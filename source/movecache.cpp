/**
 * The MoveCache class stores known board states and the moves
 * made from them in order to re-use previous computed moves
 * without the computational overhead
 *
 */

#include <movecache.h>
using namespace chess;

int MoveCache::minHits = 1;
float MoveCache::minRatio = 0.5;
map<string, MoveCache::Entry> MoveCache::cache;
