#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/SettingV3.hpp> // Para detectar cambios en ajustes
#include <chrono>

using namespace geode::prelude;

// --- Lógica de Vista Previa al cambiar ajustes ---
class $modify(MySettings, SettingV3) {
    void dispatchChanged() {
        SettingV3::dispatchChanged();
        
        // Si el usuario está en el menú de pausa y cambia algo, mostramos preview
        if (auto pauseLayer = CCScene::get()->getChildByType<PauseLayer>(0)) {
            auto label = CCLabelBMFont::create("Preview Updated!", "bigFont.fnt");
            label->setPosition(CCDirector::get()->getWinSize() / 2);
            label->setScale(0.5f);
            pauseLayer->addChild(label);
            label->runAction(CCSequence::create(CCFadeOut::create(1.0f), CCRemoveSelf::create(), nullptr));
        }
    }
};

class $modify(MyPauseLayer, PauseLayer) {
    struct Fields {
        int m_clickCount = 0;
        CCObject* m_lastButton = nullptr;
        std::chrono::system_clock::time_point m_lastClickTime;
    };

    bool init(bool unfocused) {
        if (!PauseLayer::init(unfocused)) return false;
        return true;
    }

    void handleSafeClick(CCObject* sender, std::string_view settingKey, std::function<void(CCObject*)> originalFunc) {
        if (!Mod::get()->getSettingValue<bool>(std::string(settingKey))) {
            originalFunc(sender);
            return;
        }

        auto playLayer = PlayLayer::get();
        if (playLayer && !playLayer->m_isPlatformer) {
            auto minPercent = Mod::get()->getSettingValue<int64_t>("min-percent");
            if (static_cast<int>(playLayer->getCurrentPercent()) < static_cast<int>(minPercent)) {
                originalFunc(sender);
                return;
            }
        }

        auto ahora = std::chrono::system_clock::now();
        auto speedLimit = Mod::get()->getSettingValue<int64_t>("click-speed");
        auto tiempoTranscurrido = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_fields->m_lastClickTime).count();

        if (m_fields->m_lastButton != sender || tiempoTranscurrido > speedLimit) {
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
            if (Mod::get()->getSettingValue<bool>("show-message")) {
                std::string texto = Mod::get()->getSettingValue<std::string>("custom-text");
                bool gold = Mod::get()->getSettingValue<bool>("use-gold-font");
                
                auto label = CCLabelBMFont::create(texto.c_str(), gold ? "goldFont.fnt" : "bigFont.fnt");
                auto winSize = CCDirector::get()->getWinSize();
                label->setPosition({winSize.width / 2, winSize.height / 2 - 60});
                label->setScale(0.5f);
                label->setTag(69420);
                
                this->removeChildByTag(69420);
                this->addChild(label);
                
                label->runAction(CCSequence::create(
                    CCEaseExponentialOut::create(CCScaleTo::create(0.2f, 0.55f)),
                    CCDelayTime::create(0.4f),
                    CCFadeOut::create(0.2f),
                    CCRemoveSelf::create(),
                    nullptr
                ));
            }
        }
    }

    void onQuit(CCObject* s) { handleSafeClick(s, "lock-exit", [this](CCObject* o) { PauseLayer::onQuit(o); }); }
    void onRestart(CCObject* s) { handleSafeClick(s, "lock-reset", [this](CCObject* o) { PauseLayer::onRestart(o); }); }
    void onRestartFull(CCObject* s) { handleSafeClick(s, "lock-reset-plat", [this](CCObject* o) { PauseLayer::onRestartFull(o); }); }
    
    // Unificados: Ambos usan "lock-practice-all"
    void onPracticeMode(CCObject* s) { handleSafeClick(s, "lock-practice-all", [this](CCObject* o) { PauseLayer::onPracticeMode(o); }); }
    void onNormalMode(CCObject* s) { handleSafeClick(s, "lock-practice-all", [this](CCObject* o) { PauseLayer::onNormalMode(o); }); }
};
