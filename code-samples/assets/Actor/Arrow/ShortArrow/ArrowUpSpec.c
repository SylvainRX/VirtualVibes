#include <Arrow.h>
#include <ArrowOrientation.h>
#include <Body.h>
#include <InGameTypes.h>
#include <Macros.h>
#include <BgmapSprite.h>

extern uint32 ArrowUpNormalTiles[];
extern uint16 ArrowUpNormalMap[];
extern uint32 ArrowUpFailedTiles[];
extern uint16 ArrowUpFailedMap[];


CharSetROMSpec ArrowUpNormalCharsetSpec =
{
	// number of chars in function or the number of frames to load at the same time
	3*2,

	// whether it is shared or not
	true,

	// whether the tiles are optimized or not
	false,

	// char spec
	ArrowUpNormalTiles,

	// pointer to the frames offsets
	NULL,
};

TextureROMSpec ArrowUpNormalTextureSpec =
{
	// charset spec
	(CharSetSpec*)&ArrowUpNormalCharsetSpec,

	// bgmap spec
	ArrowUpNormalMap,

	// cols (max 64)
	3,

	// rows (max 64)
	2,

	// padding for affine/hbias transformations
	{ 0, 0 },

	// number of frames
	1,

	// palette number (0-3)
	0,

	// recyclable
	true,

	// vertical flip
	false,

	// horizontal flip
	false,
};

BgmapSpriteROMSpec ArrowUpNormalSpriteSpec =
{
	{
        // VisualComponent
        {
            // Component
            {
                // Allocator
                __TYPE(BgmapSprite),
                // Component type
                kSpriteComponent
            },
            // Array of animation functions
            NULL,
        },
        // Spec for the texture to display
        (TextureSpec*)&ArrowUpNormalTextureSpec,
        // Transparency mode (__TRANSPARENCY_NONE, __TRANSPARENCY_EVEN or __TRANSPARENCY_ODD)
        __TRANSPARENCY_NONE,
        // Displacement added to the sprite's position
        {2, 2, ARROW_DISPLACEMENT_Z, ARROW_DISPLACEMENT_PARALLAX},
	},

    // Flag to indicate in which display to show the texture (__WORLD_ON, __WORLD_LON or __WORLD_RON)
    __WORLD_ON,
    // The display mode (__WORLD_BGMAP, __WORLD_AFFINE or __WORLD_HBIAS)
    __WORLD_BGMAP,
    // Pointer to affine/hbias manipulation function
    NULL
};

CharSetROMSpec ArrowUpFailedCharsetSpec =
{
	// number of chars in function or the number of frames to load at the same time
	3*2,

	// whether it is shared or not
	true,

	// whether the tiles are optimized or not
	false,

	// char spec
	ArrowUpFailedTiles,

	// pointer to the frames offsets
	NULL,
};

TextureROMSpec ArrowUpFailedTextureSpec =
{
	// charset spec
	(CharSetSpec*)&ArrowUpFailedCharsetSpec,

	// bgmap spec
	ArrowUpFailedMap,

	// cols (max 64)
	3,

	// rows (max 64)
	2,

	// padding for affine/hbias transformations
	{ 0, 0 },

	// number of frames
	1,

	// palette number (0-3)
	0,

	// recyclable
	true,

	// vertical flip
	false,

	// horizontal flip
	false,
};

BgmapSpriteROMSpec ArrowUpFailedSpriteSpec =
{
	{
        // VisualComponent
        {
            // Component
            {
                // Allocator
                __TYPE(BgmapSprite),
                // Component type
                kSpriteComponent
            },
            // Array of animation functions
            NULL,
        },
        // Spec for the texture to display
        (TextureSpec*)&ArrowUpFailedTextureSpec,
        // Transparency mode (__TRANSPARENCY_NONE, __TRANSPARENCY_EVEN or __TRANSPARENCY_ODD)
        __TRANSPARENCY_NONE,
        // Displacement added to the sprite's position
        {2, 2, ARROW_DISPLACEMENT_Z, ARROW_DISPLACEMENT_PARALLAX},
	},

    // Flag to indicate in which display to show the texture (__WORLD_ON, __WORLD_LON or __WORLD_RON)
    __WORLD_ON,
    // The display mode (__WORLD_BGMAP, __WORLD_AFFINE or __WORLD_HBIAS)
    __WORLD_BGMAP,
    // Pointer to affine/hbias manipulation function
    NULL
};

ComponentSpec* const ArrowUpActorComponentSpecs[] =
{
	(ComponentSpec*)&ArrowUpNormalSpriteSpec,
	(ComponentSpec*)&ArrowUpFailedSpriteSpec,
	NULL
};

ArrowROMSpec ArrowUpActorSpec =
{
    {
        // Class allocator
        __TYPE(Arrow),
        // Component specs
        (ComponentSpec**)ArrowUpActorComponentSpecs,
        // Children specs
        NULL,
        // Extra info
        NULL,
        // Pixel size (x, y, z)
        {0, 0, 0},
        // In-game type
        kTypeArrow,
        // Animation to play automatically
        NULL,
    },
    kArrowOrientationUp,
};
