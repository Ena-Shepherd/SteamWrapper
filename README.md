![1 1_8LbQy3Sx9B_5BfL7f70g](https://clan.akamai.steamstatic.com/images/3284297/b27ff50de898a52d9ea8dbda746c47ec2045bad4_960x311.jpg)
## <p align="center">Integrate creating and editing of Steam workshop items for your app in seconds</p>
## It only takes a few lines of code
```cpp
#include "easySteam.h"

// Do only once

easySteam::initializeSteamHelper();


// Specify the app ID to create a new item

easySteam::appID = 480;


// Create your new item

easySteam::createItem(easySteam::appID);


// Or specify the item ID if you want to edit an existing item
// easySteam::itemID = 3106420873; 
    

// Customize the workshop item as needed
easySteam::initUpdateHandle();


// Rest of the API

easySteam::setPreviewImage("SomePath\\ena.jpg");
easySteam::setWorkshopItemTitle("Custom Title");
easySteam::setWorkshopItemDescription("Custom Description");
easySteam::setWorkshopItemContent("SomePath\\YourContent");


// Submit your changes

easySteam::submitWorkshopItemUpdate(easySteam::itemID, "Custom changelog note");

```

## No more Async to worry about
Asynchronous operations are now handled inside the library <br/>
> Default timeout for operations is 4 minutes

## Build and add to your app
- Refer to `build.sh` if you don't know CMake
- Link against `steam_wrapper` and `steamapi_64`, either .dll, .lib or .a
- Done !

 ## Special Thanks
<a href="https://github.com/vittorioromeo">Vittorio Romeo</a> for the base code from which i built the API. <br/>
Go check his game <a href="https://github.com/vittorioromeo">OpenHexagon</a> on Steam, it's great, and it allowed me to run the tests for the wrapper.
