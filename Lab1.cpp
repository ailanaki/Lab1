#include <random>
#include <iostream>
#include <string>
#include <fstream>
#include <cctype>


#ifdef _WIN32
#include <windows.h>
#endif

class Game {
public:

	Game(): engine_(std::random_device{}())
	{
	}

    void setSaveFileName(const std::string& name) {
        saveFileName_ = name;
    }

	void startGame() {
        bool loaded = load();

        if (loaded && round_ != -1){
            while (true){
                int userInput = read("Повелитель, хотите ли продолжить предыдущую игру? (Для продолжения игры, введите 1."
                                     " Для того, чтобы начать заново, нажмите 2) ");
                if (userInput == 1) break;
                if (userInput == 2){
                    reset();
                    break;
                }
                std::cout << "Введено неверное значение: "<< userInput  << std::endl;
            }
        }else{
            reset();
        }
        bool exit = false;


		while (round_ <= 10 && !exit) {
			newRound();
			if (revolt_ || allDead_) {
				break;
			}

            round_++;
            while (true){
                int userInput = read("Повелитель, хотите ли продолжить играть? (Для остановки и сохранения прогресса, введите 1. Для продолжения, нажмите 2) ");

                if (userInput == 1){
                    save();
                    exit = true;
                    break;
                }
                if (userInput == 2){
                    break;
                }
                std::cout << "Введено неверное значение: "<< userInput  << std::endl;
            }
		}

		if (revolt_) {
			std::cout << "О повелитель!" << std::endl;
			std::cout << "За один год умерло более 45% населения от голода." << std::endl;
			std::cout << "Народ восстал и сверг тебя. Твоё правление окончено. "<< std::endl;
		}
		else if (allDead_) {
			std::cout << "О повелитель!" << std::endl;
			std::cout << "Все население мертво." << std::endl;
		}
		else {
            if (exit) return;
			writeFinalStats();
		}
	}


private:
	int round_ = -1;
	int deadPeopleCount_ = 0;
	int newPeopleCount_ = 0;
	bool plague_ = false;

	int currentPeopleCount_ = 0;
	int wheatGetCount_ = 0;
	int wheatPerAcreCount_ = 0;
	int wheatRatEaten_ = 0;
	int wheatCount_ = 0;

	int acreCount_ = 0;
	int acrePrice_ = 0;

	int wheatToEat_ = 0;
	int acreToWheat_ = 0;

	double starvedPercentSum_ = 0.0;
	bool revolt_ = false;
	bool allDead_ = false;

    std::string saveFileName_= "save1.txt";

	std::mt19937 engine_;


    template<typename T>
    void writeLineReport(const std::string& start, T var, const std::string& end){
        std::cout <<'\t' << start << var << end << std::endl;
    }

    int randomInt(int minValue, int maxValue) {
        std::uniform_int_distribution<int> dist(minValue, maxValue);
        return dist(engine_);
    }


    static int read(const std::string& prompt) {
        while (true) {
            std::cout << prompt;
            std::string userInput;
            std::cin >> userInput;
            if (userInput.empty()) {
                std::cout << "Нужно ввести целое число (0 или больше).\n";
                continue;
            }

            bool ok = true;
            for (char ch : userInput) {
                if (!std::isdigit(static_cast<unsigned char>(ch))) {
                    ok = false;
                    break;
                }
            }

            if (!ok) {
                std::cout << "Нужно ввести целое неотрицательное число.\n";
                continue;
            }
            return std::stoi(userInput);
        }
    }


    void writeStats() {
        std::cout << "Мой повелитель, соизволь поведать тебе" << std::endl;
        writeLineReport("в году ", round_ - 1 , " твоего высочайшего правления");
        if (deadPeopleCount_ != 0) {
            writeLineReport("", deadPeopleCount_, " человек умерли с голоду ");
        }
        if (newPeopleCount_ != 0) {
            writeLineReport("", newPeopleCount_, " человек прибыли в наш великий город;");
        }

        if (plague_) {
            writeLineReport("","Чума уничтожила половину населения;", "");
        }
        writeLineReport("Мы собрали ", wheatGetCount_," бушелей пшеницы, по ");
        writeLineReport("", wheatPerAcreCount_, " бушеля с акра;");
        writeLineReport("Крысы истребили ", wheatRatEaten_, " бушелей пшеницы, оставив ");
        writeLineReport("", wheatCount_, " бушелей в амбарах; ");
        writeLineReport("Город сейчас занимает ", acreCount_, " акров; ");
        writeLineReport("В городе ", currentPeopleCount_, " человек; ");
        writeLineReport("1 акр земли стоит сейчас ", acrePrice_ , " бушелей;");

    }


