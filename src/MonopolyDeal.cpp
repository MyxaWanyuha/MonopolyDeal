#include "Monopoly_pch.h"
#include "Game.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "JsonConstants.h"
#include <fstream>

class ConsoleGame : public Monopoly::Game
{
public:
    ConsoleGame(const std::string& fileName)
        : Game(fileName)
    {
    }

    ConsoleGame()
    {
#if NDEBUG
        const uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();
#else
        const uint32_t seed = 1337322228u;
#endif
        int playersCount;
        do {
            std::cout << "Enter players count(2-5): ";
            std::cin >> playersCount;
        } while (!Game::Init(playersCount, seed));
    }

    json GetAllValidPlayerTurns(int index) const
    {
        json turns;
        turns += {Monopoly::c_JSON_Command, ETurn::Pass};
        const auto& player = Game::GetPlayers()[index];

        for (int i = 0; i < player.GetCardsInHand().size(); ++i)
        {
            const auto& card = player.GetCardsInHand()[i];
            if (card->GetType() == Monopoly::ECardType::Money || card->GetType() == Monopoly::ECardType::Property)
            {
                turns += { {Monopoly::c_JSON_Command, ETurn::PlayCard}, { Monopoly::c_JSON_CardIndex, i } };
            }
            else if (card->GetType() == Monopoly::ECardType::Action)
            {
                auto extra = ExtraActionInformation(card, player);

                json eCopy;
                eCopy[Monopoly::c_JSON_Command] = ETurn::PlayCard;
                eCopy[Monopoly::c_JSON_CardIndex] = i;
                eCopy[Monopoly::c_JSON_ActionCommand] = Monopoly::Game::EActionInput::ToBank;
                turns += eCopy;

                for (auto& e : extra)
                {
                    eCopy[Monopoly::c_JSON_ActionCommand] = Monopoly::Game::EActionInput::Play;
                    eCopy.merge_patch(e);
                    turns += eCopy;
                }
            }
        }
        return turns;
    }

