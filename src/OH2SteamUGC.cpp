// Thanks to Vittorio Romeo for the base class
// I added an  will add more features (i.e. setting title, description)
// Go check his game OpenHexagon on Steam

// ----------------------------------------------------------------------------
// Steam includes.
#include <inttypes.h> // Steam libs need this.
#include "../include/steamHelper.h"

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

// ----------------------------------------------------------------------------
// Utilities.
[[nodiscard]] std::ostream& log(const std::string_view category) noexcept
{
    std::cout << "[" << category << "] ";
    return std::cout;
}

template <typename T>
[[nodiscard]] T read_integer() noexcept
{
    T result;
    while(!(std::cin >> result))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Please insert an integer.\n";
    }

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return result;
}

[[nodiscard]] bool cin_getline_string(std::string& result) noexcept
{
    return static_cast<bool>(std::getline(std::cin, result));
}

[[nodiscard]] bool cin_getline_path(std::filesystem::path& result) noexcept
{
    std::string buf;

    if(!cin_getline_string(buf))
    {
        return false;
    }

    std::cout << "Read '" << buf << "'\n";
    result = buf;

    return true;
}

[[nodiscard]] std::filesystem::path read_directory_path() noexcept
{
    std::filesystem::path result;
    std::error_code ec;

    while(!cin_getline_path(result) || !std::filesystem::exists(result, ec) ||
          !std::filesystem::is_directory(result, ec))
    {
        std::cout << "Please insert a valid path to an existing directory. "
                     "Error code: '"
                  << ec << "'\n";
    }

    return result;
}

[[nodiscard]] std::filesystem::path read_file_path() noexcept
{
    std::filesystem::path result;
    std::error_code ec;

    while(!cin_getline_path(result) || !std::filesystem::exists(result, ec) ||
          !std::filesystem::is_regular_file(result, ec))
    {
        std::cout
            << "Please insert a valid path to an existing file. Error code: '"
            << ec << "'\n";
    }

    return result;
}

[[nodiscard]] std::string read_string() noexcept
{
    std::string result;
    std::cin >> result;

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return result;
}

[[nodiscard]] bool poll_steam_callbacks(steam_helper& _steam_helper) noexcept {
    using clock = std::chrono::high_resolution_clock;
    using time_point = clock::time_point;

    const time_point loop_begin_time = clock::now();

    while (_steam_helper.any_pending_operation()) {
        if (!_steam_helper.run_callbacks()) {
            log("CLI") << "Could not run Steam API callbacks\n";
            return false;
        }

        if (clock::now() - loop_begin_time > std::chrono::seconds(240)) {
            log("CLI") << "Timed out\n";
            return false;
        }
    }

    return true;
}

// ----------------------------------------------------------------------------
// Steam stuff.

steam_helper::~steam_helper() noexcept {
    if (_initialized)
    {
        log("Steam") << "Shutting down Steam API\n";
        SteamAPI_Shutdown();
        log("Steam") << "Shut down Steam API\n";
    }
}

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
[[nodiscard]] bool steam_helper::initialize_steamworks() {
    log("Steam") << "Initializing Steam API\n";

    if(SteamAPI_Init())
    {
        log("Steam") << "Steam API successfully initialized\n";
        return true;
    }

    log("Steam") << "Failed to initialize Steam API\n";
    return false;
}

// ------------------------------------------------------------------------
// Steam API callback handlers.
void steam_helper::on_create_item(CreateItemResult_t* result, bool io_failure) {
    const auto guard = scope_guard{[this] { remove_pending_operation(); }};

    if(io_failure)
    {
        log("Steam") << "Error creating item. IO failure.\n";
        return;
    }

    if(const EResult rc = result->m_eResult; rc != EResult::k_EResultOK)
    {
        log("Steam") << "Error creating item. Error code '"
                        << static_cast<int>(rc) << "' ("
                        << result_to_string(rc) << ")\n";

        return;
    }

    const PublishedFileId_t fileId = result->m_nPublishedFileId;
    log("Steam") << "Successfully created workshop item with id '" << fileId
                    << "'\n";

    assert(_create_item_continuation);
    _create_item_continuation(fileId);
    _create_item_continuation = create_item_continuation{};
}

