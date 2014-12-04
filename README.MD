# RadiantUI

CEF Based Web Browser Integration into Unreal Engine 4

Released under the MIT license.

[![Demo](http://img.youtube.com/vi/g6X76aFz2BA/0.jpg)](https://www.youtube.com/watch?v=g6X76aFz2BA)

https://www.youtube.com/watch?v=g6X76aFz2BA

# Key Features:

* Render HTML/CSS/JavaScript to any surface/material in the game.
* No C++ coding required, everything is done through blueprints and components.
* Automatic Input handling: Walk up to any WebActor and use the mouse and keyboard automatically (Doom3 style interfaces).
* Supports non-flat surfaces: You can map a WebActor onto any mesh and still interact with it (Curved computer screens etc).
* Networked gameplay support: Multiple players can interact with the same WebActor and see the same output.

# Known Issues

* UE version 4.5 removed the ability to do non-rectangular hit-testing. Currently any Radiant WebView HUD Elements that have 
hit testing enabled consume input from the entire screen. This probably could be fixed by adjusting the slate widgets.
