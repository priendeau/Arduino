#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>             // Arduino SPI library
 
// ST7789 TFT module connections
#define TFT_CS                10  // define chip select pin
#define TFT_DC                9  // define data/command pin
#define TFT_RST               8  // define reset pin, or set to -1 and connect to Arduino RESET pin

#define NONE_WAIT             0
#define SHORT_WAIT            700
#define NORMAL_WAIT           1400
#define REAL_LONG_WAIT        5000

/*
Definition of some personale color : 
 
*/
#define ST77XX_LIGHTGREY      0xA537
#define ST77XX_DARKGREY       0x4AAC 
#define ST77XX_ORANGE         0xD365 

/*
This is a Reference color from :

  github.com/ayushoriginal/Optimized-RGB-To-ColorName 
  and from image from rgb2 to color name . 
*/


/*RED from RGB2 API Name */


#define ST77XX_RED_MARASCHINO     0xE1E7 
#define ST77XX_RED_CAYENNE        0xA107

/* PURPLE from RGB2 API Name */
#define ST77XX_PURPLE_MAROON      0x7946
#define ST77XX_PURPLE_PLUM        0x8272
#define ST77XX_PURPLE_EGGPLANT    0x61EF
#define ST77XX_PURPLE_GRAPE       0x8150
#define ST77XX_PURPLE_ORCHID      0x9B52
#define ST77XX_PURPLE_LAVENDER    0xACD8
     

/*  from RGB2 API Name */
#define ST77XX_PINK_CARNATION     0xEC53 
#define ST77XX_PINK_STRAWBERRY    0xD08F 
#define ST77XX_PINK_BUBBLEGUM     0xE475 
#define ST77XX_PINK_MAGENTA       0xE030 
#define ST77XX_PINK_SALMON        0xE3CE 

#define ST77XX_ORANGE_TANGERINE   0xEBC4 // ED7A29 EBC4 -> EF7B28  from E3C4 to EBC4
#define ST77XX_ORANGE_CANTALOUPE  0xECCE // F69C79->ECCE F69C79-> ECEE, from ECCE to ECEE 

#define ST77XX_YELLOW_BANANA      0xEE81 // F3D60C 
#define ST77XX_YELLOW_LEMON       0xE701 // E7E60F EAE613 from E701 to E702

#define ST77XX_GREEN_HONEYDEW     0xC6CC // CDDB6A
#define ST77XX_GREEN_LIME         0x65E7 /* -> 0x65C041 0x6DC7 -> 6BBD41 */
#define ST77XX_GREEN_SPRING       0xC6CC //11AF50
#define ST77XX_GREEN_CLOVER       0x0C6C //099163
#define ST77XX_GREEN_FERN         0x0329 //04664D
#define ST77XX_GREEN_MOSS         0x3CE8 //40A049
#define ST77XX_GREEN_FLORA        0xA667 //ABCF3B
#define ST77XX_GREEN_SEAMFOAM     0x6E15 //6BC3B3

#define ST77XX_BLUE_SPINDRIFT     0x8E59 //90CBCF
#define ST77XX_BLUE_TEAL          0x0434 //0588A6
#define ST77XX_BLUE_SKY           0x0458 //038BC7
#define ST77XX_BLUE_TURQUOISE     0x15D6 //13BCB7


/*
This is a 6-bit palette coded in RGB-565 . 

*/

#define ST77XX_DARK_RED       0x5000
#define ST77XX_RED            0xA000
#define ST77XX_LIGHT_RED      0xF800

#define ST77XX_DARK_GREEN     0x02A0
#define ST77XX_GREEN          0x0540
#define ST77XX_LIGHT_GREEN    0x07E0

#define ST77XX_DARK_BLUE      0x000A
#define ST77XX_BLUE           0x0014
#define ST77XX_LIGHT_BLUE     0x001F

#define ST77XX_DARK_CYAN      0x02AA
#define ST77XX_CYAN           0x0554
#define ST77XX_LIGHT_CYAN     0x07FF

#define ST77XX_DARK_MAGENTA   0x500A
#define ST77XX_MAGENTA        0xA014
#define ST77XX_LIGHT_MAGENTA  0xF81F

#define ST77XX_DARK_YELLOW    0x52A0
#define ST77XX_YELLOW         0xA540
#define ST77XX_LIGHT_YELLOW   0xFFE0

#define ST77XX_DARK_GREY      0x52AA
#define ST77XX_LIGHT_GREY     0xA554
#define ST77XX_WHITE          0xFFFF



/*
Mostly all SVG and X11 color name  
color index name in RGB-565
*/

