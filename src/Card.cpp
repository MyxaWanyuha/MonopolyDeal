#include "Card.h"
#include "JsonConstants.h"

namespace Monopoly
{

    json Card::ToJSON() const
    {
        json result;
        result[c_JSON_Card_Name] = m_Name;
        result[c_JSON_Card_Type] = m_Type;
        result[c_JSON_Card_Value] = m_Value;
        result[c_JSON_Card_Color] = m_MainColor;
        result[c_JSON_Card_SecondColor] = m_SecondColor;
        result[c_JSON_Card_Action] = m_Action;
        result[c_JSON_Card_IsFlipped] = m_bIsFlipped;
        return result;
    }

}