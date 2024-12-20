#pragma once

// ----------------------------------------------------------------------------
// Steam includes.
#include <inttypes.h> // Steam libs need this.
#include <steam_api.h>

// ----------------------------------------------------------------------------
// Standard includes.
#include <iostream>
#include <atomic>
#include <string_view>
#include <chrono>
#include <cassert>
#include <utility>
#include <functional>
#include <limits>
#include <optional>
#include <variant>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <iomanip>
#include <cstdint>

// ----------------------------------------------------------------------------
// Utilities.
[[nodiscard]] std::ostream& log(const std::string_view category) noexcept;

template <typename F>
struct scope_guard : F
{
    explicit scope_guard(F&& f) noexcept : F{std::move(f)}
    {}

    ~scope_guard() noexcept
    {
        static_cast<F&>(*this)();
    }
};

template <typename T>
[[nodiscard]] T read_integer() noexcept;

[[nodiscard]] bool cin_getline_string(std::string& result) noexcept;

[[nodiscard]] bool cin_getline_path(std::filesystem::path& result) noexcept;

[[nodiscard]] std::filesystem::path read_directory_path() noexcept;

[[nodiscard]] std::filesystem::path read_file_path() noexcept;

[[nodiscard]] std::string read_string() noexcept;

class steam_helper {

private:
    // ------------------------------------------------------------------------
    // Constants.

    // ------------------------------------------------------------------------
    // Type aliases.
    using create_item_continuation = std::function<void(PublishedFileId_t)>;
    using submit_item_continuation = std::function<void()>;
    using submit_query_continuation = std::function<void(UGCQueryHandle_t)>;

    typedef struct query_result {
        SteamUGCDetails_t item_details;
        char* image_url;
    } query_result_t;

    // ------------------------------------------------------------------------
    // Data members.
    bool _initialized;
    std::atomic<int> _pending_operations;
    std::vector<query_result_t> _query_results;

    CCallResult<steam_helper, CreateItemResult_t> _create_item_result;
    create_item_continuation _create_item_continuation;

    CCallResult<steam_helper, SubmitItemUpdateResult_t> _submit_item_result;
    submit_item_continuation _submit_item_continuation;

    CCallResult<steam_helper, SteamUGCQueryCompleted_t> _submit_query_result;
    submit_query_continuation _submit_query_continuation;

    // ------------------------------------------------------------------------
    // Initialization utils.
    [[nodiscard]] static bool initialize_steamworks();

    // ------------------------------------------------------------------------
    // Other utils.
    [[nodiscard]] static constexpr std::string_view result_to_string(const EResult rc) noexcept;

    // ------------------------------------------------------------------------
    // Steam API callback handlers.
    void on_create_item(CreateItemResult_t* result, bool io_failure);

    void on_submit_item(SubmitItemUpdateResult_t* result, bool io_failure);

    void on_query_completed(SteamUGCQueryCompleted_t* result, bool io_failure);

public:

    AppId_t app_id = 0;
    PublishedFileId_t item_id = 0;

    steam_helper() noexcept : _initialized{initialize_steamworks()}, _pending_operations{0} {};

    ~steam_helper() noexcept;

    void get_query_results(std::vector<SteamUGCDetails_t> &itemDetails, std::vector<char*> &previewImageURL) noexcept;

    void create_workshop_item(create_item_continuation&& continuation) noexcept;
    
    void create_user_query(UGCQueryHandle_t &query_handle, AccountID_t accountID,
                        EUserUGCList listType, EUGCMatchingUGCType matchingType,
                        EUserUGCListSortOrder sortOrder, uint32_t creatorAppID,
                        uint32_t consumerAppID, uint32_t page) noexcept;
    
    void create_all_query(UGCQueryHandle_t &query_handle,
                        EUGCQuery listType, EUGCMatchingUGCType matchingType,
                        AppId_t creatorAppID, AppId_t consumerAppID,
                        uint32_t page) noexcept;
    
    [[nodiscard]] bool set_cloud_filename_filter(const UGCQueryHandle_t query_handle, const char* match_cloud_name) noexcept;

    [[nodiscard]] bool set_match_anytag(const UGCQueryHandle_t query_handle, const bool match_any_tag) noexcept;

    [[nodiscard]] bool set_search_text(const UGCQueryHandle_t query_handle, const char* searchText) noexcept;

    [[nodiscard]] bool set_ranked_by_trend_days(const UGCQueryHandle_t query_handle, const uint32 unDays) noexcept;

    [[nodiscard]] bool add_required_tag(const UGCQueryHandle_t query_handle, const char* tagName) noexcept;

    [[nodiscard]] bool add_excluded_tag(const UGCQueryHandle_t query_handle, const char* tagName) noexcept;

    [[nodiscard]] bool return_long_description(const UGCQueryHandle_t query_handle, const bool returnLongDescription) noexcept;

    [[nodiscard]] bool return_total_only(const UGCQueryHandle_t query_handle, const bool returnTotalOnly) noexcept;

    [[nodiscard]] bool allow_cached_response(const UGCQueryHandle_t query_handle, const uint32 maxAgeSeconds) noexcept;

    void send_query_request(UGCQueryHandle_t query_handle, submit_query_continuation&& continuation) noexcept;

    void release_query_handle(UGCQueryHandle_t query_handle) noexcept;

    [[nodiscard]] std::optional<UGCUpdateHandle_t> start_workshop_item_update(const PublishedFileId_t item_id) noexcept;
    
    bool set_workshop_item_content(const UGCUpdateHandle_t update_handle, const std::filesystem::path& directory_path) noexcept;
    
    bool set_workshop_item_description(const UGCUpdateHandle_t update_handle, const std::string& description) noexcept;

    bool set_workshop_item_preview_image(const UGCUpdateHandle_t update_handle, const std::filesystem::path& file_path) noexcept;

    bool set_workshop_item_title(const UGCUpdateHandle_t update_handle, const std::string title) noexcept;

    void submit_item_update(const UGCUpdateHandle_t handle, const char* change_note, submit_item_continuation&& continuation) noexcept;

    bool get_item_upload_progress(const UGCUpdateHandle_t update_handle, uint64_t *Processed, uint64_t *Total) noexcept;

    bool run_callbacks() noexcept;

    [[nodiscard]] bool unsubscribe_item(PublishedFileId_t item_id) noexcept;

    [[nodiscard]] bool initialized() const noexcept;

    [[nodiscard]] bool any_pending_operation() const noexcept;

    void add_pending_operation() noexcept;

    void remove_pending_operation() noexcept;

};