#define ST77XX_ALICEBLUE            0xEFBF // F0F8FF
#define ST77XX_ANTIQUEWHITE         0xF75A // FAEBD7
#define ST77XX_AQUA                 0x07FF // 00FFFF
#define ST77XX_AQUAMARINE           0x7FF9 // 7FFFD4
#define ST77XX_AZURE                0xF7FF // F0FFFF 
#define ST77XX_BEIGE                0xF7BB // F5F5DC 
#define ST77XX_BISQUE               0xFF38 // FFE4C4 
#define ST77XX_BLACK                0x0000 // 000000 
#define ST77XX_BLANCHEDALMOND       0xFF59 // FFEBCD 
#define ST77XX_BLUE                 0x001F // 0000FF 
#define ST77XX_BLUEVIOLET           0x895C // 8A2BE2 
#define ST77XX_BROWN                0xA545 // A52A2A 
#define ST77XX_BURLYWOOD            0xDDD0 // DEB887 
#define ST77XX_CADETBLUE            0x5DF4 // 5F9EA0 
#define ST77XX_CHARTREUSE           0x7FE0 // 7FFF00 
#define ST77XX_CHOCOLATE            0xD343 // D2691E 
#define ST77XX_CORAL                0xFFEA // FF7F50 
#define ST77XX_CORNFLOWERBLUE       0x64BD // 6495ED 
#define ST77XX_CORNSILK             0xFFDB // FFF8DC 
#define ST77XX_CRIMSON              0xD087 // DCA7 D087 DC143C 
#define ST77XX_CYAN                 0x07FF // 00FFFF 
#define ST77XX_DARKBLUE             0x0011 // 00008B 
#define ST77XX_DARKCYAN             0x0451 // 008B8B 
#define ST77XX_DARKGOLDENROD        0xBC21 // B8860B 
#define ST77XX_DARKGRAY             0xAD55 // A9A9A9 
#define ST77XX_DARKKHAKI            0xBDAD // BDB76B 
#define ST77XX_DARKMAGENTA          0x8911 // 8B008B 
#define ST77XX_DARKOLIVEGREEN       0x5745 // 556B2F 
#define ST77XX_DARKORANGE           0xFD60 // FF8C00 
#define ST77XX_DARKORCHID           0x9999 // 9932CC 
#define ST77XX_DARKRED              0x8900 // 8B0000 
#define ST77XX_DARKSALMON           0xEDAF // E9967A 
#define ST77XX_DARKSEAGREEN         0x8DF1 // 8FBC8F 
#define ST77XX_DARKSLATEBLUE        0x49F1 // 483D8B 
#define ST77XX_DARKSLATEGRAY        0x2F69 // 2F4F4F 
#define ST77XX_DARKTURQUOISE        0x067A // 00CED1 
#define ST77XX_DARKVIOLET           0x941A // 9400D3 
#define ST77XX_DEEPPINK             0xFDB2 // FF1493 
#define ST77XX_DEEPSKYBLUE          0x05FF // 00BFFF 
#define ST77XX_DIMGRAY              0x6B4D // 696969 
#define ST77XX_DODGERBLUE           0x1C9F // 1E90FF 
#define ST77XX_FIREBRICK            0xB104 // B22222 
#define ST77XX_FLORALWHITE          0xFFDE // FFFAF0 
#define ST77XX_FORESTGREEN          0x2444 // 228B22 
#define ST77XX_FUCHSIA              0xFD1F // FF00FF 
#define ST77XX_GAINSBORO            0xDEFB // DCDCDC 
#define ST77XX_GHOSTWHITE           0xFFDF // F8F8FF 
#define ST77XX_GOLD                 0xFFA0 // FFD700 
#define ST77XX_GOLDENROD            0xDD24 // DAA520 
#define ST77XX_GRAY                 0x8410 // 808080 
#define ST77XX_GREEN                0x0400 // 008000 
#define ST77XX_GREENYELLOW          0xAFE5 // ADFF2F 
#define ST77XX_HONEYDEW             0xF7FE // F0FFF0 
#define ST77XX_HOTPINK              0xFF56 // FF69B4 
#define ST77XX_INDIANRED            0xCFEB // CD5C5C 
#define ST77XX_INDIGO               0x4910 // 4B0082 
#define ST77XX_IVORY                0xFFFE // FFFFF0 
#define ST77XX_KHAKI                0xF731 // F0E68C 
#define ST77XX_LAVENDER             0xE73F // E6E6FA 
#define ST77XX_LAVENDERBLUSH        0xFF9E // FFF0F5 
#define ST77XX_LAWNGREEN            0x7FE0 // 7CFC00 
#define ST77XX_LEMONCHIFFON         0xFFD9 // FFFACD 
#define ST77XX_LIGHTBLUE            0xAFDC // ADD8E6 
#define ST77XX_LIGHTCORAL           0xF410 // F08080 
#define ST77XX_LIGHTCYAN            0xE7FF // E0FFFF 
#define ST77XX_LIGHTGOLDENRODYELLOW 0xFFDA // FAFAD2 
#define ST77XX_LIGHTGRAY            0xD79A // D3D3D3 
#define ST77XX_LIGHTGREEN           0x9772 // 90EE90 
#define ST77XX_LIGHTPINK            0xFDB8 // FFB6C1 
#define ST77XX_LIGHTSALMON          0xFD0F // FFA07A 
#define ST77XX_LIGHTSEAGREEN        0x2595 // 20B2AA 
#define ST77XX_LIGHTSKYBLUE         0x877F // 87CEFA 
#define ST77XX_LIGHTSLATEGRAY       0x7553 // 778899 
#define ST77XX_LIGHTSTEELBLUE       0xB63B // B0C4DE 
#define ST77XX_LIGHTYELLOW          0xFFFC // FFFFE0 
#define ST77XX_LIME                 0x07E0 // 00FF00 
#define ST77XX_MAGENTA              0xFD1F // FF00FF 
#define ST77XX_MAROON               0x8000 // 800000 
#define ST77XX_MEDIUMAQUAMARINE     0x6675 // 66CDAA 
#define ST77XX_MEDIUMBLUE           0x0019 // 0000CD 
#define ST77XX_MEDIUMORCHID         0xBABA // BA55D3 
#define ST77XX_MEDIUMPURPLE         0x939B // 9370DB 
#define ST77XX_MEDIUMSEAGREEN       0x3D8E // 3CB371 
#define ST77XX_MEDIUMSLATEBLUE      0x7B5D // 7B68EE 
#define ST77XX_MEDIUMSPRINGGREEN    0x07D3 // 00FA9A 
#define ST77XX_MEDIUMTURQUOISE      0x4E99 // 48D1CC 
#define ST77XX_MEDIUMVIOLETRED      0xC5B0 // C71585 
#define ST77XX_MIDNIGHTBLUE         0x19CE // 191970 
#define ST77XX_MINTCREAM            0xF7FF // F5FFFA 
#define ST77XX_MISTYROSE            0xFF3C // FFE4E1 
#define ST77XX_MOCCASIN             0xFF36 // FFE4B5 
#define ST77XX_NAVAJOWHITE          0xFFF5 // FFDEAD 
#define ST77XX_NAVY                 0x0010 // 000080 
#define ST77XX_OLDLACE              0xFFBC // FDF5E6 
#define ST77XX_OLIVE                0x8400 // 808000 
#define ST77XX_OLIVEDRAB            0x6D64 // 6B8E23 
#define ST77XX_ORANGE               0xFD20 // FFA500 
#define ST77XX_ORANGERED            0xFF20 // FF4500 
#define ST77XX_ORCHID               0xDB9A // DA70D6 
#define ST77XX_PALEGOLDENROD        0xEF55 // EEE8AA 
#define ST77XX_PALEGREEN            0x9FD3 // 98FB98 
#define ST77XX_PALETURQUOISE        0xAF7D // AFEEEE 
#define ST77XX_PALEVIOLETRED        0xDB92 // DB7093 
#define ST77XX_PAPAYAWHIP           0xFF7A // FFEFD5 
#define ST77XX_PEACHPUFF            0xFFD7 // FFDAB9 
#define ST77XX_PERU                 0xCD27 // CD853F 
#define ST77XX_PINK                 0xFF19 // FFC0CB 
#define ST77XX_PLUM                 0xDD1B // DDA0DD 
#define ST77XX_POWDERBLUE           0xB71C  // B0E0E6 
#define ST77XX_PURPLE               0x8010 // 800080 
#define ST77XX_ROSYBROWN            0xBC71 // BC8F8F 
#define ST77XX_ROYALBLUE            0x435C // 4169E1 
#define ST77XX_SADDLEBROWN          0x8B22 // 8B4513 
#define ST77XX_SALMON               0xFC0E // FA8072 
#define ST77XX_SANDYBROWN           0xF52C // F4A460 
#define ST77XX_SEAGREEN             0x2C4A // 2E8B57 
#define ST77XX_SEASHELL             0xFFBD // FFF5EE 
#define ST77XX_SIENNA               0xA285 // A0522D 
#define ST77XX_SILVER               0xC618 // C0C0C0 
#define ST77XX_SKYBLUE              0x877D // 87CEEB 
#define ST77XX_SLATEBLUE            0x6AD9 // 6A5ACD 
#define ST77XX_SLATEGRAY            0x7412 // 708090 
#define ST77XX_SNOW                 0xFFDF // FFFAFA 
#define ST77XX_SPRINGGREEN          0x07EF // 00FF7F 
#define ST77XX_STEELBLUE            0x4416 // 4682B4 
#define ST77XX_TAN                  0xD5B1 // D2B48C 
#define ST77XX_TEAL                 0x0410 // 008080 
#define ST77XX_THISTLE              0xDDFB // D8BFD8 
#define ST77XX_TOMATO               0xFF08 // FF6347 
#define ST77XX_TURQUOISE            0x471A // 40E0D0 
#define ST77XX_VIOLET               0xEC1D // EE82EE 
#define ST77XX_WHEAT                0xF7F6 // F5DEB3 
#define ST77XX_WHITE                0xFFFF // FFFFFF 
#define ST77XX_WHITESMOKE           0xF7BE // F5F5F5 
#define ST77XX_YELLOW               0xFFE0 // FFFF00 
#define ST77XX_YELLOWGREEN          0x9E66 // 9ACD32

