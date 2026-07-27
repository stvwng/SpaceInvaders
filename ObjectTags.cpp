#include "ObjectTags.h"
#include "DevelopState.h"

const string ObjectTags::START_OF_OBJECT = "[START OBJECT]";
const string ObjectTags::END_OF_OBJECT = "[END OBJECT]";
const string ObjectTags::COMPONENT = "[COMPONENT]";
// The data has always used "[-COMPONENT]", matching the "[-X]" convention of
// every other closing tag in the format. This constant said "[-END COMPONENT]",
// so no component name was ever extracted correctly: the old length-based
// parser returned a truncated "Standard Grap", and the factory matched none of
// its cases. Every GameObject in the level was therefore built with no
// graphics, no transform and no update component at all.
const string ObjectTags::COMPONENT_END = "[-COMPONENT]";
const string ObjectTags::NAME = "[NAME]";
const string ObjectTags::NAME_END = "[-NAME]";
const string ObjectTags::WIDTH = "[WIDTH]";
const string ObjectTags::WIDTH_END = "[-WIDTH]";
const string ObjectTags::HEIGHT = "[HEIGHT]";
const string ObjectTags::HEIGHT_END = "[-HEIGHT]";
const string ObjectTags::LOCATION_X = "[LOCATION X]";
const string ObjectTags::LOCATION_X_END = "[-LOCATION X]";
const string ObjectTags::LOCATION_Y = "[LOCATION Y]";
const string ObjectTags::LOCATION_Y_END = "[-LOCATION Y]";
const string ObjectTags::BITMAP_NAME = "[BITMAP NAME]";
const string ObjectTags::BITMAP_NAME_END = "[-BITMAP NAME]";
const string ObjectTags::SPEED = "[SPEED]";
const string ObjectTags::SPEED_END = "[-SPEED]";
const string ObjectTags::ENCOMPASSING_RECT_COLLIDER = "[ENCOMPASSING RECT COLLIDER]";
const string ObjectTags::ENCOMPASSING_RECT_COLLIDER_END = "[-ENCOMPASSING RECT COLLIDER]";