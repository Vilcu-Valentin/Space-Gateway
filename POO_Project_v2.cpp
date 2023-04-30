#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <math.h>
using namespace std;

static int dT = 100;
struct Results
{
	int m_price; 
	int power;
	int stability;
	int radioactivity; 
	int op_price;
};

// Helper functions
// Used to set the color of the text inside the console
void SetColor(int ForgC)
{
	WORD wColor;

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (GetConsoleScreenBufferInfo(hStdOut, &csbi))
	{
		wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
		SetConsoleTextAttribute(hStdOut, wColor);
	}
	return;
}
// Generates a random number inside a given range
int RandomRange(int min, int max)
{
	int x = min + (rand() % max);
	return x;
}

class Element
{
protected:
	string name;
	string description;
	int amount;
	Results properties;
public:
	Element(string _name, string _description, Results _propert)
	{
		name = _name;
		description = _description;
		amount = 0;
		properties = _propert;
	}

	virtual Results get_properties() = 0;

	string get_name()
	{
		return get_name();
	}
	string get_description()
	{
		return description;
	}
	int get_amount()
	{
		return amount;
	}

	void inc_amount()
	{
		amount += 1;
	}
	void dec_amount()
	{
		amount -= 1;
		if (amount <= 0)
			amount = 0;
	}
};
class Fluxium : public Element
{
protected:
	int e_ratio; // Enrichment ratio, this changes the isotope ratio of the Fluxium, the bigger the ratio, the bigger the m_price and power, also changes stability and radioactivity
public:
	Fluxium(string name, string description, Results propert) :Element(name, description, propert)
	{
		e_ratio = 5;
	}

	int get_ratio()
	{
		return e_ratio;
	}
	Results get_properties()
	{
		Results temp_propert = {
			(int)(properties.m_price * amount * (float)(0.031f *(float) pow(e_ratio, 1.123f) + 0.568)),
			(int)(properties.power * amount * (float)(2.123 *(float) log(0.143 * e_ratio + 1.118f) - 0.699f)),
			(int)((properties.stability * (float)(17.312f /(float) pow(e_ratio,0.693f) - 0.705f)) / (amount * 0.25f)),
			(int)(properties.radioactivity * amount * (float)(5.168f - 1.865f *(float) pow(e_ratio,0.217f))),
			properties.op_price * amount
		};
		return temp_propert;
	}

	void set_ratio(int _e_ratio)
	{
		if (_e_ratio <= 2)
			_e_ratio = 2;
		if (_e_ratio >= 99)
			_e_ratio = 99;
		e_ratio = _e_ratio;
	}
};
class Moderators : public Element
{
	int quality; // Higher quality materials will result in more powerfull changes and also higher m_price
public:
	Moderators(string name, string description, Results propert) :Element(name, description, propert)
	{
		quality = 0;
	}

	int get_quality()
	{
		return quality;
	}
	Results get_properties()
	{
		Results temp_propert = {
			(int)(properties.m_price * amount * (float)(0.1f + 0.08f *(float) (quality + 15))),
			(int)(properties.power * amount * (float)(0.05f + 0.0485f *(float) (quality + 15))),
			(int)(properties.stability * amount * (float)(0.05f + 0.0485f *(float) (quality + 15))),
			(int)(properties.radioactivity * amount * (float)(0.05f + 0.0485f *(float) (quality + 15))),
			(int)(properties.op_price * amount * (float)(0.5f + 0.0335f *(float) (-quality + 15)))
		};
		return temp_propert;
	}

	void set_quality(int _quality)
	{
		if (_quality <= -15)
			_quality = -15;
		if (_quality >= 15)
			_quality = 15;
		quality = _quality;
	}
};

