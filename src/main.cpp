#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
using namespace geode::prelude;

CCMenu* st2BG() {
	auto winSize = CCDirector::sharedDirector()->getWinSize();

	auto menu = CCMenu::create();
	menu->setContentSize(winSize);
	menu->setPosition(0, 0);
	menu->setAnchorPoint({0, 0});
	menu->setID("st2-background");

	auto background = CCSprite::create("background.png"_spr);
	background->setID("background");
	background->setPosition(winSize / 2);
	auto bgScale = std::max(
		winSize.height / background->getContentHeight(),
		winSize.width / background->getContentWidth()
	);
	background->setScale(bgScale);

	auto bgActions = CCArray::create();
	bgActions->addObject(CCScaleTo::create(20, bgScale*1.1));
	bgActions->addObject(CCScaleTo::create(20, bgScale));
	background->runAction(CCRepeatForever::create(CCSequence::create(bgActions)));

	menu->addChild(background, 0);


	auto tree = CCSprite::create("tree.png"_spr);
	tree->setID("tree");
	tree->setPosition(winSize / 2);
	auto treeScale = std::max(
		winSize.height / tree->getContentHeight(),
		winSize.width / tree->getContentWidth()
	);
	tree->setScale(treeScale);

	auto treeActions = CCArray::create();
	treeActions->addObject(CCScaleTo::create(20, treeScale*1.2));
	treeActions->addObject(CCScaleTo::create(20, treeScale));
	tree->runAction(CCRepeatForever::create(CCSequence::create(treeActions)));

	menu->addChild(tree, 1);


	if (Mod::get()->getSettingValue<bool>("particles")) {
		auto dict = CCDictionary::createWithContentsOfFileThreadSafe("dragEffect.plist");
		dict->setObject(CCString::create("0"), "emitterType");

		dict->setObject(CCString::create("150"), "maxParticles");
		dict->setObject(CCString::create("-1"), "duration");
		dict->setObject(CCString::create("2"), "particleLifespan");
		dict->setObject(CCString::create("0"), "particleLifespanVariance");
		// no emission :(
		dict->setObject(CCString::create("-90"), "angle");
		dict->setObject(CCString::create("90"), "angleVariance");
		dict->setObject(CCString::create("29"), "speed");
		dict->setObject(CCString::create("0"), "speedVariance");
		dict->setObject(CCString::create("0"), "sourcePositionx");
		dict->setObject(CCString::create("0"), "sourcePositiony");
		dict->setObject(CCString::createWithFormat("%f", winSize.width), "sourcePositionVariancex");
		dict->setObject(CCString::create("0"), "sourcePositionVariancey");
		dict->setObject(CCString::create("0"), "gravityx");
		dict->setObject(CCString::createWithFormat("%f", winSize.height/1.5f), "gravityy");
		dict->setObject(CCString::create("0"), "radialAcceleration");
		dict->setObject(CCString::create("0"), "radialAccelVariance");
		dict->setObject(CCString::create("0"), "tangentialAcceleration");
		dict->setObject(CCString::create("14"), "tangentialAccelVariance");

		dict->setObject(CCString::create("4"), "startParticleSize");
		dict->setObject(CCString::create("3"), "startParticleSizeVariance");
		dict->setObject(CCString::create("2"), "endParticleSize");
		dict->setObject(CCString::create("2"), "endParticleSizeVariance");
		dict->setObject(CCString::create("0"), "rotationStart");
		dict->setObject(CCString::create("0"), "rotationStartVariance");
		dict->setObject(CCString::create("0"), "rotationEnd");
		dict->setObject(CCString::create("0"), "rotationEndVariance");
		dict->setObject(CCString::create("1"), "startColorRed");
		dict->setObject(CCString::create("0"), "startColorVarianceRed");
		dict->setObject(CCString::create("1"), "startColorGreen");
		dict->setObject(CCString::create("0"), "startColorVarianceGreen");
		dict->setObject(CCString::create("1"), "startColorBlue");
		dict->setObject(CCString::create("0"), "startColorVarianceBlue");
		dict->setObject(CCString::create("1"), "startColorAlpha");
		dict->setObject(CCString::create("0.25"), "startColorVarianceAlpha");
		dict->setObject(CCString::create("1"), "finishColorRed");
		dict->setObject(CCString::create("0"), "finishColorVarianceRed");
		dict->setObject(CCString::create("1"), "finishColorGreen");
		dict->setObject(CCString::create("0"), "finishColorVarianceGreen");
		dict->setObject(CCString::create("1"), "finishColorBlue");
		dict->setObject(CCString::create("0"), "finishColorVarianceBlue");
		dict->setObject(CCString::create("1"), "finishColorAlpha");
		dict->setObject(CCString::create("0"), "finishColorVarianceAlpha");

		dict->setObject(CCString::create("particle_25_001.png"), "textureFileName");
		
		auto emitter = CCParticleSystemQuad::create();
		emitter->initWithDictionary(dict, false);

		emitter->setPosition(winSize / 2);
		emitter->setPositionY(0);
		emitter->setID("emitter");
		menu->addChild(emitter, 2);
	}

	return menu;
}

