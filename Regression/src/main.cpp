#include <version.h>
#include <windows.h>

class Regression final : public RE::BSTEventSink<RE::TESDeathEvent> {
private:
  Regression() noexcept = default;
  Regression(Regression&& other) = delete;
  Regression(const Regression& other) = delete;
  Regression& operator=(Regression&& other) = delete;
  Regression& operator=(const Regression& other) = delete;

public:
  static void Listener(SKSE::MessagingInterface::Message* message) noexcept
  {
    switch (message->type) {
    case SKSE::MessagingInterface::kDataLoaded:
      if (auto manager = GetSingleton(); manager->Initialize()) {
        constexpr int major = PROJECT_VERSION_MAJOR;
        constexpr int minor = PROJECT_VERSION_MINOR;
        constexpr int patch = PROJECT_VERSION_PATCH;
        Log("Regression {}.{}.{} loaded.", major, minor, patch);
      }
      break;
    }
  }

  bool Initialize() noexcept
  {
    auto sesh = RE::ScriptEventSourceHolder::GetSingleton();
    if (!sesh) {
      Log("Could not get script event source holder.");
      return false;
    }
    sesh->AddEventSink<RE::TESDeathEvent>(this);
    return true;
  }

  static Regression* GetSingleton() noexcept
  {
    static Regression regression;
    return &regression;
  }

protected:
  RE::BSEventNotifyControl ProcessEvent(const RE::TESDeathEvent* event, RE::BSTEventSource<RE::TESDeathEvent>*) override
  {
    if (!event || !event->actorDying || !event->actorDying->IsPlayerRef() || !event->dead) {
      return RE::BSEventNotifyControl::kContinue;
    }
    try {
      const auto skyrim = GetSkyrimPath();
      const auto backup = skyrim / "Backup";
      const auto plugins = skyrim / "Data" / "SKSE" / "Plugins";

      if (!std::filesystem::exists(backup)) {
        if (!std::filesystem::create_directory(backup)) {
          throw std::runtime_error("Could not create directory: " + backup.string());
        }
      }
      if (!std::filesystem::is_directory(backup)) {
        throw std::runtime_error("Not a directory: " + backup.string());
      }

      const auto ctz = std::chrono::current_zone();
      const auto now = ctz->to_local(std::chrono::system_clock::now());
      const auto day = std::chrono::time_point_cast<std::chrono::days>(now);
      const auto ymd = std::chrono::year_month_day(day);

      const auto tod = now - day;
      const auto h = std::chrono::duration_cast<std::chrono::hours>(tod);
      const auto m = std::chrono::duration_cast<std::chrono::minutes>(tod) - h;
      const auto s = std::chrono::duration_cast<std::chrono::seconds>(tod) - h - m;

      const auto filename = std::format(
        "regression-{:04}{:02}{:02}-{:02}{:02}{:02}",
        static_cast<int>(ymd.year()),
        static_cast<unsigned>(ymd.month()),
        static_cast<unsigned>(ymd.day()),
        h.count(),
        m.count(),
        s.count());

      UpdateDeaths(backup, filename, skyrim / "regression.json");
      UpdateSpells(backup, filename, skyrim / "regression.txt");
      UpdateSkills(backup, filename, plugins / "SkyrimUncapper.ini");
      RE::DebugMessageBox("Regression!");
    }
    catch (const std::exception& e) {
      Log(e.what());
      return RE::BSEventNotifyControl::kContinue;
    }
    catch (...) {
      Log("Unhandled exception.");
    }

    return RE::BSEventNotifyControl::kContinue;
  }

private:
  using Skills = std::unordered_map<RE::ActorValue, std::string>;
  using Values = std::unordered_map<RE::ActorValue, unsigned>;
  using Spells = std::map<RE::ActorValue, std::map<RE::FormID, std::string>>;

