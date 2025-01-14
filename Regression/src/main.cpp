#include <version.h>
#include <windows.h>

class Regression final : public RE::BSTEventSink<RE::InputEvent*>, public RE::BSTEventSink<RE::TESDeathEvent> {
private:
  static inline RE::TESDataHandler* Data{ nullptr };
  static inline RE::PlayerCharacter* Player{ nullptr };
  static inline std::unordered_map<RE::ActorValue, std::string> Stats;
  static inline std::unordered_map<RE::ActorValue, std::string> Skills;
  static inline std::vector<std::pair<RE::BGSPerk*, std::string>> Perks;
  static inline std::vector<RE::BGSPerk*> PerksExtra;
  static inline std::vector<RE::SpellItem*> Powers;

  static inline std::string_view Skyrim{ "Skyrim.esm" };
  static inline std::string_view Dawnguard{ "Dawnguard.esm" };
  static inline std::string_view Dragonborn{ "Dragonborn.esm" };
  static inline std::string_view Requiem{ "Requiem.esp" };
  static inline std::string_view Alchemy{ "Requiem - Classic Alchemy Overhaul Plotinuz Update.esp" };

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
    case SKSE::MessagingInterface::kPostLoadGame:
      if (auto manager = GetSingleton()) {
        if (message->dataLen > 0 && static_cast<char>(reinterpret_cast<uintptr_t>(message->data))) {
          manager->OnPostLoadGame();
        }
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

    // Initialize perks.
    Perks.clear();
    try {
      LoadPerk(0x0F2CA9, Skyrim,  "Illusion: Novice Illusion");
      LoadPerk(0x0C44C3, Skyrim,  "Illusion: Apprentice Illusion");
      LoadPerk(0x0C44C4, Skyrim,  "Illusion: Adept Illusion");
      LoadPerk(0x0C44C5, Skyrim,  "Illusion: Expert Illusion");
      LoadPerk(0x0C44C6, Skyrim,  "Illusion: Master Illusion");
      LoadPerk(0x0153D0, Skyrim,  "Illusion: Acoustic Manipulation");
      LoadPerk(0x059B78, Skyrim,  "Illusion: Visual Manipulation");
      LoadPerk(0x22FDA7, Requiem, "Illusion: Environmental Manipulation");
      LoadPerk(0x0C44B5, Skyrim,  "Illusion: Shadow Shaping");
      LoadPerk(0x5D0BDC, Requiem, "Illusion: Phantasmagoria");
      LoadPerk(0x0581E1, Skyrim,  "Illusion: Delusive Phantasms");
      LoadPerk(0x0581E2, Skyrim,  "Illusion: Otherworldly Phantasms");
      LoadPerk(0x059B77, Skyrim,  "Illusion: Pain and Agony");
      LoadPerk(0x0581FD, Skyrim,  "Illusion: Obliterate the Mind");
      LoadPerk(0x059B76, Skyrim,  "Illusion: Domination");

      LoadPerk(0x0F2CA7, Skyrim,  "Conjuration: Novice Conjuration");
      LoadPerk(0x0C44BB, Skyrim,  "Conjuration: Apprentice Conjuration");
      LoadPerk(0x0C44BC, Skyrim,  "Conjuration: Adept Conjuration");
      LoadPerk(0x0C44BD, Skyrim,  "Conjuration: Expert Conjuration");
      LoadPerk(0x0C44BE, Skyrim,  "Conjuration: Master Conjuration");
      LoadPerk(0x105F30, Skyrim,  "Conjuration: Stabilized Binding");
      LoadPerk(0xAD385A, Requiem, "Conjuration: Spiritual Binding");
      LoadPerk(0x0CB419, Skyrim,  "Conjuration: Extended Binding");
      LoadPerk(0x0CB41A, Skyrim,  "Conjuration: Elemental Binding");
      LoadPerk(0x0153CE, Skyrim,  "Conjuration: Summoner's Insight");
      LoadPerk(0x185736, Requiem, "Conjuration: Cognitive Flexibility (1/2)");
      LoadPerk(0x185737, Requiem, "Conjuration: Cognitive Flexibility (2/2)");
      LoadPerk(0x0581DD, Skyrim,  "Conjuration: Necromancy");
      LoadPerk(0x17911B, Requiem, "Conjuration: Ritualism");
      LoadPerk(0x0581DE, Skyrim,  "Conjuration: Dark Infusion");
      LoadPerk(0x0640B3, Skyrim,  "Conjuration: Mystic Binding");
      LoadPerk(0x0D799E, Skyrim,  "Conjuration: Mystic Maelstrom");
      LoadPerk(0x0D799C, Skyrim,  "Conjuration: Mystic Banishment");
      LoadPerk(0x17911A, Requiem, "Conjuration: Mystic Disruption");

      LoadPerk(0x0F2CA8, Skyrim,  "Destruction: Novice Destruction");
      LoadPerk(0x0C44BF, Skyrim,  "Destruction: Apprentice Destruction");
      LoadPerk(0x0C44C0, Skyrim,  "Destruction: Adept Destruction");
      LoadPerk(0x0C44C1, Skyrim,  "Destruction: Expert Destruction");
      LoadPerk(0x0C44C2, Skyrim,  "Destruction: Master Destruction");
      LoadPerk(0x0581E7, Skyrim,  "Destruction: Pyromancy (1/2)");
      LoadPerk(0x10FCF8, Skyrim,  "Destruction: Pyromancy (2/2)");
      LoadPerk(0x0F392E, Skyrim,  "Destruction: Cremation");
      LoadPerk(0x179121, Requiem, "Destruction: Fire Mastery");
      LoadPerk(0x0581EA, Skyrim,  "Destruction: Cryomancy (1/2)");
      LoadPerk(0x10FCF9, Skyrim,  "Destruction: Cryomancy (2/2)");
      LoadPerk(0x0F3933, Skyrim,  "Destruction: Deep Freeze");
      LoadPerk(0x179123, Requiem, "Destruction: Frost Mastery");
      LoadPerk(0x058200, Skyrim,  "Destruction: Electromancy (1/2)");
      LoadPerk(0x10FCFA, Skyrim,  "Destruction: Electromancy (2/2)");
      LoadPerk(0x0F3F0E, Skyrim,  "Destruction: Electrostatic Discharge");
      LoadPerk(0x179124, Requiem, "Destruction: Lightning Mastery");
      LoadPerk(0x105F32, Skyrim,  "Destruction: Rune Mastery");
      LoadPerk(0x0153CF, Skyrim,  "Destruction: Empowered Elements");
      LoadPerk(0x0153D2, Skyrim,  "Destruction: Impact");

      LoadPerk(0x0F2CAA, Skyrim,  "Restoration: Novice Restoration");
      LoadPerk(0x0C44C7, Skyrim,  "Restoration: Apprentice Restoration");
      LoadPerk(0x0C44C8, Skyrim,  "Restoration: Adept Restoration");
      LoadPerk(0x0C44C9, Skyrim,  "Restoration: Expert Restoration");
      LoadPerk(0x0C44CA, Skyrim,  "Restoration: Master Restoration");
      LoadPerk(0x0581F8, Skyrim,  "Restoration: Improved Healing");
      LoadPerk(0x0581F9, Skyrim,  "Restoration: Respite");
      LoadPerk(0x0581E4, Skyrim,  "Restoration: Mysticism");
      LoadPerk(0x068BCC, Skyrim,  "Restoration: Iimproved Wards");
      LoadPerk(0x0581F4, Skyrim,  "Restoration: Focused Mind");
      LoadPerk(0x0A3F64, Skyrim,  "Restoration: Power of Life");
      LoadPerk(0x17E062, Requiem, "Restoration: Essence of Life");
      LoadPerk(0x0153D1, Skyrim,  "Restoration: Benefactor's Insight");

      LoadPerk(0x0F2CA6, Skyrim,  "Alteration: Novice Alteration");
      LoadPerk(0x0C44B7, Skyrim,  "Alteration: Apprentice Alteration");
      LoadPerk(0x0C44B8, Skyrim,  "Alteration: Adept Alteration");
      LoadPerk(0x0C44B9, Skyrim,  "Alteration: Expert Alteration");
      LoadPerk(0x0C44BA, Skyrim,  "Alteration: Master Alteration");
      LoadPerk(0x0153CD, Skyrim,  "Alteration: Empowered Alterations");
      LoadPerk(0x0D7999, Skyrim,  "Alteration: Improved Mage Armor");
      LoadPerk(0x0581FC, Skyrim,  "Alteration: Stability");
      LoadPerk(0x21792B, Requiem, "Alteration: Metamagical Thesis");
      LoadPerk(0x21792C, Requiem, "Alteration: Metamagical Empowerment");
      LoadPerk(0x0581F7, Skyrim,  "Alteration: Magical Absorption");
      LoadPerk(0x21792A, Requiem, "Alteration: Spell Armor");
      LoadPerk(0x053128, Skyrim,  "Alteration: Magic Resistamce (1/3)");
      LoadPerk(0x053129, Skyrim,  "Alteration: Magic Resistamce (2/3)");
      LoadPerk(0x05312A, Skyrim,  "Alteration: Magic Resistamce (3/3)");

      LoadPerk(0x0BEE97, Skyrim,  "Enchanting: Enchanter's Insight (1/2)");
      LoadPerk(0x0C367C, Skyrim,  "Enchanting: Enchanter's Insight (2/2)");
      LoadPerk(0x058F80, Skyrim,  "Enchanting: Elemental Lore");
      LoadPerk(0x058F81, Skyrim,  "Enchanting: Corpus Lore");
      LoadPerk(0x058F82, Skyrim,  "Enchanting: Skill Lore");
      LoadPerk(0x058F7C, Skyrim,  "Enchanting: Soul Gem Mastery");
      LoadPerk(0x058F7E, Skyrim,  "Enchanting: Arcane Experimentation");
      LoadPerk(0x058F7D, Skyrim,  "Enchanting: Artificer's Insight");
      LoadPerk(0x058F7F, Skyrim,  "Enchanting: Enchantment Mastery");

      LoadPerk(0x0CB40D, Skyrim,  "Smithing: Craftsmanship");
      LoadPerk(0x05218E, Skyrim,  "Smithing: Advanced Blacksmithing");
      LoadPerk(0x309D25, Requiem, "Smithing: Arcane Craftsmanship");
      LoadPerk(0x17B8BF, Requiem, "Smithing: Legendary Blacksmithing");
      LoadPerk(0x0CB414, Skyrim,  "Smithing: Advanced Light Armors");
      LoadPerk(0x0CB40F, Skyrim,  "Smithing: Elven Smithing");
      LoadPerk(0x0CB411, Skyrim,  "Smithing: Glass Smithing");
      LoadPerk(0x052190, Skyrim,  "Smithing: Draconic Blacksmithing");
      LoadPerk(0x0CB40E, Skyrim,  "Smithing: Dwarven Smithing");
      LoadPerk(0x0CB410, Skyrim,  "Smithing: Orcish Smithing");
      LoadPerk(0x0CB412, Skyrim,  "Smithing: Ebony Smithing");
      LoadPerk(0x0CB413, Skyrim,  "Smithing: Daedric Smithing");

      LoadPerk(0x0BCD2A, Skyrim,  "Heavy Armor: Conditioning");
      LoadPerk(0x07935E, Skyrim,  "Heavy Armor: Relentless Onslaught");
      LoadPerk(0x058F6E, Skyrim,  "Heavy Armor: Combat Casting");
      LoadPerk(0x0BCD2B, Skyrim,  "Heavy Armor: Combat Trance");
      LoadPerk(0x058F6D, Skyrim,  "Heavy Armor: Combat Meditation");
      LoadPerk(0x187ED2, Requiem, "Heavy Armor: Battle Mage");
      LoadPerk(0x058F6F, Skyrim,  "Heavy Armor: Combat Training");
      LoadPerk(0x058F6C, Skyrim,  "Heavy Armor: Fortitude");
      LoadPerk(0x107832, Skyrim,  "Heavy Armor: Power of the Combatant");
      LoadPerk(0x105F33, Skyrim,  "Heavy Armor: Juggernaut");

      LoadPerk(0x0BCCAE, Skyrim,  "Block: Improved Blocking");
      LoadPerk(0x079355, Skyrim,  "Block: Experienced Blocking");
      LoadPerk(0x058F68, Skyrim,  "Block: Strong Grip");
      LoadPerk(0x058F69, Skyrim,  "Block: Elemental Protection");
      LoadPerk(0x106253, Skyrim,  "Block: Defensive Stance");
      LoadPerk(0x058F67, Skyrim,  "Block: Powerful Bashes");
      LoadPerk(0x05F594, Skyrim,  "Block: Overpowering Bashes");
      LoadPerk(0x058F66, Skyrim,  "Block: Disarming Bash");
      LoadPerk(0x058F6A, Skyrim,  "Block: Unstoppable Charge");

      LoadPerk(0x0BABE8, Skyrim,  "Two-Handed: Great Weapon Mastery (1/2)");
      LoadPerk(0x079346, Skyrim,  "Two-Handed: Great Weapon Mastery (2/2)");
      LoadPerk(0x052D51, Skyrim,  "Two-Handed: Barbaric Might");
      LoadPerk(0xADDFB0, Requiem, "Two-Handed: Quarterstaff Focus (1/3)");
      LoadPerk(0xADDFB1, Requiem, "Two-Handed: Quarterstaff Focus (2/3)");
      LoadPerk(0xADDFB2, Requiem, "Two-Handed: Quarterstaff Focus (3/3)");
      LoadPerk(0x0C5C05, Skyrim,  "Two-Handed: Battle Axe Focus (1/3)");
      LoadPerk(0x0C5C06, Skyrim,  "Two-Handed: Battle Axe Focus (2/3)");
      LoadPerk(0x0C5C07, Skyrim,  "Two-Handed: Battle Axe Focus (3/3)");
      LoadPerk(0x03AF83, Skyrim,  "Two-Handed: Greatsword Focus (1/3)");
      LoadPerk(0x0C1E94, Skyrim,  "Two-Handed: Greatsword Focus (2/3)");
      LoadPerk(0x0C1E95, Skyrim,  "Two-Handed: Greatsword Focus (3/3)");
      LoadPerk(0x03AF84, Skyrim,  "Two-Handed: Warhammer Focus (1/3)");
      LoadPerk(0x0C1E96, Skyrim,  "Two-Handed: Warhammer Focus (2/3)");
      LoadPerk(0x0C1E97, Skyrim,  "Two-Handed: Warhammer Focus (3/3)");
      LoadPerk(0x0CB407, Skyrim,  "Two-Handed: Devastating Charge");
      LoadPerk(0x052D52, Skyrim,  "Two-Handed: Devastating Strike");
      LoadPerk(0x03AF9E, Skyrim,  "Two-Handed: Cleave");
      LoadPerk(0x03AFA7, Skyrim,  "Two-Handed: Devastating Cleave");
      LoadPerk(0x182F9B, Requiem, "Two-Handed: Mighty Strike");

      LoadPerk(0x0BABE4, Skyrim,  "One-Handed: Weapon Mastery (1/2)");
      LoadPerk(0x079343, Skyrim,  "One-Handed: Weapon Mastery (2/2)");
      LoadPerk(0x0AD7A3, Requiem, "One-Handed: Martial Arts");
      LoadPerk(0x052D50, Skyrim,  "One-Handed: Penetrating Strikes");
      LoadPerk(0xAD399A, Requiem, "One-Handed: Dagger Focus (1/3)");
      LoadPerk(0xAD3999, Requiem, "One-Handed: Dagger Focus (2/3)");
      LoadPerk(0xAD3998, Requiem, "One-Handed: Dagger Focus (3/3)");
      LoadPerk(0x03FFFA, Skyrim,  "One-Handed: War Axe Focus (1/3)");
      LoadPerk(0x0C3678, Skyrim,  "One-Handed: War Axe Focus (2/3)");
      LoadPerk(0x0C3679, Skyrim,  "One-Handed: War Axe Focus (3/3)");
      LoadPerk(0x05F592, Skyrim,  "One-Handed: Mace Focus (1/3)");
      LoadPerk(0x0C1E92, Skyrim,  "One-Handed: Mace Focus (2/3)");
      LoadPerk(0x0C1E93, Skyrim,  "One-Handed: Mace Focus (3/3)");
      LoadPerk(0x05F56F, Skyrim,  "One-Handed: Sword Focus (1/3)");
      LoadPerk(0x0C1E90, Skyrim,  "One-Handed: Sword Focus (2/3)");
      LoadPerk(0x0C1E91, Skyrim,  "One-Handed: Sword Focus (3/3)");
      LoadPerk(0x03AF81, Skyrim,  "One-Handed: Powerful Strike");
      LoadPerk(0x0CB406, Skyrim,  "One-Handed: Powerful Charge");
      LoadPerk(0x03AFA6, Skyrim,  "One-Handed: Stunning Charge");
      LoadPerk(0x106256, Skyrim,  "One-Handed: Flurry (1/2)");
      LoadPerk(0x106257, Skyrim,  "One-Handed: Flurry (2/2)");
      LoadPerk(0x106258, Skyrim,  "One-Handed: Storm of Steel");

      LoadPerk(0x0BABED, Skyrim,  "Marksman: Ranged Combat Training");
      LoadPerk(0x058F63, Skyrim,  "Marksman: Ranger");
      LoadPerk(0x058F61, Skyrim,  "Marksman: Eagle Eye");
      LoadPerk(0x103ADA, Skyrim,  "Marksman: Marksman's Focus");
      LoadPerk(0x17B8C1, Requiem, "Marksman: Rapid Reload");
      LoadPerk(0x058F62, Skyrim,  "Marksman: Power Shot");
      LoadPerk(0x105F19, Skyrim,  "Marksman: Quick Shot");
      LoadPerk(0x07934A, Skyrim,  "Marksman: Precise Aim");
      LoadPerk(0x105F1C, Skyrim,  "Marksman: Piercing Shot");
      LoadPerk(0x105F1E, Skyrim,  "Marksman: Penetrating Shot");
      LoadPerk(0x058F64, Skyrim,  "Marksman: Stunning Precision");

      LoadPerk(0x0BE123, Skyrim,  "Evasion: Agility");
      LoadPerk(0x18A66F, Requiem, "Evasion: Agile Spellcasting");
      LoadPerk(0x051B1B, Skyrim,  "Evasion: Finesse");
      LoadPerk(0x051B1C, Skyrim,  "Evasion: Dexterity");
      LoadPerk(0x105F22, Skyrim,  "Evasion: Wind Walker");
      LoadPerk(0x18F5A8, Requiem, "Evasion: Vexing Flanker");
      LoadPerk(0x051B17, Skyrim,  "Evasion: Combat Reflexes");
      LoadPerk(0x107831, Skyrim,  "Evasion: Meteoric Reflexes");
      LoadPerk(0x079376, Skyrim,  "Evasion: Dodge");

      LoadPerk(0x0BE126, Skyrim,  "Sneak: Stealth (1/2)");
      LoadPerk(0x0C07C6, Skyrim,  "Sneak: Stealth (2/2)");
      LoadPerk(0x058213, Skyrim,  "Sneak: Muffled Movement");
      LoadPerk(0x05820C, Skyrim,  "Sneak: Light Steps");
      LoadPerk(0x105F23, Skyrim,  "Sneak: Acrobatics");
      LoadPerk(0x058214, Skyrim,  "Sneak: Shadowrunner");
      LoadPerk(0x058210, Skyrim,  "Sneak: Deft Strike");
      LoadPerk(0x1036F0, Skyrim,  "Sneak: Anatomical Lore");
      LoadPerk(0x058211, Skyrim,  "Sneak: Advanced Anatomical Lore");

      LoadPerk(0x0F392A, Skyrim,  "Lockpicking: Cheap Tricks");
      LoadPerk(0x0BE125, Skyrim,  "Lockpicking: Advanced Lockpicking");
      LoadPerk(0x0C3680, Skyrim,  "Lockpicking: Sophisticated Lockpicking");
      LoadPerk(0x0C3681, Skyrim,  "Lockpicking: Masterly Lockpicking");
      LoadPerk(0x105F26, Skyrim,  "Lockpicking: Treasure Hunter");

      LoadPerk(0x0BE124, Skyrim,  "Pickpocket: Nimble Fingers (1/2)");
      LoadPerk(0x018E6A, Skyrim,  "Pickpocket: Nimble Fingers (2/2)");
      LoadPerk(0x058202, Skyrim,  "Pickpocket: Cutpurse");
      LoadPerk(0x058204, Skyrim,  "Pickpocket: Nightly Thief");
      LoadPerk(0x058201, Skyrim,  "Pickpocket: Misdirection");
      LoadPerk(0x058205, Skyrim,  "Pickpocket: Perfect Art");
      LoadPerk(0x096590, Skyrim,  "Pickpocket: Mighty Greed");

      LoadPerk(0x0BE128, Skyrim,  "Speech: Haggling");
      LoadPerk(0x058F72, Skyrim,  "Speech: Silver Tongue");
      LoadPerk(0x3CDF4F, Requiem, "Speech: Masquerade (1/2)");
      LoadPerk(0x30EC6A, Requiem, "Speech: Masquerade (2/2)");
      LoadPerk(0x427139, Requiem, "Speech: Leadership");
      LoadPerk(0x058F7A, Skyrim,  "Speech: Merchant");
      LoadPerk(0x058F79, Skyrim,  "Speech: Fencing");
      LoadPerk(0x394934, Requiem, "Speech: Destructive Urge");
      LoadPerk(0x0D02C5, Requiem, "Speech: Lore of the Thu'um");
      LoadPerk(0x394935, Requiem, "Speech: Indomitable Force");
      LoadPerk(0x394932, Requiem, "Speech: Spiritual Equilibrium");
      LoadPerk(0x3970D0, Requiem, "Speech: The Way of the Voice");
      LoadPerk(0x38F9F8, Requiem, "Speech: Tongue's Insight");

      LoadPerk(0x0BE127, Skyrim,  "Alchemy: Alchemical Lore (1/2)");
      LoadPerk(0x0C07CA, Skyrim,  "Alchemy: Alchemical Lore (2/2)");
      LoadPerk(0x058216, Skyrim,  "Alchemy: Improved Elixirs");
      LoadPerk(0x105F2F, Skyrim,  "Alchemy: Concentrated Poisons");
      LoadPerk(0x058217, Skyrim,  "Alchemy: Improved Poisons");
      LoadPerk(0x058218, Skyrim,  "Alchemy: Catalysis (1/2)");
      LoadPerk(0x105F2B, Skyrim,  "Alchemy: Catalysis (2/2)");
      LoadPerk(0x05821D, Skyrim,  "Alchemy: Purification Process");

      LoadPerk(0x105F2C, Skyrim);   // Alchemy: Immunization (Taproot)
      LoadPerk(0x1CD495, Requiem);  // Alchemy: Night Vision (Sabre Cat Eye)
      LoadPerk(0x1CD48F, Requiem);  // Alchemy: Regeneration (1/2, Spriggan Sap)
      LoadPerk(0x1CD492, Requiem);  // Alchemy: Regeneration (2/2, Troll Fat)
      LoadPerk(0x1CD497, Requiem);  // Alchemy: Fortified Muscles (Mammoth Heart)
      LoadPerk(0x1D9AAB, Requiem);  // Alchemy: Alchemical Intellect (Daedra Heart)
    }
    catch (const std::exception& e) {
      Log("ERROR {}", e.what());
      return false;
    }

    // Initialize powers.
    Powers.clear();

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

    // Bind death events.
    auto sesh = RE::ScriptEventSourceHolder::GetSingleton();
    if (!sesh) {
      Log("Could not get script event source holder.");
      return false;
    }
    sesh->AddEventSink<RE::TESDeathEvent>(this);

    // Bind keyboard events.
#ifndef NDEBUG
    auto input = RE::BSInputDeviceManager::GetSingleton();
    if (!input) {
      Log("Could not get input device manager.");
      return false;
    }
    input->AddEventSink<RE::InputEvent*>(this);
#endif

    return true;
  }