// ------------------------------------------------------------------------
// Other utils.
[[nodiscard]] constexpr std::string_view steam_helper::result_to_string(const EResult rc) noexcept {
    #define RETURN_IF_EQUALS(e) \
    do                      \
    {                       \
        if(rc == e)         \
        {                   \
            return #e;      \
        }                   \
    }                       \
    while(false)

        RETURN_IF_EQUALS(EResult::k_EResultNone);
        RETURN_IF_EQUALS(EResult::k_EResultOK);
        RETURN_IF_EQUALS(EResult::k_EResultFail);
        RETURN_IF_EQUALS(EResult::k_EResultNoConnection);
        RETURN_IF_EQUALS(EResult::k_EResultInvalidPassword);
        RETURN_IF_EQUALS(EResult::k_EResultLoggedInElsewhere);
        RETURN_IF_EQUALS(EResult::k_EResultInvalidProtocolVer);
        RETURN_IF_EQUALS(EResult::k_EResultInvalidParam);
        RETURN_IF_EQUALS(EResult::k_EResultFileNotFound);
        RETURN_IF_EQUALS(EResult::k_EResultBusy);
        RETURN_IF_EQUALS(EResult::k_EResultInvalidState);
        RETURN_IF_EQUALS(EResult::k_EResultInvalidName);
        RETURN_IF_EQUALS(EResult::k_EResultInvalidEmail);
        RETURN_IF_EQUALS(EResult::k_EResultDuplicateName);
        RETURN_IF_EQUALS(EResult::k_EResultAccessDenied);
        RETURN_IF_EQUALS(EResult::k_EResultTimeout);
        RETURN_IF_EQUALS(EResult::k_EResultBanned);
        RETURN_IF_EQUALS(EResult::k_EResultAccountNotFound);
        RETURN_IF_EQUALS(EResult::k_EResultInvalidSteamID);
        RETURN_IF_EQUALS(EResult::k_EResultServiceUnavailable);
        RETURN_IF_EQUALS(EResult::k_EResultNotLoggedOn);
        RETURN_IF_EQUALS(EResult::k_EResultPending);
        RETURN_IF_EQUALS(EResult::k_EResultEncryptionFailure);
        RETURN_IF_EQUALS(EResult::k_EResultInsufficientPrivilege);
        RETURN_IF_EQUALS(EResult::k_EResultLimitExceeded);
        RETURN_IF_EQUALS(EResult::k_EResultRevoked);
        RETURN_IF_EQUALS(EResult::k_EResultExpired);
        RETURN_IF_EQUALS(EResult::k_EResultAlreadyRedeemed);
        RETURN_IF_EQUALS(EResult::k_EResultDuplicateRequest);
        RETURN_IF_EQUALS(EResult::k_EResultAlreadyOwned);
        RETURN_IF_EQUALS(EResult::k_EResultIPNotFound);
        RETURN_IF_EQUALS(EResult::k_EResultPersistFailed);
        RETURN_IF_EQUALS(EResult::k_EResultLockingFailed);
        RETURN_IF_EQUALS(EResult::k_EResultLogonSessionReplaced);
        RETURN_IF_EQUALS(EResult::k_EResultConnectFailed);
        RETURN_IF_EQUALS(EResult::k_EResultHandshakeFailed);
        RETURN_IF_EQUALS(EResult::k_EResultIOFailure);
        RETURN_IF_EQUALS(EResult::k_EResultRemoteDisconnect);
        RETURN_IF_EQUALS(EResult::k_EResultShoppingCartNotFound);
        RETURN_IF_EQUALS(EResult::k_EResultBlocked);
        RETURN_IF_EQUALS(EResult::k_EResultIgnored);
        RETURN_IF_EQUALS(EResult::k_EResultNoMatch);
        RETURN_IF_EQUALS(EResult::k_EResultAccountDisabled);
        RETURN_IF_EQUALS(EResult::k_EResultServiceReadOnly);
        RETURN_IF_EQUALS(EResult::k_EResultAccountNotFeatured);
        RETURN_IF_EQUALS(EResult::k_EResultAdministratorOK);
        RETURN_IF_EQUALS(EResult::k_EResultContentVersion);
        RETURN_IF_EQUALS(EResult::k_EResultTryAnotherCM);
        RETURN_IF_EQUALS(EResult::k_EResultPasswordRequiredToKickSession);
        RETURN_IF_EQUALS(EResult::k_EResultAlreadyLoggedInElsewhere);
        RETURN_IF_EQUALS(EResult::k_EResultSuspended);
        RETURN_IF_EQUALS(EResult::k_EResultCancelled);
        RETURN_IF_EQUALS(EResult::k_EResultDataCorruption);
        RETURN_IF_EQUALS(EResult::k_EResultDiskFull);
        RETURN_IF_EQUALS(EResult::k_EResultRemoteCallFailed);
        RETURN_IF_EQUALS(EResult::k_EResultPasswordUnset);
        RETURN_IF_EQUALS(EResult::k_EResultExternalAccountUnlinked);
        RETURN_IF_EQUALS(EResult::k_EResultPSNTicketInvalid);
        RETURN_IF_EQUALS(EResult::k_EResultExternalAccountAlreadyLinked);
        RETURN_IF_EQUALS(EResult::k_EResultRemoteFileConflict);
        RETURN_IF_EQUALS(EResult::k_EResultIllegalPassword);
        RETURN_IF_EQUALS(EResult::k_EResultSameAsPreviousValue);
        RETURN_IF_EQUALS(EResult::k_EResultAccountLogonDenied);
        RETURN_IF_EQUALS(EResult::k_EResultCannotUseOldPassword);
        RETURN_IF_EQUALS(EResult::k_EResultInvalidLoginAuthCode);
        RETURN_IF_EQUALS(EResult::k_EResultAccountLogonDeniedNoMail);
        RETURN_IF_EQUALS(EResult::k_EResultHardwareNotCapableOfIPT);
        RETURN_IF_EQUALS(EResult::k_EResultIPTInitError);
        RETURN_IF_EQUALS(EResult::k_EResultParentalControlRestricted);
        RETURN_IF_EQUALS(EResult::k_EResultFacebookQueryError);
        RETURN_IF_EQUALS(EResult::k_EResultExpiredLoginAuthCode);
        RETURN_IF_EQUALS(EResult::k_EResultIPLoginRestrictionFailed);
        RETURN_IF_EQUALS(EResult::k_EResultAccountLockedDown);
        RETURN_IF_EQUALS(
            EResult::k_EResultAccountLogonDeniedVerifiedEmailRequired);
        RETURN_IF_EQUALS(EResult::k_EResultNoMatchingURL);
        RETURN_IF_EQUALS(EResult::k_EResultBadResponse);
        RETURN_IF_EQUALS(EResult::k_EResultRequirePasswordReEntry);
        RETURN_IF_EQUALS(EResult::k_EResultValueOutOfRange);
        RETURN_IF_EQUALS(EResult::k_EResultUnexpectedError);
        RETURN_IF_EQUALS(EResult::k_EResultDisabled);
        RETURN_IF_EQUALS(EResult::k_EResultInvalidCEGSubmission);
        RETURN_IF_EQUALS(EResult::k_EResultRestrictedDevice);
        RETURN_IF_EQUALS(EResult::k_EResultRegionLocked);
        RETURN_IF_EQUALS(EResult::k_EResultRateLimitExceeded);
        RETURN_IF_EQUALS(EResult::k_EResultLimitExceeded);
        RETURN_IF_EQUALS(EResult::k_EResultAccountLoginDeniedNeedTwoFactor);
        RETURN_IF_EQUALS(EResult::k_EResultItemDeleted);
        RETURN_IF_EQUALS(EResult::k_EResultAccountLoginDeniedThrottle);
        RETURN_IF_EQUALS(EResult::k_EResultTwoFactorCodeMismatch);
        RETURN_IF_EQUALS(EResult::k_EResultTwoFactorActivationCodeMismatch);
        RETURN_IF_EQUALS(EResult::k_EResultAccountAssociatedToMultiplePartners);
        RETURN_IF_EQUALS(EResult::k_EResultNotModified);
        RETURN_IF_EQUALS(EResult::k_EResultNoMobileDevice);
        RETURN_IF_EQUALS(EResult::k_EResultTimeNotSynced);
        RETURN_IF_EQUALS(EResult::k_EResultSmsCodeFailed);
        RETURN_IF_EQUALS(EResult::k_EResultAccountLimitExceeded);
        RETURN_IF_EQUALS(EResult::k_EResultAccountActivityLimitExceeded);
        RETURN_IF_EQUALS(EResult::k_EResultPhoneActivityLimitExceeded);
        RETURN_IF_EQUALS(EResult::k_EResultRefundToWallet);
        RETURN_IF_EQUALS(EResult::k_EResultEmailSendFailure);
        RETURN_IF_EQUALS(EResult::k_EResultNotSettled);
        RETURN_IF_EQUALS(EResult::k_EResultNeedCaptcha);
        RETURN_IF_EQUALS(EResult::k_EResultGSLTDenied);
        RETURN_IF_EQUALS(EResult::k_EResultGSOwnerDenied);
        RETURN_IF_EQUALS(EResult::k_EResultInvalidItemType);
        RETURN_IF_EQUALS(EResult::k_EResultIPBanned);
        RETURN_IF_EQUALS(EResult::k_EResultGSLTExpired);
        RETURN_IF_EQUALS(EResult::k_EResultInsufficientFunds);
        RETURN_IF_EQUALS(EResult::k_EResultTooManyPending);
        RETURN_IF_EQUALS(EResult::k_EResultNoSiteLicensesFound);
        RETURN_IF_EQUALS(EResult::k_EResultWGNetworkSendExceeded);
        RETURN_IF_EQUALS(EResult::k_EResultAccountNotFriends);
        RETURN_IF_EQUALS(EResult::k_EResultLimitedUserAccount);
        RETURN_IF_EQUALS(EResult::k_EResultCantRemoveItem);
        RETURN_IF_EQUALS(EResult::k_EResultAccountDeleted);
        RETURN_IF_EQUALS(EResult::k_EResultExistingUserCancelledLicense);
        RETURN_IF_EQUALS(EResult::k_EResultCommunityCooldown);
    #undef RETURN_IF_EQUALS

    return "unknown EResult value";
}