    json ExtraActionInformation(const Monopoly::CardContainerElem& card, const Monopoly::Player& player) const
    {
        json res;
        switch (card->GetActionType())
        {
            case Monopoly::EActionType::PassGo:
            {}break;
            case Monopoly::EActionType::DoubleTheRent:
            {}break;
            case Monopoly::EActionType::JustSayNo:
            {}break;
            case Monopoly::EActionType::Hotel:
            {
                for (int i = 0; i < player.GetCardSets().size(); ++i)
                {
                    const auto& set = player.GetCardSets()[i];
                    if (set.IsFull() && set.IsHasHouse() && !set.IsHasHotel())
                    {
                        json v;
                        v[Monopoly::c_JSON_VictimSetIndex] = i;
                        res += v;
                    }
                }
            }break;
            case Monopoly::EActionType::House:
            {
                for (int i = 0; i < player.GetCardSets().size(); ++i)
                {
                    const auto& set = player.GetCardSets()[i];
                    if (set.IsFull() && !set.IsHasHouse())
                    {
                        json v;
                        v[Monopoly::c_JSON_VictimSetIndex] = i;
                        res += v;
                    }
                }
            }break;
            case Monopoly::EActionType::DealBreaker:
            {
                for (int p = 0; p < Game::GetPlayersCount(); ++p)
                {
                    const auto& victim = Game::GetPlayers()[p];
                    if (&victim == &player) continue;

                    for (int i = 0; i < victim.GetCardSets().size(); ++i)
                    {
                        if (victim.GetCardSets()[i].IsFull())
                        {
                            json v;
                            v[Monopoly::c_JSON_VictimIndex] = p;
                            v[Monopoly::c_JSON_VictimSetIndex] = i;
                            res += v;
                        }
                    }
                }
            }break;
            case Monopoly::EActionType::SlyDeal:
            {
                for (int p = 0; p < Game::GetPlayersCount(); ++p)
                {
                    const auto& victim = Game::GetPlayers()[p];
                    if (&victim == &player) continue;
                    for (int i = 0; i < victim.GetCardSets().size(); ++i)
                    {
                        const auto& set = victim.GetCardSets()[i];
                        if (set.IsFull()) continue;
                        for(int j = 0; j < set.GetCards().size(); ++j)
                        {
                            json v;
                            v[Monopoly::c_JSON_VictimIndex] = p;
                            v[Monopoly::c_JSON_VictimSetIndex] = i;
                            v[Monopoly::c_JSON_VictimPropertyIndexInSet] = j;
                            res += v;
                        }
                    }
                }
            }break;
            case Monopoly::EActionType::ForcedDeal:
            {
                json playerProperties;
                for (int i = 0; i < player.GetCardSets().size(); ++i)
                {
                    const auto& set = player.GetCardSets()[i];
                    if (set.IsFull()) continue;
                    for (int j = 0; j < set.GetCards().size(); ++j)
                    {
                        json v;
                        v[Monopoly::c_JSON_PlayerSetIndex] = i;
                        v[Monopoly::c_JSON_PlayerPropertyIndexInSet] = j;
                        playerProperties += v;
                    }
                }
                if (playerProperties.empty()) break;

                for (int p = 0; p < Game::GetPlayersCount(); ++p)
                {
                    const auto& victim = Game::GetPlayers()[p];
                    if (&victim == &player) continue;

                    for (int i = 0; i < victim.GetCardSets().size(); ++i)
                    {
                        const auto& set = victim.GetCardSets()[i];
                        if (set.IsFull()) continue;
                        for (int j = 0; j < set.GetCards().size(); ++j)
                        {
                            for (const auto& e : playerProperties)
                            {
                                json turn = e;
                                json v;
                                v[Monopoly::c_JSON_VictimIndex] = p;
                                v[Monopoly::c_JSON_VictimSetIndex] = i;
                                v[Monopoly::c_JSON_VictimPropertyIndexInSet] = j;
                                turn.merge_patch(v);
                                res += turn;
                            }
                        }
                    }
                }
            }break;
            case Monopoly::EActionType::ItsMyBirthday:
            {}break;
            case Monopoly::EActionType::DebtCollector:
            {
                for (int i = 0; i < Game::GetPlayersCount(); ++i)
                {
                    const auto& victim = Game::GetPlayers()[i];
                    if (&victim == &player) continue;
                    json v;
                    v[Monopoly::c_JSON_VictimIndex] = i;
                    res += v;
                }
            }break;
            case Monopoly::EActionType::RentWild:
            {
                json dtr = GetDoubleTheRentMayUse(player);
                json sets;
                for (int i = 0; i < player.GetCardSets().size(); ++i)
                {
                    json v;
                    v[Monopoly::c_JSON_PlayerSetIndex] = i;
                    sets += v;
                }
                if (sets.empty()) break;

                for (int i = 0; i < Game::GetPlayersCount(); ++i)
                {
                    if (&Game::GetPlayers()[i] == &player) continue;
                    json victim;
                    victim[Monopoly::c_JSON_VictimIndex] = i;
                    for (const auto& set : sets)
                    {
                        json victimSet = victim;
                        victimSet.merge_patch(set);

                        if (dtr.empty())
                        {
                            res += victimSet;
                        }
                        else
                        {
                            for (const auto& d : dtr)
                            {
                                json victimSetDTR = victimSet;
                                victimSetDTR.merge_patch(d);
                                res += victimSetDTR;
                            }
                        }
                    }
                }
            }break;
            case Monopoly::EActionType::RentLightBlueBrown:
            {
                RentTwoColorAction(player, Monopoly::EColor::LightBlue, Monopoly::EColor::Brown, res);
            }break;
            case Monopoly::EActionType::RentOrangePink:
            {
                RentTwoColorAction(player, Monopoly::EColor::Orange, Monopoly::EColor::Pink, res);
            }break;
            case Monopoly::EActionType::RentYellowRed:
            {
                RentTwoColorAction(player, Monopoly::EColor::Yellow, Monopoly::EColor::Red, res);
            }break;
            case Monopoly::EActionType::RentUtilityRailroad:
            {
                RentTwoColorAction(player, Monopoly::EColor::Utility, Monopoly::EColor::Railroad, res);
            }break;
            case Monopoly::EActionType::RentBlueGreen:
            {
                RentTwoColorAction(player, Monopoly::EColor::Blue, Monopoly::EColor::Green, res);
            }break;
        }
        return res;
    }

    void RentTwoColorAction(const Monopoly::Player& player, Monopoly::EColor color1, Monopoly::EColor color2, json& res) const
    {
        json dtr = GetDoubleTheRentMayUse(player);
        json sets;
        for (int i = 0; i < player.GetCardSets().size(); ++i)
        {
            const auto color = player.GetCardSets()[i].GetColor();
            if (color == color1 || color == color2)
            {
                json v;
                v[Monopoly::c_JSON_PlayerSetIndex] = i;
                sets += v;
                if (dtr.empty())
                {
                    res += sets;
                }
                else
                {
                    for (const auto& d : dtr)
                    {
                        json t = v;
                        t.merge_patch(d);
                        res += t;
                    }
                }
            }
        }
    }

