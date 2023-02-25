#include <iostream>
#include <conio.h>
#include<thread>
#include<chrono>
#include<windows.h>
using namespace std;
using namespace std::literals::chrono_literals;//для исполизования сек,мин и тд

//#define MIN_TANK_VOLUME 20//глобальная переменная
//#define MAX_TANK_VOLUME 80
//# показывае что дальше идет деректива
//директива - это указание компилятору на выполнение некоторых действий
// #include указывает компилятору что к нашему файлу нужно подключить другой файл
//#define создает макроопределение типа ИМЯ ТИПА
// и везде где он видет вставляет такое значение
#define Enter 13
#define Esc 27

class Tank
{
	static const int MIN_TANK_VOLUME = 20;//глобальная переменная которая выделяет память только в классе
	static const int MAX_TANK_VOLUME = 80;// а не в каждом объекте
	const int VOLUME;
	double fuel_level;
public:
	Tank(int volume)
		:VOLUME
		(
			volume < MIN_TANK_VOLUME ? MIN_TANK_VOLUME :
			volume > MAX_TANK_VOLUME ? MAX_TANK_VOLUME :
			volume
		)
	{
		this->fuel_level = 0;
		cout << "TConstructor:\t" << this << endl;
	}
	~Tank()
	{
		cout << "TDestructor:\t" << this << endl;
	}
	//
	int getVOLUME()const
	{
		return VOLUME;
	}
	double getFuel()const
	{
		return fuel_level;
	}
	double give_Fuel(double amount)
	{
		fuel_level -= amount;
		if (fuel_level < 0)fuel_level = 0;
		return fuel_level;
	}
	void fill(double amount)
	{
		if (amount < 0)return;
		if (fuel_level + amount > VOLUME) fuel_level = VOLUME;
		else fuel_level += amount;
	}
	void info()const
	{
		cout << "Volume:     " << VOLUME << endl;
		cout << "Fuel level: " << fuel_level << endl;
	}
};
class Engine
{
	static const int MIN_ENGINE_CONSUPTION = 3;
	static const int MAX_ENGINE_CONSUPTION = 30;
	const double CONSUMPTION;
	double consumption_per_second;
	double default_consumption_per_second;
	bool is_started;
public:
	Engine(int consumtion)
		: CONSUMPTION
		(
			consumtion < MIN_ENGINE_CONSUPTION ? MIN_ENGINE_CONSUPTION :
			consumtion > MAX_ENGINE_CONSUPTION ? MAX_ENGINE_CONSUPTION :
			consumtion
		)
	{
		consumption_per_second = default_consumption_per_second = CONSUMPTION * 3.0e-5;
		is_started = false;
		cout << "EConstructor:\t" << this << endl;
	}
	~Engine()
	{
		cout << "EDestructor:\t" << this << endl;
	}
	//Methods
	double get_CONSUMPTION()const
	{
		return this->CONSUMPTION;
	}
	//1 - 60 км / ч -> 0, 0020 л / сек
	//	. 61 - 100 км / ч -> 0, 0014 л / сек
	//	. 101 - 140 км / ч -> 0, 0020 л / сек
	//	. 141 - 200 км / ч -> 0, 0025 л / сек.вторая
	//	201 - 250 км / ч -> 0, 0030 л / с
	double get_consumption_per_second(int speed,bool gas_pressed)
	{
		if (!started())return  0;
		if (gas_pressed) {
			consumption_per_second = CONSUMPTION * 3.0e-5;
			if (speed > 0 && speed <= 60)
			{
				this->consumption_per_second = CONSUMPTION * 3.0e-5 * 6.666666;
			}
			else if (speed > 60 && speed <= 100)
			{
				this->consumption_per_second = CONSUMPTION * 3.0e-5 * 4.666666;
			}
			else if (speed > 100 && speed <= 140)
			{
				this->consumption_per_second = CONSUMPTION * 3.0e-5 * 6.666666;
			}
			else if (speed > 140 && speed <= 200)
			{
				this->consumption_per_second = CONSUMPTION * 3.0e-5 * 8.333333;
			}
			else if (speed > 200)
			{
				this->consumption_per_second = CONSUMPTION * 3.0e-5 * 10;
			}
			return this->consumption_per_second;
		}
		else return default_consumption_per_second;
	}
	void start()
	{
		is_started = true;
	}
	void stop()
	{
		is_started = false;
	}
	bool started()const
	{
		return is_started;
	}
	void info()const
	{
		cout << "Consumption per 100 km:   " << CONSUMPTION << " liters\n";
		cout << "Consumption per 1 second: " << consumption_per_second << " liters\n";
	}
};

