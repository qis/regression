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
      // Create backup directory.
      const auto skyrim = GetSkyrimPath();
      const auto backup = skyrim / "Backup";

      if (!std::filesystem::exists(backup)) {
        if (!std::filesystem::create_directory(backup)) {
          throw std::runtime_error("Could not create directory: " + backup.string());
        }
      }
      if (!std::filesystem::is_directory(backup)) {
        throw std::runtime_error("Not a directory: " + backup.string());
      }

      // Construct json file path.
      const auto src = skyrim / "regression.json";

      // Construct backup file path.
      const auto ctz = std::chrono::current_zone();
      const auto now = ctz->to_local(std::chrono::system_clock::now());
      const auto day = std::chrono::time_point_cast<std::chrono::days>(now);
      const auto ymd = std::chrono::year_month_day(day);

      const auto tod = now - day;
      const auto h = std::chrono::duration_cast<std::chrono::hours>(tod);
      const auto m = std::chrono::duration_cast<std::chrono::minutes>(tod) - h;
      const auto s = std::chrono::duration_cast<std::chrono::seconds>(tod) - h - m;

      // clang-format off
      const auto dst = backup / std::format(
          "regression-{:04}{:02}{:02}-{:02}{:02}{:02}.json",
          static_cast<int>(ymd.year()),
          static_cast<unsigned>(ymd.month()),
          static_cast<unsigned>(ymd.day()),
          h.count(), m.count(), s.count());
      // clang-format on

      // Read json data.
      boost::json::object info;
      if (std::fstream file{ src, std::ios::in | std::ios::binary }) {
        if (const auto value = boost::json::parse(file); value.is_object()) {
          info = value.as_object();
        }
      }

      // Update json data.
      UpdateSpells(info);
      UpdateSkills(info);
      UpdateDeaths(info);

      // Create json backup.
      if (std::filesystem::exists(src)) {
        if (!std::filesystem::is_regular_file(src)) {
          throw std::runtime_error("Not a regular file: " + src.string());
        }
        if (std::filesystem::exists(dst)) {
          throw std::runtime_error("File already exists: " + dst.string());
        }
        std::error_code ec;
        if (!std::filesystem::copy_file(src, dst, ec) || ec) {
          throw std::runtime_error("Could not create file: " + dst.string());
        }
      }

      // Write json contents.
      std::fstream file{ src, std::ios::out | std::ios::trunc | std::ios::binary };
      if (!file) {
        throw std::runtime_error("Could not open file: " + src.string());
      }
      Write(file, info);
      file.close();
      if (!file) {
        std::error_code ec;
        std::filesystem::copy_file(dst, src, ec);
        throw std::runtime_error("Could not write file: " + src.string());
      }

      Log(" ");
      Log("Updated: {}", src.string());
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

  static void UpdateSpells(boost::json::object& info)
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
      boost::json::array& spells;

      SpellsVisitor(boost::json::array& spells) :
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
          Regression::Log("SPELL {:08X} {}", spell->GetFormID(), name);
          spells.push_back(boost::json::string{ std::format("{:08X}", spell->GetFormID()) });
          break;
        }
        return RE::BSContainer::ForEachResult::kContinue;
      }
    };

    boost::json::array spells;
    SpellsVisitor visitor{ spells };
    actor->VisitSpells(visitor);
    info["Spells"] = spells;
  }

  static void UpdateSkills(boost::json::object& info)
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

    if (!info["Skills"].is_object()) {
      info["Skills"] = boost::json::object{};
    }
    auto& values = info["Skills"].as_object();

    Log(" ");
    for (const auto& [skill, name] : skills) {
      const auto old = values[name].is_double() ? static_cast<float>(values[name].as_double()) : 0.0f;
      const auto per = actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kPermanent, skill);
      const auto cur = std::max(0.0f, avo->GetPermanentActorValue(skill) - per);
      if (cur > old && cur - old > 0.1f) {
        Log("SKILL {:5.1f} {}", cur, name);
      }
      values[name] = static_cast<double>(std::max(old, cur));
    }
  }

  static void UpdateDeaths(boost::json::object& info)
  {
    const auto calendar = RE::Calendar::GetSingleton();
    if (!calendar) {
      throw std::runtime_error("Could not get calendar.");
    }
    const auto days = calendar->GetDaysPassed();

    if (info["Days"].is_double()) {
      info["Days"] = std::max(info["Days"].as_double(), static_cast<double>(days));
    } else {
      info["Days"] = days;
    }

    if (info["Deaths"].is_int64()) {
      info["Deaths"] = info["Deaths"].as_int64() + 1;
    } else {
      info["Deaths"] = 1;
    }

    Log(" ");
    Log("Deaths: {}", info["Deaths"].as_int64());
    Log("Days: {}", info["Days"].as_double());
  }

  static void Write(std::ostream& os, const boost::json::value& value, std::string* indent = nullptr)
  {
    std::unique_ptr<std::string> indent_storage;
    if (!indent) {
      indent_storage = std::make_unique<std::string>();
      indent = indent_storage.get();
    }
    switch (value.kind()) {
    case boost::json::kind::object:
      if (const auto& obj = value.get_object(); !obj.empty()) {
        std::map<std::string, boost::json::value> entries;
        for (const auto& e : obj) {
          entries[e.key()] = e.value();
        }
        os << "{\n";
        indent->append(2, ' ');
        for (auto it = entries.cbegin(); true;) {
          os << *indent << boost::json::serialize(it->first) << ": ";
          Write(os, it->second, indent);
          if (++it == entries.cend()) {
            break;
          }
          os << ",\n";
        }
        indent->resize(indent->size() - 2);
        os << '\n' << *indent << '}';
      } else {
        os << "{}";
      }
      break;
    case boost::json::kind::array:
      if (const auto& arr = value.get_array(); !arr.empty()) {
        os << "[\n";
        indent->append(2, ' ');
        for (auto it = arr.begin(); true;) {
          os << *indent;
          Write(os, *it, indent);
          if (++it == arr.end()) {
            break;
          }
          os << ",\n";
        }
        indent->resize(indent->size() - 2);
        os << '\n' << *indent << ']';
      } else {
        os << "[]";
      }
      break;
    case boost::json::kind::string:
      os << boost::json::serialize(value.get_string());
      break;
    case boost::json::kind::uint64:
    case boost::json::kind::int64:
      os << value;
      break;
    case boost::json::kind::double_:
      std::format_to(std::ostream_iterator<char>(os), "{:.1f}", value.get_double());
      break;
    case boost::json::kind::bool_:
      os << value.get_bool() ? "true" : "false";
      break;
    case boost::json::kind::null:
      os << "null";
      break;
    }
    if (indent->empty()) {
      os << '\n';
    }
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
