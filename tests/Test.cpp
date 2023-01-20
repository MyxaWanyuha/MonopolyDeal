#include "Monopoly_pch.h"
#include <gtest/gtest.h>
#include "Game.h"
using namespace Monopoly;
constexpr uint32_t seed = 1337322228u;

class TestController : public IControllerIO
{
public:
    TestController(const int index, const Game& g)
        : IControllerIO(index, g)
    {
    }

    int m_VictimIndex = 0;
    int m_CardIndex = 0;
    int m_SetIndex = 0;
    int m_PropertyIndexInSet = 0;
    IControllerIO::ETurn m_Turn = IControllerIO::ETurn::Pass;
    IControllerIO::EActionInput m_ACtionInput = IControllerIO::EActionInput::Play;
    int m_VictimSetIndex = 0;
    int m_VictimPropertyIndexInSet = 0;
    int m_PlayerSetIndex = 0;
    int m_PlayerPropertyIndexInSet = 0;
    int m_EmptyIndex = 0;

    bool useJustSayNo = false;

    virtual void InputIndexesToRemove(const int extraCardsCount, std::vector<int>& container) override
    {
    }

    virtual void InputMove(ETurn& turn, int& cardIndex, int& setIndex) const override
    {
        cardIndex = m_CardIndex;
        setIndex = m_SetIndex;
        turn = m_Turn;
    }

    virtual EActionInput GetActionInput() const override
    {
        return m_ACtionInput;
    }

    virtual int SelectSetIndex(const std::vector<int>& indices) const override
    {
        // TODO
        return 0;
    }

    virtual void InputDealBreaker(int& victimIndex, int& setIndex) const override
    {
        victimIndex = m_VictimIndex;
        setIndex = m_SetIndex;
    }

    virtual void InputSlyDeal(int& victimIndex, int& setIndex, int& propertyIndexInSet) override
    {
        victimIndex = m_VictimIndex;
        setIndex = m_SetIndex;
        propertyIndexInSet = m_PropertyIndexInSet;
    }

    virtual void InputForcedDeal(int& victimIndex, int& victimSetIndex, 
        int& victimPropertyIndexInSet, int& playerSetIndex, int& playerPropertyIndexInSet) override
    {
        victimIndex = m_VictimIndex;
        victimSetIndex = m_VictimSetIndex;
        victimPropertyIndexInSet = m_VictimPropertyIndexInSet;
        playerSetIndex = m_PlayerSetIndex;
        playerPropertyIndexInSet = m_PlayerPropertyIndexInSet;
    }

    virtual void InputDebtCollector(int& victimIndex) override
    {
        victimIndex = m_VictimIndex;
    }

    virtual void InputPay(const int notUsed, const int amount, std::vector<int>& moneyIndices, 
        std::unordered_map<int, std::vector<int>>& setIndices) override
    {
        // TODO
    }

    virtual void InputRentWild(int& victimIndex, int& setIndex) override
    {
        victimIndex = m_VictimIndex;
        setIndex = m_SetIndex;
    }

    virtual void InputRentTwoColors(int& setIndex) override
    {
        setIndex = m_SetIndex;
    }

    virtual void InputMoveHouseHotelFromTableToFullSet(const std::vector<int>& emptyHouseSetsIndexes, 
        const std::vector<int>& emptyHotelSetsIndexes, const std::vector<int>& fullSetsWithoutHouseIndexes, 
        const std::vector<int>& fullSetsWithoutHotelsIndexes, int& emptyIndex, int& setIndex) override
    {
        emptyIndex = m_EmptyIndex;
        setIndex = m_SetIndex;
    }

    virtual bool InputUseJustSayNo(const int victimIndex) const override
    {
        return useJustSayNo;
    }

    virtual void InputDoubleTheRent(const int doubleTheRentCount, int& howManyCardsToUse) const override
    {
        // TODO
    }

};

