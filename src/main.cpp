#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>

using namespace geode::prelude;

class $modify(PauseDoubleClick, PauseLayer) {
    struct Fields {
        std::chrono::steady_clock::time_point m_lastClick;
        bool m_hasNotificationActive = false;
    };

    // Cambiamos el hook a la función que maneja el botón directamente
    void onQuit(CCObject* sender) {
        auto mod = Mod::get();
        bool enableMod = mod->getSettingValue<bool>("enable-double-click");
        
        // Obtenemos el índice del enum (0, 1 o 2)
        int64_t gameMode = mod->getSettingValue<int64_t>("game-mode");

        bool isPlatformer = false;
        if (auto playLayer = PlayLayer::get()) {
            if (playLayer->m_level) isPlatformer = playLayer->m_level->isPlatformer();
        }

        bool shouldApply = (gameMode == 0) || (gameMode == 1 && isPlatformer) || (gameMode == 2 && !isPlatformer);

        if (!enableMod || !shouldApply) {
            PauseLayer::onQuit(sender);
            return;
        }

        auto ahora = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_fields->m_lastClick).count();
        int64_t speedLimit = mod->getSettingValue<int64_t>("click-speed");

        if (diff < speedLimit) {
            // SEGUNDO CLIC EXITOSO: 
            // Aquí llamamos a la función original de RobTop. 
            // Si el usuario tiene "Confirm Exit" activo en GD, saldrá SU mensaje ahora.
            PauseLayer::onQuit(sender);
        } else {
            // PRIMER CLIC:
            // Guardamos el tiempo pero NO llamamos a la función original.
            // Así evitamos que el mensaje de RobTop salga todavía.
            m_fields->m_lastClick = ahora;

            if (mod->getSettingValue<bool>("show-notification") && !m_fields->m_hasNotificationActive) {
                m_fields->m_hasNotificationActive = true;
                Notification::create(mod->getSettingValue<std::string>("custom-text"), NotificationIcon::None, 0.8f)->show();
                
                this->getScheduler()->scheduleSelector(
                    schedule_selector(PauseDoubleClick::resetNotifFlag), this, 1.0f, 0, 0.0f, false
                );
            }
        }
    }

    void resetNotifFlag(float dt) {
        m_fields->m_hasNotificationActive = false;
    }
};
