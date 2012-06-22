# Style Tags Overview

## Style Overview
This document describes how style tags can be specified, how they affect your
widgets, and how to use specific built-in style tags in otclient, as well
as how to define new custom style tags.

## Style Value Types

{valhead:Integer}
Integer number, used by most of style proprieties

**Examples:**

* 1
* 9
* 5

{valhead:Float}
Float number

**Examples:**

* 1.0
* 9
* 1.5

{valhead:String}
String containing a text


**Examples:**

* this is a string
* it doesn't need quotes or escaping

{valhead:Boolean}
Value with two states, **true** or **false**

**Examples:**

* true
* false

{valhead:Size}
Size type, defined by 2 integers, width followed by height

    size: width height


**Examples:**

* 10 20
* 30 10

{valhead:Rect}

Rect type, defined by 4 integers

    rect: x y width height

**Examples:**

* 0 0 32 32
* 10 20 30 40

{valhead:Color}

{valhead:Image}

{valhead:Font}


## UIWidget

{taghead:rect Rect}
Defines widget's rect

**Examples:**

    !!!plain
    rect: 32 32 32 32

**See also:** {tag:pos}, {tag:size}

{taghead:size Size}
Defines size of a widget in pixels

**Examples:**

    !!!plain
    size: 32 32

{taghead:width Integer}
Defines width of a widget

{taghead:height Integer}
Defines height of a widget

{taghead:pos Point}
Defines absolute position of the widget relative to the window top left, this is generialy not used,
if you want to position relative to parent widget, use anchors and margins


**Examples:**

    !!!plain
    pos: 32 32

{taghead:x Integer}
Defines x absolute position

**See also:** {tag:pos}

{taghead:y Integer}
Defines y absolute position

**See also:** {tag:pos}


{taghead:color Color}
Foreground color, mostly used for coloring texts

**Examples:**

    !!!plain
    color: red
    color: #ff0000
    color: #ff000080

{taghead:background Color}
Same as {tag:background-color}

{taghead:background-color Color}
Defines background color


{taghead:background-rect Rect}
{taghead:background-offset Point}
{taghead:background-offset-x Integer}
{taghead:background-offset-y Integer}
{taghead:background-size Size}
{taghead:background-width Integer}
{taghead:background-height Integer}


{taghead:icon Image}
{taghead:icon-source Image}
{taghead:icon-color Color}
{taghead:icon-offset-x Integer}
{taghead:icon-offset-y Integer}
{taghead:icon-offset Point}
{taghead:icon-width Integer}
{taghead:icon-height Integer}
{taghead:icon-size Size}
{taghead:icon-rect Rect}
{taghead:opacity Float}
{taghead:enabled Boolean}
{taghead:visible Boolean}
{taghead:checked Boolean}
{taghead:dragable Boolean}
{taghead:on Boolean}
{taghead:focusable Boolean}
{taghead:phantom Boolean}
{taghead:fixed-size Boolean}
{taghead:clipping Boolean}
{taghead:border Integer}
{taghead:border-width Integer}
{taghead:border-width-top Integer}
{taghead:border-width-right Integer}
{taghead:border-width-bottom Integer}
{taghead:border-width-left Integer}
{taghead:border-color Color}
{taghead:border-color-top Color}
{taghead:border-color-right Color}
{taghead:border-color-bottom Color}
{taghead:border-color-left Color}
{taghead:margin-top Integer}
{taghead:margin-right Integer}
{taghead:margin-bottom Integer}
{taghead:margin-left Integer}
{taghead:margin Integer}
{taghead:padding-top Integer}
{taghead:padding-right Integer}
{taghead:padding-bottom Integer}
{taghead:padding-left Integer}
{taghead:padding Integer}
{taghead:layout Layout}

{taghead:image-source}
{taghead:image-offset-x Integer}
{taghead:image-offset-y Integer}
{taghead:image-offset Point}
{taghead:image-width Integer}
{taghead:image-height Integer}
{taghead:image-size Size}
{taghead:image-rect Rect}
{taghead:image-clip Rect}
{taghead:image-fixed-ratio Boolean}
{taghead:image-repeated Boolean}
{taghead:image-smooth Boolean}
{taghead:image-color Color}
{taghead:image-border-top Integer}
{taghead:image-border-right Integer}
{taghead:image-border-bottom Integer}
{taghead:image-border-left Integer}
{taghead:image-border Integer}

{taghead:text}
{taghead:text-align Align}
{taghead:text-offset Point}
{taghead:text-wrap Boolean}
{taghead:text-auto-resize Boolean}
{taghead:font Font}

## UITextEdit
{taghead:text-margin}
{taghead:always-active}
{taghead:shift-navigation}
{taghead:multiline}
{taghead:max-length}