//#define ST77XX_RED                  0xFD00 // FF0000 


long ArrayColorDef[  ] =    { ST77XX_ALICEBLUE,         ST77XX_ANTIQUEWHITE,        ST77XX_AQUA,
                              ST77XX_AQUAMARINE,        ST77XX_AZURE,ST77XX_BEIGE,  ST77XX_BISQUE,
                              ST77XX_BLACK,             ST77XX_BLANCHEDALMOND,      ST77XX_BLUE,
                              ST77XX_BLUE_SKY,          ST77XX_BLUE_SPINDRIFT,      ST77XX_BLUE_TEAL,
                              ST77XX_BLUE_TURQUOISE,    ST77XX_BLUEVIOLET,          ST77XX_BROWN,
                              ST77XX_BURLYWOOD,         ST77XX_CADETBLUE,           ST77XX_CHARTREUSE,
                              ST77XX_CHOCOLATE,         ST77XX_CORAL,               ST77XX_CORNFLOWERBLUE,
                              ST77XX_CORNSILK,          ST77XX_CRIMSON,             ST77XX_CYAN,
                              ST77XX_DARK_BLUE,         ST77XX_DARKBLUE,            ST77XX_DARK_CYAN,
                              ST77XX_DARKCYAN,          ST77XX_DARKGOLDENROD,       ST77XX_DARKGRAY,
                              ST77XX_DARK_GREEN,        ST77XX_DARK_GREY,           ST77XX_DARKGREY,
                              ST77XX_DARKKHAKI,         ST77XX_DARK_MAGENTA,        ST77XX_DARKMAGENTA,
                              ST77XX_DARKOLIVEGREEN,    ST77XX_DARKORANGE,          ST77XX_DARKORCHID,
                              ST77XX_DARK_RED,          ST77XX_DARKRED,             ST77XX_DARKSALMON,
                              ST77XX_DARKSEAGREEN,      ST77XX_DARKSLATEBLUE,       ST77XX_DARKSLATEGRAY,
                              ST77XX_DARKTURQUOISE,     ST77XX_DARKVIOLET,          ST77XX_DARK_YELLOW,
                              ST77XX_DEEPPINK,          ST77XX_DEEPSKYBLUE,         ST77XX_DIMGRAY,
                              ST77XX_DODGERBLUE,        ST77XX_FIREBRICK,           ST77XX_FLORALWHITE,
                              ST77XX_FORESTGREEN,       ST77XX_FUCHSIA,             ST77XX_GAINSBORO,
                              ST77XX_GHOSTWHITE,        ST77XX_GOLD,                ST77XX_GOLDENROD,
                              ST77XX_GRAY,              ST77XX_GREEN,               ST77XX_GREEN_CLOVER,
                              ST77XX_GREEN_FERN,        ST77XX_GREEN_FLORA,         ST77XX_GREEN_HONEYDEW,
                              ST77XX_GREEN_LIME,        ST77XX_GREEN_MOSS,          ST77XX_GREEN_SEAMFOAM,
                              ST77XX_GREEN_SPRING,      ST77XX_GREENYELLOW,         ST77XX_HONEYDEW,
                              ST77XX_HOTPINK,           ST77XX_INDIANRED,           ST77XX_INDIGO,
                              ST77XX_IVORY,             ST77XX_KHAKI,               ST77XX_LAVENDER,
                              ST77XX_LAVENDERBLUSH,     ST77XX_LAWNGREEN,           ST77XX_LEMONCHIFFON,
                              ST77XX_LIGHT_BLUE,        ST77XX_LIGHTBLUE,           ST77XX_LIGHTCORAL,
                              ST77XX_LIGHT_CYAN,        ST77XX_LIGHTCYAN,           ST77XX_LIGHTGOLDENRODYELLOW,
                              ST77XX_LIGHTGRAY,         ST77XX_LIGHT_GREEN,         ST77XX_LIGHTGREEN,
                              ST77XX_LIGHT_GREY,        ST77XX_LIGHTGREY,           ST77XX_LIGHT_MAGENTA,
                              ST77XX_LIGHTPINK,         ST77XX_LIGHT_RED,           ST77XX_LIGHTSALMON,
                              ST77XX_LIGHTSEAGREEN,     ST77XX_LIGHTSKYBLUE,        ST77XX_LIGHTSLATEGRAY,
                              ST77XX_LIGHTSTEELBLUE,    ST77XX_LIGHT_YELLOW,        ST77XX_LIGHTYELLOW,
                              ST77XX_LIME,              ST77XX_MAGENTA,             ST77XX_MAROON,
                              ST77XX_MEDIUMAQUAMARINE,  ST77XX_MEDIUMBLUE,          ST77XX_MEDIUMORCHID,
                              ST77XX_MEDIUMPURPLE,      ST77XX_MEDIUMSEAGREEN,      ST77XX_MEDIUMSLATEBLUE,
                              ST77XX_MEDIUMSPRINGGREEN, ST77XX_MEDIUMTURQUOISE,     ST77XX_MEDIUMVIOLETRED,
                              ST77XX_MIDNIGHTBLUE,      ST77XX_MINTCREAM,           ST77XX_MISTYROSE,
                              ST77XX_MOCCASIN,          ST77XX_NAVAJOWHITE,         ST77XX_NAVY,
                              ST77XX_OLDLACE,           ST77XX_OLIVE,               ST77XX_OLIVEDRAB,
                              ST77XX_ORANGE,            ST77XX_ORANGE_CANTALOUPE,   ST77XX_ORANGERED,
                              ST77XX_ORANGE_TANGERINE,  ST77XX_ORCHID,              ST77XX_PALEGOLDENROD,
                              ST77XX_PALEGREEN,         ST77XX_PALETURQUOISE,       ST77XX_PALEVIOLETRED,
                              ST77XX_PAPAYAWHIP,        ST77XX_PEACHPUFF,           ST77XX_PERU,
                              ST77XX_PINK,              ST77XX_PINK_BUBBLEGUM,      ST77XX_PINK_CARNATION,
                              ST77XX_PINK_MAGENTA,      ST77XX_PINK_SALMON,         ST77XX_PINK_STRAWBERRY,
                              ST77XX_PLUM,              ST77XX_POWDERBLUE,          ST77XX_PURPLE,
                              ST77XX_PURPLE_EGGPLANT,   ST77XX_PURPLE_GRAPE,        ST77XX_PURPLE_LAVENDER,
                              ST77XX_PURPLE_MAROON,     ST77XX_PURPLE_ORCHID,       ST77XX_PURPLE_PLUM,
                              ST77XX_RED,               ST77XX_RED_CAYENNE,         ST77XX_RED_MARASCHINO,
                              ST77XX_ROSYBROWN,         ST77XX_ROYALBLUE,           ST77XX_SADDLEBROWN,
                              ST77XX_SALMON,            ST77XX_SANDYBROWN,          ST77XX_SEAGREEN,
                              ST77XX_SEASHELL,          ST77XX_SIENNA,              ST77XX_SILVER,
                              ST77XX_SKYBLUE,           ST77XX_SLATEBLUE,           ST77XX_SLATEGRAY,
                              ST77XX_SNOW,              ST77XX_SPRINGGREEN,         ST77XX_STEELBLUE,
                              ST77XX_TAN,               ST77XX_TEAL,                ST77XX_THISTLE,
                              ST77XX_TOMATO,            ST77XX_TURQUOISE,           ST77XX_VIOLET,
                              ST77XX_WHEAT,             ST77XX_WHITE,               ST77XX_WHITESMOKE,
                              ST77XX_YELLOW,            ST77XX_YELLOW_BANANA,       ST77XX_YELLOWGREEN,
                              ST77XX_YELLOW_LEMON } ;  
