#include "Hooks.h"

namespace Hooks {
    std::uintptr_t SetInjured(RE::TESObjectREFR* a_ref, bool a_injured) {
        static REL::Relocation<decltype(&SetInjured)> func{ RELOCATION_ID(0, 37223) };
        logger::debug("Calling SetInjured hook");
        return func(a_ref, a_injured);
    }

    std::uintptr_t RestoreActorValue(RE::Actor* a_actor, RE::ActorValue a_actorValue, float a_value) {
        static REL::Relocation<decltype(&RestoreActorValue)> func{ RELOCATION_ID(37816, 38455) };
        logger::debug("Calling RestoreActorValue hook");
        return func(a_actor, a_actorValue, a_value);
    }

    float GetActorValuePercent(RE::Actor* a_actor, RE::ActorValue a_actorValue) {
        static REL::Relocation<decltype(&GetActorValuePercent)> func{ RELOCATION_ID(36347, 37337) };
        logger::debug("Calling GetActorValuePercent hook");
        return func(a_actor, a_actorValue);
    }
}