    json GetDoubleTheRentMayUse(const Monopoly::Player& player) const
    {
        int dtrCount = GetDoubleTheRentCountMayUse(player);
        json dtr;
        for (int i = 0; i < (dtrCount + 1); ++i)
        {
            // 0 1 2
            json v;
            v[Monopoly::c_JSON_DoubleTheRentUseCount] = i;
            dtr += v;
        }
        return dtr;
    }

private:
    auto Color(Monopoly::EColor color) const
    {
        for (auto it = Monopoly::c_ColorStrToEnum.begin(); it != Monopoly::c_ColorStrToEnum.end(); ++it) {
            if (it->second == color)
                return it->first;
        }
    }

    void ShowCard(const int index, const Monopoly::CardContainerElem& card) const
    {
        std::cout << "\t\t\t" << index << ". " << card->GetShortData() << "\n";
        
        //std::cout << "\n";
        //if (card->GetType() == Monopoly::ECardType::Property)
        //{
        //    std::cout << "\t\t\t" << index << ". Property \"" << card->GetName() << "\"\n";
        //}
        //else if (card->GetType() == Monopoly::ECardType::Money)
        //{
        //    std::cout << "\t\t\t" << index << ". Money \"" << card->GetName() << "\"\n";
        //}
        //else if (card->GetType() == Monopoly::ECardType::Action)
        //{
        //    std::cout << "\t\t\t" << index << ". Action \"" << card->GetName() << "\"\n";
        //}

        //if (card->GetValue() > 0)
        //    std::cout << "\t\t\tValue " << card->GetValue() << "\n";
        //if (card->GetCurrentColor() != Monopoly::EColor::None)
        //    std::cout << "\t\t\tColor " << Color(card->GetCurrentColor()) << "\n";
        //if (card->GetColor2() != Monopoly::EColor::None)
        //    std::cout << "\t\t\tColor for flip " << Color(card->GetColor2()) << "\n";
    }

    mutable json validTurns;
    virtual void InputTurn(ETurn& turn, int& cardIndex, int& setIndex) const override
    {
        validTurns = GetAllValidPlayerTurns(Game::GetCurrentPlayerIndex());
        std::cout << "Input command:\n" 
            << static_cast<int>(Game::ETurn::Pass) << " - pass\n"
            << static_cast<int>(Game::ETurn::FlipCard) << " - flip\n"
            << static_cast<int>(Game::ETurn::PlayCard) << " - play\n";
        do {
            std::cin >> (int&)turn;
            switch (turn)
            {
            case Game::ETurn::Pass:
                return;
            case Game::ETurn::FlipCard:
            {
                std::cout << "Enter set index: ";
                std::cin >> setIndex;
                std::cout << "Enter card index: ";
                std::cin >> cardIndex;
                return;
            }
            case Game::ETurn::PlayCard:
            {
                std::cout << "Enter card index: ";
                std::cin >> cardIndex;
                return;
            }
            case Game::ETurn::HouseHotelOnTable:
            {
                std::cout << "Enter card index: ";
                std::cin >> cardIndex;
                return;
            }
            default:
                std::cerr << "Input is incorrect! Try again:\n";
            }
        } while (1);
    }

    virtual EActionInput GetActionInput() const override
    {
        std::cout << "What do you want to do with this action card(0 - to bank, 1 - play): ";
        EActionInput input;
        do {
            std::cin >> (int&)input;
            if (input == EActionInput::ToBank || input == EActionInput::Play)
            {
                return input;
            }
            std::cout << "Input is incorrect! Try again:\n";
        } while (1);
    }

    int SelectIndex(const std::vector<int>& indices, const char* message) const
    {
        if (indices.size() == 1)
        {
            return indices[0];
        }

        int res = -1;
        do {
            std::cout << message;
            for (const auto& index : indices)
            {
                std::cout << index << ".\n";
            }
            std::cout << "Select index: ";
            std::cin >> res;
            if (std::find(indices.begin(), indices.end(), res) == indices.end())
            {
                std::cout << "Invalid index!\n";
                res = -1;
            }
        } while (res == -1);
        return res;
    }
    