/*old configuration { ST77XX_RED_MARASCHINO     ,ST77XX_RED_CAYENNE      , ST77XX_ALICEBLUE,
                            ST77XX_PURPLE_MAROON       ,ST77XX_PURPLE_PLUM      , ST77XX_ANTIQUEWHITE,
                            ST77XX_PURPLE_EGGPLANT     ,ST77XX_PURPLE_GRAPE     , ST77XX_AQUA,
                            ST77XX_PURPLE_ORCHID       ,ST77XX_PURPLE_LAVENDER  , ST77XX_AQUAMARINE,
                            ST77XX_PINK_CARNATION      ,ST77XX_PINK_STRAWBERRY  , ST77XX_AZURE,
                            ST77XX_PINK_BUBBLEGUM      ,ST77XX_PINK_MAGENTA     , ST77XX_BEIGE,
                            ST77XX_PINK_SALMON         ,ST77XX_ORANGE_TANGERINE , ST77XX_BISQUE,
                            ST77XX_ORANGE_CANTALOUPE   ,ST77XX_YELLOW_BANANA    , ST77XX_BLACK,
                            ST77XX_YELLOW_LEMON        ,ST77XX_GREEN_HONEYDEW   , ST77XX_BLANCHEDALMOND,
                            ST77XX_GREEN_LIME          ,ST77XX_GREEN_SPRING     , ST77XX_BLUE,
                            ST77XX_GREEN_CLOVER        ,ST77XX_GREEN_FERN       , ST77XX_BLUEVIOLET,
                            ST77XX_GREEN_MOSS          ,ST77XX_GREEN_FLORA      , ST77XX_BROWN,
                            ST77XX_GREEN_SEAMFOAM      ,ST77XX_BLUE_SPINDRIFT   , ST77XX_BURLYWOOD,
                            ST77XX_BLUE_TEAL           ,ST77XX_BLUE_SKY         , ST77XX_CADETBLUE,
                            ST77XX_BLUE_TURQUOISE      ,ST77XX_DARK_RED         , ST77XX_CHARTREUSE,
                            ST77XX_RED                 ,ST77XX_LIGHT_RED        , ST77XX_CHOCOLATE,
                            ST77XX_DARK_GREEN          ,ST77XX_GREEN            , ST77XX_CORAL,
                            ST77XX_LIGHT_GREEN         ,ST77XX_DARK_BLUE        , ST77XX_CORNFLOWERBLUE,
                            ST77XX_BLUE                ,ST77XX_LIGHT_BLUE       , ST77XX_CORNSILK,
                            ST77XX_DARK_CYAN           ,ST77XX_CYAN             , ST77XX_CRIMSON,
                            ST77XX_LIGHT_CYAN          ,ST77XX_DARK_MAGENTA     , ST77XX_CYAN,
                            ST77XX_MAGENTA             ,ST77XX_LIGHT_MAGENTA    , ST77XX_DARKBLUE,
                            ST77XX_DARK_YELLOW         ,ST77XX_YELLOW           , ST77XX_DARKCYAN,
                            ST77XX_LIGHT_YELLOW        ,ST77XX_DARK_GREY        , ST77XX_DARKGOLDENROD,
                            ST77XX_LIGHT_GREY          ,ST77XX_WHITE            , ST77XX_DARKGRAY,
                            ST77XX_DARKKHAKI           ,ST77XX_DARKMAGENTA      , ST77XX_DARKOLIVEGREEN,
                            ST77XX_DARKORANGE          ,ST77XX_DARKORCHID       , ST77XX_DARKRED,
                            ST77XX_DARKSALMON          ,ST77XX_DARKSEAGREEN     , ST77XX_DARKSLATEBLUE,
                            ST77XX_DARKSLATEGRAY       ,ST77XX_LIGHTGREY        , ST77XX_DARKGREY,
                            ST77XX_ORANGE              ,ST77XX_AQUAMARINE       , ST77XX_AZURE, 
                            ST77XX_BEIGE               ,ST77XX_BISQUE           , ST77XX_BLACK, 
                            ST77XX_BLANCHEDALMOND      ,ST77XX_BLUE             , ST77XX_BLUEVIOLET, 
                            ST77XX_BROWN               ,ST77XX_BURLYWOOD        , ST77XX_CADETBLUE, 
                            ST77XX_CHARTREUSE          ,ST77XX_CHOCOLATE        , ST77XX_CORAL, 
                            ST77XX_CORNFLOWERBLUE      ,ST77XX_CORNSILK         , ST77XX_CRIMSON, 
                            ST77XX_CYAN                ,ST77XX_DARKBLUE         , ST77XX_DARKCYAN, 
                            ST77XX_DARKGOLDENROD       ,ST77XX_DARKGRAY         , ST77XX_DARKKHAKI, 
                            ST77XX_DARKMAGENTA         ,ST77XX_DARKOLIVEGREEN   , ST77XX_DARKORANGE, 
                            ST77XX_DARKORCHID          ,ST77XX_DARKRED          , ST77XX_DARKSALMON, 
                            ST77XX_DARKSEAGREEN        ,ST77XX_DARKSLATEBLUE    , ST77XX_DARKSLATEGRAY, 
                            ST77XX_DARKTURQUOISE       ,ST77XX_DARKVIOLET       , ST77XX_DEEPPINK, 
                            ST77XX_DEEPSKYBLUE         ,ST77XX_DIMGRAY          , ST77XX_DODGERBLUE, 
                            ST77XX_FIREBRICK           ,ST77XX_FLORALWHITE      , ST77XX_FORESTGREEN, 
                            ST77XX_FUCHSIA             ,ST77XX_GAINSBORO        , ST77XX_GHOSTWHITE, 
                            ST77XX_GOLD                ,ST77XX_GOLDENROD        , ST77XX_GRAY, 
                            ST77XX_GREEN               ,ST77XX_GREENYELLOW      , ST77XX_HONEYDEW, 
                            ST77XX_HOTPINK             ,ST77XX_INDIANRED        , ST77XX_INDIGO, 
                            ST77XX_IVORY               ,ST77XX_KHAKI            , ST77XX_LAVENDER, 
                            ST77XX_LAVENDERBLUSH       ,ST77XX_LAWNGREEN        , ST77XX_LEMONCHIFFON, 
                            ST77XX_LIGHTBLUE           ,ST77XX_LIGHTCORAL       , ST77XX_LIGHTCYAN, 
                            ST77XX_LIGHTGOLDENRODYELLOW,ST77XX_LIGHTGRAY        , ST77XX_LIGHTGREEN, 
                            ST77XX_LIGHTPINK           ,ST77XX_LIGHTSALMON      , ST77XX_LIGHTSEAGREEN, 
                            ST77XX_LIGHTSKYBLUE        ,ST77XX_LIGHTSLATEGRAY   , ST77XX_LIGHTSTEELBLUE, 
                            ST77XX_LIGHTYELLOW         ,ST77XX_LIME             , ST77XX_MAGENTA, 
                            ST77XX_MAROON              ,ST77XX_MEDIUMAQUAMARINE , ST77XX_MEDIUMBLUE, 
                            ST77XX_MEDIUMORCHID        ,ST77XX_MEDIUMPURPLE     , ST77XX_MEDIUMSEAGREEN, 
                            ST77XX_MEDIUMSLATEBLUE     ,ST77XX_MEDIUMSPRINGGREEN, ST77XX_MEDIUMTURQUOISE, 
                            ST77XX_MEDIUMVIOLETRED     ,ST77XX_MIDNIGHTBLUE     , ST77XX_MINTCREAM, 
                            ST77XX_MISTYROSE           ,ST77XX_MOCCASIN         , ST77XX_NAVAJOWHITE, 
                            ST77XX_NAVY                ,ST77XX_OLDLACE          , ST77XX_OLIVE, 
                            ST77XX_OLIVEDRAB           ,ST77XX_ORANGE           , ST77XX_ORANGERED, 
                            ST77XX_ORCHID              ,ST77XX_PALEGOLDENROD    , ST77XX_PALEGREEN, 
                            ST77XX_PALETURQUOISE       ,ST77XX_PALEVIOLETRED    , ST77XX_PAPAYAWHIP, 
                            ST77XX_PEACHPUFF           ,ST77XX_PERU             , ST77XX_PINK, 
                            ST77XX_PLUM                ,ST77XX_POWDERBLUE       , ST77XX_PURPLE, 
                            ST77XX_RED                 ,ST77XX_ROSYBROWN        , ST77XX_ROYALBLUE, 
                            ST77XX_SADDLEBROWN         ,ST77XX_SALMON           , ST77XX_SANDYBROWN, 
                            ST77XX_SEAGREEN            ,ST77XX_SEASHELL         , ST77XX_SIENNA, 
                            ST77XX_SILVER              ,ST77XX_SKYBLUE          , ST77XX_SLATEBLUE, 
                            ST77XX_SLATEGRAY           ,ST77XX_SNOW             , ST77XX_SPRINGGREEN, 
                            ST77XX_STEELBLUE           ,ST77XX_TAN              , ST77XX_TEAL, 
                            ST77XX_THISTLE             ,ST77XX_TOMATO           , ST77XX_TURQUOISE, 
                            ST77XX_VIOLET              ,ST77XX_WHEAT            , ST77XX_WHITE, 
                            ST77XX_WHITESMOKE          ,ST77XX_YELLOW           , ST77XX_YELLOWGREEN } ; 
*/ 

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>             // Arduino SPI library
 
