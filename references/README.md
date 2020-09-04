# inkview.h
## Functions
### void SetShowPanelReader(int flag)

Allows enable/disable top and bottom bars of the reader.

|360|FW2|FW4|FW5|
|-|-|-|-|
|no|no|yes|yes|

#### Parameters
**flag**
0 disable, other values enable
#### Additional information
The OpenScreen function should be called before - if not SEGV possible.
If toolbar is enabled, than manipulating on the whole screen (is it possible to calculate (?)) will bring wrong position problem.

### void OpenKeyboard(const char *title, char *buffer, int maxlen, int flags, iv_keyboardhandler hproc)

Shows the keyboard dialog.

#### Parameters
**title**
This will be displayed in the text field as a title
**buffer**
Should point on the buffer, which will be filled by the keyboard dialog.
**maxlen**
The Maximum length +1 of characters to input. I don't understand, why it fills a one character more - maybe its some API bug.
**flags**
Flags for selecting type of keyboard
|Flag|Description|
|-|-|
|KBD_NORMAL|the normal keyboard|
|KBD_ENTEXT|???|
|KBD_PHONE|???|
|KBD_NUMERIC|the numeric keyboard|
|KBD_IPADDR|the IP address keyboard|
|KBD_FILENAME|???|
|KBD_URL|the URL form keyboard|
|KBD_DATE|???|
|KBD_TIME|???|
|KBD_DATETIME|???|
|KBD_HEX|???|
|KBD_HOURS|???|

Flags responsible for up/low cases
|Flag|Description|
|-|-|
|KBD_UPPER|upper|
|KBD_LOWER|lower|
|KBD_FIRSTUPPER|first upper ((i)not works for me)|

Other flags
|Flag|Description|
|-|-|
|KBD_PASSWORD|password keyboard type (hiding)|
|KBD_NOSELECT|???|
|KBD_SCREENTOP|show keyboard on the top (by default on the bottom)|
|KBD_NOHEADER|???|
|KBD_NOEVTSHOW|???|
|KBD_SENDKEYBOARDSTATEEVENTS|???|
|KBD_NOTOUCH|???|
|KBD_PASSEVENTS|???|
|KBD_NOUPDATE_AFTER_CLOSE|???|
|KBD_EN_DEFAULT|???|
|KBD_NOUPDATE_AFTER_OPEN|???|
|KBD_NO_SELFCLOSE_ON_OK|It'll be not closed after OK. I checked with use CloseKeyboard in the handler, but it not works. I don't test the handler is even called. Maybe simple cleaning and do next operation may kill this dialog.|
|KBD_CUSTOM_ENTER_KEY|???|
|KBD_MARKED_ENTER_KEY|???|
|KBD_PASSWORD_WIFI|???|
|KBD_NEXT|???|

**hproc**
Handler called, when text is typed.

#### Additional information
1. The call of this method is asynchronous. So after call the dialog will be shown, but there's no wait point, so the next lines of code will be called.
1. To the handler the pointer to filled text is an argue. It's the buffer, which is invoked in the OpenKeyboard as buffer. But there's a special case - when the keboard is closed by 'X' button, than the NULL will be used in the handler. It's important, if buffer is dynamically allocated or we expect that it will be always as an argue.
