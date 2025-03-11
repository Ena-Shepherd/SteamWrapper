mkdir build
cd build
cmake .. -G"Visual Studio 17 2022" #-DSTEAM_FOLDER="C:/yoursdkfolder"
cmake --build . --config Release