// ST7789 TFT module connections
#define USE_SERIAL_DEBUG      1

#define TFT_CS               10  // define chip select pin
#define TFT_DC                9  // define data/command pin
#define TFT_RST               8  // define reset pin, or set to -1 and connect to Arduino RESET pin

#define NONE_WAIT             0
#define INTERNAL_WAIT        10
#define SHORT_WAIT          700
#define STANDARD_WAIT      1000
#define NORMAL_WAIT        1400
#define REAL_LONG_WAIT     5000

#define NB_RANDOM_COLOR       3
#define MIN_CIRCLE_INSIZE     4
#define MAX_NB_CIRCLE        14
#define PI_CONSTANT           3.141592654

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
 
float flPi = 3.141592654;
long  *lRandColor ; 
int   iColorVal, iCx, iSizeArrayColor; 

void setup(void) 
{
  Serial.begin(9600);
  delay( INTERNAL_WAIT );
  
  randomSeed( analogRead(0) );
  delay( INTERNAL_WAIT );

#ifdef USE_SERIAL_DEBUG  
  Serial.println(F("ST7789 Multimedia test"));
  delay( INTERNAL_WAIT );
#endif  
 
  // if the display has CS pin try with SPI_MODE0
  tft.init(240, 240, SPI_MODE3);    // Init ST7789 display 240x240 pixel
 
  // if the screen is flipped, remove this command
  //tft.setRotation(2);
 
#ifdef USE_SERIAL_DEBUG  
  Serial.println(F("Initialized"));
  delay( INTERNAL_WAIT );
#endif  
 
  uint16_t time = millis();
  
  time = millis() - time;
 
#ifdef USE_SERIAL_DEBUG  
  Serial.println( "Start Time :" + String(time, DEC) );
  delay( INTERNAL_WAIT );
#endif  
  
  // large block of text
  tft.fillScreen(ST77XX_BLACK);

#ifdef USE_SERIAL_DEBUG  
  Serial.print( String("Color Mixer with ") +String(NB_RANDOM_COLOR  )+ String(" color(s)\n") ) ; 
  delay( INTERNAL_WAIT ); 
#endif 
  

}