    void writeFinalStats() {
        std::cout << "Итоги твоего правления, о великий правитель!" << std::endl;

        double avgStarvedPercent =
                starvedPercentSum_ / static_cast<double>(round_ - 1);

        double acresPerPerson = 0.0;
        if (currentPeopleCount_ > 0) {
            acresPerPerson =
                    static_cast<double>(acreCount_) /
                    static_cast<double>(currentPeopleCount_);
        }

        writeLineReport("Среднегодовой процент умерших от голода: ", avgStarvedPercent, "");
        writeLineReport("Акров земли на одного жителя: ", acresPerPerson, "") ;

        if (avgStarvedPercent > 33.0 && acresPerPerson < 7.0) {
            std::cout << "Из-за вашей некомпетентности в управлении, народ устроил бунт и изгнал вас их города. " ;
            std::cout << "Теперь вы вынуждены влачить жалкое существование в изгнании";
        }
        else if (avgStarvedPercent > 10.0 && acresPerPerson < 9.0) {
            std::cout << "Ты правил железной рукой, подобно Нерону и Ивану Грозному. ";
            std::cout << "Народ вздохнул с облегчением, и никто больше не желает видеть вас правителем.";
        }
        else if (avgStarvedPercent > 3.0 && acresPerPerson < 10.0) {
            std::cout << "Ты справился вполне неплохо. ";
            std::cout << "У тебя есть недоброжелатели, но многие  хотели бы увидеть вас во главе города снова";
        }
        else {
            std::cout << "Фантастика! Карл Великий, Дизраэли и Джефферсон вместе не справились бы лучше!";
        }
    }


    void writeStartGameStats() {
        std::cout << "Мой повелитель, соизволь поведать тебе" << std::endl;
        writeLineReport("Город сейчас занимает ", acreCount_, " акров; ");
        writeLineReport("В городе ", currentPeopleCount_, " человек; ");
        writeLineReport("", wheatCount_, " бушелей в амбарах; ");
        writeLineReport("1 акр земли стоит сейчас ", acrePrice_ , " бушелей;");
    }

    void newRound() {
        acrePrice_ = randomInt(17, 26);

        if (round_ == 1) {
            writeStartGameStats();
        }
        else {
            writeStats();
        }

        deadPeopleCount_ = 0;
        newPeopleCount_ = 0;
        plague_ = false;
        wheatRatEaten_ = 0;

        trade();

        wheatPerAcreCount_ = randomInt(1, 6);
        wheatGetCount_ = wheatPerAcreCount_ * acreToWheat_;
        wheatCount_ += wheatGetCount_;

        int maxRats = static_cast<int>(wheatCount_ * 0.07);
        if (maxRats > 0) {
            wheatRatEaten_ = randomInt(0, maxRats);
            wheatCount_ -= wheatRatEaten_;
        }

        int peopleCanBeFed = wheatToEat_ / 20;

        if (peopleCanBeFed <= currentPeopleCount_) {
            deadPeopleCount_ = currentPeopleCount_ - peopleCanBeFed;
        }

        double percentStarved = deadPeopleCount_ * 100.0 / currentPeopleCount_;

        if (percentStarved > 45.0) {
            revolt_ = true;
            return;
        }

        starvedPercentSum_ += percentStarved;

        currentPeopleCount_ -= deadPeopleCount_;
        if (currentPeopleCount_ <= 0) {
            allDead_ = true;
            return;
        }

        newPeopleCount_ = deadPeopleCount_ / 2 + (5 - wheatPerAcreCount_) * (wheatCount_ / 600) + 1;
        if (newPeopleCount_ < 0) {
            newPeopleCount_ = 0;
        }
        if (newPeopleCount_ > 50) {
            newPeopleCount_ = 50;
        }

        currentPeopleCount_ += newPeopleCount_;

        plague();

    }

