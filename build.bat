@echo off
echo Building Apex Cyber Drive...
g++ -O3 -std=c++14 AudioSynth.cpp ParticleSystem.cpp Road.cpp Car.cpp Traffic.cpp Renderer.cpp Game.cpp main.cpp -o CyberTorque.exe -lgdi32 -lmsimg32 -lwinmm -lgdiplus -mwindows

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================================
    echo Build SUCCESSFUL! Created CyberTorque.exe
    echo Launching Apex Cyber Drive...
    echo ========================================================
    start CyberTorque.exe
) else (
    echo.
    echo [ERROR] Build failed! Check the compiler output above.
)