class FuelCell
{
private:
	float m_price = 0; // The material price of the finalised fuel cell
	float power = 0; // How much power does one fuel cell produce in kWh
	float stability = 0; // The more stable it is the better, low stability means it's unusable for energy production/increases op price, too much stability will generate less power
	float radioactivity = 0; // The less radioactive the better, too radioactive means it's unusable/increaseas op the price
	float op_price = 0; // How much does it cost to run the fuel cell per hour

	int max_size = 50; // Max size of the fuel cell, in-game cap
	int current_size = 0; // The smaller the better op_price

	// ### ELEMENTS ###
	Fluxium flux{"Fluxium", "Description", Results{100, 50, 100, 100, 5}};
	Moderators el1{"Exodite", "Description", Results{85, 200, -100, 175, 10} },
		el2{"Titanite", "Description", Results{10, 0, 75, 1, -10} },
		el3{"Helixite", "Description", Results{50, 0, 5, -150, 5} },
		el4{"Stabilon", "Description", Results{80, -35, 500, 250, 50} },
		el5{"Tesseractite", "Description", Results{15, 50, 0, 50, 5} };
public:
	Results get_results()
	{
		m_price = flux.get_properties().m_price + el1.get_properties().m_price + el2.get_properties().m_price +
			el3.get_properties().m_price + el4.get_properties().m_price + el5.get_properties().m_price;
		power = flux.get_properties().power + el1.get_properties().power + el2.get_properties().power + el3.get_properties().power +
			el4.get_properties().power + el5.get_properties().power;
		stability = flux.get_properties().stability + el1.get_properties().stability + el2.get_properties().stability + el3.get_properties().stability +
			el4.get_properties().stability + el5.get_properties().stability;
		radioactivity = flux.get_properties().radioactivity + el1.get_properties().radioactivity + el2.get_properties().radioactivity + el3.get_properties().radioactivity +
			el4.get_properties().radioactivity + el5.get_properties().radioactivity;
		op_price = (flux.get_properties().op_price + el1.get_properties().op_price + el2.get_properties().op_price + el3.get_properties().op_price +
			el4.get_properties().op_price + el5.get_properties().op_price) * ((current_size /(float) max_size) + 0.5f); // redo stability and radioactivity
		if (m_price <= 1)
			m_price = 1;
		if (power <= 1)
			power = 1;
		if (stability <= 1)
			stability = 1;
		if (radioactivity <= 1)
			radioactivity = 1;
		if (op_price <= 1)
			op_price = 1;
		return Results{ (int)m_price, (int)power, (int)stability, (int)radioactivity, (int)op_price };
	}
	int get_max_size()
	{
		return max_size;
	}
	int get_current_size()
	{
		current_size = flux.get_amount() + el1.get_amount() + el2.get_amount() + el3.get_amount() + el4.get_amount() + el5.get_amount();
		return current_size;
	}

