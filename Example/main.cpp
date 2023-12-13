
#include "../include/easySteam.h"

int main() {

    easySteam::initializeSteamHelper();

    easySteam::appID = 1358090; // Specify the app ID to create a new item
    //easySteam::itemID = 3106420873; // Specify the item ID if you don't want to create a new item
    easySteam::createItem(easySteam::appID);


    // Customize the workshop item as needed
    easySteam::initUpdateHandle();
    easySteam::setPreviewImage("C:\\Users\\you\\Pictures\\Saved Pictures\\yourImage.jpg");
    easySteam::setWorkshopItemTitle("Custom Title");
    easySteam::setWorkshopItemDescription("Custom Description");
    easySteam::setWorkshopItemContent("C:\\Users\\you\\Content\\My_content");

    easySteam::submitWorkshopItemUpdate(easySteam::itemID, "Custom changelog note");

    return 0;
}
