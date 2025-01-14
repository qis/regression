#include <version.h>
#include <windows.h>

class Regression final : public RE::BSTEventSink<RE::InputEvent*>, public RE::BSTEventSink<RE::TESDeathEvent> {
private:
  static inline RE::TESDataHandler* Data{ nullptr };
  static inline RE::PlayerCharacter* Player{ nullptr };
  static inline std::unordered_map<RE::ActorValue, std::string> Skills;
  static inline std::unordered_map<RE::ActorValue, std::string> Stats;
  static inline std::vector<RE::SpellItem*> Powers;

  static inline std::string_view Skyrim{ "Skyrim.esm" };
  static inline std::string_view Dawnguard{ "Dawnguard.esm" };
  static inline std::string_view Dragonborn{ "Dragonborn.esm" };
  static inline std::string_view Requiem{ "Requiem.esp" };

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
    // Get singletons.
    if (!(Data = RE::TESDataHandler::GetSingleton())) {
      Log("Could not get data singleton.");
      return false;
    }
    if (!(Player = RE::PlayerCharacter::GetSingleton())) {
      Log("Could not get player singleton.");
      return false;
    }

    // clang-format off

    // Initialize stats.
    Stats.clear();
    Stats[RE::ActorValue::kHealth]  = "Health";
    Stats[RE::ActorValue::kMagicka] = "Magicka";
    Stats[RE::ActorValue::kStamina] = "Stamina";

    // Initialize skills.
    Skills.clear();
    Skills[RE::ActorValue::kIllusion]    = "Illusion";
    Skills[RE::ActorValue::kConjuration] = "Conjuration";
    Skills[RE::ActorValue::kDestruction] = "Destruction";
    Skills[RE::ActorValue::kRestoration] = "Restoration";
    Skills[RE::ActorValue::kAlteration]  = "Alteration";
    Skills[RE::ActorValue::kEnchanting]  = "Enchanting";
    Skills[RE::ActorValue::kSmithing]    = "Smithing";
    Skills[RE::ActorValue::kHeavyArmor]  = "HeavyArmor";
    Skills[RE::ActorValue::kBlock]       = "Block";
    Skills[RE::ActorValue::kTwoHanded]   = "TwoHanded";
    Skills[RE::ActorValue::kOneHanded]   = "OneHanded";
    Skills[RE::ActorValue::kArchery]     = "Marksman";
    Skills[RE::ActorValue::kLightArmor]  = "LightArmor";
    Skills[RE::ActorValue::kSneak]       = "Sneak";
    Skills[RE::ActorValue::kLockpicking] = "LockPicking";
    Skills[RE::ActorValue::kPickpocket]  = "Pickpocket";
    Skills[RE::ActorValue::kSpeech]      = "SpeechCraft";
    Skills[RE::ActorValue::kAlchemy]     = "Alchemy";

    // Initialize powers.
    Powers.clear();

    // Requiem
    //Powers.push_back(Data->LookupForm(0x00080C, Requiem)->As<RE::SpellItem>());     // Lesser Power: Bound Quiver

    // Black Book: Epistolary Acumen
    Powers.push_back(Data->LookupForm(0x02647B, Dragonborn)->As<RE::SpellItem>());  // Ability: Dragonborn Force
    Powers.push_back(Data->LookupForm(0x02647D, Dragonborn)->As<RE::SpellItem>());  // Ability: Dragonborn Flame
    Powers.push_back(Data->LookupForm(0x02647E, Dragonborn)->As<RE::SpellItem>());  // Ability: Dragonborn Frost

    // Black Book: Filament and Filigree
    Powers.push_back(Data->LookupForm(0x01E7FD, Dragonborn)->As<RE::SpellItem>());  // Greater Power: Secret of Arcana
    Powers.push_back(Data->LookupForm(0x01E800, Dragonborn)->As<RE::SpellItem>());  // Greater Power: Secret of Protection
    Powers.push_back(Data->LookupForm(0x01E7FA, Dragonborn)->As<RE::SpellItem>());  // Greater Power: Secret of Strength

    // Black Book: The Hidden Twilight
    Powers.push_back(Data->LookupForm(0x031842, Dragonborn)->As<RE::SpellItem>());  // Greater Power: Mora's Agony
    Powers.push_back(Data->LookupForm(0x01E7F7, Dragonborn)->As<RE::SpellItem>());  // Greater Power: Mora's Boon
    Powers.push_back(Data->LookupForm(0x031844, Dragonborn)->As<RE::SpellItem>());  // Greater Power: Mora's Grasp

