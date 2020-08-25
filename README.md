# projewski-pocket-book
Some codes related with creating applications on PocketBook

This repository is based on my experiences with PocketBook InkPad 3.
I've found such repository: https://github.com/blchinezu/pocketbook-sdk, which, I may say, contains the newest version of PocketBook SDK.
The said news, that it's not fresh.

# How to work with SDK
First of all we should download the SDK from repository https://github.com/blchinezu/pocketbook-sdk.
There're free folders to work with different versions of firmware:
* PBSDK - for 360 devices
* FRSCSDK - for framework 2 and 4 (FW2/FW4)
* SDK_481 - for framework 5 (FW5)

Every such framework folder contains
* cross compiler - it allows to compile a code on dedicated architecture (PocketBook ARM) on local machine
* compiled set of libraries/applications from device (compiled to ARM)
* include files for developing

My PocketBook uses FW5, but the version, which it's on the repository is not too fresh. In some cases (for some extra functions only), I'd some problems with compile/link/execute application. To do so, I've made such trick, which I recommend to develop with it. But in most cases it should be enough to execute code compiled with SDK_481.
1. Copy SDK_481 as MYSDK.
2. Download libraries from your PocketBook (TODO: How to do this)
3. Replace such libraries to version from your PocketBook.

To compile source code use cross-compiler from selected framework. (TODO: Helpful project)

The compiled application put in folder applications with extension app. It'll be available from the down status bar of device.
