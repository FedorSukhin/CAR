#include <iostream>
#include <conio.h>
#include<thread>
#include<chrono>
#include<windows.h>
using namespace std;
using namespace std::literals::chrono_literals;//��� ������������� ���,��� � ��

//#define MIN_TANK_VOLUME 20//���������� ����������
//#define MAX_TANK_VOLUME 80
//# ��������� ��� ������ ���� ���������
//��������� - ��� �������� ����������� �� ���������� ��������� ��������
// #include ��������� ����������� ��� � ������ ����� ����� ���������� ������ ����
//#define ������� ���������������� ���� ��� ����
// � ����� ��� �� ����� ��������� ����� ��������
#define Enter 13
#define Esc 27

class Tank
{
	static const int MIN_TANK_VOLUME = 20;//���������� ���������� ������� �������� ������ ������ � ������
	static const int MAX_TANK_VOLUME = 80;// � �� � ������ �������
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
		consumption_per_second = CONSUMPTION * 3.0e-5;
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
	//1 - 60 �� / � -> 0, 0020 � / ���
	//	. 61 - 100 �� / � -> 0, 0014 � / ���
	//	. 101 - 140 �� / � -> 0, 0020 � / ���
	//	. 141 - 200 �� / � -> 0, 0025 � / ���.������
	//	201 - 250 �� / � -> 0, 0030 � / �
	double get_consumption_per_second(int speed)
	{
		consumption_per_second = CONSUMPTION * 3.0e-5;
		if (speed > 0 && speed <= 60)
		{
			this->consumption_per_second = CONSUMPTION * 3.0e-5*6.666666;
		}
		else if (speed > 60 && speed <= 100)
		{
			this->consumption_per_second = CONSUMPTION * 3.0e-5*4.666666;
		}
		else if (speed > 100 && speed <= 140)
		{
			this->consumption_per_second = CONSUMPTION * 3.0e-5*6.666666;
		}
		else if (speed > 140 && speed <= 200)
		{
			this->consumption_per_second = CONSUMPTION * 3.0e-5*8.333333;
		}
		else if (speed > 200)
		{
			this->consumption_per_second = CONSUMPTION * 3.0e-5*10;
		}
		return this->consumption_per_second;
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
		threads.panel_thread = std::thread(&CAR::panel, this);//��������� panel() � ������ panel_thread
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
				case 'I':case'i'://ignition -���������
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
					ac�elerate();
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
		//�������� ��� ���������
		while (engine.started() && tank.give_Fuel(engine.get_consumption_per_second(speed)))
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
	void ac�elerate()
	{
		if (speed < MAX_SPEED && engine.started())
		{
			speed += 10;
			if (speed >= MAX_SPEED)
			{
				speed = MAX_SPEED;
			}
			std::this_thread::sleep_for(1s);
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
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);//�������� ���������� ���� �������
		while (driver_inside)
		{
			system("CLS");
			scale(hConsole);
			cout << "Speed  " << speed << " km/h" << endl;
			cout << "Fuel level:\t" << tank.getFuel() << " liters\t";
			if (tank.getFuel() < 5)
			{
				SetConsoleTextAttribute(hConsole, 0xCF);//0xCF-������� ���"�","F"-����� �����
				cout << "LOW FUEL";
				SetConsoleTextAttribute(hConsole, 0x07);
			}
			cout << endl;
			cout << "Consumption per second " << engine.get_consumption_per_second(speed)<<" liters" << endl;
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
//��������� ������� ���� ������ ���, � �� ���� �������� ��������
//��� #ifdef .. #endif
int main()
{
	setlocale(LC_ALL, "");
#if defined TANK_CHECK
	//���� ���������� TANK_CHECK, ����������� ��� �� ��������� #endif ����� ����� ���������� 
	Tank tank(40);
	tank.info();
	do
	{
		int fuel;
		cout << "������� ����� �������:"; cin >> fuel;
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