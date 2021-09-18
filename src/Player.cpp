#include "Player.h"
#include "Game.h"

Player::Player(const std::string& name, const unsigned int area, const Cell cell) : name(name), area(area), cell(cell) {}

Player::~Player() {}