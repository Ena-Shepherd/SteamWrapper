#include "../include/easySteam.h" 

namespace easySteam {

    uint64_t appID = 0;
    uint64_t itemID = 0;
    std::unique_ptr<steam_helper> _steam_helper = nullptr;
    std::optional<UGCUpdateHandle_t> update_handle;
    UGCQueryHandle_t queryHandle;
    bool initUpdateHandleCalled = false;

    void initializeSteamHelper() {
        _steam_helper.reset(new steam_helper);
    }

    bool setCloudFilenameFilter(const char* cloudFileName) {
        if (!_steam_helper) {
            std::cout << "Error: _steam_helper is not initialized.\n";
            return false;
        }
        if(queryHandle == 0) {
            std::cout << "Please create a query first.\n";
            return false;
        }

        return _steam_helper->set_cloud_filename_filter(queryHandle, cloudFileName);
    }

    /// @brief Enforce or not if all specified tags must be matching.
    ///
    /// You have to add at least one tag to your request along this function
    /// @param matchAnyTag
    bool setMatchAnyTag(const bool matchAnyTag) {
        if (!_steam_helper) {
            std::cout << "Error: _steam_helper is not initialized.\n";
            return false;
        }
        if(queryHandle == 0) {
            std::cout << "Please create a query first.\n";
            return false;
        }

        return _steam_helper->set_match_anytag(queryHandle, matchAnyTag);
    }

    bool setSearchText(const char* searchText) {
        if (!_steam_helper) {
            std::cout << "Error: _steam_helper is not initialized.\n";
            return false;
        }
        if(queryHandle == 0) {
            std::cout << "Please create a query first.\n";
            return false;
        }

        return _steam_helper->set_search_text(queryHandle, searchText);
    }

    bool setRankedByTrendDays(const uint32 nbDays) {
        if (!_steam_helper) {
            std::cout << "Error: _steam_helper is not initialized.\n";
            return false;
        }
        if(queryHandle == 0) {
            std::cout << "Please create a query first.\n";
            return false;
        }

        return _steam_helper->set_ranked_by_trend_days(queryHandle, nbDays);
    }
    
    bool addRequiredTag(const char* tagName) {
        if (!_steam_helper) {
            std::cout << "Error: _steam_helper is not initialized.\n";
            return false;
        }
        if(queryHandle == 0) {
            std::cout << "Please create a query first.\n";
            return false;
        }

        return _steam_helper->add_required_tag(queryHandle, tagName);
    }

    bool addExcludedTag(const char* tagName) {
        if (!_steam_helper) {
            std::cout << "Error: _steam_helper is not initialized.\n";
            return false;
        }
        if(queryHandle == 0) {
            std::cout << "Please create a query first.\n";
            return false;
        }

        return _steam_helper->add_excluded_tag(queryHandle, tagName);
    }

    bool returnLongDescription(const bool enabled) {
        if (!_steam_helper) {
            std::cout << "Error: _steam_helper is not initialized.\n";
            return false;
        }
        if(queryHandle == 0) {
            std::cout << "Please create a query first.\n";
            return false;
        }

        return _steam_helper->return_long_description(queryHandle, enabled);
    }

    bool returnTotalOnly(const bool enabled) {
        if (!_steam_helper) {
            std::cout << "Error: _steam_helper is not initialized.\n";
            return false;
        }
        if(queryHandle == 0) {
            std::cout << "Please create a query first.\n";
            return false;
        }

        return _steam_helper->return_total_only(queryHandle, enabled);
    }

    bool allowCachedResponse(const uint32 maxAgeSeconds) {
        if (!_steam_helper) {
            std::cout << "Error: _steam_helper is not initialized.\n";
            return false;
        }
        if(queryHandle == 0) {
            std::cout << "Please create a query first.\n";
            return false;
        }

        return _steam_helper->allow_cached_response(queryHandle, maxAgeSeconds);
    }

    /// @brief "User" query creation function.
    /// @param accountID
    /// @param listType
    /// @param matchingType
    /// @param sortOrder
    /// @param creatorAppID
    /// @param consumerAppID
    /// @param page
    void createQuery(AccountID_t accountID, EUserUGCList listType, EUGCMatchingUGCType matchingType, EUserUGCListSortOrder sortOrder, AppId_t creatorAppID, AppId_t consumerAppID, uint32_t page) {
        if (!_steam_helper) {
            std::cout << "Error: _steam_helper is not initialized.\n";
            return;
        }
        if (creatorAppID == 0 || consumerAppID == 0) {
            std::cout << "Please set your app ID.\n";
            return;
        }

        _steam_helper->create_user_query(queryHandle, accountID, listType, matchingType, sortOrder, creatorAppID, consumerAppID, page);
    }
    
    /// @brief "All" query creation function.
    /// @param listType 
    /// @param matchingType 
    /// @param creatorAppID 
    /// @param consumerAppID 
    /// @param page 
    void createQuery(EUGCQuery listType, EUGCMatchingUGCType matchingType, AppId_t creatorAppID, AppId_t consumerAppID, uint32_t page) {
        if (!_steam_helper) {
            std::cout << "Error: _steam_helper is not initialized.\n";
            return;
        }
        if (creatorAppID == 0 || consumerAppID == 0) {
            std::cout << "Please set your app ID.\n";
            return;
        }

        _steam_helper->create_all_query(queryHandle, listType, matchingType, creatorAppID, consumerAppID, page);
    }

