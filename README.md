# ES-PCB-for-L476RG-demo
Demo application for the [ES-PCB-for-L476RG](https://github.com/paspf/ES-PCB-for-L476RG). This demo application uses the STM32Cube HAL and comes with drivers for all hardware parts on the [ES-PCB-for-L476RG](https://github.com/paspf/ES-PCB-for-L476RG) board, including:

 - A button debouncer
 - A driver to display text and/or geometric objects on the screen
 - A driver for the leds
 - A driver for the ws2812b leds

## How to setup
Create a STM32Cube HAL project for the Nucleo L476RG in your favorite IDE (e.g. CubeIDE, System Workbench for STM32, VS Code + PlatformIO), copy the project files into your project and you are ready to go.

## How to use
After you have installed the demo on your microcontroller, the screen lights up and tells you the status of the component tests. At first the buttons are tested. To complete the test, the user has to click the buttons on the expansion board, as well as the on-board button of the Nucleo. Next, D1-D8 light up. When all leds lit up, the user must press the "OK" button, S1. If some leds have not lit up, press S2, the "not okay" button. The last two tests are dealing with the on-board led and the ws2812b leds. If all tests are passed, the screen should look like this:


![Image showing the board](/images/board-demo-end.jpg)
