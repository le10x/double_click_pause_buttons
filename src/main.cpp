#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono> // Necesario para medir el tiempo

using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer) {
    struct Fields {
        int m_clickCount = 0;
        // Guardamos el momento exacto del último clic
        std::chrono::system_clock::time_point m_lastClickTime;
    };

    void onResume(CCObject* sender) {
        auto ahora = std::chrono::system_clock::now();
        
        // Calculamos cuánto tiempo pasó desde el último clic (en milisegundos)
        auto tiempoTranscurrido = std::chrono::duration_cast<std::chrono::milliseconds>(
            ahora - m_fields->m_lastClickTime
        ).count();

        // Si pasó más de 500ms (medio segundo), reiniciamos el contador
        if (tiempoTranscurrido > 500) {
            m_fields->m_clickCount = 0;
        }

        m_fields->m_clickCount++;
        m_fields->m_lastClickTime = ahora; // Actualizamos el tiempo del último clic

        if (m_fields->m_clickCount >= 2) {
            PauseLayer::onResume(sender);
        } else {
            // Feedback visual de que necesita otro clic rápido
            auto notification = Notification::create("¡Rápido! Haz clic otra vez", NotificationIcon::None, 0.5f);
            notification->show();
        }
    }
};
