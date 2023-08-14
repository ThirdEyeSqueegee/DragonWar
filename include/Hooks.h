#pragma once

namespace Hooks {
    std::uintptr_t SetInjured(RE::TESObjectREFR* a_ref, bool a_injured = true);

    std::uintptr_t RestoreActorValue(RE::Actor* a_actor, RE::ActorValue a_actorValue, float a_value);

    float GetActorValuePercent(RE::Actor* a_actor, RE::ActorValue a_actorValue);
}