void replaceBG(CCLayer* layer) {
	int i = 0;
	bool done = false;
	while (i < layer->getChildrenCount()) {
		auto node = getChildOfType<CCSprite>(layer, i);

		if (auto textureProtocol = typeinfo_cast<CCTextureProtocol*>(node)) {
			if (auto texture = textureProtocol->getTexture()) {
				auto* cachedTextures = CCTextureCache::sharedTextureCache()->m_pTextures;
				for (auto [key, obj] : CCDictionaryExt<std::string, CCTexture2D*>(cachedTextures)) {
					if (obj == texture) {
						if (Loader::get()->isModLoaded("zalphalaneous.minecraft")) {
							if (typeinfo_cast<CreatorLayer*>(layer)) {
								done = true;
								break;
							}
						}
						if (key.ends_with("GJ_gradientBG.png") || key.ends_with("GJ_gradientBG-hd.png")) {
							node->setVisible(false);
                            layer->addChild(st2BG(), node->getZOrder()-1);

							CCArrayExt<CCNode*> children = layer->getChildren();
                            for (auto child : children) {
                                if (auto v = typeinfo_cast<CCScale9Sprite*>(child)) {
                                    v->setOpacity(100);
                                    v->setColor(ccc3(0, 0, 0));

                                    child->setScale(child->getScale() * 0.5f);
                                    child->setContentSize(child->getContentSize() / 0.5f);
                                } else if (auto v = typeinfo_cast<CCTextInputNode*>(child)) {
                                    v->setLabelPlaceholderColor(ccc3(255, 255, 255));
                                }
                            }

                            if (typeinfo_cast<LevelSelectLayer*>(layer))
                                getChildOfType<GJGroundLayer>(layer, 0)->setVisible(false);

							done = true;
						    break;
                        }
					}
				}
				if (done) break;
			}
		}

		i++;
	}
}

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        getChildOfType<MenuGameLayer>(this, 0)->setVisible(false);
		this->addChild(st2BG(), -1);
        
        return true;
    }
};

class $modify(CCDirector) {
	void willSwitchToScene(CCScene* scene) {
		CCDirector::willSwitchToScene(scene);

		if (auto layer = getChildOfType<CCLayer>(scene, 0)) { 
			if (!Mod::get()->getSettingValue<bool>("only_main")) {
				if (!typeinfo_cast<SecretLayer*>(layer)
				&& !typeinfo_cast<SecretLayer2*>(layer)
				&& !typeinfo_cast<SecretLayer3*>(layer)
				&& !typeinfo_cast<SecretLayer4*>(layer)
				&& !typeinfo_cast<SecretRewardsLayer*>(layer)
				&& !typeinfo_cast<GauntletSelectLayer*>(layer)
				) replaceBG(layer);
			}
		}
	}
};

class $modify(LevelInfoLayer) {
    void onPlay(CCObject* sender) {
        LevelInfoLayer::onPlay(sender);

        if (Mod::get()->getSettingValue<bool>("only_main")) {
            auto sprite = this->getChildByID("play-menu")->getChildByID("play-button")->getChildByTag(1);
            CCArrayExt<CCNodeRGBA*> children = sprite->getChildren();
            for (auto* c : children) {
                if (!typeinfo_cast<CCProgressTimer*>(c)) {
                    if (c->getZOrder() == -4) {
                        c->setVisible(false);
                    } else {
                        c->setOpacity(100);
                        c->setColor(ccc3(0, 0, 0));
                        c->setCascadeOpacityEnabled(false);
                    }
                }
            }
        }
    }
};