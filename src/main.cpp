#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>

using namespace geode::prelude;

class $modify(PauseDoubleClick, PauseLayer) {
    struct Fields {
        std::chrono::steady_clock::time_point m_lastClick;
        bool m_isNotifying = false;
    };

    bool init(bool unfocused) {
        if (!PauseLayer::init(unfocused)) return false;

        // Buscamos en TODOS los menús del PauseLayer para encontrar el botón de Exit
        // Recorremos los hijos del PauseLayer (this)
        auto children = this->getChildren();
        for (int i = 0; i < children->count(); ++i) {
            if (auto menu = dynamic_cast<CCMenu*>(children->objectAtIndex(i))) {
                // Ahora recorremos los botones dentro de cada menú
                auto buttons = menu->getChildren();
                for (int j = 0; j < buttons->count(); ++j) {
                    if (auto btn = dynamic_cast<CCMenuItemSpriteExtra*>(buttons->objectAtIndex(j))) {
                        
                        // Si el botón está configurado para llamar a onQuit, lo secuestramos
                        // Este es el método más seguro porque no depende de IDs que cambian
                        if (btn->getTarget() == this) { 
                            btn->setTarget(this, menu_selector(PauseDoubleClick::onCustomQuit));
                        }
                    }
                }
            }
        }
        return true;
    }

    void onCustomQuit(CCObject* sender) {
        auto mod = Mod::get();
        
        // Detección de modo plataforma
        bool isPlat = false;
        if (auto pl = PlayLayer::get()) {
            isPlat = pl->m_level->isPlatformer();
        }

        // Lógica de filtrado
        if (!mod->getSettingValue<bool>("enable-double-click") || (mod->getSettingValue<bool>("plat-only") && !isPlat)) {
            PauseLayer::onQuit(sender);
            return;
        }

        auto ahora = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_fields->m_lastClick).count();
        int64_t speedLimit = mod->getSettingValue<int64_t>("click-speed");

        if (diff < speedLimit) {
            // SEGUNDO CLIC: Llamamos a la original (aquí saldrá la confirmación nativa)
            PauseLayer::onQuit(sender);
        } else {
            // PRIMER CLIC
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