    // Black Book: The Sallow Regent
    Powers.push_back(Data->LookupForm(0x034834, Dragonborn)->As<RE::SpellItem>());  // Ability: Seeker of Might
    Powers.push_back(Data->LookupForm(0x034838, Dragonborn)->As<RE::SpellItem>());  // Ability: Seeker of Shadows
    Powers.push_back(Data->LookupForm(0x034837, Dragonborn)->As<RE::SpellItem>());  // Ability: Seeker of Sorcery

    // Black Book: The Winds of Change
    Powers.push_back(Data->LookupForm(0x01E7F5, Dragonborn)->As<RE::SpellItem>());  // Ability: Companion's Insight
    Powers.push_back(Data->LookupForm(0x01E7F3, Dragonborn)->As<RE::SpellItem>());  // Ability: Lover's Insight
    Powers.push_back(Data->LookupForm(0x01E7EF, Dragonborn)->As<RE::SpellItem>());  // Ability: Scholar's Insight

    // Black Book: Untold Legends
    Powers.push_back(Data->LookupForm(0x029F12, Dragonborn)->As<RE::SpellItem>());  // Lesser Power: Bardic Knowledge
    Powers.push_back(Data->LookupForm(0x01EEC6, Dragonborn)->As<RE::SpellItem>());  // Lesser Power: Black Market
    Powers.push_back(Data->LookupForm(0x01FF21, Dragonborn)->As<RE::SpellItem>());  // Lesser Power: Secret Servant

    if (std::find(Powers.begin(), Powers.end(), nullptr) != Powers.end()) {
      Log("Could not load all powers.");
      return false;
    }

    // clang-format on

    // Bind keyboard events.
    auto input = RE::BSInputDeviceManager::GetSingleton();
    if (!input) {
      Log("Could not get input device manager.");
      return false;
    }
    input->AddEventSink<RE::InputEvent*>(this);

    // Bind death events.
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
  RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* eventPtr, RE::BSTEventSource<RE::InputEvent*>*) override
  {
    if (!eventPtr || !*eventPtr) {
      return RE::BSEventNotifyControl::kContinue;
    }
    const auto event = *eventPtr;
    if (event->GetDevice() != RE::INPUT_DEVICE::kKeyboard) {
      return RE::BSEventNotifyControl::kContinue;
    }
    if (event->GetEventType() != RE::INPUT_EVENT_TYPE::kButton) {
      return RE::BSEventNotifyControl::kContinue;
    }
    const auto button = event->AsButtonEvent();
    if (button->IsRepeating()) {
      return RE::BSEventNotifyControl::kContinue;
    }
    try {
      switch (button->idCode) {
      case RE::BSKeyboardDevice::Keys::kF11:
        OnDeath();
        break;
      case RE::BSKeyboardDevice::Keys::kF12:
        OnRegression();
        break;
      }
    }
    catch (const std::exception& e) {
      Log(e.what());
    }
    catch (...) {
      Log("Unhandled exception.");
    }
    return RE::BSEventNotifyControl::kContinue;
  }

  RE::BSEventNotifyControl ProcessEvent(const RE::TESDeathEvent* event, RE::BSTEventSource<RE::TESDeathEvent>*) override
  {
    if (!event || !event->actorDying || !event->actorDying->IsPlayerRef() || !event->dead) {
      return RE::BSEventNotifyControl::kContinue;
    }
    try {
      OnDeath();
    }
    catch (const std::exception& e) {
      Log(e.what());
    }
    catch (...) {
      Log("Unhandled exception.");
    }
    return RE::BSEventNotifyControl::kContinue;
  }

