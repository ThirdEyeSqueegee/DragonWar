#pragma once

class Utility {
protected:
    Utility() = default;
    ~Utility() = default;

public:
    Utility(const Utility&) = delete;            // Prevent copy construction
    Utility(Utility&&) = delete;                 // Prevent move construction
    Utility& operator=(const Utility&) = delete; // Prevent copy assignment
    Utility& operator=(Utility&&) = delete;      // Prevent move assignment

    static Utility* GetSingleton() {
        static Utility singleton;
        return std::addressof(singleton);
    }

    inline static RE::TESConditionItem cond_item;
    inline static RE::TESCondition is_injured;
    inline static RE::TESGlobal* injury_chance_100 = nullptr;
    inline static RE::TESGlobal* injury_chance_50 = nullptr;
    inline static RE::TESGlobal* injury_chance_25 = nullptr;
    inline static RE::TESGlobal* cd_increase_75 = nullptr;
    inline static RE::TESGlobal* cd_increase_50 = nullptr;
    inline static RE::TESGlobal* cd_increase_25 = nullptr;
    inline static std::vector<RE::TESShout*> shouts;

    using ShoutCDMapType = std::unordered_map<RE::TESShout*, std::chrono::system_clock::time_point>;
    inline static std::unordered_map<RE::Actor*, ShoutCDMapType> dragon_shout_map;

    static void InitCondition() {
        cond_item.data.comparisonValue.f = 1.0f;
        cond_item.data.functionData.function = RE::FUNCTION_DATA::FunctionID::kGetIsInjured;
        is_injured.head = &cond_item;
        logger::info("Initialized injured condition");
    }

    static void InitGlobals() {
        const auto handler = RE::TESDataHandler::GetSingleton();
        injury_chance_100 = handler->LookupForm<RE::TESGlobal>(0xb05, "DragonCrisis.esp"sv);
        injury_chance_50 = handler->LookupForm<RE::TESGlobal>(0xb06, "DragonCrisis.esp"sv);
        injury_chance_25 = handler->LookupForm<RE::TESGlobal>(0xb07, "DragonCrisis.esp"sv);
        cd_increase_75 = handler->LookupForm<RE::TESGlobal>(0xb08, "DragonCrisis.esp"sv);
        cd_increase_50 = handler->LookupForm<RE::TESGlobal>(0xb00, "DragonCrisis.esp"sv);
        cd_increase_25 = handler->LookupForm<RE::TESGlobal>(0xb0a, "DragonCrisis.esp"sv);
        logger::info("Initialized globals");
    }

    static void CacheShouts() {
        for (const auto handler = RE::TESDataHandler::GetSingleton(); const auto& shout : handler->GetFormArray(RE::FormType::Shout)) {
            // if (!shout->GetFile()->GetFilename().compare("DragonWar.esp"sv))
            shouts.emplace_back(shout->As<RE::TESShout>());
        }

        for (const auto& shout : shouts)
            logger::info("Cached shout {} (0x{:x})", shout->GetName(), shout->GetFormID());

        logger::info("Cached {} shouts", shouts.size());
    }
};
