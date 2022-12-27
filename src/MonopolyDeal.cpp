#include <iostream>
#include "Game.h"

class ConsoleGame : protected Monopoly::Game
{
    struct TurnInput
    {
        TurnInput(Game::ETurn input, int cardIndex = -1, int setIndex = -1)
            : Input(input), CardIndex(cardIndex), SetIndex(setIndex)
        {
        }
        Game::ETurn Input;
        int CardIndex;
        int SetIndex;
    };
public:
    ConsoleGame()
    {
        std::cout << "Input players count(2-5): ";
        int playersCount;
        std::cin >> playersCount;
        Game::Init(playersCount);
    }

    int Run()
    {
        while (Game::IsNotEnded())
        {
            Game::BeginTurn();
            // TODO Send data to screen
            // Game::GetCurrentPlayerIndex();
            // Game::GetCurrentPlayerAllData();
            // Game::GetOtherPlayersPublicData();
            Game::ETurnOutput turnOutput;
            do
            {
                TurnInput turn = InputTurn();
                turnOutput = Game::Turn(turn.Input, turn.CardIndex, turn.SetIndex);
                if (turnOutput == Game::ETurnOutput::IncorrectInput)
                {
                    std::cout << "Input is incorrect!\n";
                }
                else if (turnOutput == Game::ETurnOutput::IncorrectIndex)
                {
                    std::cout << "Index is incorrect!\n";
                }
            } while (turnOutput != Game::ETurnOutput::NextPlayer);

            auto extraCards = Game::GetExtraCardsCount();
            if (extraCards > 0)
            {
                std::cout << "Player has extra cards: " << extraCards << '\n'
                    << "Input the indexes of the cards you want to remove: ";
                Monopoly::CardIndexesContainer container;
                container.resize(extraCards);
                for (int i = 0; i < extraCards; ++i)
                {
                    int index;
                    std::cin >> index;
                    container[i] = index;
                }
                Game::RemoveExtraCards(container);
            }
            Game::EndTurn();
        }
        return 0;
    }

private:
    TurnInput InputTurn()
    {
        std::cout << "Input command:\n1 - pass\n2 - flip\n3 - play\n";
        Game::ETurn turn;
        while (std::cin >> ((int&)turn))
        {
            switch (turn)
            {
            case Game::ETurn::Pass:
                return TurnInput(Game::ETurn::Pass);
            case Game::ETurn::FlipCard:
            {
                std::cout << "Input set index: ";
                int setIndex;
                std::cin >> setIndex;
                std::cout << "Input card index: ";
                int index;
                std::cin >> index;
                return TurnInput(turn, index, setIndex);
            }
            case Game::ETurn::PlayCard:
            {
                std::cout << "Input card index: ";
                int index;
                std::cin >> index;
                return TurnInput(turn, index);
            }
            default:
                std::cerr << "Input is incorrect! Try again:\n";
            }
        }
    }

    virtual EActionInput GetActionInput() const override
    {
        std::cout << "What do you want to do with this action card(0 - to bank, 1 - play): ";
        int input;
        do {
            std::cin >> input;
            if (input != EActionInput::ToBank && input != EActionInput::Play)
            {
                std::cout << "Input is incorrect! Try again:\n";
            }
            else
            {
                break;
            }
        } while (1);
        return (EActionInput)input;
    }

    virtual int SelectSetIndex(const std::vector<int>& indexes) const override
    {
        // TODO
        return 0;
    }
};

int main()
{
    ConsoleGame g;
    return g.Run();
}
