#include <Geode/Geode.hpp>
#include <string>
#include <vector>
#include <map>
#include <Geode/modify/ProfilePage.hpp>

using namespace geode::prelude;

namespace CGlobal {
    std::map<std::string, std::vector<std::string>> members = {
        {"listmoderator", {"YeahhColix"}}
    };


    template <typename T>
    bool objectExists(const T& target, const std::vector<T>& vec) {
        return std::find(vec.begin(), vec.end(), target) != vec.end();
    }

    std::string lowercaseString(std::string& _str) {
        std::string str = _str;

        for (int i = 0; i < str.length(); i++) {
            str[i] = std::tolower(str[i]);
        }

        return str;
    }

    class BadgeObject {
    public:
        CCSprite* m_sprite = nullptr;
        std::string m_role = "";
        std::string m_owner = "";

        void postSetup() {
            if (m_sprite == nullptr) return;
                
            m_sprite->setAnchorPoint({ 0.5, 0.6 });
        }
    };

    BadgeObject* setupSpriteFromUsername(std::string _username) {
        std::string username = lowercaseString(_username);
        
        bool isFound = false;
        std::string role_name;

        for (auto& [k, v] : members) {
            for (std::string& _u : v) {
                std::string u = lowercaseString(_u);

                if (u == username) {
                    isFound = true;
                    role_name = k;
                    
                    break;
                }
            }
            
            if (isFound) {
                break;
            }
        }

        if (!isFound) return nullptr;

        static std::map<std::string, std::string> spriteMap = {
            {"booster", "Booster.png"_spr},
            {"partner", "Partner.png"_spr},
            {"stargrinder", "SG.png"_spr},
            {"demonslayer", "DS.png"_spr},
            {"listmoderator", "ListModerator.png"_spr}
        };

        if (spriteMap.count(role_name) == 0) {
            log::error("role cannot be found for user {}: unknown role \"{}\"", _username, role_name);

            return nullptr;
        }

        std::string sprite = spriteMap[role_name];

        CCSprite* sprObj = CCSprite::create(sprite.c_str());
        if (sprObj == nullptr) {
            log::error("badge \"{}\" (linked with role \"{}\") cannot be found", sprite, role_name);

            return nullptr;
        }

        BadgeObject* bobj = new BadgeObject();
        bobj->m_owner = _username;
        bobj->m_role = role_name;
        bobj->m_sprite = sprObj;

        return bobj;
    }

    std::string currentUsername;
    BadgeObject currentBadge;
}

class $modify(XProfilePage, ProfilePage) {
    std::string getPopupTitle() {
        return "Badge Info";
    }

    void onCherryMember(CCObject *sender) {
        std::string popupTitle = getPopupTitle();

        if (CGlobal::currentUsername == "cherryteam") {
            FLAlertLayer::create(popupTitle.c_str(), "This <cj>account</c> is used for <cy>uploading all of the collabs</c> made by cherry team.", "OK")->show();

            return;
        }

        std::string str = fmt::format("{} is the <cy>member</c> of <cr>cherry team</c>.", CGlobal::currentUsername);

        std::string role = CGlobal::currentBadge.m_role;

        if (role == "leader") {
            return onCherryLeader(sender);
        }

        std::map<std::string, std::string> roleMap1 = {
            {"tech", "technician or programmer"},
            {"creator", "decorator, layout maker or part joiner"},
            {"artist", "artist"},
            {"slayer", "demon slayer"},
            {"musician", "musician"},
            {"grinder", "star grinder"},
            {"media", "active in cherry team's social media"},
            {"listmoderator", "hi"},
            {"record", "recording showcases for cherry team members"}
        };
        static std::map<std::string, std::string> roleMap2 = {
            {"tech", "<cj>makes mods</c> and/or <cj>software</c> for cherry team. Sometimes player with this role also <cl>know how to work with the triggers</c>"},
            {"creator", "<cj>is working on levels made by cherry team</c>. This player also <cp>can know the editor very well</c> or.. <cb>just work on level layouts or deco</c>"},
            {"artist", "is working on <cy>arts</c> for cherry team levels"},
            {"musician", "<cj>makes music</c> for cherry team levels"},
            {"media", "<cj>runs official cherry team accounts and/or channels</c> in <cy>GD</c> or <cy>Telegram</c> for example"}
        };

        if (roleMap1.count(role)) {
            str += fmt::format(" He is <cy>{}</c>.", roleMap1[role]);
        }

        if (roleMap2.count(role)) {
            str += fmt::format(" This player {}.", roleMap2[role]);
        }

        FLAlertLayer::create(popupTitle.c_str(), str, "OK")->show();
    }
    void onCherryLeader(CCObject* sender) {
        std::string title = getPopupTitle();
        std::string str = fmt::format("{} is the <cp>leader</c> of <cr>cherry team</c>.", CGlobal::currentUsername);

        FLAlertLayer::create(title.c_str(), str, "OK")->show();
    }

    void loadPageFromUserInfo(GJUserScore * a2) {
        ProfilePage::loadPageFromUserInfo(a2);
        auto layer = m_mainLayer;

        CCMenu* username_menu = typeinfo_cast<CCMenu*>(layer->getChildByIDRecursive("username-menu"));

        bool isMain = false;
        bool isLeader = false;

        std::string username = a2->m_userName;
        CGlobal::currentUsername = username;
        
        auto badge = CGlobal::setupSpriteFromUsername(username);
        if (!badge) {
            return;
        }

        CCMenuItemSpriteExtra* badgeBtn = CCMenuItemSpriteExtra::create(badge->m_sprite, this, menu_selector(XProfilePage::onCherryMember));
        badgeBtn->setID("cherry-member-badge");

        username_menu->addChild(badgeBtn);

        username_menu->updateLayout();
        updateLayout();

        badge->postSetup();

        CGlobal::currentBadge = *badge;

        delete badge;
    }
};