	void trade() {

		while (true) {
			int acreToBuy_ = read("Сколько акров земли повелеваешь купить? ");
			int acreToSell_ = read("Сколько акров земли повелеваешь продать? ");
			wheatToEat_ = read("Сколько бушелей пшеницы повелеваешь отдать на еду? ");
            acreToWheat_ = read("Сколько акров земли повелеваешь засеять? ");

			int landAfter = acreCount_ + acreToBuy_ - acreToSell_;
			if (landAfter < 0) {
				std::cout << "О, повелитель, пощади нас! Нельзя продать больше земли, чем у нас есть." << std::endl;
				std::cout << "У нас только " << acreCount_ << " акров земли." << std::endl;
				continue;
			}

			int wheatAfterTrade = wheatCount_ - acreToBuy_ * acrePrice_ + acreToSell_ * acrePrice_;
			if (wheatAfterTrade < 0) {
				std::cout << "О, повелитель, пощади нас! У нас нет столько пшеницы, чтобы купить столько земли." << std::endl;
				std::cout << "В амбарах всего " << wheatCount_ << " бушелей." << std::endl;
				continue;
			}

			if (wheatToEat_ > wheatAfterTrade) {
				std::cout << "О, повелитель, пощади нас! У нас нет столько пшеницы на еду." << std::endl;
				std::cout << "После сделок у нас будет только "
					<< wheatAfterTrade << " бушелей." << std::endl;
				continue;
			}

			if (acreToWheat_ > landAfter) {
				std::cout << "О, повелитель, пощади нас! Нельзя засеять больше земли, чем у нас есть." << std::endl;
				std::cout << "После сделок у нас будет " << landAfter << " акров." << std::endl;
				continue;
			}

			int maxAcresByPeople = currentPeopleCount_ * 10;
			if (acreToWheat_ > maxAcresByPeople) {
				std::cout << "О, повелитель, пощади нас! Люди не смогут обработать столько земли." << std::endl;
				continue;
			}

			int seedNeeded = acreToWheat_ / 2;
			int wheatAfterFood = wheatAfterTrade - wheatToEat_;
			if (seedNeeded > wheatAfterFood) {
				std::cout << "О, повелитель, пощади нас! У нас недостаточно пшеницы на семена." << std::endl;
				std::cout << "После еды останется только "
					<< wheatAfterFood << " бушелей." << std::endl;
				continue;
			}
			acreCount_ = landAfter;
			wheatCount_ = wheatAfterTrade - wheatToEat_ - seedNeeded;
            break;
		}
	}

	void plague() {
		std::uniform_real_distribution<double> dist(0, 1.0);

        double plagueChance = dist(engine_);

		if (plagueChance < 0.15) {
			plague_ = true;
			currentPeopleCount_ -= currentPeopleCount_ / 2;
			if (currentPeopleCount_ <= 0) {
				allDead_ = true;
			}
		}
	}


    void reset() {
        round_ = 1;
        deadPeopleCount_ = 0;
        newPeopleCount_ = 0;
        plague_ = false;

        currentPeopleCount_ = 100;
        wheatGetCount_ = 0;
        wheatPerAcreCount_ = 0;
        wheatRatEaten_ = 0;
        wheatCount_ = 2800;

        acreCount_ = 1000;
        acrePrice_ = 0;

        wheatToEat_ = 0;
        acreToWheat_ = 0;

        starvedPercentSum_ = 0.0;
        revolt_ = false;
        allDead_ = false;

    }

    void save() const {
        std::ofstream out(saveFileName_);
        if (!out) {
            std::cout << "Не удалось открыть файл для сохранения." << std::endl;
            return;
        }

        out << round_ << std::endl;
        out << deadPeopleCount_ << std::endl;
        out << newPeopleCount_ << std::endl;
        out << plague_ << std::endl;
        out << currentPeopleCount_ << std::endl;
        out << wheatGetCount_ << std::endl;
        out << wheatPerAcreCount_ << std::endl;
        out << wheatRatEaten_ << std::endl;
        out << wheatCount_ << std::endl;
        out << acreCount_ << std::endl;
        out << wheatToEat_ << std::endl;
        out << acreToWheat_ << std::endl;
        out << starvedPercentSum_ << std::endl;
    }

    bool load() {
        if (saveFileName_.empty()) {
            return false;
        }

        std::ifstream in(saveFileName_);
        if (!in) {
            return false;
        }

        in >> round_;
        in >> deadPeopleCount_;
        in >> newPeopleCount_;
        in >> plague_;
        in >> currentPeopleCount_;
        in >> wheatGetCount_;
        in >> wheatPerAcreCount_;
        in >> wheatRatEaten_;
        in >> wheatCount_;
        in >> acreCount_;
        in >> wheatToEat_;
        in >> acreToWheat_;
        in >> starvedPercentSum_;

        if (!in) {
            return false;
        }

        return true;
    }

};



int main()
{
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
#endif

	Game game;
	game.startGame();
	return 0;
}
