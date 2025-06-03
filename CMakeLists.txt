add_executable(vga_invaders
    snd/MajorTom.cpp
    snd/Laser.cpp
    snd/MoveLeft.cpp
    snd/MoveRight.cpp
    snd/Explosion1.cpp
    snd/Explosion2.cpp
    snd/Shooterkilled.cpp
    snd/InvaderShoot.cpp
    src/main.cpp
    src/sound.cpp
    img/Arecibo_Reply.cpp
    img/AlienPizza.cpp
    img/block2.cpp
    img/block3.cpp
    img/gameover.cpp
    img/clouds.cpp
    img/shooter.cpp
    img/shooter-sm.cpp
    img/invader1.cpp
    img/invader2.cpp
    img/invader3.cpp
    img/invader4.cpp
    img/invader5.cpp
    img/fire1.cpp
    img/fire2.cpp
    img/explode.cpp
)

add_picovga(vga_invaders)

# for vga_config.h include
target_include_directories(vga_invaders PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)

# create map/bin/hex file etc.
pico_add_extra_outputs(vga_invaders)

# Allow stdio to USB
pico_enable_stdio_usb(vga_invaders 1)


# Enable uart output
#pico_enable_stdio_uart(${PROJECT_NAME} 1)
#pico_enable_stdio_usb(${PROJECT_NAME} 0)
