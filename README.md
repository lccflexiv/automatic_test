# Automatic Test by Flexiv RDK

### RDK provides Linux C++ & Python Interface
1. Check with RCA's version and make sure you have the correct rdk lib files
2. Make sure you are licensed with your robot or simulator
3. This little pack is based on v2.9.1-rawforcesensor, you can just copy & paste some head files and test_auto.cpp to your work folder and it should work as well. 
4. Change your IP address in SystemParams.h in Include/autotest
5. Configure CMake and compile all C++ example programs for the x64 processor platform:

        cd flexiv_rdk
        mkdir build && cd build
        cmake ..
        make -j4

   If compiling for the arm64 processor platform, set the additional CMake option when configuring:

        cmake .. -DBUILD_FOR_ARM64=ON