void loop() 
{

  /*
  Doing the pointer declaration here from loop because 
  the setup seems to let the pointer pass and not being considered.  
  */
#ifdef _ESP32_HAL_I2C_H_ // For ESP32
  /*
    Apparently the ESP32 Board does not accept to directly allow an malloc operation
    over lRandColor into DEFAULT long int* it try ( void *) because it focus on malloc 
    and not the type . 
  */
  lRandColor = static_cast< long int* >(  malloc( sizeof( long ) * NB_RANDOM_COLOR ) ) ;
#else
  lRandColor = malloc( sizeof( long ) * NB_RANDOM_COLOR ) ; 
#endif


  iSizeArrayColor=(sizeof( ArrayColorDef ) / sizeof( ArrayColorDef[0] )) ; 
  delay( INTERNAL_WAIT ); 

#ifdef USE_SERIAL_DEBUG  
  Serial.println( "Number of colors " + String( iSizeArrayColor  )) ; 
  delay( INTERNAL_WAIT );
#endif  
  /*Adding NB_RANDOM_COLOR inside lRandColor */
#ifdef USE_SERIAL_DEBUG  
  Serial.println( "Color choosed :" );
#endif  
  for( iCx=0 ; iCx <=  NB_RANDOM_COLOR-1 ; iCx++ )
  {
    iColorVal=random(1,iSizeArrayColor ) ; 
#ifdef USE_SERIAL_DEBUG  
    Serial.println( String("\tcolor") + String(iCx) + String(" :") + String( ArrayColorDef[iColorVal] ) + String( ", HEX(565): " )+ String( ArrayColorDef[iColorVal], HEX ) ) ;
#endif  
    *((lRandColor)+(iCx)) = ArrayColorDef[iColorVal] ;
    delay( INTERNAL_WAIT ); 
  }

  tft.fillScreen(ST77XX_BLACK);
  
  MediaButtons( lRandColor );
  
  delay( STANDARD_WAIT * 7 ); 
  
  DemoCircle( MAX_NB_CIRCLE, MIN_CIRCLE_INSIZE, lRandColor ) ; 

  delay(STANDARD_WAIT * 2 );

  tft.fillScreen(ST77XX_BLACK);  

  delete( lRandColor ) ; 

}