    virtual int SelectSetIndex(const std::vector<int>& indices) const override
    {
        return SelectIndex(indices, "Set indices: \n");
    }

    int SelectPropertyIndex(const std::vector<int>& indices) const
    {
        return SelectIndex(indices, "Property indices: \n");
    }

    void InputVictimIndex(int& victimIndex) const
    {
        victimIndex = -1;
        do {
            std::cout << "Victim indices: \n";
            const auto playersCount = Game::GetPlayersCount();
            for (int i = 0; i < playersCount; ++i)
            {
                if (i != Game::GetCurrentPlayerIndex())
                {
                    std::cout << i << ".\n";
                }
            }
            std::cout << "Select index: ";
            std::cin >> victimIndex;
            if (victimIndex == Game::GetCurrentPlayerIndex() || victimIndex < 0 || victimIndex > Game::GetPlayersCount())
            {
                victimIndex = -1;
                std::cout << "Is invalid index!\n";
            }
        } while (victimIndex == -1);
    }

    virtual void InputDealBreaker(int& victimIndex, int& setIndex) const override
    {
        std::vector<int> fullSetIndices;
        do {
            InputVictimIndex(victimIndex);
            fullSetIndices = GetPlayers()[victimIndex].GetFullSetsIndices();
            if (fullSetIndices.size() == 0)
            {
                std::cout << "Invalid index! Victim doesn't have full sets!\n";
                victimIndex = -1;
            }
        } while (victimIndex == -1);
        setIndex = SelectSetIndex(fullSetIndices);
    }

    void InputPropertyIndex(const int player, const int setIndex, int& propertyIndexInSet) const
    {
        std::vector<int> indices;
        propertyIndexInSet = -1;
        const auto& set = GetPlayers()[player].GetCardSets()[setIndex];
        const auto cardsCount = set.GetCards().size();
        for (int i = 0; i < cardsCount; ++i)
        {
            std::cout << i << ".\n";
            indices.emplace_back(i);
        }
        if (set.IsHasHouse())
        {
            indices.emplace_back(cardsCount);
            std::cout << cardsCount << ".\n";
        }
        if (set.IsHasHotel())
        {
            indices.emplace_back(cardsCount + 1);
            std::cout << cardsCount + 1 << ".\n";
        }
        propertyIndexInSet = SelectPropertyIndex(indices);
    }

    virtual void InputSlyDeal(int& victimIndex, int& setIndex, int& propertyIndexInSet) override
    {
        std::vector<int> setIndices;
        do {
            InputVictimIndex(victimIndex);
            setIndices = GetPlayers()[victimIndex].GetNotFullSetsIndices();
            if (setIndices.size() == 0)
            {
                std::cout << "Invalid index! Victim doesn't have full sets!\n";
                victimIndex = -1;
            }
        } while (victimIndex == -1);
        setIndex = SelectSetIndex(setIndices);
        InputPropertyIndex(victimIndex, setIndex, propertyIndexInSet);
    }

    virtual void InputForcedDeal(int& victimIndex, int& victimSetIndex, int& victimPropertyIndexInSet, int& playerSetIndex, int& playerPropertyIndexInSet) override
    {
        InputSlyDeal(victimIndex, victimSetIndex, victimPropertyIndexInSet);

        auto playerIndex = Game::GetCurrentPlayerIndex();
        auto setIndices = GetPlayers()[playerIndex].GetNotFullSetsIndices();
        playerSetIndex = SelectSetIndex(setIndices);
        InputPropertyIndex(playerIndex, playerSetIndex, playerPropertyIndexInSet);
    }

    virtual void InputDebtCollector(int& victimIndex) override
    {
        InputVictimIndex(victimIndex);
    }

