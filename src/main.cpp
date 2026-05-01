#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>

using namespace geode::prelude;

class $modify(MyExitProtect, PauseLayer) {
    struct Fields {
        std::chrono::steady_clock::time_point m_lastClick;
    };

    void onQuit(CCObject* sender) {
        // 1. Obtener ajustes del mod.json
        bool shouldConfirm = Mod::get()->getSettingValue<bool>("enable-confirm");
        std::string customText = Mod::get()->getSettingValue<std::string>("custom-text");
        int speedLimit = Mod::get()->getSettingValue<int>("click-speed");

        // 2. Seguridad: Bloquear si alguien intenta hackear el límite de 1000ms
        if (speedLimit > 1000) speedLimit = 1000;
        if (speedLimit < 0) speedLimit = 0;

        // Si la confirmación está desactivada, salimos normal
        if (!shouldConfirm) {
            PauseLayer::onQuit(sender);
            return;
        }

        auto ahora = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_fields->m_lastClick).count();

        // 3. Lógica de doble clic
        if (diff < speedLimit) {
            PauseLayer::onQuit(sender);
        } else {
            m_fields->m_lastClick = ahora;
            
            // Crear notificación visual (menos intrusiva que un popup)
            Notification::create(customText, NotificationIcon::Info, 1.0f)->show();
        }
    }
};