void steam_helper::on_submit_item(SubmitItemUpdateResult_t* result, bool io_failure)
{
    const auto guard = scope_guard{[this] { remove_pending_operation(); }};

    if(io_failure)
    {
        log("Steam") << "Error creating item. IO failure.\n";
        return;
    }


    if(const EResult rc = result->m_eResult; rc != EResult::k_EResultOK)
    {
        log("Steam") << "Error updating item. Error code '"
                        << static_cast<int>(rc) << "' ("
                        << result_to_string(rc) << ")\n";

        return;
    }

    assert(_submit_item_continuation);
    _submit_item_continuation();
    _submit_item_continuation = submit_item_continuation{};
}

void steam_helper::create_workshop_item(create_item_continuation&& continuation) noexcept {
    if(!initialized())
    {
        return;
    }

    log("Steam") << "Creating workshop item...\n";
    add_pending_operation();

    const SteamAPICall_t api_call = SteamUGC()->CreateItem(
        app_id, EWorkshopFileType::k_EWorkshopFileTypeCommunity);

    _create_item_result.Set(api_call, this, &steam_helper::on_create_item);
    _create_item_continuation = std::move(continuation);
}

[[nodiscard]] std::optional<UGCUpdateHandle_t> steam_helper::start_workshop_item_update(const PublishedFileId_t item_id) noexcept {
    const UGCUpdateHandle_t handle =
        SteamUGC()->StartItemUpdate(app_id, item_id);

    if(handle == k_UGCUpdateHandleInvalid)
    {
        log("Steam") << "Invalid update handle for file id '" << item_id
                        << "'\n";

        return std::nullopt;
    }

    return {handle};
}

