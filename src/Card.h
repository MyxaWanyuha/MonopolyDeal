#pragma once
#include <cstdint>
#include <array>
#include <string>

namespace Monopoly
{
    const int g_BirthdayGiftAmount = 2;
    const int g_DebtAmount = 5;

    enum class EActionType
    {
        PassGo = 0,
        DoubleTheRent,
        JustSayNo,
        Hotel,
        House,
        DealBreaker,
        SlyDeal,
        ForcedDeal,
        ItsMyBirthday,
        DebtCollector,
        RentWild,
        RentLightBlueBrown,
        RentOrangePink,
        RentYellowRed,
        RentUtilityRailroad,
        RentBlueGreen,
        None
    };

    enum class EColor : uint32_t
    {
        None       = 0,
        Yellow     = 1 << 0,
        Utility   = 1 << 1,
        Brown      = 1 << 2,
        Blue       = 1 << 3,
        Green      = 1 << 4,
        LightBlue  = 1 << 5,
        Orange     = 1 << 6,
        Pink       = 1 << 7,
        Railroad   = 1 << 8,
        Red        = 1 << 9,
        All = Yellow | Utility | Brown | Blue | Green | LightBlue | Orange | Pink | Railroad | Red
    };

    enum class ECardType
    {
        None = 0,
        Money,
        Property,
        Action,
        Enhancement
    };

    enum class EEnhancementType
    {
        None = 0,
        House,
        Hotel
    };

    class Card
    {
    public:
        Card(std::string&& name, ECardType type, int32_t value, EColor color, EActionType action, EEnhancementType enhancement, bool isFlipped)
            : m_Name(name), m_Type(type), m_Value(value), m_Color(color), m_Action(action), m_Enhancement(enhancement), m_bIsFlipped(isFlipped)
        {
        }

    private:
        std::string m_Name;
        ECardType m_Type = ECardType::None;
        int32_t m_Value = -1;
        EColor m_Color = EColor::None;
        EActionType m_Action = EActionType::None;
        EEnhancementType m_Enhancement = EEnhancementType::None;
        bool m_bIsFlipped = false;
    };

}
