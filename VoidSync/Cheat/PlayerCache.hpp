#ifndef PLAYER_CACHE_HPP
#define PLAYER_CACHE_HPP

#include <unordered_map>
#include <string>
#include <optional>
#include <iostream>

struct Vector3 {
    float x, y, z;
};

struct Player {
    int id;
    std::string name;
    float health;
    Vector3 position; 
};

class PlayerCache {
private:
    std::unordered_map<int, Player> cache;

public:
    void Update(const Player& player);
    std::optional<Player> get(int playerId) const;
    bool exists(int playerId) const;
    void remove(int playerId);
    void clear();
};

extern PlayerCache GlobalPlayerCache;

#endif
