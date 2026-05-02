#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>

using namespace geode::prelude;

class $modify(PauseDoubleClick, PauseLayer) {
    struct Fields {
        std::chrono::steady_clock::time_point m_lastClick;
        bool m_hasNotificationActive = false;
    };

    void onQuit(CCObject* sender) {
        bool enableMod = Mod::get()->getSettingValue<bool>("enable-double-click");
        auto gameMode = Mod::get()->getSettingValue<std::string>("game-mode");
        
        // Verificar si el nivel actual es plataforma
        auto playLayer = PlayLayer::get();
        bool isPlatformer = playLayer ? playLayer->m_levelSettings->m_isPlatformer : false;

        // Lógica de filtrado por modo de juego
        bool shouldApply = false;
        if (gameMode == "both") shouldApply = true;
        else if (gameMode == "platformer" && isPlatformer) shouldApply = true;
        else if (gameMode == "normal" && !isPlatformer) shouldApply = true;

        if (!enableMod || !shouldApply) {
            PauseLayer::onQuit(sender);
            return;
        }

        auto ahora = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_fields->m_lastClick).count();
        int speedLimit = std::clamp(Mod::get()->getSettingValue<int>("click-speed"), 0, 1000);

        if (diff < speedLimit) {
            // Éxito: Se ejecuta después del doble clic (respeta el confirm exit de RobTop)
            PauseLayer::onQuit(sender);
        } else {
            m_fields->m_lastClick = ahora;

            if (Mod::get()->getSettingValue<bool>("show-notification") && !m_fields->m_hasNotificationActive) {
                m_fields->m_hasNotificationActive = true;
                
                auto notif = Notification::create(
                    Mod::get()->getSettingValue<std::string>("custom-text"), 
                    NotificationIcon::None, 
                    1.0f
                );
                
                // Usamos un scheduler para resetear la bandera de spam cuando la notif desaparezca
                this->scheduleOnce(schedule_selector(PauseDoubleClick::resetNotifFlag), 1.0f);
                notif->show();
            }
        }
    }

    void resetNotifFlag(float dt) {
        m_fields->m_hasNotificationActive = false;
    }
};
