#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>

using namespace geode::prelude;

class $modify(PauseDoubleClick, PauseLayer) {
    struct Fields {
        std::chrono::steady_clock::time_point m_lastClick;
        bool m_isNotifying = false;
    };

    void onQuit(CCObject* sender) {
        // Leer ajustes
        bool enableMod = Mod::get()->getSettingValue<bool>("enable-double-click");
        bool showNotif = Mod::get()->getSettingValue<bool>("show-notification");
        std::string customText = Mod::get()->getSettingValue<std::string>("custom-text");
        int speedLimit = std::clamp(Mod::get()->getSettingValue<int>("click-speed"), 0, 1000);

        if (!enableMod) {
            PauseLayer::onQuit(sender);
            return;
        }

        auto ahora = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_fields->m_lastClick).count();

        if (diff < speedLimit) {
            // ÉXITO: Pasó el doble clic. 
            // Llamamos al original. Si RobTop tiene "Confirm Exit" activo, 
            // el juego mostrará su popup AHORA.
            PauseLayer::onQuit(sender);
        } else {
            // Primer clic o muy lento
            m_fields->m_lastClick = ahora;

            if (showNotif && !m_fields->m_isNotifying) {
                m_fields->m_isNotifying = true;
                
                auto notif = Notification::create(customText, NotificationIcon::None, 0.5f);
                // Resetear la bandera de spam cuando la notificación desaparezca
                notif->setID("exit-notification");
                notif->show();
                
                // Usamos un delay simple para permitir otra notificación después de medio segundo
                Loader::get()->queueInMainThread([this]() {
                    m_fields->m_isNotifying = false;
                });
            }
        }
    }
};