	int get_addon(int index)
	{
		switch (index)
		{
		case 0:
			return flux.get_ratio();
		case 1:
			return el1.get_quality();
		case 2:
			return el2.get_quality();
		case 3:
			return el3.get_quality();
		case 4:
			return el4.get_quality();
		case 5:
			return el5.get_quality();
		}
	}
	int get_amount(int index)
	{
		switch (index)
		{
		case 0:
			return flux.get_amount();
		case 1:
			return el1.get_amount();
		case 2:
			return el2.get_amount();
		case 3:
			return el3.get_amount();
		case 4:
			return el4.get_amount();
		case 5:
			return el5.get_amount();
		}
		return -1;
	}
	void inc_amount(int index)
	{
		switch (index)
		{
		case 0:
			flux.inc_amount();
			break;
		case 1:
			el1.inc_amount();
			break;
		case 2:
			el2.inc_amount();
			break;
		case 3:
			el3.inc_amount();
			break;
		case 4:
			el4.inc_amount();
			break;
		case 5:
			el5.inc_amount();
			break;
		case 6:
			flux.set_ratio(flux.get_ratio() + 1);
			break;
		case 7:
			el1.set_quality(el1.get_quality() + 1);
			break;
		case 8:
			el2.set_quality(el2.get_quality() + 1);
			break;
		case 9:
			el3.set_quality(el3.get_quality() + 1);
			break;
		case 10:
			el4.set_quality(el4.get_quality() + 1);
			break;
		case 11:
			el5.set_quality(el5.get_quality() + 1);
			break;
		}
	}
	void dec_amount(int index)
	{
		switch (index)
		{
		case 0:
			flux.dec_amount();
			break;
		case 1:
			el1.dec_amount();
			break;
		case 2:
			el2.dec_amount();
			break;
		case 3:
			el3.dec_amount();
			break;
		case 4:
			el4.dec_amount();
			break;
		case 5:
			el5.dec_amount();
			break;
		case 6:
			flux.set_ratio(flux.get_ratio() - 1);
			break;
		case 7:
			el1.set_quality(el1.get_quality() - 1);
			break;
		case 8:
			el2.set_quality(el2.get_quality() - 1);
			break;
		case 9:
			el3.set_quality(el3.get_quality() - 1);
			break;
		case 10:
			el4.set_quality(el4.get_quality() - 1);
			break;
		case 11:
			el5.set_quality(el5.get_quality() - 1);
			break;
		}
	}
};

