#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>

using namespace geode::prelude;

class $modify(PauseDoubleClick, PauseLayer) {
    struct Fields {
        std::chrono::steady_clock::time_point m_lastClick = std::chrono::steady_clock::now() - std::chrono::seconds(5);
        bool m_hasNotificationActive = false;
    };

    void onQuit(CCObject* sender) {
        auto mod = Mod::get();
        
        // Lectura de ajustes corregida para evitar "Missing Setting"
        bool enableMod = mod->getSettingValue<bool>("enable-double-click");
        int64_t gameMode = mod->getSettingValue<int64_t>("game-mode"); 
        bool showNotif = mod->getSettingValue<bool>("show-notification");
        std::string customText = mod->getSettingValue<std::string>("custom-text");
        int64_t speedLimit = mod->getSettingValue<int64_t>("click-speed");

        bool isPlatformer = false;
        if (auto playLayer = PlayLayer::get()) {
            if (playLayer->m_level) {
                isPlatformer = playLayer->m_level->isPlatformer();
            }
        }

        // 0: Both, 1: Platformer, 2: Normal
        bool shouldApply = false;
        if (gameMode == 0) shouldApply = true;
        else if (gameMode == 1 && isPlatformer) shouldApply = true;
        else if (gameMode == 2 && !isPlatformer) shouldApply = true;

        if (!enableMod || !shouldApply) {
            PauseLayer::onQuit(sender);
            return;
        }

        auto ahora = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_fields->m_lastClick).count();

        if (diff < speedLimit) {
            PauseLayer::onQuit(sender);
        } else {
            m_fields->m_lastClick = ahora;

            if (showNotif && !m_fields->m_hasNotificationActive) {
                m_fields->m_hasNotificationActive = true;
                
                auto notif = Notification::create(customText, NotificationIcon::None, 0.8f);
                notif->show();
                
                // Programar el reset de la bandera de notificación
                this->getScheduler()->scheduleSelector(
                    schedule_selector(PauseDoubleClick::resetNotifFlag), 
                    this, 1.0f, 0, 0.0f, false
                );
            }
        }
    }

    void resetNotifFlag(float dt) {
        m_fields->m_hasNotificationActive = false;
    }
};