bool steam_helper::set_workshop_item_content(const UGCUpdateHandle_t update_handle, const std::filesystem::path& directory_path) noexcept {
    [[maybe_unused]] std::error_code ec;

    assert(std::filesystem::exists(directory_path, ec));
    assert(std::filesystem::is_directory(directory_path, ec));

    if(!SteamUGC()->SetItemContent(
            update_handle, directory_path.string().data()))
    {
        log("Steam") << "Failed to set workshop item contents from path '"
                        << directory_path << "'\n";

        return false;
    }

    return true;
}

bool steam_helper::set_workshop_item_description(const UGCUpdateHandle_t update_handle, const std::string& description) noexcept {
    [[maybe_unused]] std::error_code ec;

    if(!SteamUGC()->SetItemDescription(update_handle, description.data()))
    {
        log("Steam")
            << "Failed to set workshop description" << "'\n";
        return false;
    }

    return true;
}

bool steam_helper::set_workshop_item_preview_image(const UGCUpdateHandle_t update_handle, const std::filesystem::path& file_path) noexcept {
    [[maybe_unused]] std::error_code ec;

    assert(std::filesystem::exists(file_path, ec));
    assert(std::filesystem::is_regular_file(file_path, ec));

    if(!SteamUGC()->SetItemPreview(
            update_handle, file_path.string().data()))
    {
        log("Steam")
            << "Failed to set workshop item preview image from path '"
            << file_path << "'\n";

        return false;
    }

    return true;
}

