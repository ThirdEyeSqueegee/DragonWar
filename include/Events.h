#pragma once

namespace Events {
    class OnHitEventHandler : public RE::BSTEventSink<RE::TESHitEvent> {
    protected:
        OnHitEventHandler() = default;
        ~OnHitEventHandler() override = default;

    public:
        OnHitEventHandler(const OnHitEventHandler&) = delete;
        OnHitEventHandler(OnHitEventHandler&&) = delete;
        OnHitEventHandler& operator=(const OnHitEventHandler&) = delete;
        OnHitEventHandler& operator=(OnHitEventHandler&&) = delete;

        static OnHitEventHandler* GetSingleton();

        RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>* a_eventSource) override;

        static void Register();
    };

    class OnCombatEventHandler : public RE::BSTEventSink<RE::TESCombatEvent> {
    protected:
        OnCombatEventHandler() = default;
        ~OnCombatEventHandler() override = default;

    public:
        OnCombatEventHandler(const OnCombatEventHandler&) = delete;
        OnCombatEventHandler(OnCombatEventHandler&&) = delete;
        OnCombatEventHandler& operator=(const OnCombatEventHandler&) = delete;
        OnCombatEventHandler& operator=(OnCombatEventHandler&&) = delete;

        static OnCombatEventHandler* GetSingleton();

        RE::BSEventNotifyControl ProcessEvent(const RE::TESCombatEvent* a_event, RE::BSTEventSource<RE::TESCombatEvent>* a_eventSource) override;

        static void Register();
    };

    class ActionEventHandler : public RE::BSTEventSink<SKSE::ActionEvent> {
    protected:
        ActionEventHandler() = default;
        ~ActionEventHandler() override = default;

    public:
        ActionEventHandler(const ActionEventHandler&) = delete;
        ActionEventHandler(ActionEventHandler&&) = delete;
        ActionEventHandler& operator=(const ActionEventHandler&) = delete;
        ActionEventHandler& operator=(ActionEventHandler&&) = delete;

        static ActionEventHandler* GetSingleton();

        RE::BSEventNotifyControl ProcessEvent(const SKSE::ActionEvent* a_event, RE::BSTEventSource<SKSE::ActionEvent>* a_eventSource) override;

        static void Register();
    };

    class OnEquipEventHandler : public RE::BSTEventSink<RE::TESEquipEvent> {
    protected:
        OnEquipEventHandler() = default;
        ~OnEquipEventHandler() override = default;

    public:
        OnEquipEventHandler(const OnEquipEventHandler&) = delete;
        OnEquipEventHandler(OnEquipEventHandler&&) = delete;
        OnEquipEventHandler& operator=(const OnEquipEventHandler&) = delete;
        OnEquipEventHandler& operator=(OnEquipEventHandler&&) = delete;

        static OnEquipEventHandler* GetSingleton();

        RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) override;

        static void Register();
    };
}
