#include "Monopoly_pch.h"
#include "Game.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "JsonConstants.h"

class ConsoleGame : public Monopoly::Game
{
public:
    ConsoleGame()
    {
        int playersCount;
        do {
            std::cout << "Enter players count(2-5): ";
            std::cin >> playersCount;
        } while (!Game::Init(playersCount));
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
        std::cout << "\n";
        if (card->GetType() == Monopoly::ECardType::Property)
        {
            std::cout << "\t\t\t" << index << ". Property \"" << card->GetName() << "\"\n";
        }
        else if (card->GetType() == Monopoly::ECardType::Money)
        {
            std::cout << "\t\t\t" << index << ". Money \"" << card->GetName() << "\"\n";
        }
        else if (card->GetType() == Monopoly::ECardType::Action)
        {
            std::cout << "\t\t\t" << index << ". Action \"" << card->GetName() << "\"\n";
        }

        if (card->GetValue() > 0)
            std::cout << "\t\t\tValue " << card->GetValue() << "\n";
        if (card->GetCurrentColor() != Monopoly::EColor::None)
            std::cout << "\t\t\tColor " << Color(card->GetCurrentColor()) << "\n";
        if (card->GetColor2() != Monopoly::EColor::None)
            std::cout << "\t\t\tColor for flip " << Color(card->GetColor2()) << "\n";
    }

    virtual void InputTurn(ETurn& turn, int& cardIndex, int& setIndex) const override
    {
        std::cout << "Input command:\n" 
            << static_cast<int>(Game::ETurn::Pass) << " - pass\n"
            << static_cast<int>(Game::ETurn::FlipCard) << " - flip\n"
            << static_cast<int>(Game::ETurn::PlayCard) << " - play\n";
        do {
            std::cin >> (int&)turn;
            switch (turn)
            {
            case Game::ETurn::Pass:
                break;
            case Game::ETurn::FlipCard:
            {
                std::cout << "Enter set index: ";
                std::cin >> setIndex;
                std::cout << "Enter card index: ";
                std::cin >> cardIndex;
                break;
            }
            case Game::ETurn::PlayCard:
            {
                std::cout << "Enter card index: ";
                std::cin >> cardIndex;
                break;
            }
            case Game::ETurn::HouseHotelOnTable:
            {
                std::cout << "Enter card index: ";
                std::cin >> cardIndex;
                break;
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

            std::getline(std::cin, input);
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
                auto e = player.GetCardsInHand().begin();
                std::advance(e, i);
                ShowCard(i, *e);
            }
        }
        std::cout << "\t\tBank count: " << player.GetCardsInBank().size() << "\n";
        for (int i = 0; i < player.GetCardsInBank().size(); ++i)
        {
            auto e = player.GetCardsInBank().begin();
            std::advance(e, i);
            ShowCard(i, *e);
        }
        std::cout << "\t\tSets count: " << player.GetCardSets().size() << "\n";
        for (int j = 0; j < player.GetCardSets().size(); ++j)
        {
            std::cout << "\t\tCard set " << j << "\n";
            const auto& set = player.GetCardSets()[j];
            for (int i = 0; i < set.GetCards().size(); ++i)
            {
                auto e = set.GetCards().begin();
                std::advance(e, i);
                ShowCard(i, *e);
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
        ShowPlayerData(index, false);
    }

    virtual void InputIndexesToRemove(const int extraCardsCount, std::vector<int>& container) override
    {
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
    ConsoleGame g;
    return g.Run();
}
