#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>

using namespace geode::prelude;

class $modify(PauseDoubleClick, PauseLayer) {
    struct Fields {
        // Tiempos independientes para cada grupo de acciones
        std::chrono::steady_clock::time_point m_lastExit;
        std::chrono::steady_clock::time_point m_lastRestart;
        std::chrono::steady_clock::time_point m_lastPractice;
        // Puntero para gestionar la notificacion activa
        Notification* m_currentNotif = nullptr;
    };

    // Logica central para validar el doble click
    bool checkClick(std::chrono::steady_clock::time_point& lastTime) {
        auto mod = Mod::get();
        
        bool isPlat = false;
        if (auto pl = PlayLayer::get()) {
            if (pl->m_level) isPlat = pl->m_level->isPlatformer();
        }

        // Filtros de activacion segun ajustes
        if (!mod->getSettingValue<bool>("enable-double-click") || (mod->getSettingValue<bool>("plat-only") && !isPlat)) {
            return true;
        }

        auto ahora = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - lastTime).count();
        lastTime = ahora;

        if (diff < mod->getSettingValue<int64_t>("click-speed")) {
            return true; // Doble click exitoso
        } else {
            // Primer click: Manejo de notificacion sin spam
            if (mod->getSettingValue<bool>("show-notification")) {
                if (m_fields->m_currentNotif) {
                    m_fields->m_currentNotif->onClose(nullptr);
                }
                
                m_fields->m_currentNotif = Notification::create(
                    mod->getSettingValue<std::string>("custom-text"), 
                    NotificationIcon::None, 
                    0.8f
                );
                m_fields->m_currentNotif->show();
            }
            return false;
        }
    }

    // Intercepcion de botones
    void onQuit(CCObject* s) {
        if (checkClick(m_fields->m_lastExit)) PauseLayer::onQuit(s);
    }

    void onRestart(CCObject* s) {
        if (checkClick(m_fields->m_lastRestart)) PauseLayer::onRestart(s);
    }

    void onRestartFull(CCObject* s) {
        if (checkClick(m_fields->m_lastRestart)) PauseLayer::onRestartFull(s);
    }

    void onPracticeMode(CCObject* s) {
        if (checkClick(m_fields->m_lastPractice)) PauseLayer::onPracticeMode(s);
    }

    void onNormalMode(CCObject* s) {
        if (checkClick(m_fields->m_lastPractice)) PauseLayer::onNormalMode(s);
    }
};