void DemoCircle( int iMaxLoop, int iMinCircle, long *pArrayCol )
{
  int intY=iMaxLoop ;
  int intX ;  
#ifdef USE_SERIAL_DEBUG  
  Serial.println( "Color in DemoCircle : \n\tColor: " + String( *(pArrayCol+0), HEX  ) + String("\n") + 
                                                    String("\tColor: ") + String( *(pArrayCol+1 ), HEX  ) + String("\n") + 
                                                    String("\tColor: ") + String( *(pArrayCol+2 ), HEX  ) + String("\n") ) ;
#endif  
  for(intX =iMinCircle ; intX <= iMaxLoop ; intX++)
  {
    intY-- ; 
    if( intY <= iMinCircle )
    { 
     intY+=2 ; 
    }
    
    //ArrayColorDef[iRandColor[0]]
    testfillcircles((4*intX), *(pArrayCol+0)  );
    //ArrayColorDef[iRandColor[1]]
    testdrawcircles((4*intX)-2,*(pArrayCol+1)   );
    //ArrayColorDef[iRandColor[1]]
    testdrawcircles((4*intY)-1, *(pArrayCol+1)  );

    //ArrayColorDef[lRandColor[1]]
    testfillcircles((5*intX), *(pArrayCol+1)  );
    //ArrayColorDef[lRandColor[2]]
    testdrawcircles((5*intX)-4, *(pArrayCol+2) );
    //ArrayColorDef[lRandColor[2]]
    testdrawcircles((5*intY)-2, *(pArrayCol+2) );

    //ArrayColorDef[lRandColor[2]]
    testfillcircles((3*intX), *(pArrayCol+2) );
    //ArrayColorDef[lRandColor[0]]
    testdrawcircles((3*intX)-8, *(pArrayCol+0) );
    //ArrayColorDef[lRandColor[0]]
    testdrawcircles((3*intY)-4, *(pArrayCol+0) );
    
    //testdrawcircles((5*(x/2)), ArrayColorDef[iRandColor[1]] );
    //testfillcircles((10*(x/2)), ArrayColorDef[iRandColor[0]] );
    //testdrawcircles((15*(x/2)), ArrayColorDef[iRandColor[2]] );
   
  }
} 