  static std::filesystem::path GetSkyrimPath()
  {
    DWORD size = 0;
    std::wstring str;
    do {
      str.resize(str.size() + MAX_PATH);
      size = GetModuleFileName(nullptr, &str[0], static_cast<DWORD>(str.size()));
    } while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);
    str.resize(size);
    return std::filesystem::canonical(str).parent_path();
  }

  static void UpdateDeaths(
    const std::filesystem::path& backup,
    const std::string& filename,
    const std::filesystem::path& src)
  {
    const auto calendar = RE::Calendar::GetSingleton();
    if (!calendar) {
      throw std::runtime_error("Could not get calendar.");
    }
    const auto days = calendar->GetDaysPassed();

    boost::json::object info;
    if (std::fstream file{ src, std::ios::in | std::ios::binary }) {
      if (const auto value = boost::json::parse(file); value.is_object()) {
        info = value.as_object();
      }
    }

    if (!info.contains("deaths") || !info["deaths"].is_number()) {
      info["deaths"] = 1;
    } else {
      info["deaths"] = info["deaths"].as_int64() + 1;
    }

    if (!info.contains("days") || !info["days"].is_number()) {
      info["days"] = days;
    } else {
      info["days"] = info["days"].as_double() + days;
    }

    Log(" ");
    Log("Deaths: {}", info["deaths"].as_int64());
    Log("Days: {}", info["days"].as_double());

    // Create json file backup.
    if (std::filesystem::exists(src)) {
      if (!std::filesystem::is_regular_file(src)) {
        throw std::runtime_error("Not a regular file: " + src.string());
      }
      const auto dst = backup / (filename + ".json");
      if (std::filesystem::exists(dst)) {
        throw std::runtime_error("File already exists: " + dst.string());
      }
      std::error_code ec;
      if (!std::filesystem::copy_file(src, dst, ec) || ec) {
        throw std::runtime_error("Could not create file: " + dst.string());
      }
    }

    // Write json file contents.
    std::fstream file{ src, std::ios::out | std::ios::trunc | std::ios::binary };
    if (!file) {
      throw std::runtime_error("Could not open file: " + src.string());
    }

    file << boost::json::serialize(info);

    file.close();
    if (!file) {
      throw std::runtime_error("Could not write file: " + src.string());
    }

    Log(" ");
    Log("Updated: {}", src.string());
  }

  static void UpdateSpells(
    const std::filesystem::path& backup,
    const std::string& filename,
    const std::filesystem::path& src)
  {
    // Get player spells.
    const auto actor = RE::PlayerCharacter::GetSingleton();
    if (!actor) {
      throw std::runtime_error("Could not get player actor.");
    }

    const auto base = actor->GetActorBase();
    if (!base) {
      throw std::runtime_error("Could not get player actor base.");
    }

    class SpellsVisitor : public RE::Actor::ForEachSpellVisitor {
    public:
      Spells& spells;

      SpellsVisitor(Spells& spells) :
        spells(spells)
      {}

      RE::BSContainer::ForEachResult Visit(RE::SpellItem* spell) override
      {
        if (spell->GetSpellType() != RE::MagicSystem::SpellType::kSpell) {
          return RE::BSContainer::ForEachResult::kContinue;
        }
        const auto name = spell->GetName();
        if (!name || std::string_view{ name }.empty()) {
          return RE::BSContainer::ForEachResult::kContinue;
        }
        switch (const auto skill = spell->GetAssociatedSkill()) {
        case RE::ActorValue::kAlteration:
        case RE::ActorValue::kConjuration:
        case RE::ActorValue::kDestruction:
        case RE::ActorValue::kIllusion:
        case RE::ActorValue::kRestoration:
          spells[skill].emplace(spell->GetFormID(), name);
          break;
        }
        return RE::BSContainer::ForEachResult::kContinue;
      }
    };

    Spells spells;
    SpellsVisitor visitor{ spells };
    actor->VisitSpells(visitor);

    // Create txt file backup.
    if (std::filesystem::exists(src)) {
      if (!std::filesystem::is_regular_file(src)) {
        throw std::runtime_error("Not a regular file: " + src.string());
      }
      const auto dst = backup / (filename + ".txt");
      if (std::filesystem::exists(dst)) {
        throw std::runtime_error("File already exists: " + dst.string());
      }
      std::error_code ec;
      if (!std::filesystem::copy_file(src, dst, ec) || ec) {
        throw std::runtime_error("Could not create file: " + dst.string());
      }
    }

    // Write txt file contents.
    std::fstream file{ src, std::ios::out | std::ios::trunc | std::ios::binary };
    if (!file) {
      throw std::runtime_error("Could not open file: " + src.string());
    }
    auto os = std::ostream_iterator<char>(file);

    auto first = true;
    for (const auto& [skill, list] : spells) {
      Log(" ");
      if (first) {
        first = false;
      } else {
        file.put('\n');
      }
      Log(std::to_string(skill));
      std::format_to(os, "; {}\n", std::to_string(skill));
      for (const auto& [id, name] : list) {
        Log("SPELL {:08X} {}", id, name);
        std::format_to(os, "player.addspell {:08X}  ; {}\n", id, name);
      }
    }

    file.close();
    if (!file) {
      throw std::runtime_error("Could not write file: " + src.string());
    }

    Log(" ");
    Log("Updated: {}", src.string());
  }

  static void UpdateSkills(
    const std::filesystem::path& backup,
    const std::string& filename,
    const std::filesystem::path& src)
  {
    // Get player skill values.
    const auto actor = RE::PlayerCharacter::GetSingleton();
    if (!actor) {
      throw std::runtime_error("Could not get player actor.");
    }
    const auto avo = actor->AsActorValueOwner();
    if (!avo) {
      throw std::runtime_error("Could not get player actor value owner.");
    }

    Skills skills;

    // clang-format off
    skills[RE::ActorValue::kIllusion]    = "Illusion";
    skills[RE::ActorValue::kConjuration] = "Conjuration";
    skills[RE::ActorValue::kDestruction] = "Destruction";
    skills[RE::ActorValue::kRestoration] = "Restoration";
    skills[RE::ActorValue::kAlteration]  = "Alteration";
    skills[RE::ActorValue::kEnchanting]  = "Enchanting";
    skills[RE::ActorValue::kSmithing]    = "Smithing";
    skills[RE::ActorValue::kHeavyArmor]  = "HeavyArmor";
    skills[RE::ActorValue::kBlock]       = "Block";
    skills[RE::ActorValue::kTwoHanded]   = "TwoHanded";
    skills[RE::ActorValue::kOneHanded]   = "OneHanded";
    skills[RE::ActorValue::kArchery]     = "Marksman";
    skills[RE::ActorValue::kLightArmor]  = "LightArmor";
    skills[RE::ActorValue::kSneak]       = "Sneak";
    skills[RE::ActorValue::kLockpicking] = "LockPicking";
    skills[RE::ActorValue::kPickpocket]  = "Pickpocket";
    skills[RE::ActorValue::kSpeech]      = "SpeechCraft";
    skills[RE::ActorValue::kAlchemy]     = "Alchemy";
    // clang-format on

    Values values;

    for (const auto& [skill, name] : skills) {
      const auto max = avo->GetPermanentActorValue(skill);
      const auto per = actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kPermanent, skill);
      values[skill] = static_cast<unsigned>(std::round(std::max(0.0f, max - per)));
    }

    // Read ini file.
    if (!std::filesystem::is_regular_file(src)) {
      throw std::runtime_error("Not a regular file: " + src.string());
    }

    std::fstream file{ src, std::ios::in | std::ios::ate | std::ios::binary };
    if (!file) {
      throw std::runtime_error("Could not open file: " + src.string());
    }
    std::string data;
    const auto size = static_cast<unsigned>(file.tellg());
    file.seekg(std::ios::beg);
    data.resize(size);
    file.read(data.data(), size);
    file.close();
    if (!file) {
      throw std::runtime_error("Could not read file: " + src.string());
    }

    // Parse ini file.
    std::vector<std::string> lines;
    boost::algorithm::split(lines, data, [](char c) { return c == '\n'; });
    for (auto& line : lines) {
      if (const auto pos = line.find_last_not_of('\r'); pos != std::string::npos) {
        line.resize(pos + 1);
      } else {
        line.clear();
      }
    }
    const auto eexp = std::string{ R"INI(\s*(\d+)\s*=\s*(\d+).*)INI" };
    const auto ereg = boost::regex{
      eexp, boost::regex_constants::ECMAScript | boost::regex_constants::icase | boost::regex_constants::optimize
    };
    for (const auto& [skill, name] : skills) {
      // Find skill section.
      auto found = false;
      const auto current = values[skill];
      const auto sexp = std::format(R"INI(\s*\[SkillExpGainMults\\BaseSkillLevel\\{}\])INI", name);
      const auto sreg = boost::regex{ sexp, boost::regex_constants::ECMAScript | boost::regex_constants::icase };
      for (std::size_t i = 0; i < lines.size(); i++) {
        if (boost::regex_match(lines[i], sreg)) {
          // Find skill section "level = value" entries.
          const auto replace = i + 1;
          std::map<unsigned, unsigned> entries;
          for (boost::smatch match; boost::regex_match(lines[i + 1], match, ereg); i++) {
            auto level = 0u;
            const auto match1 = match[1].str();
            if (std::from_chars(match1.data(), match1.data() + match1.size(), level).ec != std::errc{}) {
              throw std::runtime_error("Could not parse " + name + " level: " + match1);
            }
            auto value = 0u;
            const auto match2 = match[2].str();
            if (std::from_chars(match2.data(), match2.data() + match2.size(), value).ec != std::errc{}) {
              throw std::runtime_error("Could not parse " + name + " value: " + match2);
            }
            entries[level] = value;
          }

          // The list must not be empty.
          if (entries.empty()) {
            throw std::runtime_error("Missing entries in " + name + ".");
          }

          // The first entry must be for level 0.
          if (entries.begin()->first != 0) {
            throw std::runtime_error("Missing level 0 entry in " + name + ".");
          }

          // The last entry must have a factor of 1.
          auto it = entries.rbegin();
          const auto end = entries.rend();
          if (it->second != 1u) {
            throw std::runtime_error("The largest factor in " + name + " is not 1.");
          }

          // Lower levels must have higher factor.
          for (auto c = it, n = std::next(it); n != end; ++c, ++n) {
            if (n->second <= c->second) {
              throw std::runtime_error("Invalid factor order in " + name + ".");
            }
          }

          // Skip all entries with a higher level.
          auto factor = 0u;
          for (; it != end && it->first > current; ++it) {
            factor = it->second;
          }

          // Sanity check - should always be false.
          if (it == end) {
            throw std::runtime_error(std::format("All levels in {} are higher, than {}.", name, current));
          }

          // Create a new entry if the level is missing.
          std::optional<unsigned> create;
          if (it->first != current) {
            create = factor + 1;
            it->second += 1;
          }

          // Increase factors of all entries with a lower level by 1.
          for (++it; it != end; ++it) {
            it->second += 1;
          }

          // Create a new entry if necessary.
          if (create) {
            entries[current] = create.value();
          }

          // Replace entries.
          auto insert = lines.erase(lines.begin() + replace, lines.begin() + i + 1);
          for (const auto [level, value] : entries) {
            insert = std::next(lines.insert(insert, std::format("{} = {}.0", level, value)));
          }

          Log(" ");
          for (auto line = std::prev(lines.begin() + replace); line != insert; ++line) {
            Log(*line);
          }

          found = true;
          break;
        }
      }
      if (!found) {
        throw std::runtime_error("Could not find " + name + " in " + src.string());
      }
    }

    // Create ini file backup.
    const auto dst = backup / (filename + ".ini");
    if (std::filesystem::exists(dst)) {
      throw std::runtime_error("File already exists: " + dst.string());
    }

    std::error_code ec;
    if (!std::filesystem::copy_file(src, dst, ec) || ec) {
      throw std::runtime_error("Could not create file: " + dst.string());
    }

    // Write ini file contents.
    file.open(src, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!file) {
      throw std::runtime_error("Could not reopen file: " + src.string());
    }

    for (const auto& line : lines) {
      file.write(line.data(), static_cast<std::streamsize>(line.size()));
      file.write("\r\n", 2);
    }
    file.close();

    if (!file) {
      throw std::runtime_error("Could not write file: " + src.string());
    }

    Log(" ");
    Log("Updated: {}", src.string());
  }

  static void Log(const std::string& msg)
  {
    if (const auto log = RE::ConsoleLog::GetSingleton()) {
      log->Print("%s", msg.data());
    }
  }

  template <class Arg, class... Args>
  static void Log(std::format_string<Arg, Args...> fmt, Arg&& arg, Args&&... args)
  {
    Log(std::vformat(fmt.get(), std::make_format_args(arg, args...)));
  }
};

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
  SKSE::Init(skse);
  if (!SKSE::GetMessagingInterface()->RegisterListener(Regression::Listener)) {
    return false;
  }
  return true;
}