    virtual void InputPay(const int victimIndex, const int amount, std::vector<int>& moneyIndices, std::unordered_map<int, std::vector<int>>& setIndices) override
    {
        const static std::string bank = "Bank";
        const static std::string set = "Set";
        const int errorCode = -1;
        auto split = [](const std::string& target, const char c = ' ')
        {
            std::string temp;
            std::stringstream stringstream{ target };
            std::vector<std::string> result;
            while (std::getline(stringstream, temp, c))
                result.push_back(temp);
            return result;
        };
        auto readBank = [&](const std::vector<std::string>& args)
        {
            int i = 1;
            for (; i < args.size(); ++i)
            {
                if (args[i] == set)
                {
                    return i;
                }

                int arg;
                if (sscanf(args[i].c_str(), "%d", &arg) != 1)
                {
                    std::cout << "Invalid input!\n";
                    return errorCode;
                }
                if (std::find(moneyIndices.begin(), moneyIndices.end(), arg) != moneyIndices.end())
                {
                    std::cout << "You enter same index twice!\n";
                    return errorCode;
                }
                moneyIndices.emplace_back(arg);
            }
            return i;
        };
        auto readSet = [&](const int startIndex, const std::vector<std::string>& args)
        {
            int arg;
            int i = startIndex;
            int setIndex;
            if (sscanf(args[startIndex].c_str(), "%d", &setIndex) != 1)
            {
                std::cout << "Invalid input!\n";
                return errorCode;
            }
            if (setIndices.find(setIndex) != setIndices.end())
                return errorCode;
            setIndices[setIndex];

            for (i = startIndex + 1; i < args.size(); ++i)
            {
                if (args[i] == set)
                    return i;
                if (sscanf(args[i].c_str(), "%d", &arg) != 1)
                {
                    std::cout << "Invalid input!\n";
                    return errorCode;
                }
                if (std::find(setIndices[setIndex].begin(), setIndices[setIndex].end(), arg) != setIndices[setIndex].end())
                {
                    std::cout << "You enter same index twice!\n";
                    return errorCode;
                }
                setIndices[setIndex].emplace_back(arg);
            }
            return i;
        };
        auto readSets = [&](const int startIndex, const std::vector<std::string>& args)
        {
            for (int i = startIndex; i < args.size(); ++i)
            {
                if (args[i] == set)
                {
                    auto indexNextSet = readSet(i + 1, args);
                    if (indexNextSet == errorCode)
                        return errorCode;
                    if (i == args.size())
                        return 0;
                    i = indexNextSet - 1;
                }
                else
                    return errorCode;
            }
            return 0;
        };
        do {
            std::cout << "Input template: \"Bank <cardIndex1> <cardIndex2>... Set <setIndex1> <propertyIndexInSet1> <propertyIndexInSet2>... Set <setIndex2>...\"\n";
            std::cout << "Player " << victimIndex << " select your cards to pay: \n";
            std::string input;
            moneyIndices.clear();
            setIndices.clear();
            std::getline(std::cin >> std::ws, input);
            auto args = split(input);
            auto setIndex = 0;
            if (args[setIndex] == bank)
            {
                setIndex = readBank(args);
                if (setIndex == errorCode)
                    continue;
            }
            if (setIndex < args.size() && args[setIndex] == set)
            {
                if (readSets(setIndex, args) == errorCode)
                    continue;
            }
            auto check = Game::GetPlayers()[victimIndex].GetValueOfCards(amount, moneyIndices, setIndices);
            if (check == Monopoly::Player::ValueLessThanAmount)
            {
                std::cout << "Not enought!\n";
                continue;
            }
            else if (check == Monopoly::Player::InvalidIndex)
            {
                std::cout << "Invalid input!\n";
                continue;
            }
            break;
        } while (1);
    }

    virtual void InputRentWild(int& victimIndex, int& setIndex) override
    {
        InputVictimIndex(victimIndex);
        InputRentTwoColors(setIndex);
    }

    virtual void InputRentTwoColors(int& setIndex) override
    {
        const auto playerIndex = Game::GetCurrentPlayerIndex();
        const auto setsCount = Game::GetPlayers()[playerIndex].GetCardSets().size();
        std::vector<int> setIndices;
        setIndices.resize(setsCount);
        for (int i = 0; i < setsCount; ++i)
            setIndices[i] = i;
        setIndex = SelectIndex(setIndices, "Enter the index of the set you want to rent from: \n");
    }

    virtual bool InputUseJustSayNo(const int victimIndex) const override
    {
        std::cout << "Do you want use JastSayNo card?\n"
                  << "1 - yes\n" << "0 - no\n";
        int input = -1;
        do {
            std::cout << ": ";
            std::cin >> input;
            if (input == 0 || input == 1)
            {
                return static_cast<bool>(input);
            }
            std::cout << "Incorrect input!\n";
            input = -1;
        } while (input == -1);
    }

