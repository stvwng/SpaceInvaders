#pragma once

// Debug logging is controlled by the build, not by editing this header:
//     cmake -B build -DSPACEINVADERS_DEBUG_LOG=ON
//
// This used to be a bare `#define debuggingConsole` here, while the code
// actually tested two *other* spellings (`debuggingErrors` and
// `debuggingOnConsole`). Every debug block was therefore dead, and none of
// them had ever been compiled -- one contained `<< end;` instead of `<< endl;`
// and would not have built. A single build-controlled macro keeps the blocks
// honest, because the build can compile with it on.

class DevelopState {};