bool steam_helper::set_workshop_item_title(const UGCUpdateHandle_t update_handle, const std::string title) noexcept {
    [[maybe_unused]] std::error_code ec;

    if(!SteamUGC()->SetItemTitle(update_handle, title.data()))
    {
        log("Steam")
            << "Failed to set workshop title" << "'\n";
        return false;
    }

    return true;
}

void steam_helper::submit_item_update(const UGCUpdateHandle_t handle, const char* change_note, submit_item_continuation&& continuation) noexcept {
    log("Steam") << "Submitting workshop item update...\n";
    add_pending_operation();

    const SteamAPICall_t api_call =
        SteamUGC()->SubmitItemUpdate(handle, change_note);

    _submit_item_result.Set(api_call, this, &steam_helper::on_submit_item);
    _submit_item_continuation = std::move(continuation);
}

bool steam_helper::run_callbacks() noexcept {
    if(!initialized())
    {
        return false;
    }

    SteamAPI_RunCallbacks();
    return true;
}

[[nodiscard]] bool steam_helper::initialized() const noexcept { return _initialized; }

[[nodiscard]] bool steam_helper::any_pending_operation() const noexcept { return _pending_operations.load() > 0; }

void steam_helper::add_pending_operation() noexcept {
    _pending_operations.store(_pending_operations.load() + 1);
    log("Steam") << "Added pending operation\n";
}

void steam_helper::remove_pending_operation() noexcept {
    assert(any_pending_operation());

    _pending_operations.store(_pending_operations.load() - 1);
    log("Steam") << "Removed pending operation\n";
}

// ----------------------------------------------------------------------------