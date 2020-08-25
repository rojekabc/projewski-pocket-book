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
#### Additional informations
The OpenScreen function should be called before - if not SEGV possible.
If toolbar is enabled, than manipulating on the whole screen (is it possible to calculate (?)) will bring wrong position problem.
