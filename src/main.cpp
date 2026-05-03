#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>

using namespace geode::prelude;

class $modify(PauseDoubleClick, PauseLayer) {
    struct Fields {
        std::chrono::steady_clock::time_point m_lastExit;
        std::chrono::steady_clock::time_point m_lastRestart;
        std::chrono::steady_clock::time_point m_lastPractice;
        bool m_isNotifying = false;
    };

    bool checkClick(std::chrono::steady_clock::time_point& lastTime) {
        auto mod = Mod::get();
        bool isPlat = false;
        if (auto pl = PlayLayer::get()) {
            if (pl->m_level) isPlat = pl->m_level->isPlatformer();
        }

        if (!mod->getSettingValue<bool>("enable-double-click") || (mod->getSettingValue<bool>("plat-only") && !isPlat)) {
            return true;
        }

        auto ahora = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - lastTime).count();
        lastTime = ahora;

        if (diff < mod->getSettingValue<int64_t>("click-speed")) {
            return true;
        } else {
            if (mod->getSettingValue<bool>("show-notification") && !m_fields->m_isNotifying) {
                m_fields->m_isNotifying = true;
                Notification::create(mod->getSettingValue<std::string>("custom-text"), NotificationIcon::None, 0.8f)->show();
                this->scheduleOnce(schedule_selector(PauseDoubleClick::resetNotif), 1.0f);
            }
            return false;
        }
    }

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

    void resetNotif(float dt) { m_fields->m_isNotifying = false; }
};
