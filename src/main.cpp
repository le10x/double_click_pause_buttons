#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>

using namespace geode::prelude;

class $modify(PauseDoubleClick, PauseLayer) {
    struct Fields {
        std::chrono::steady_clock::time_point m_lastClick;
        bool m_isNotifying = false;
    };

    // Corregido para 2.2081
    bool init(bool unfocused) {
        return PauseLayer::init(unfocused);
    }

    void onQuit(CCObject* sender) {
        auto mod = Mod::get();
        bool isPlat = false;
        if (auto pl = PlayLayer::get()) isPlat = pl->m_level->isPlatformer();

        // Filtros de activación
        if (!mod->getSettingValue<bool>("enable-double-click") || (mod->getSettingValue<bool>("plat-only") && !isPlat)) {
            PauseLayer::onQuit(sender);
            return;
        }

        auto ahora = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_fields->m_lastClick).count();

        if (diff < mod->getSettingValue<int64_t>("click-speed")) {
            // ÉXITO: Salir
            PauseLayer::onQuit(sender);
        } else {
            // PRIMER CLIC: Notificar y guardar tiempo
            m_fields->m_lastClick = ahora;

            if (mod->getSettingValue<bool>("show-notification") && !m_fields->m_isNotifying) {
                m_fields->m_isNotifying = true;
                Notification::create(mod->getSettingValue<std::string>("custom-text"), NotificationIcon::None, 0.8f)->show();
                this->scheduleOnce(schedule_selector(PauseDoubleClick::resetNotif), 1.0f);
            }
        }
    }

    void resetNotif(float dt) { m_fields->m_isNotifying = false; }
};
