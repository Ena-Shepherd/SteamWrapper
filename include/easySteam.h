#pragma once

#include <iostream>
#include <filesystem>
#include <optional>

#include "../include/steamHelper.h"

[[nodiscard]] bool poll_steam_callbacks(steam_helper& _steam_helper) noexcept;

namespace easySteam {

    extern uint64_t appID;
    extern uint64_t itemID;
    extern std::unique_ptr<steam_helper> _steam_helper;
    extern bool initUpdateHandleCalled;
    std::optional<UGCUpdateHandle_t> getUpdateHandle();

    typedef struct {
        std::string title;
        std::string description;
        std::string url;
    } WorkshopItem_t;

    void initializeSteamHelper();
    void createItem(uint64_t app_id);
    
    void createQuery(   AccountID_t accountID, EUserUGCList listType,
                        EUGCMatchingUGCType matchingType, EUserUGCListSortOrder sortOrder,
                        AppId_t creatorAppID, AppId_t consumerAppID, uint32_t page
    );
    void createQuery(   EUGCQuery listType,
                        EUGCMatchingUGCType matchingType,
                        AppId_t creatorAppID, AppId_t consumerAppID, uint32_t page
    );

    bool setSearchText(const char* searchText);
    
    // Also processes the results and cleaning
    std::vector<WorkshopItem_t> sendQuery();

    void updateItem(uint64_t app_id, uint64_t item_id);
    void initUpdateHandle();
    void setPreviewImage(const std::filesystem::path& file_path);
    void setWorkshopItemTitle(const std::string& title);
    void setWorkshopItemDescription(const std::string& description);
    void setWorkshopItemContent(const std::filesystem::path& directory_path);
    void submitWorkshopItemUpdate(uint64_t item_id, const std::string& changelog_note);
    void getWorkshopItemUploadProgress(long* remaining, long* totalSize);
    void unsubscribeWorkshopItem(uint64_t item_id);
} // namespace easySteam