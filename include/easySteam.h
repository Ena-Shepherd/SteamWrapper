#pragma once

#include <iostream>
#include <filesystem>
#include <optional>

#include "../include/steamHelper.h"

[[nodiscard]] bool poll_steam_callbacks(steam_helper& _steam_helper) noexcept;

namespace easySteam {

    extern uint64_t appID; // Only used for modifying workshop items
    extern uint64_t itemID;
    extern std::unique_ptr<steam_helper> _steam_helper;
    extern bool initUpdateHandleCalled;
    std::optional<UGCUpdateHandle_t> getUpdateHandle();

    void initializeSteamHelper();
    void createItem(uint64_t app_id);
    void updateItem(uint64_t app_id, uint64_t item_id);
    void initUpdateHandle();
    void setPreviewImage(const std::filesystem::path& file_path);
    void setWorkshopItemTitle(const std::string& title);
    void setWorkshopItemDescription(const std::string& description);
    void setWorkshopItemContent(const std::filesystem::path& directory_path);
    void submitWorkshopItemUpdate(uint64_t item_id, const std::string& changelog_note);
} // namespace easySteam