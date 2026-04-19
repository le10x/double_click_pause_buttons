#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>
#include <algorithm>

using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer) {
    struct Fields {
        int m_clickCount = 0;
        CCObject* m_lastButton = nullptr;
        std::chrono::system_clock::time_point m_lastClickTime;
    };

    bool init(bool unfocused) {
        if (!PauseLayer::init(unfocused)) return false;

        auto winSize = CCDirector::get()->getWinSize();
        int64_t posMode = Mod::get()->getSettingValue<int64_t>("settings-btn-pos");

        if (posMode != 0) { // 0 es desactivado
            auto sprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
            sprite->setScale(0.7f);
            
            auto btn = CCMenuItemSpriteExtra::create(
                sprite, this, menu_selector(MyPauseLayer::onOpenMySettings)
            );

            auto menu = CCMenu::create();
            if (posMode == 1) { // Izquierda
                menu->setPosition({30, 30});
            } else { // Derecha
                menu->setPosition({winSize.width - 30, 30});
            }

            menu->addChild(btn);
            menu->setID("settings-shortcut-menu");
            this->addChild(menu);
        }

        return true;
    }

    void onOpenMySettings(CCObject*) {
        geode::openSettings(Mod::get());
    }

    void handleSafeClick(CCObject* sender, std::string_view settingKey, std::function<void(CCObject*)> originalFunc) {
        if (!Mod::get()->getSettingValue<bool>(std::string(settingKey))) {
            originalFunc(sender);
            return;
        }

        auto playLayer = PlayLayer::get();
        if (playLayer && !playLayer->m_isPlatformer) {
            int minP = std::clamp(static_cast<int>(Mod::get()->getSettingValue<int64_t>("min-percent")), 0, 100);
            if (static_cast<int>(playLayer->getCurrentPercent()) < minP) {
                originalFunc(sender);
                return;
            }
        }

        auto ahora = std::chrono::system_clock::now();
        // Bloqueo de seguridad: máximo 1000ms
        int speedLimit = std::clamp(static_cast<int>(Mod::get()->getSettingValue<int64_t>("click-speed")), 100, 1000);
        auto tiempo = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_fields->m_lastClickTime).count();

        if (m_fields->m_lastButton != sender || tiempo > speedLimit) {
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
                int opc = std::clamp(static_cast<int>(Mod::get()->getSettingValue<int64_t>("message-opacity")), 0, 100);
                
                auto label = CCLabelBMFont::create(texto.c_str(), gold ? "goldFont.fnt" : "bigFont.fnt");
                label->setPosition({CCDirector::get()->getWinSize().width / 2, CCDirector::get()->getWinSize().height / 2 - 60});
                label->setScale(0.5f);
                label->setTag(69420);
                label->setOpacity(static_cast<GLubyte>((opc * 255) / 100));
                
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
    void onPracticeMode(CCObject* s) { handleSafeClick(s, "lock-practice-all", [this](CCObject* o) { PauseLayer::onPracticeMode(o); }); }
    void onNormalMode(CCObject* s) { handleSafeClick(s, "lock-practice-all", [this](CCObject* o) { PauseLayer::onNormalMode(o); }); }
};
