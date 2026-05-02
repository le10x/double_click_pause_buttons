#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>

using namespace geode::prelude;

class $modify(PauseDoubleClick, PauseLayer) {
    struct Fields {
        std::chrono::steady_clock::time_point m_lastClick;
        bool m_isNotifying = false;
    };

    // 1. Cuando se crea el menú de pausa...
    bool init() {
        if (!PauseLayer::init()) return false;

        // Buscamos el botón de salir entre los nodos
        // El botón de salir en el menú de pausa suele tener este selector originalmente
        auto menu = this->getChildByID("left-button-menu");
        if (menu) {
            if (auto exitBtn = static_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("exit-button"))) {
                // AQUÍ ESTÁ EL TRUCO: Cambiamos la función que se ejecuta al darle clic
                // En lugar de llamar a la de RobTop, llamará a nuestra 'onCustomQuit'
                exitBtn->setTarget(this, menu_selector(PauseDoubleClick::onCustomQuit));
            }
        }
        return true;
    }

    // 2. Nuestra función personalizada para el botón
    void onCustomQuit(CCObject* sender) {
        auto mod = Mod::get();
        bool isPlat = PlayLayer::get() && PlayLayer::get()->m_level->isPlatformer();

        // Si el mod no debe actuar, llamamos a la original de RobTop inmediatamente
        if (!mod->getSettingValue<bool>("enable-double-click") || (mod->getSettingValue<bool>("plat-only") && !isPlat)) {
            PauseLayer::onQuit(sender);
            return;
        }

        auto ahora = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_fields->m_lastClick).count();

        if (diff < mod->getSettingValue<int64_t>("click-speed")) {
            // SEGUNDO CLIC: Ahora sí liberamos la función original de RobTop
            PauseLayer::onQuit(sender);
        } else {
            // PRIMER CLIC: Solo guardamos tiempo y notificamos
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