private:
  void OnRegression()
  {
    // Load json data.
    boost::json::object info;
    const auto skyrim = GetSkyrimPath();
    const auto src = skyrim / "regression.json";
    if (std::fstream file{ src, std::ios::in | std::ios::binary }) {
      if (const auto value = boost::json::parse(file); value.is_object()) {
        info = value.as_object();
      } else {
        throw std::runtime_error{ "Could not load json data: " + src.string() };
      }
    } else {
      throw std::runtime_error{ "Could not load json file: " + src.string() };
    }

    const auto avo = Player->AsActorValueOwner();
    if (!avo) {
      throw std::runtime_error{ "Could not get player actor value owner." };
    }

    // Restore level.
    const auto level = info["Level"].is_int64() ? info["Level"].as_int64() : 0;
    if (level < static_cast<int64_t>(Player->GetLevel())) {
      throw std::runtime_error{ "Current level higher, than regression level." };
    }
    ExecuteCommand(std::format("Player.SetLevel {}", level));

    // Restore perks.
    const auto perks = static_cast<int>(GetPerkPoints() + level - Player->GetLevel());
    SetPerkPoints(perks);

    // Restore spells.
    const auto spells = info["Spells"].as_array();
    for (const auto& e : spells) {
      if (!e.is_string()) {
        continue;
      }
      std::vector<std::string> entry;
      if (boost::split(entry, e.as_string(), boost::is_any_of(":")).size() < 2) {
        continue;
      }
      RE::FormID base = 0;
      if (std::from_chars(entry[1].data(), entry[1].data() + entry[1].size(), base, 16).ec != std::errc{}) {
        continue;
      }
      auto form = Data->LookupForm(base, entry[0]);
      if (!form) {
        form = Data->LookupFormRaw(base, entry[0]);
        if (!form) {
          Log("ERROR Could not get spell file: {:06X} {}", base, entry[0]);
          continue;
        }
      }
      Player->AddSpell(form->As<RE::SpellItem>());
      const auto name = form->GetName();
      Log("SPELL {:08X} {}", form->GetFormID(), name ? name : "");
    }

    // Restore powers.
    const auto powers = info["Powers"].as_array();
    for (const auto& e : powers) {
      if (!e.is_string()) {
        continue;
      }
      for (const auto power : Powers) {
        if (std::string_view{ power->GetName() } == std::string_view{ e.as_string() }) {
          Player->AddSpell(power);
          const auto name = power->GetName();
          Log("POWER {:08X} {}", power->GetFormID(), name ? name : "");
          break;
        }
      }
    }

    // Restore skills.
    const auto skills = info["Skills"].as_object();
    for (const auto& [skill, name] : Skills) {
      if (const auto it = skills.find(name); it != skills.end()) {
        if (!it->value().is_int64()) {
          continue;
        }
        const auto value = static_cast<float>(it->value().as_int64());
        avo->SetBaseActorValue(skill, value);
        Log("SKILL {:3} {}", value, name);
      }
    }

    // Restore stats.
    if (info["Stats"].is_object()) {
      const auto& stats = info["Stats"].as_object();
      for (const auto& [stat, name] : Stats) {
        if (const auto it = stats.find(name); it != stats.end() && it->value().is_int64()) {
          const auto value = static_cast<float>(it->value().as_int64());
          avo->SetBaseActorValue(stat, value);
          Log("STATS {:3} {}", value, name);
        }
      }
    }

    // Report level and perks.
    Log("LEVEL {:3}", level);
    if (perks > 0) {
      Log("PERKS {:3}", perks);
    }

    // Report deaths and days.
    auto message = std::format("{} Deaths in ", info["Deaths"].is_int64() ? info["Deaths"].as_int64() : 0);
    if (const auto days = info["Days"].is_double() ? info["Days"].as_double() : 0.0; days >= 360.0) {
      std::format_to(std::back_inserter(message), "{:.1f} Years", days / 360.0);
    } else if (days >= 30.0) {
      std::format_to(std::back_inserter(message), "{:.1f} Months", days / 30.0);
    } else if (days >= 7.0) {
      std::format_to(std::back_inserter(message), "{:.1f} Weeks", days / 7.0);
    } else {
      std::format_to(std::back_inserter(message), "{:.1f} Days", days);
    }
    RE::DebugNotification(message.data());
    Log(message);
  }

  void OnDeath()
  {
    // Create backup directory.
    const auto skyrim = GetSkyrimPath();
    const auto backup = skyrim / "Backup";

    if (!std::filesystem::exists(backup)) {
      if (!std::filesystem::create_directory(backup)) {
        throw std::runtime_error{ "Could not create directory: " + backup.string() };
      }
    }
    if (!std::filesystem::is_directory(backup)) {
      throw std::runtime_error{ "Not a directory: " + backup.string() };
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
    Log(" ");
    UpdateSpells(info);
    UpdatePowers(info);
    UpdateValues(info);
    UpdateDeaths(info);

    // Create json backup.
    if (std::filesystem::exists(src)) {
      if (!std::filesystem::is_regular_file(src)) {
        throw std::runtime_error{ "Not a regular file: " + src.string() };
      }
      if (std::filesystem::exists(dst)) {
        throw std::runtime_error{ "File already exists: " + dst.string() };
      }
      std::error_code ec;
      if (!std::filesystem::copy_file(src, dst, ec) || ec) {
        throw std::runtime_error{ "Could not create file: " + dst.string() };
      }
    }

    // Write json contents.
    std::fstream file{ src, std::ios::out | std::ios::trunc | std::ios::binary };
    if (!file) {
      throw std::runtime_error{ "Could not open file: " + src.string() };
    }
    Write(file, info);
    file.close();
    if (!file) {
      std::error_code ec;
      std::filesystem::copy_file(dst, src, ec);
      throw std::runtime_error{ "Could not write file: " + src.string() };
    }
    // Log("Updated: {}", src.string());

    auto message = std::format("Regression!\n{} Deaths in ", info["Deaths"].as_int64());
    if (const auto days = info["Days"].as_double(); days >= 360.0) {
      std::format_to(std::back_inserter(message), "{:.1f} Years", days / 360.0);
    } else if (days >= 30.0) {
      std::format_to(std::back_inserter(message), "{:.1f} Months", days / 30.0);
    } else if (days >= 7.0) {
      std::format_to(std::back_inserter(message), "{:.1f} Weeks", days / 7.0);
    } else {
      std::format_to(std::back_inserter(message), "{:.1f} Days", days);
    }
    RE::DebugMessageBox(message.data());
  }

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
        case RE::ActorValue::kRestoration: {
          const auto id = spell->GetRawFormID();
          const auto base = id & 0x00FFFFFF;
          const auto file = spell->GetFile(0);
          if (!file) {
            Log("SPELL Could not get file: {:08X} \"{}\"", id, name);
            return RE::BSContainer::ForEachResult::kContinue;
          }
          auto form = Data->LookupForm(base, file->GetFilename());
          if (!form) {
            form = Data->LookupFormRaw(base, file->GetFilename());
            if (!form) {
              Log("SPELL Could not get form: {:08X} {} \"{}\"", id, file->GetFilename(), name);
              return RE::BSContainer::ForEachResult::kContinue;
            }
          }
          spells.emplace_back(std::format("{}:{:06X}:{}", file->GetFilename(), base, name));
          Log("SPELL {}", name);
        } break;
        }
        return RE::BSContainer::ForEachResult::kContinue;
      }
    };

    boost::json::array spells;
    SpellsVisitor visitor{ spells };
    Player->VisitSpells(visitor);
    const auto empty = spells.empty();
    info["Spells"] = std::move(spells);
    if (!empty) {
      Log(" ");
    }
  }

  static void UpdatePowers(boost::json::object& info)
  {
    boost::json::array powers;
    for (const auto power : Powers) {
      if (Player->HasSpell(power)) {
        powers.emplace_back(power->GetName());
        Log("POWER {}", power->GetName());
      }
    }
    const auto empty = powers.empty();
    info["Powers"] = std::move(powers);
    if (!empty) {
      Log(" ");
    }
  }

  static void UpdateValues(boost::json::object& info)
  {
    // Get player skill values.
    const auto avo = Player->AsActorValueOwner();
    if (!avo) {
      throw std::runtime_error{ "Could not get player actor value owner." };
    }

    // Update perks.
    // class PerkEntryVisitor : public RE::PerkEntryVisitor {
    // public:
    //   RE::BSContainer::ForEachResult Visit(RE::BGSPerkEntry* entry) override
    //   {
    //     RE::BGSPerk* perk = nullptr;
    //     uint32_t rank = entry->GetRank();
    //
    //
    //     return RE::BSContainer::ForEachResult::kContinue;
    //   }
    // } visitor;
    // Player->ForEachPerk(visitor);

    // Update perks.
    // const auto base = Player->GetActorBase();
    // if (!base) {
    //   throw std::runtime_error{ "Could not get player actor base." };
    // }
    // for (uint32_t i = 0; i < base->perkCount; i++) {
    //   const RE::BGSPerk* perk = base->perks[i].perk;
    //   if (!perk) {
    //     throw std::runtime_error{ "Missing perk entry." };
    //   }
    //   if (perk->data.hidden) {
    //     continue;
    //   }
    //   const auto rank = base->perks[i].currentRank;
    //   if (rank < 0) {
    //     throw std::runtime_error{ "Negative perk rank." };
    //   }
    //   auto s = std::format("PERKS {:08X} {}", perk->GetFormID(), static_cast<int>(rank));
    //   s.append(perk->data.playable ? " PLAYABLE" : "         ");
    //   s.append(perk->data.trait ? " TRAIT" : "      ");
    //   Log(s);
    // }

    // Update skills.
    if (!info["Skills"].is_object()) {
      info["Skills"] = boost::json::object{};
    }
    auto& skills = info["Skills"].as_object();
    for (const auto& [skill, name] : Skills) {
      const auto old = skills[name].is_int64() ? skills[name].as_int64() : 0;
      const auto per = Player->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kPermanent, skill);
      const auto cur = static_cast<int64_t>(std::max(0.0f, avo->GetPermanentActorValue(skill) - per));
      if (old != cur) {
        Log("SKILL {:11} {:3} -> {}", name, old, cur);
      } else {
        Log("SKILL {:11} {:3}", name, cur);
      }
      skills[name] = cur;
    }

    // Update stats.
    if (!info["Stats"].is_object()) {
      info["Stats"] = boost::json::object{};
    }
    auto& stats = info["Stats"].as_object();
    for (const auto& [stat, name] : Stats) {
      const auto old = stats[name].is_int64() ? stats[name].as_int64() : 0;
      const auto per = Player->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kPermanent, stat);
      const auto cur = static_cast<int64_t>(std::max(0.0f, avo->GetPermanentActorValue(stat) - per));
      if (old != cur) {
        Log("STATS {:11} {:3} -> {}", name, old, cur);
      } else {
        Log("STATS {:11} {:3}", name, cur);
      }
      stats[name] = cur;
    }

    // Update level.
    const auto old_level = info["Level"].is_int64() ? info["Level"].as_int64() : 0;
    const auto cur_level = static_cast<int64_t>(Player->GetLevel());
    if (old_level != cur_level) {
      Log("LEVEL {} -> {}", old_level, cur_level);
    } else {
      Log("LEVEL {}", cur_level);
    }
    info["Level"] = cur_level;
  }

  static void UpdateDeaths(boost::json::object& info)
  {
    // Add days.
    const auto calendar = RE::Calendar::GetSingleton();
    if (!calendar) {
      throw std::runtime_error{ "Could not get calendar." };
    }
    double days = calendar->GetDaysPassed();
    if (info["Days"].is_double()) {
      days += info["Days"].as_double();
    }
    info["Days"] = days;

    // Increment deaths.
    int64_t deaths = 1;
    if (info["Deaths"].is_int64()) {
      deaths += info["Deaths"].as_int64();
    }
    info["Deaths"] = deaths;

    Log("DEATH {} in {:.1f} days", deaths, days);
  }

  static int GetPerkPoints()
  {
    return Player->GetGameStatsData().perkCount;
  }

  static void SetPerkPoints(int perks)
  {
    const auto vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
    if (!vm) {
      throw std::runtime_error{ "Could not get virtual machine." };
    }
    const auto policy = vm->GetObjectHandlePolicy();
    if (!policy) {
      throw std::runtime_error{ "Could not get object handle policy." };
    }
    RE::BSFixedString name = "Game";
    RE::BSFixedString function = "SetPerkPoints";
    RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> result;
    auto args = RE::MakeFunctionArguments(int{ perks });
    if (!vm->DispatchStaticCall(name, function, args, result)) {
      throw std::runtime_error{ std::format("Could not execute function: Game.SetPerkPoints({})", perks) };
    }
  }

  static void ExecuteCommand(const std::string& command)
  {
    const auto vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
    if (!vm) {
      throw std::runtime_error{ "Could not get virtual machine." };
    }
    const auto policy = vm->GetObjectHandlePolicy();
    if (!policy) {
      throw std::runtime_error{ "Could not get object handle policy." };
    }
    RE::BSFixedString name = "ConsoleUtil";
    RE::BSFixedString function = "ExecuteCommand";
    RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> result;
    auto args = RE::MakeFunctionArguments(std::string{ command });
    if (!vm->DispatchStaticCall(name, function, args, result)) {
      throw std::runtime_error{ "Could not execute command: " + command };
    }
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
      std::format_to(std::ostream_iterator<char>(os), "{:.1f}", std::floor(value.get_double() * 10.0) / 10.0);
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
