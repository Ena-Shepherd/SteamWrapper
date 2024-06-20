#pragma once

// ----------------------------------------------------------------------------
// Steam includes.
#include <inttypes.h> // Steam libs need this.
#include "../../SteamAPI/include/steam_api.h"

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

    // ------------------------------------------------------------------------
    // Data members.
    bool _initialized;
    std::atomic<int> _pending_operations;

    CCallResult<steam_helper, CreateItemResult_t> _create_item_result;
    create_item_continuation _create_item_continuation;

    CCallResult<steam_helper, SubmitItemUpdateResult_t> _submit_item_result;
    submit_item_continuation _submit_item_continuation;

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
    

public:

    AppId_t app_id = 0;
    PublishedFileId_t item_id = 0;

    steam_helper() noexcept : _initialized{initialize_steamworks()}, _pending_operations{0} {};

    ~steam_helper() noexcept;

    void create_workshop_item(create_item_continuation&& continuation) noexcept;

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
