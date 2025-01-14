#pragma once
#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>
#include <boost/json/value.hpp>

#include <atomic>
#include <condition_variable>
#include <mutex>

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iterator>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

using namespace std::literals;