TEST(GameTest, CreateGame)
{
    constexpr int playersCount = 2;
    Game g;
    {
        g.InitNewGame(playersCount, seed);
        Game::Controllers controllers;
        for (int i = 0; i < playersCount; ++i)
            controllers.emplace_back(std::make_shared<TestController>(i, g));
        g.InitControllers(std::move(controllers));
    }

    EXPECT_EQ(g.GetDeckCardsCount(), 106 - playersCount * 5);
    EXPECT_EQ(g.GetDrawCards().size(), 0);
    EXPECT_EQ(g.GetPlayersCount(), playersCount);
    for (const auto& p : g.GetPlayers())
    {
        EXPECT_EQ(p.GetCardsInHand().size(), 5);
        EXPECT_EQ(p.GetCardsInBank().size(), 0);
        EXPECT_EQ(p.GetCardSets().size(), 0);
    }
}

TEST(GameTest, LoadGame)
{
    Game g("tests/saves/FivePlayers.json");
    EXPECT_EQ(g.CardsInGameCount(), 106);
    EXPECT_EQ(g.GetDeckCardsCount(), 75);
    EXPECT_EQ(g.GetDrawCards().size(), 0);
    EXPECT_EQ(g.GetPlayersCount(), 5);
    {
        const auto& p = g.GetPlayers()[0];
        EXPECT_EQ(p.GetCardsInHand().size(), 5);
        EXPECT_EQ(p.GetCardsInBank().size(), 0);
        EXPECT_EQ(p.GetCardSets().size(), 0);
    }
    {
        const auto& p = g.GetPlayers()[1];
        EXPECT_EQ(p.GetCardsInHand().size(), 4);
        EXPECT_EQ(p.GetCardsInBank().size(), 0);
        EXPECT_EQ(p.GetCardSets().size(), 2);

        EXPECT_EQ(p.GetCardSets()[0].GetCards().size(), 2);
        EXPECT_EQ(p.GetCardSets()[0].IsHasHotel(), false);
        EXPECT_EQ(p.GetCardSets()[0].IsHasHouse(), false);
        EXPECT_EQ(p.GetCardSets()[1].GetCards().size(), 1);
        EXPECT_EQ(p.GetCardSets()[1].IsHasHotel(), false);
        EXPECT_EQ(p.GetCardSets()[1].IsHasHouse(), false);
    }
    {
        const auto& p = g.GetPlayers()[2];
        EXPECT_EQ(p.GetCardsInHand().size(), 4);
        EXPECT_EQ(p.GetCardsInBank().size(), 1);
        EXPECT_EQ(p.GetCardSets().size(), 2);

        EXPECT_EQ(p.GetCardSets()[0].GetCards().size(), 1);
        EXPECT_EQ(p.GetCardSets()[0].IsHasHotel(), false);
        EXPECT_EQ(p.GetCardSets()[0].IsHasHouse(), false);

        EXPECT_EQ(p.GetCardSets()[1].GetCards().size(), 1);
        EXPECT_EQ(p.GetCardSets()[1].IsHasHotel(), false);
        EXPECT_EQ(p.GetCardSets()[1].IsHasHouse(), false);
    }
    {
        const auto& p = g.GetPlayers()[3];
        EXPECT_EQ(p.GetCardsInHand().size(), 7);
        EXPECT_EQ(p.GetCardsInBank().size(), 0);
        EXPECT_EQ(p.GetCardSets().size(), 0);
    }
    {
        const auto& p = g.GetPlayers()[4];
        EXPECT_EQ(p.GetCardsInHand().size(), 5);
        EXPECT_EQ(p.GetCardsInBank().size(), 0);
        EXPECT_EQ(p.GetCardSets().size(), 0);
    }
}

TEST(GameTest, Play)
{
    //TestGame g("tests/saves/newsave2.json");
    //g.m_Turn = Game::ETurn::PlayCard;
    //g.m_CardIndex = 3;
    //g.m_ACtionInput = Game::EActionInput::Play;
    //g.m_VictimIndex = 0;
    //g.useJustSayNo = false;

    //g.GameBody();
}