    void parseQueryResults(std::vector<std::string>& itemList, std::vector<WorkshopItem_t>& workshopItems) {
        
        int it = 1;

        for (const auto& itemInfo : itemList) {
            // Parse the item info after each third index
            if (it % 3 == 0) {
                
                WorkshopItem_t item;
                
                item.title = itemList[it - 3];
                item.description = itemList[it - 2];
                item.url = itemList[it - 1];

                workshopItems.push_back(item);
            }

            it++;
        }
    }

    /// @brief Sends the query to the Steam API and returns found items information.
    /// @note This function should be called after creating a query.
    void sendQuery(std::vector<SteamUGCDetails_t> &itemListDetails, std::vector<char*> &imageListURL) {

        if (!_steam_helper) {
            std::cout << "Error: _steam_helper is not initialized.\n";
        }
        if(queryHandle == 0) {
            std::cout << "Please create a query first.\n";
        }

        _steam_helper->send_query_request(queryHandle,
            [&](UGCQueryHandle_t query_handle) {
                std::cout << "Query went successfully.\n";
            });

        if (poll_steam_callbacks(*_steam_helper)) {
            std::cout << "Steam callbacks processed successfully.\n";
        } else {
            std::cout << "Error processing Steam callbacks or timed out.\n";
        }

        _steam_helper->get_query_results(itemListDetails, imageListURL);

        _steam_helper->release_query_handle(queryHandle);

        // if (workshopItems.empty()) {
        //     std::cout << "No items found.\n";
        //     return {};
        // }
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
        if (initUpdateHandleCalled)
        {
            std::cout << "Error : you should call initUpdateHandle only once, before changing your item content.\n";
            return;
        }

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

        if (!easySteam::update_handle.has_value())
        {
            std::cout << "Error : you should call initUpdateHandle before getting the upload progress.\n";
            return;
        }

        if (easySteam::update_handle.value() != item_id)
        {
            std::cout << "Error : you should call initUpdateHandle with the correct item_id.\n";
            return;
        }

        if (!update_handle.has_value()) {
            std::cout << "Failure getting update handle\n";
            return;
        }

        std::cout << "Update handle obtained successfully.\n";
    }

    void setPreviewImage(const std::filesystem::path& file_path) {
        if (!easySteam::update_handle.has_value())
        {
            std::cout << "Error : you should call initUpdateHandle before getting the upload progress.\n";
            return;
        }

        UGCUpdateHandle_t handle = easySteam::update_handle.value();

        if (!_steam_helper->set_workshop_item_preview_image(handle, file_path))
        {
            std::cout << "Failure setting workshop item preview image\n";
        }
    }

    void setWorkshopItemTitle(const std::string& title) {
        if (!easySteam::update_handle.has_value())
        {
            std::cout << "Error : you should call initUpdateHandle before getting the upload progress.\n";
            return;
        }

        UGCUpdateHandle_t handle = easySteam::update_handle.value();
        if (!_steam_helper->set_workshop_item_title(handle, title)) {
            std::cout << "Failure setting workshop item title\n";
        }
    }

    void setWorkshopItemDescription(const std::string& description) {
        if (!easySteam::update_handle.has_value())
        {
            std::cout << "Error : you should call initUpdateHandle before getting the upload progress.\n";
            return;
        }

        UGCUpdateHandle_t handle = easySteam::update_handle.value();
        if (!_steam_helper->set_workshop_item_description(handle, description)) {
            std::cout << "Failure setting workshop item description\n";
        }
    }

    void setWorkshopItemContent(const std::filesystem::path& directory_path) {
        if (!easySteam::update_handle.has_value())
        {
            std::cout << "Error : you should call initUpdateHandle before getting the upload progress.\n";
            return;
        }

        UGCUpdateHandle_t handle = easySteam::update_handle.value();
        if (!_steam_helper->set_workshop_item_content(handle, directory_path)) {
            std::cout << "Failure setting workshop item content\n";
        }
    }

    void submitWorkshopItemUpdate(uint64_t item_id, const std::string& changelog_note) {
        if (!easySteam::update_handle.has_value())
        {
            std::cout << "Error : you should call initUpdateHandle before getting the upload progress.\n";
            return;
        }

        UGCUpdateHandle_t handle = easySteam::update_handle.value();
        _steam_helper->submit_item_update(handle,
            changelog_note.c_str(),
            [item_id] {
                std::cout << "Successfully updated workshop item: " << item_id << ".\n";
            });

    }

    void getWorkshopItemUploadProgress(long *remaining, long *totalSize) {
        if (!easySteam::update_handle.has_value())
        {
            std::cout << "Error : you should call initUpdateHandle before getting the upload progress.\n";
            return;
        }

        uint64_t processed = 0, total = 0;

        UGCUpdateHandle_t handle = easySteam::update_handle.value();

        if ( _steam_helper->get_item_upload_progress(handle, &processed, &total) != false) {
            //std::cout << "Processed : " << processed << " Total : " << total << std::endl;
        } else {
            *totalSize = 1;
            *remaining = 1;
            return;
        }
        *remaining = static_cast<long>(processed);
        *totalSize = static_cast<long>(total);
    }

    void unsubscribeWorkshopItem(uint64_t item_id) {

        if (!_steam_helper) {
            std::cout << "Error: _steam_helper is not initialized.\n";
            return;
        }

        if (!_steam_helper->unsubscribe_item(item_id)) {
            std::cout << "Failed to unsubscribe from workshop item: " << item_id << ".\n";
        }

    }

} // namespace easySteam
