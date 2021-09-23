#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <unordered_map>
#include <cstdio>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <set>

typedef int Cell;
typedef size_t AreaID;
typedef size_t ItemID;
typedef size_t EnemyID;

std::string ReadFile(const std::string& filename);
std::vector<std::string> ReadLines(const std::string& filename);