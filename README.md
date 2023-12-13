![1 1_8LbQy3Sx9B_5BfL7f70g](https://github.com/Ena-Shepherd/EasySteamUGC/assets/62568994/4f210b5f-0407-4aa1-986f-7fc6d3eb09b8)
## <p align="center">Integrate creating and editing of Steam workshop items for your app in seconds</p>
## It only takes few lines of code
```cpp
#include "easySteam.h"

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
-> Default timeout for operations is 4 minutes

## Compiling
- You will need `steamapi_64` added to your project
- link against `libeasysteam.a` and `steamapi_64`, either .dll or .lib
- Done !

 ## Special Thanks
<a href="https://github.com/vittorioromeo">Vittorio Romeo</a> for the base code from which i built the API. <br/>
Go check his game <a href="https://github.com/vittorioromeo">OpenHexagon</a> on Steam, it's great, and it allowed me to run the tests for easySteam