class CAR
{
	Engine engine;
	Tank tank;
	bool driver_inside;
	int speed;
	static const int MAX_SPEED_LOW_LIMIT = 90;
	static const int MAX_SPEED_MAX_LIMIT = 390;
	const int MAX_SPEED;
	bool gas_pedal;
	struct Control
	{
		std::thread panel_thread;
		std::thread engine_idle_thread;
		std::thread free_wheeling_thread;
	}threads;
public:
	CAR(int volume, int consumption, int max_speed = 250)
		:tank(volume), engine(consumption),
		MAX_SPEED
		(
			max_speed<MAX_SPEED_LOW_LIMIT ? MAX_SPEED_LOW_LIMIT :
			max_speed>MAX_SPEED_MAX_LIMIT ? MAX_SPEED_MAX_LIMIT :
			max_speed
		)
	{
		cout << "Your car is ready, press Enter to get in" << endl;
		driver_inside = false;
		speed = 0;
	}
	~CAR()
	{
		cout << "Car is over!" << endl;
	}
	//Methods
	void get_in()
	{
		driver_inside = true;
		threads.panel_thread = std::thread(&CAR::panel, this);//запускаем panel() в потоке panel_thread
	}
	void get_out()
	{
		driver_inside = false;
		if (threads.panel_thread.joinable())threads.panel_thread.join();
		system("CLS");
		cout << "You are out of your car" << endl;
	}
	void start_engine()
	{
		if (driver_inside && tank.getFuel())
		{
			engine.start();
			threads.engine_idle_thread = std::thread(&CAR::engine_edle, this);
		}
	}
	void stop_engine()
	{
		if (driver_inside)
		{
			engine.stop();
			if (threads.engine_idle_thread.joinable())threads.engine_idle_thread.join();
		}
	}
	void control()
	{
		char key;
		do
		{
			key = 0;
			if (_kbhit()) {
				key = _getch();
				switch (key)
				{
				case Enter: driver_inside ? get_out() : get_in();
					break;
				case 'I':case'i'://ignition -зажигание
					engine.started() ? stop_engine() : start_engine();
					break;
				case 'F':case'f':
					if (driver_inside)cout << "get out of car!" << endl;
					else
					{
						double amount;
						cout << " Enetr voluem gas: "; cin >> amount;
						tank.fill(amount);
						cout << "Your car is ready, press Enter to get in" << endl;
					}
					break;
				case'W':case'w':
					acсelerate();
					break;
				case'S':case's':
					slow_down();
					break;
				case Esc:
					stop_engine();
					get_out();
					break;
				}
			}
			gas_pedal = false;
			if (tank.getFuel() == 0)stop_engine();
			if (speed <= 0) speed = 0;
			if (speed == 0 && threads.free_wheeling_thread.joinable())
			{
				threads.free_wheeling_thread.join();
			}
		} while (key != Esc);
	}
	void engine_edle()
	{
		//холостой ход двигателя
		while (engine.started() && tank.give_Fuel(engine.get_consumption_per_second(speed,gas_pedal)))
		{
			std::this_thread::sleep_for(1s);
		}
	}
	void free_wheeling()
	{
		while (speed-- > 0)
		{
			if (speed <= 0) speed = 0;
			std::this_thread::sleep_for(1s);
		}
	}
	void acсelerate()
	{
		if (driver_inside) 
		{
			gas_pedal = true;
			if (speed < MAX_SPEED && engine.started())
			{
				speed += 10;
				if (speed >= MAX_SPEED)
				{
					speed = MAX_SPEED;
				}
				std::this_thread::sleep_for(1s);
			}
		}
		if (speed > 0)
		{
			if (!threads.free_wheeling_thread.joinable())
				threads.free_wheeling_thread = std::thread(&CAR::free_wheeling, this);
		}
	}
	void slow_down()
	{
		if (speed > 0)
		{
			speed -= 20;
		}
		if (speed <= 0)   speed = 0;
		std::this_thread::sleep_for(1s);
	}
	void scale(HANDLE hConsole)const
	{
		for (size_t i = 0; i < MAX_SPEED / 5; i++)
		{
			SetConsoleTextAttribute(hConsole, 0x07);
			if (i >= 30)SetConsoleTextAttribute(hConsole, 0x0E);
			if (i >= 40)SetConsoleTextAttribute(hConsole, 0x0C);
			cout << (i < speed / 5 ? '!' : '.');
			SetConsoleTextAttribute(hConsole, 0x07);
		}
		cout << endl;
	}
	void panel()
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);//Получаем обработчик окна консоли
		while (driver_inside)
		{
			system("CLS");
			scale(hConsole);
			cout << "Speed  " << speed << " km/h" << endl;
			cout << "Fuel level:\t" << tank.getFuel() << " liters\t";
			if (tank.getFuel() < 5)
			{
				SetConsoleTextAttribute(hConsole, 0xCF);//0xCF-красный фон"С","F"-белые буквы
				cout << "LOW FUEL";
				SetConsoleTextAttribute(hConsole, 0x07);
			}
			cout << endl;
			cout << "Consumption per second " << engine.get_consumption_per_second(speed,gas_pedal)<<" liters" << endl;
			cout << "Engine " << (engine.started() ? "started" : "stopped") << endl;
			std::this_thread::sleep_for(1s);
		}
	}
	void info()const
	{
		engine.info();
		tank.info();
	}
};
//#define TANK_CHECK
//#define ENGINE_CHECK 
//Некоторые макросы дают тоьлко имя, и не дают никокого значения
//это #ifdef .. #endif
int main()
{
	setlocale(LC_ALL, "");
#if defined TANK_CHECK
	//если определено TANK_CHECK, тоследующий код до дериктивы #endif будет виден компилятор 
	Tank tank(40);
	tank.info();
	do
	{
		int fuel;
		cout << "Введите объем топлива:"; cin >> fuel;
		tank.fill(fuel);
		tank.info();
	} while (_getch() != 27);
#endif
#if defined ENGINE_CHECK

	Engine engine(10);
	engine.info();
#endif
	CAR BMW(40, 10);
	BMW.info();
	BMW.control();
	return 0;
}