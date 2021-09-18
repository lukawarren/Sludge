#include "Player.h"
#include "Game.h"

Player::Player(const std::string& name, const unsigned int area, const Cell cell) : name(name), level(0), area(area), cell(cell) {}

Player::~Player() {}