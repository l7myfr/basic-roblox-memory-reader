#include <cstdint>
#include <vector>
#include <string>
#include <cstdint> // For uintptr_t

#include <cstdint> // For uintptr_t

namespace RobloxOffsets {

    inline constexpr uintptr_t Rebase = 0x400000;

    namespace Player {
        
        inline constexpr uintptr_t Player = 0x130;
        inline constexpr uintptr_t Character = 0x360; // its called modelinstance
        inline constexpr uintptr_t WalkSpeed = 0x1D4; // its called huamnoid
        inline constexpr uintptr_t Health = 0x194; // its called huamnoid
    }

    namespace Game {
        inline constexpr uintptr_t VisualEngineToDatamodel = 0x700;
        inline constexpr uintptr_t VisualEnginePointer = 0x7A681F0;
        inline constexpr uintptr_t FakeDataModelPointer = 0x7D02458;
        inline constexpr uintptr_t FakeDatamodelToRealDatamodel = 0x1C0; // v
        inline constexpr uintptr_t GameLoaded = 0x638;

    }

    namespace Children {
        inline constexpr uintptr_t ChildrenStart = 0x70; // F"Children" or childrenstart
        inline constexpr uintptr_t ChildrenEnd = 0x8;    // "ChildrenEnd" or children size
    }

    namespace Misc {
        inline constexpr uintptr_t Name = 0xB0;
        inline constexpr uintptr_t StringLength = 0x10;   //  "StringLength" or name size in offset dumps
    }

    namespace Camera {
    }

    namespace Workspace {
        inline constexpr uintptr_t Position = 0xE4;
        inline constexpr uintptr_t Primitive = 0x148; // stores shit for position orientation stuff liek that i think

    }

    namespace Humanoid {
    }
} // namespace RobloxOffsets