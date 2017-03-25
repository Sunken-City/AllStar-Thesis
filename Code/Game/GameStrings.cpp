#include "Game/GameStrings.hpp"
#include "Engine/Math/MathUtils.hpp"

//-----------------------------------------------------------------------------------
const char* GameStrings::GetAwesomeStatString()
{
    switch (MathUtils::GetRandomIntFromZeroTo(10))
    {
    case 0:
        return "Wow!";
    case 1:
        return "Sugoi!";
    case 2:
        return "Great!";
    case 3:
        return "Fancy!";
    case 4:
        return "Incredible!";
    case 5:
        return "Nice!";
    case 6:
        return "Cool!";
    case 7:
        return ":D";
    case 8:
        return "Tryhard!";
    case 9:
        return "Sweet!";
    default:
        return "Meh!";
    }
}

//-----------------------------------------------------------------------------------
const char* GameStrings::GetTerribleStatString()
{
    switch (MathUtils::GetRandomIntFromZeroTo(10))
    {
    case 0:
        return "Oh my!";
    case 1:
        return ":c";
    case 2:
        return "D:";
    case 3:
        return "Ouch!";
    case 4:
        return "Maybe next time!";
    case 5:
        return "You did your best!";
    case 6:
        return "You'll be fine!";
    case 7:
        return "Don't cry!";
    case 8:
        return ";w;";
    case 9:
        return "Uh-oh!";
    default:
        return "Meh!";
    }
}

