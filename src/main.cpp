#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>
#include <functional>

using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer) {
    struct Fields {
        int m_clickCount = 0;
        CCObject* m_lastButton = nullptr;
        std::chrono::system_clock::time_point m_lastClickTime;
    };

    void handleSafeClick(CCObject* sender, std::string_view settingKey, std::function<void(CCObject*)> originalFunc) {
        if (!Mod::get()->getSettingValue<bool>(std::string(settingKey))) {
            originalFunc(sender);
            return;
        }

        auto ahora = std::chrono::system_clock::now();
        auto tiempoTranscurrido = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_fields->m_lastClickTime).count();

        if (m_fields->m_lastButton != sender || tiempoTranscurrido > 500) {
            m_fields->m_clickCount = 0;
            this->removeChildByTag(69420);
        }

        m_fields->m_clickCount++;
        m_fields->m_lastButton = sender;
        m_fields->m_lastClickTime = ahora;

        if (m_fields->m_clickCount >= 2) {
            m_fields->m_clickCount = 0;
            originalFunc(sender);
        } else {
            // Verificamos si el usuario quiere ver el mensaje
            if (Mod::get()->getSettingValue<bool>("show-message")) {
                // Obtenemos el texto personalizado de los ajustes
                std::string texto = Mod::get()->getSettingValue<std::string>("custom-text");
                
                auto label = CCLabelBMFont::create(texto.c_str(), "bigFont.fnt");
                auto winSize = CCDirector::get()->getWinSize();
                label->setPosition({winSize.width / 2, winSize.height / 2 - 50});
                label->setScale(0.5f);
                label->setTag(69420);
                
                this->removeChildByTag(69420);
                this->addChild(label);
                
                label->runAction(CCSequence::create(
                    CCFadeIn::create(0.1f),
                    CCDelayTime::create(0.5f),
                    CCFadeOut::create(0.2f),
                    CCRemoveSelf::create(),
                    nullptr
                ));
            }
        }
    }

    // Solo los botones que suelen dar problemas por clics accidentales
    void onQuit(CCObject* s) { handleSafeClick(s, "lock-exit", [this](CCObject* o) { PauseLayer::onQuit(o); }); }
    void onRestart(CCObject* s) { handleSafeClick(s, "lock-reset", [this](CCObject* o) { PauseLayer::onRestart(o); }); }
    void onRestartFull(CCObject* s) { handleSafeClick(s, "lock-reset-plat", [this](CCObject* o) { PauseLayer::onRestartFull(o); }); }
    void onPracticeMode(CCObject* s) { handleSafeClick(s, "lock-practice", [this](CCObject* o) { PauseLayer::onPracticeMode(o); }); }
    void onNormalMode(CCObject* s) { handleSafeClick(s, "lock-exit-practice", [this](CCObject* o) { PauseLayer::onNormalMode(o); }); }
};
