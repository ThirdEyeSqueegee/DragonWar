#include "Events.h"

#include "Hooks.h"
#include "Utility.h"

namespace Events {
    OnHitEventHandler* OnHitEventHandler::GetSingleton() {
        static OnHitEventHandler singleton;
        return std::addressof(singleton);
    }

    RE::BSEventNotifyControl OnHitEventHandler::ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>* a_eventSource) {
        if (!a_event)
            return RE::BSEventNotifyControl::kContinue;

        if (!a_event->target || !a_event->cause)
            return RE::BSEventNotifyControl::kContinue;

        if (a_event->cause->IsPlayerRef()) {
            if (const auto actor = a_event->target->As<RE::Actor>()) {
                if (actor->HasKeywordString("ActorTypeDragon"sv)) {
                    // Health injury
                    logger::debug("Dragon {} hit", actor->GetName());
                    std::random_device rd;
                    std::mt19937 rng(rd());
                    const auto health_pct = Hooks::GetActorValuePercent(actor, RE::ActorValue::kHealth);
                    logger::debug("Current health for {}: {}%", actor->GetName(), health_pct * 100);
                    auto chance = Utility::injury_chance_100->value;
                    if (health_pct <= 0.5f && health_pct > 0.25f)
                        chance = Utility::injury_chance_50->value;
                    else if (health_pct <= 0.25f)
                        chance = Utility::injury_chance_25->value;
                    logger::debug("Injury probability: {}", chance);
                    if (std::bernoulli_distribution dist(chance); dist(rng)) {
                        if (!Utility::is_injured(actor, actor)) {
                            logger::debug("Injury roll passed, setting injured bool");
                            Hooks::SetInjured(a_event->target.get());
                        }
                    }
                    // Magicka injury
                    const auto magicka_pct = Hooks::GetActorValuePercent(actor, RE::ActorValue::kMagicka);
                    logger::debug("Current magicka for {}: {}%", actor->GetName(), magicka_pct * 100);
                    const auto shout_recovery_mult = actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kShoutRecoveryMult);
                    logger::debug("Current shout recovery mult: {}", shout_recovery_mult);
                    if (magicka_pct <= 0.75f && magicka_pct > 0.5f) {
                        Hooks::RestoreActorValue(actor, RE::ActorValue::kShoutRecoveryMult, Utility::cd_increase_75->value);
                        logger::debug("\tIncreased {}'s shout cooldown by {}%", actor->GetName(), Utility::cd_increase_75->value * 100);
                    }
                    if (magicka_pct <= 0.5f && magicka_pct > 0.25f) {
                        Hooks::RestoreActorValue(actor, RE::ActorValue::kShoutRecoveryMult, Utility::cd_increase_50->value);
                        logger::debug("\tIncreased {}'s shout cooldown by {}%", actor->GetName(), Utility::cd_increase_50->value * 100);
                    }
                    if (magicka_pct <= 0.25f) {
                        Hooks::RestoreActorValue(actor, RE::ActorValue::kShoutRecoveryMult, Utility::cd_increase_25->value);
                        logger::debug("\tIncreased {}'s shout cooldown by {}%", actor->GetName(), Utility::cd_increase_25->value * 100);
                    }
                }
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }

    void OnHitEventHandler::Register() {
        const auto holder = RE::ScriptEventSourceHolder::GetSingleton();
        holder->AddEventSink(GetSingleton());
        logger::info("Registered hit event handler");
    }

    OnCombatEventHandler* OnCombatEventHandler::GetSingleton() {
        static OnCombatEventHandler singleton;
        return std::addressof(singleton);
    }

    RE::BSEventNotifyControl OnCombatEventHandler::ProcessEvent(const RE::TESCombatEvent* a_event,
                                                                RE::BSTEventSource<RE::TESCombatEvent>* a_eventSource) {
        if (!a_event)
            return RE::BSEventNotifyControl::kContinue;

        if (!a_event->actor)
            return RE::BSEventNotifyControl::kContinue;

        if (!a_event->targetActor)
            return RE::BSEventNotifyControl::kContinue;

        if (a_event->newState != RE::ACTOR_COMBAT_STATE::kCombat)
            return RE::BSEventNotifyControl::kContinue;

        if (const auto actor = a_event->actor->As<RE::Actor>()) {
            if (actor->HasKeywordString("ActorTypeDragon"sv)) {
                if (a_event->targetActor->IsPlayerRef()) {
                    if (!Utility::dragon_shout_map.contains(actor)) {
                        // Randomize flight data
                        logger::debug("Dragon {} started combat with player", actor->GetName());
                        std::random_device rd;
                        std::mt19937 rng(rd());
                        std::uniform_real_distribution dist(0.0f, 1.0f);
                        const auto cs = actor->GetActorRuntimeData().combatController->combatStyle;
                        const auto dive_bomb_chance = dist(rng);
                        const auto flying_attack_chance = dist(rng);
                        const auto ground_attack_chance = dist(rng);
                        const auto ground_attack_mult = dist(rng);
                        const auto hover_chance = dist(rng);
                        const auto hover_mult = dist(rng);
                        const auto perch_attack_chance = dist(rng);
                        const auto perch_attack_mult = dist(rng);
                        cs->flightData.diveBombChance = dive_bomb_chance;
                        logger::debug("\tSet dive bomb chance to {}", dive_bomb_chance);
                        cs->flightData.flyingAttackChance = flying_attack_chance;
                        logger::debug("\tSet flying attack chance to {}", flying_attack_chance);
                        cs->flightData.groundAttackChance = ground_attack_chance;
                        logger::debug("\tSet ground attack chance to {}", ground_attack_chance);
                        cs->flightData.groundAttackTimeMult = ground_attack_mult;
                        logger::debug("\tSet ground attack time mult to {}", ground_attack_mult);
                        cs->flightData.hoverChance = hover_chance;
                        logger::debug("\tSet hover chance to {}", hover_chance);
                        cs->flightData.hoverTimeMult = hover_mult;
                        logger::debug("\tSet hover time mult to {}", hover_mult);
                        cs->flightData.perchAttackChance = perch_attack_chance;
                        logger::debug("\tSet perch attack chance to {}", perch_attack_chance);
                        cs->flightData.perchAttackTimeMult = perch_attack_mult;
                        logger::debug("\tSet perch attack time mult to {}", perch_attack_mult);
                        logger::debug("\tRandomized {}'s flight data sliders", actor->GetName());
                        // Cache dragon and its shouts
                        Utility::ShoutCDMapType shout_cd_map;
                        for (const auto& shout : Utility::shouts) {
                            if (actor->HasShout(shout)) {
                                shout_cd_map.emplace(shout, std::chrono::system_clock::time_point::min());
                                logger::debug("\tCached shout {} for {}", shout->GetName(), actor->GetName());
                            }
                        }
                        Utility::dragon_shout_map.emplace(actor, shout_cd_map);
                        logger::debug("\tCached {} shouts for {}", shout_cd_map.size(), actor->GetName());
                    }
                }
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }

    void OnCombatEventHandler::Register() {
        const auto holder = RE::ScriptEventSourceHolder::GetSingleton();
        holder->AddEventSink(GetSingleton());
        logger::info("Registered object loaded event handler");
    }

    ActionEventHandler* ActionEventHandler::GetSingleton() {
        static ActionEventHandler singleton;
        return std::addressof(singleton);
    }

    RE::BSEventNotifyControl ActionEventHandler::ProcessEvent(const SKSE::ActionEvent* a_event,
                                                              RE::BSTEventSource<SKSE::ActionEvent>* a_eventSource) {
        if (!a_event)
            return RE::BSEventNotifyControl::kContinue;

        if (a_event->type != SKSE::ActionEvent::Type::kVoiceFire)
            return RE::BSEventNotifyControl::kContinue;

        if (!a_event->actor)
            return RE::BSEventNotifyControl::kContinue;

        if (const auto actor = a_event->actor; actor->HasKeywordString("ActorTypeDragon"sv)) {
            const auto current_shout = actor->GetActorRuntimeData().currentProcess->high->currentShout;
            logger::debug("Current shout: {}", current_shout->GetName());
            if (Utility::dragon_shout_map.contains(actor)) {
                if (auto& shout_cd_map = Utility::dragon_shout_map[actor]; shout_cd_map.contains(current_shout)) {
                    shout_cd_map[current_shout]
                        = std::chrono::system_clock::now() + std::chrono::seconds(static_cast<int>(current_shout->variations[2].recoveryTime));
                    logger::debug("{}: Updated cooldown for {}", actor->GetName(), current_shout->GetName());
                    if (const auto values = shout_cd_map | std::views::values;
                        std::ranges::find(values, std::chrono::system_clock::time_point::min()) != values.end()) {
                        std::random_device rd;
                        std::mt19937 rng(rd());
                        std::uniform_int_distribution dist(0, static_cast<int>(shout_cd_map.size() - 1));
                        auto random_it = std::next(shout_cd_map.begin(), dist(rng));
                        while (random_it->first->GetFormID() == current_shout->GetFormID()
                               && random_it->second != std::chrono::system_clock::time_point::min())
                            random_it = std::next(shout_cd_map.begin(), dist(rng));
                        const auto new_shout = random_it->first;
                        logger::debug("Equipping new shout {} to {}", new_shout->GetName(), actor->GetName());
                        std::jthread([=] {
                            std::this_thread::sleep_for(2s);
                            SKSE::GetTaskInterface()->AddTask([=] {
                                const auto manager = RE::ActorEquipManager::GetSingleton();
                                manager->EquipShout(actor, new_shout);
                                actor->GetActorRuntimeData().currentProcess->high->currentShout = new_shout;
                                logger::debug("New current shout: {}", actor->GetActorRuntimeData().currentProcess->high->currentShout->GetName());
                                logger::debug("Recovery time: {}", actor->GetActorRuntimeData().currentProcess->high->voiceRecoveryTime);
                                actor->GetActorRuntimeData().currentProcess->high->voiceRecoveryTime = 0.0f;
                                logger::debug("New recovery time: {}", actor->GetActorRuntimeData().currentProcess->high->voiceRecoveryTime);
                                logger::debug("Equipped {} to {}", new_shout->GetName(), actor->GetName());
                            });
                        }).detach();
                        for (auto& [shout, cd] : shout_cd_map) {
                            if (shout->GetFormID() != current_shout->GetFormID() && cd >= std::chrono::system_clock::now()) {
                                cd = std::chrono::system_clock::time_point::min();
                                logger::debug("\tReset cooldown for {}", shout->GetName());
                            }
                        }
                    }
                }
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }

    void ActionEventHandler::Register() {
        SKSE::GetActionEventSource()->AddEventSink(GetSingleton());
        logger::info("Registered action event handler");
    }

    OnEquipEventHandler* OnEquipEventHandler::GetSingleton() {
        static OnEquipEventHandler singleton;
        return std::addressof(singleton);
    }

    RE::BSEventNotifyControl OnEquipEventHandler::ProcessEvent(const RE::TESEquipEvent* a_event,
                                                               RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) {
        if (!a_event)
            return RE::BSEventNotifyControl::kContinue;

        if (!a_event->equipped)
            return RE::BSEventNotifyControl::kContinue;

        if (!a_event->actor)
            return RE::BSEventNotifyControl::kContinue;

        if (const auto actor = a_event->actor->As<RE::Actor>(); actor->HasKeywordString("ActorTypeDragon"sv)) {
            if (const auto shout = RE::TESForm::LookupByID<RE::TESShout>(a_event->baseObject)) {
                logger::debug("{} equipped to {} in OnEquip", shout->GetName(), actor->GetName());
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }

    void OnEquipEventHandler::Register() {
        const auto holder = RE::ScriptEventSourceHolder::GetSingleton();
        holder->AddEventSink(GetSingleton());
        logger::info("Registered equip event handler");
    }
}
