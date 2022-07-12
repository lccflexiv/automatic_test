# Automatic Test by Flexiv RDK

### This pack provides Linux C++ Interface for auto test
#### Steps:
1. Check with RCA's version and make sure you have the correct rdk lib files and headers.
2. Make sure you are licensed with your robot or simulator if you need.
3. This pack has v2.9.1 & v2.10 for now. You can check with the branches.
4. If you are using a simulator, you should be fine. Just be careful about your local configurations. If you are using a real robot, please change your robot and local IP address in SystemParams.h inside include/autotest folder.
5. Configure CMake and compile all C++ example programs for the x64 processor platform:

        cd flexiv_rdk
        mkdir build && cd build
        cmake ..
        make -j4
        ./test/test_auto

   If compiling for the arm64 processor platform, set the additional CMake option when configuring:

        cmake .. -DBUILD_FOR_ARM64=ON
