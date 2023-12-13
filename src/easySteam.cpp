#include "../include/easySteam.h" 


namespace easySteam {

    uint64_t appID = 0; // Only used for modifying workshop items
    uint64_t itemID = 0;
    steam_helper* _steam_helper = nullptr;
    std::optional<UGCUpdateHandle_t> update_handle;

    void initializeSteamHelper() {
        _steam_helper = new steam_helper;
    }

    void createItem(uint64_t app_id) {
        if (app_id == 0) {
            std::cout << "Please set your app ID.\n";
            return;
        }

        _steam_helper->app_id = app_id;

        _steam_helper->create_workshop_item(
            [&](const PublishedFileId_t new_item_id) {
                std::cout << "Successfully created new workshop item: " << new_item_id << ".\n";
                itemID = new_item_id;
            });

        if (poll_steam_callbacks(*_steam_helper)) {
            std::cout << "Steam callbacks processed successfully.\n";
        } else {
            std::cout << "Error processing Steam callbacks or timed out.\n";
        }
    }

    void initUpdateHandle()
    {
        if (!_steam_helper)
        {
            std::cout << "Steam helper is not initialized.\n";
            return;
        }
        if (easySteam::appID != 0) {
            _steam_helper->app_id = easySteam::appID;
        }

        // Initialize the update handle
        update_handle = _steam_helper->start_workshop_item_update(itemID);

        if (!update_handle.has_value())
        {
            std::cout << "Failed to initialize update handle.\n";
        }
    }

    std::optional<UGCUpdateHandle_t> getUpdateHandle()
    {
        return update_handle;
    }

    void updateItem(uint64_t app_id, uint64_t item_id) {
        _steam_helper->app_id = app_id;

        if (easySteam::update_handle.has_value())
        {
            UGCUpdateHandle_t handle = easySteam::update_handle.value();

            if (!update_handle.has_value()) {
                std::cout << "Failure getting update handle\n";
                return;
            }

            std::cout << "Update handle obtained successfully.\n";
        }
    }

    void setPreviewImage(const std::filesystem::path& file_path) {
        if (easySteam::update_handle.has_value())
        {
            UGCUpdateHandle_t handle = easySteam::update_handle.value();

            if (!_steam_helper->set_workshop_item_preview_image(handle, file_path))
            {
                std::cout << "Failure setting workshop item preview image\n";
            }
        }
    }

    void setWorkshopItemTitle(const std::string& title) {
        if (easySteam::update_handle.has_value())
        {
            UGCUpdateHandle_t handle = easySteam::update_handle.value();
            if (!_steam_helper->set_workshop_item_title(handle, title)) {
                std::cout << "Failure setting workshop item title\n";
            }
        }
    }

    void setWorkshopItemDescription(const std::string& description) {
        if (easySteam::update_handle.has_value())
        {
            UGCUpdateHandle_t handle = easySteam::update_handle.value();
            if (!_steam_helper->set_workshop_item_description(handle, description)) {
                std::cout << "Failure setting workshop item description\n";
            }
        }
    }

    void setWorkshopItemContent(const std::filesystem::path& directory_path) {
        if (easySteam::update_handle.has_value())
        {
            UGCUpdateHandle_t handle = easySteam::update_handle.value();
            if (!_steam_helper->set_workshop_item_content(handle, directory_path)) {
                std::cout << "Failure setting workshop item content\n";
            }
        }
    }

    void submitWorkshopItemUpdate(uint64_t item_id, const std::string& changelog_note) {
        if (easySteam::update_handle.has_value())
        {
            UGCUpdateHandle_t handle = easySteam::update_handle.value();
            _steam_helper->submit_item_update(handle,
                changelog_note.c_str(),
                [item_id] {
                    std::cout << "Successfully updated workshop item: " << item_id << ".\n";
                });
        }
    }
} // namespace easySteam