void draw_frame(FuelCell fuel, int selection)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	system("CLS");
	SetConsoleTextAttribute(hConsole, 15);
	cout << "Cell Size:   " << fuel.get_current_size() << "/" << fuel.get_max_size() << " [";
	int temp_f_s = ((float)fuel.get_current_size() / fuel.get_max_size()) * 10;
	for (int i = 0; i < 10; i++)
	{
		if (temp_f_s > i)
		{
			SetConsoleTextAttribute(hConsole, 8);
			cout << "#";
		}
		else
		{
			SetConsoleTextAttribute(hConsole, 8);
			cout << ".";
		}
	}
	SetConsoleTextAttribute(hConsole, 15);
	cout << "]" << endl << endl;

	if(selection == 0)
		SetConsoleTextAttribute(hConsole, 112);
	cout << "Fluxium:     " << fuel.get_amount(0);
	SetConsoleTextAttribute(hConsole, 15);
	if(selection == 6)
		SetConsoleTextAttribute(hConsole, 112);
	cout << "   Ratio [235/238]: " << fuel.get_addon(0);
	SetConsoleTextAttribute(hConsole, 15); 
	cout << " [";
	temp_f_s = fuel.get_addon(0) / 10;
	for (int i = 0; i < 10; i++)
	{
		if (temp_f_s >= i)
		{
			SetConsoleTextAttribute(hConsole, 4);
			cout << "#";
		}
		else
		{
			SetConsoleTextAttribute(hConsole, 1);
			cout << "#";
		}
	}
	SetConsoleTextAttribute(hConsole, 15);
	cout<< "]" << endl << endl;

	if (selection == 1)
		SetConsoleTextAttribute(hConsole, 112);
	cout << "Exodite:     " << fuel.get_amount(1);
	SetConsoleTextAttribute(hConsole, 15);
	if (selection == 7)
		SetConsoleTextAttribute(hConsole, 112);
	cout << "   Quality: " << fuel.get_addon(1) << endl;
	SetConsoleTextAttribute(hConsole, 15);

	if (selection == 2)
		SetConsoleTextAttribute(hConsole, 112);
	cout << "Titanite:    " << fuel.get_amount(2);
	SetConsoleTextAttribute(hConsole, 15);
	if (selection == 8)
		SetConsoleTextAttribute(hConsole, 112);
	cout << "   Quality: " << fuel.get_addon(2) << endl;
	SetConsoleTextAttribute(hConsole, 15);

	if (selection == 3)
		SetConsoleTextAttribute(hConsole, 112);
	cout << "Helixite:    " << fuel.get_amount(3);
	SetConsoleTextAttribute(hConsole, 15);
	if (selection == 9)
		SetConsoleTextAttribute(hConsole, 112);
	cout << "   Quality: "<< fuel.get_addon(3) << endl;
	SetConsoleTextAttribute(hConsole, 15);

	if (selection == 4)
		SetConsoleTextAttribute(hConsole, 112);
	cout << "Stabilon:    " << fuel.get_amount(4);
	SetConsoleTextAttribute(hConsole, 15);
	if (selection == 10)
		SetConsoleTextAttribute(hConsole, 112);
	cout << "   Quality: " << fuel.get_addon(4) << endl;
	SetConsoleTextAttribute(hConsole, 15);

	if (selection == 5)
		SetConsoleTextAttribute(hConsole, 112);
	cout << "Tesseracite: " << fuel.get_amount(5);
	SetConsoleTextAttribute(hConsole, 15);
	if (selection == 11)
		SetConsoleTextAttribute(hConsole, 112);
	cout << "   Quality: " << fuel.get_addon(5) << endl << endl << endl;
	SetConsoleTextAttribute(hConsole, 15);

	if (fuel.get_results().m_price > 6000)
		SetConsoleTextAttribute(hConsole, 4);
	if (fuel.get_results().m_price <= 6000)
		SetConsoleTextAttribute(hConsole, 6);
	if(fuel.get_results().m_price <= 2000)
		SetConsoleTextAttribute(hConsole, 2);
	cout << "Material-Price ($/p):  " << (float)fuel.get_results().m_price / 100 << endl;
	if (fuel.get_results().power > 2500)
		SetConsoleTextAttribute(hConsole, 2);
	if (fuel.get_results().power <= 2500)
		SetConsoleTextAttribute(hConsole, 6);
	if (fuel.get_results().power <= 500)
		SetConsoleTextAttribute(hConsole, 4);
	cout << "Power (mWh):           " << (float)fuel.get_results().power / 100 << endl;
	if (fuel.get_results().stability > 5000)
		SetConsoleTextAttribute(hConsole, 2);
	if (fuel.get_results().stability <= 5000)
		SetConsoleTextAttribute(hConsole, 6);
	if (fuel.get_results().stability <= 2500)
		SetConsoleTextAttribute(hConsole, 4);
	cout << "Stability:             " << (float)fuel.get_results().stability / 100 << endl;
	if (fuel.get_results().radioactivity > 6000)
		SetConsoleTextAttribute(hConsole, 4);
	if (fuel.get_results().radioactivity <= 6000)
		SetConsoleTextAttribute(hConsole, 6);
	if (fuel.get_results().radioactivity <= 3500)
		SetConsoleTextAttribute(hConsole, 2);
	cout << "Radioactivity:         " << (float)fuel.get_results().radioactivity / 100 << endl;
	if (fuel.get_results().op_price > 1500)
		SetConsoleTextAttribute(hConsole, 4);
	if (fuel.get_results().op_price <= 1500)
		SetConsoleTextAttribute(hConsole, 6);
	if (fuel.get_results().op_price <= 500)
		SetConsoleTextAttribute(hConsole, 2);
	cout << "Operating-Price ($/h): " << (float)fuel.get_results().op_price / 100 << endl;
	SetConsoleTextAttribute(hConsole, 15);

	SetConsoleTextAttribute(hConsole, 4);
	if (fuel.get_current_size() > 0 && fuel.get_amount(0) == 0)
	{
		cout << endl << "No Fluxium in the rod, please add some" << endl;
	}
	if (fuel.get_results().stability <= 1000 && fuel.get_current_size() > 0)
		cout << endl << "Stability too low, try...." << endl;
	if (fuel.get_results().radioactivity >= 9000 && fuel.get_current_size() > 0)
		cout << endl << "Radioactivity too high, try...." << endl;
	if (fuel.get_results().op_price >= 1500 && fuel.get_current_size() > 0)
		cout << endl << "Opereting price too high, try...." << endl;
	if (fuel.get_results().m_price >= 10000 && fuel.get_current_size() > 0)
		cout << endl << "Fuel rod price too high, try...." << endl;
	SetConsoleTextAttribute(hConsole, 15);

	SetConsoleTextAttribute(hConsole, 8);
	cout << endl << endl << endl << "Controls:";
	cout << endl << "W/S to navigate up/down";
	cout << endl << "F to switch to the right collumn";
	cout << endl << "A/D to increase / decrease the numbers";
	cout << endl << "H for help";
}
void draw_help()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	system("CLS");
	SetConsoleTextAttribute(hConsole, 15);
	cout << "The goal of this simulator is to make the best fuel cell possible" << endl;
	cout << "by having a good power output, that has great stability, low radioactivity and low cost." << endl << endl << endl ;
	cout << "Cell size: refers to the amount of materials inside the cell, the lower the amount the lower the operating cost."<< endl << endl << endl;
	cout << "Fluxium: this is the material that generates power, you can change the enrichment ratio [235/238]." << endl;
	cout << "By adjusting the amount and the ratio you can generate more power, however bigger ratio and amounts also leads to more cost and decreased stability."<< endl << endl;
	cout << "Exodite: this is a very volatile moderator that increase power explosively, however it is very radioactive and also unstable. Adding more of it to the fuel cell also increases operating costs." << endl << endl;
	cout << "Titanite: this is a very cheap moderator used to slightly increase stability and decrease operating costs, this material also has no effect on power output." << endl << endl;
	cout << "Helixite: this moderator is used as shielding against radiation while also not decreasing the reaction power or stability." << endl << endl;
	cout << "Stabilon: this moderator is used to quickly stabilise the reaction, however it tends to decrease power ouput significantly and also increase radiation by a lot." << endl << endl;
	cout << "Tesseractite: this moderator can slightly increase the power output without sacrificing stability, and only adding a tiny amount of radiation in the process" << endl << endl << endl;
	cout << "The quality setting make moderators more potent (it increases both good and bad effects) while decreasing operating cost, for the an increased material cost. If the quality setting is decrease the reverse will be applied" << endl << endl;
	cout << "The results are colored, green being a good value, yellow being mediocre and red being bad." << endl;
	cout << "Additionally you can receive red flags, if any appear the resulting fuel cell is inefective.";
}