    virtual void InputMoveHouseHotelFromTableToFullSet(const std::vector<int>& emptyHouseSetsIndexes, const std::vector<int>& emptyHotelSetsIndexes,
        const std::vector<int>& fullSetsWithoutHouseIndexes, const std::vector<int>& fullSetsWithoutHotelsIndexes, int& emptyIndex, int& setIndex) override
    {
        if (emptyHouseSetsIndexes.size() != 0 && fullSetsWithoutHouseIndexes.size() != 0)
        {
            emptyIndex = SelectIndex(emptyHouseSetsIndexes, "Select the house you want to add to: \n");
            setIndex = SelectIndex(fullSetsWithoutHouseIndexes, "Select the set you want to add house: \n");
        }
        else
        {
            emptyIndex = SelectIndex(emptyHotelSetsIndexes, "Select the hotel you want to add to: \n");
            setIndex = SelectIndex(fullSetsWithoutHotelsIndexes, "Select the set you want to add hotel: \n");
        }
    }

    void ShowPlayerData(const int index, const bool isPublic) const
    {
        const auto& player = GetPlayers()[index];
        if (isPublic)
        {
            std::cout << "\tPlayer(" << index << "): \n";
        }
        else
        {
            std::cout << "\tCurrent player(" << index << "): \n";
            std::cout << "\t\tHand count: " << player.GetCardsInHand().size() << "\n";
            for (int i = 0; i < player.GetCardsInHand().size(); ++i)
            {
                ShowCard(i, player.GetCardsInHand()[i]);
            }
        }
        std::cout << "\t\tBank count: " << player.GetCardsInBank().size() << "\n";
        for (int i = 0; i < player.GetCardsInBank().size(); ++i)
        {
            ShowCard(i, player.GetCardsInBank()[i]);
        }
        std::cout << "\t\tSets count: " << player.GetCardSets().size() << "\n";
        for (int j = 0; j < player.GetCardSets().size(); ++j)
        {
            std::cout << "\t\tCard set " << j << "\n";
            const auto& set = player.GetCardSets()[j];
            for (int i = 0; i < set.GetCards().size(); ++i)
            {
                ShowCard(i, set.GetCards()[i]);
            }
            if (set.GetColor() != Monopoly::EColor::Railroad && set.GetColor() != Monopoly::EColor::Utility)
            {
                std::cout << "\t\t\tHouse: " << set.IsHasHouse();
                std::cout << "\t\t\tHotel: " << set.IsHasHotel();
            }
            std::cout << "\n";
        }
    }

    virtual void ShowPublicPlayerData(const int index) const override
    {
        ShowPlayerData(index, true);
    }
    virtual void ShowPrivatePlayerData(const int index) const override
    {
        std::cout << "Deck cards count: " << Game::GetDeckCardsCount() << "\n";
        std::cout << "Draw: \n";
        for (int i = 0; i < Game::GetDrawCards().size(); ++i)
            ShowCard(i, Game::GetDrawCards()[i]);

        ShowPlayerData(index, false);
    }

    virtual void InputIndexesToRemove(const int extraCardsCount, std::vector<int>& container) override
    {
        std::cout << "Player has extra cards: " << extraCardsCount << '\n'
            << "Enter the indices of the cards you want to remove: ";
        container.resize(extraCardsCount);
        for (int i = 0; i < extraCardsCount; ++i)
        {
            int index;
            std::cin >> index;
            container[i] = index;
        }
    }

    virtual void InputDoubleTheRent(const int doubleTheRentCount, int& howManyCardsToUse) const override
    {
        do
        {
            std::cout << "How many cards DoubleTheRent to use?\n";
            for (int i = 0; i < (doubleTheRentCount + 1); ++i)
            {
                std::cout << i << ".\n";
            }
            std::cin >> howManyCardsToUse;
            if (howManyCardsToUse < 0 || howManyCardsToUse >(doubleTheRentCount + 1))
            {
                std::cout << "Invalid input!\n";
                continue;
            }
            break;
        } while (1);
    }

};

int main()
{
    std::unique_ptr<ConsoleGame> g;

    int input;
    while (true)
    {
        std::cout << "Load(1) or new game(2):";
        std::cin >> input;
        if (input == 1)
        {
            std::cout << "Enter filename:";
            std::string name;
            std::cin >> name;
            std::ifstream f(name.c_str());
            if (f.good() == false)
                continue;
            g = std::make_unique<ConsoleGame>(name);
            break;
        }
        else if (input == 2)
        {
            g = std::make_unique<ConsoleGame>();
            break;
        }
        std::cerr << "Incorrect input!\n";
    }
    return g->Run();
}