void testfillcircles(uint8_t radius, uint16_t color) 
{
  for (int16_t x=radius; x < tft.width(); x+=radius*2) 
  {
    for (int16_t y=radius; y < tft.height(); y+=radius*2) 
    {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) 
{
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) 
  {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) 
    {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

 
void testroundrects( int iColor )  
{
  tft.fillScreen(ST77XX_BLACK);
  delay(SHORT_WAIT);
  tft.fillScreen(iColor);
  int color = iColor;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) 
  {
    int x = 0;
    int y = 0;
    int w = tft.width()-2;
    int h = tft.height()-2;
    for(i = 0 ; i <= 16; i+=1) 
    {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}

/*
 Play Button Color flag : ST77XX_CRIMSON
*/
void drawPlayButton( long flagColor = ST77XX_CRIMSON )
{
  tft.fillRoundRect(25, 10, 78, 60, 8, ST77XX_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, flagColor);  
}

/*
 Pause Button Color flag : ST77XX_GREEN
*/
void drawPauseButton( long flagColor = ST77XX_GREEN )
{
  tft.fillRoundRect(25, 90, 78, 60, 8, ST77XX_WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, flagColor);
  tft.fillRoundRect(69, 98, 20, 45, 5, flagColor);
  
}

/*
 Rewind Button Color flag : ST77XX_GREEN_FERN
*/
void drawRewindButton( long flagColor =ST77XX_GREEN_FERN )
{
  //tft.fillRoundRect(25, 10, 78, 60, 8, ST77XX_WHITE);
  //tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_RED);  

  tft.fillRoundRect(133, 10, 78, 60, 8, ST77XX_WHITE);
  tft.fillTriangle(152, 20, 
                   152, 60, 
                   175, 40, flagColor);
  tft.fillTriangle(172, 20, 
                   172, 60, 
                   195, 40, flagColor);
}

void drawFastForwardButton( long flagColor = ST77XX_GREEN_FERN )
{ 
  tft.fillRoundRect(133, 90, 78, 60, 8, ST77XX_WHITE);
  tft.fillTriangle(170, 100, 
                   170, 140, 
                   147, 120, flagColor);
  tft.fillTriangle(190, 100, 
                   190, 140, 
                   167, 120, flagColor);  
} 

void MediaButtons( long *pArrayCol ) 
{
  // play
  tft.fillScreen(ST77XX_BLACK);

  if( sizeof(pArrayCol)/sizeof(long) > 0 )
  {
    drawPlayButton( *(pArrayCol+0) ) ; 
  }
  
  delay(SHORT_WAIT);
  // pause

  /*
  Depending of number of color, for 1 color it 
  draw all button with the color 1.
  if The color mixer mix 2 color at the time 
  button 1 is a color all the rest are the 
  other color. And at 4 color it should give 
  one color by button 
  */

  if( NB_RANDOM_COLOR == 1 )
  {
    drawPlayButton( *(pArrayCol+0) ) ; 
    drawPauseButton( *(pArrayCol+0) ) ;
    drawRewindButton( *(pArrayCol+0) ) ; 
    drawFastForwardButton( *(pArrayCol+0) ) ; 
  }
  if( NB_RANDOM_COLOR == 2 )
  {
    drawPlayButton( *(pArrayCol+0) ) ; 
    drawPauseButton( *(pArrayCol+1) ) ;
    drawRewindButton( *(pArrayCol+2) ) ; 
    drawFastForwardButton( *(pArrayCol+2) ) ; 
  }
  if( NB_RANDOM_COLOR == 3 )
  {
    drawPlayButton( *(pArrayCol+0) ) ; 
    drawPauseButton( *(pArrayCol+1) ) ;
    drawRewindButton( *(pArrayCol+2) ) ; 
    drawFastForwardButton( *(pArrayCol+2) ) ; 
  }
  if( NB_RANDOM_COLOR == 4 )
  {
    drawPlayButton( *(pArrayCol+0) ) ; 
    drawPauseButton( *(pArrayCol+1) ) ;
    drawRewindButton( *(pArrayCol+2) ) ; 
    drawFastForwardButton( *(pArrayCol+3) ) ; 
  }

  
  delay(SHORT_WAIT);
  
}