int main()
{
	srand((unsigned)time(0));
	FuelCell fuel_cell;
	draw_frame(fuel_cell, 0);

	int selection = 0;
	bool isHelping = false;

	do
	{
		if (GetKeyState('A') & 0x8000)
		{
			fuel_cell.dec_amount(selection);
			draw_frame(fuel_cell, selection);
		}
		if (GetKeyState('D') & 0x8000)
		{
			if (fuel_cell.get_current_size() < fuel_cell.get_max_size() && selection < 6)
			{
				fuel_cell.inc_amount(selection);
				draw_frame(fuel_cell, selection);
			}

			if (selection >= 6)
			{
				fuel_cell.inc_amount(selection);
				draw_frame(fuel_cell, selection);
			}
		}
		if (GetKeyState('W') & 0x8000)
		{
			if(selection > 0)
				selection--;
			draw_frame(fuel_cell, selection);
		}
		if (GetKeyState('S') & 0x8000)
		{
			if(selection < 5)
				selection++;
			draw_frame(fuel_cell, selection);
		}
		if (GetKeyState('F') & 0x8000)
		{
			if (selection <= 5)
				selection += 6;
			else
				selection -= 6;
			draw_frame(fuel_cell, selection);
		}
		if (GetKeyState('H') & 0x8000)
		{
			if (isHelping == false)
			{
				draw_help();
			}
			else {
				draw_frame(fuel_cell, selection);
			}
			isHelping = !isHelping;
		}
		Sleep(100);
	} while (true);
}