  void OnPostLoadGame() noexcept
  {
    try {
      if (Player && Player->GetLevel() == 1) {
        OnRegression();
      }
    }
    catch (const std::exception& e) {
      Log("Regression: {}", e.what());
    }
    catch (...) {
      Log("Regression: Unhandled exception.");
    }
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
      Log("Regression: {}", e.what());
    }
    catch (...) {
      Log("Regression: Unhandled exception.");
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
  static void LoadPerk(RE::FormID id, std::string_view mod, std::string name)
  {
    const auto form = Data->LookupForm(id, mod);
    if (!form) {
      throw std::runtime_error{ std::format("Could not find perk \"{}\" in mod: {}", name, mod) };
    }
    if (!form->Is(RE::BGSPerk::FORMTYPE)) {
      throw std::runtime_error{ std::format("Invalid \"{}\" perk type in mod: {}", name, mod) };
    }
    Perks.emplace_back(form->As<RE::BGSPerk>(), std::move(name));
  }

  static void LoadPerk(RE::FormID id, std::string_view mod)
  {
    const auto form = Data->LookupForm(id, mod);
    if (!form) {
      throw std::runtime_error{ std::format("Could not find perk {:06X} in mod: {}", id, mod) };
    }
    if (!form->Is(RE::BGSPerk::FORMTYPE)) {
      throw std::runtime_error{ std::format("Invalid {:06X} perk type in mod: {}", id, mod) };
    }
    PerksExtra.emplace_back(form->As<RE::BGSPerk>());
  }

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
    const auto level = info["Level"].is_int64() ? info["Level"].as_int64() : 1;
    if (level < static_cast<int64_t>(Player->GetLevel())) {
      throw std::runtime_error{ "Current level higher, than regression level." };
    }
    ExecuteCommand(std::format("Player.SetLevel {}", level));

    // Restore perk points.
    const auto perk_points = info["PerkPoints"].is_int64() ? info["PerkPoints"].as_int64() : 0;
    SetPerkPoints(static_cast<int>(perk_points));

    // Restore spells.
    for (const auto& e : info["Spells"].as_array()) {
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
    for (const auto& e : info["Powers"].as_array()) {
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
    const auto& skills = info["Skills"].as_object();
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

    // Restore perks.
    for (const auto& e : info["Perks"].as_array()) {
      if (!e.is_string()) {
        continue;
      }
      for (const auto [perk, name] : Perks) {
        if (name == std::string_view{ e.as_string() }) {
          Player->AddPerk(perk);
          Log("PERKS {:08X} {}", perk->GetFormID(), name);
          break;
        }
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

    // Report level and perk points.
    Log("LEVEL {:3}", level);
    if (perk_points > 0) {
      Log("PERKS {:3}", perk_points);
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
    Log(message);
    ShowNotification(1.0f, std::move(message));
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

    // Update perks.
    boost::json::array perks;
    for (const auto& [perk, name] : Perks) {
      if (Player->HasPerk(perk)) {
        perks.emplace_back(name);
        Log("PERKS {}", name);
      }
    }
    info["Perks"] = std::move(perks);

    // Update perk points.
    auto perk_points = static_cast<int64_t>(Player->GetGameStatsData().perkCount);
    for (const auto perk : PerksExtra) {
      if (Player->HasPerk(perk)) {
        perk_points++;
      }
    }
    info["PerkPoints"] = perk_points;
    Log("PERKS {}", perk_points);

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

  static void ShowNotification(float seconds, std::string message)
  {
    const auto vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
    if (!vm) {
      throw std::runtime_error{ "Could not get virtual machine." };
    }
    const auto policy = vm->GetObjectHandlePolicy();
    if (!policy) {
      throw std::runtime_error{ "Could not get object handle policy." };
    }
    RE::BSFixedString name = "Utility";
    RE::BSFixedString function = "Wait";
    auto args = RE::MakeFunctionArguments(float{ seconds });

    class WaitCallback : public RE::BSScript::IStackCallbackFunctor {
    public:
      WaitCallback(std::string message) :
        message_(std::move(message))
      {}

      void operator()(RE::BSScript::Variable result) override
      {
        RE::DebugNotification(message_.data());
      }

      void SetObject(const RE::BSTSmartPointer<RE::BSScript::Object>& object) override {}

    private:
      std::string message_;
    };

    RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> result{ new WaitCallback{ std::move(message) } };
    if (!vm->DispatchStaticCall(name, function, args, result)) {
      throw std::runtime_error{ std::format("Could not execute function: Utility.Wait({})", seconds) };
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
