#include "PlayerCache.hpp"

void PlayerCache::Update(const Player& player) {
    cache[player.id] = player;
        
}

std::optional<Player> PlayerCache::get(int playerId) const {
    auto it = cache.find(playerId);
    if (it != cache.end())
        return it->second;
    return std::nullopt;
}

bool PlayerCache::exists(int playerId) const {
    return cache.find(playerId) != cache.end();
}

void PlayerCache::remove(int playerId) {
    cache.erase(playerId);
}

void PlayerCache::clear() {
    cache.clear();
}
