#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>

using namespace geode::prelude;

class $modify(PauseDoubleClick, PauseLayer) {
    struct Fields {
        // Inicializamos el tiempo en el pasado para que el primer clic funcione bien
        std::chrono::steady_clock::time_point m_lastClick = std::chrono::steady_clock::now() - std::chrono::seconds(5);
        bool m_hasNotificationActive = false;
    };

    void onQuit(CCObject* sender) {
        // 1. Cargar Ajustes de forma segura
        auto mod = Mod::get();
        bool enableMod = mod->getSettingValue<bool>("enable-double-click");
        int gameMode = mod->getSettingValue<int64_t>("game-mode"); // Los enum se leen como int64_t
        bool showNotif = mod->getSettingValue<bool>("show-notification");
        std::string customText = mod->getSettingValue<std::string>("custom-text");
        int speedLimit = mod->getSettingValue<int64_t>("click-speed");

        // 2. Detectar si es Platformer
        bool isPlatformer = false;
        if (auto playLayer = PlayLayer::get()) {
            if (playLayer->m_level) {
                isPlatformer = playLayer->m_level->isPlatformer();
            }
        }

        // 3. Verificar si el mod debe actuar según el modo de juego
        // 0: Both, 1: Platformer Only, 2: Normal Only
        bool shouldApply = false;
        if (gameMode == 0) shouldApply = true;
        else if (gameMode == 1 && isPlatformer) shouldApply = true;
        else if (gameMode == 2 && !isPlatformer) shouldApply = true;

        if (!enableMod || !shouldApply) {
            PauseLayer::onQuit(sender);
            return;
        }

        // 4. Lógica de tiempo
        auto ahora = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_fields->m_lastClick).count();

        if (diff < speedLimit) {
            // ÉXITO: Segundo clic dentro del tiempo
            PauseLayer::onQuit(sender);
        } else {
            // FALLO: Primer clic o demasiado lento
            m_fields->m_lastClick = ahora;

            if (showNotif && !m_fields->m_hasNotificationActive) {
                m_fields->m_hasNotificationActive = true;
                
                Notification::create(customText, NotificationIcon::None, 1.0f)->show();
                
                // Resetear bandera para evitar spam
                this->scheduleOnce(schedule_selector(PauseDoubleClick::resetNotifFlag), 1.0f);
            }
        }
    }

    void resetNotifFlag(float dt) {
        m_fields->m_hasNotificationActive = false;
    }
};
