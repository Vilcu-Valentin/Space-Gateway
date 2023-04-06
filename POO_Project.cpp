#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
using namespace std;

// ############## GENERATION SETTINGS ############## //

int occupancy = 50; // 0-Free 100-Full
int occup_random = 30; // 0-No randomness 50-Max randomness

int ship_size = 100; // 3-Smalles 1000-Max
int ship_size_randomness = 25; // 1-Minimal 50-Max randomness
int ships_no = 10; // Max number of ships to be generated 1-Min 50-Max

int class_distribution_randomness = 2;

// ##################### DATA ###################### //

struct Ship_ID
{
    int id;
    char name[5];
}ship_names[11] = {
    {0, "ALCE"},
    {1, "BHMT"},
    {2, "PLCN"},
    {3, "METO"},
    {4, "EVOX"},
    {5, "HLIO"},
    {6, "FATE"},
    {7, "ENTR"},
    {8, "ATLS"},
    {9, "ROYL"}
};
struct Destination_ID
{
    int id;
    char name[16];
}destination_names[11] = {
    {0, "Alpha Centauri"},
    {1, "Sirius A      "},
    {2, "Barnad's Star "},
    {3, "Betelgeuse    "},
    {4, "Procyon A     "},
    {5, "Tau Ceti      "},
    {6, "Gliese 1      "},
    {7, "UI Scuti      "},
    {8, "Vega          "},
    {9, "Arcturus      "}
};

// ################## STRUCTURES ################### //

struct Vector2
{
    int x;
    int y;
};
struct Vector3
{
    int x;
    int y;
    int z;
};

class Seat
{
private:
    int type; // 1-First Class 2-Business Class 3-Economy Class
    int seat_num; // 1-99....
    int price; // how much does the seat cost

    bool isTaken; // is the seat taken ?
    int ship_id; // Keep track of parent ship
public:
    Seat()
    {
        type = -1;
        seat_num = -1;
        price = -1;
        isTaken = false;
        ship_id = -1;
    }
    Seat(int _type, int _seat_num, int _price, bool _isTaken, int _ship_id)
    {
        type = _type;
        seat_num = _seat_num;
        price = _price;
        isTaken = _isTaken;
        ship_id = _ship_id;
    }

    void set_Taken(bool taken)
    {
        isTaken = taken;
    }
    void set_price(int _price)
    {
        price = _price;
    }

    int get_type()
    {
        return type;
    }
    int get_number()
    {
        return seat_num;
    }
    int get_price()
    {
        return price;
    }
    int get_ship_id()
    {
        return ship_id;
    }
    bool get_taken()
    {
        return isTaken;
    }
};
class Ship
{
private:
    int ship_id;
    int destination_id;

    int duration;

    Seat seats[1001];
    int seats_num;
public:
    Ship()
    {
        ship_id = -1;
        destination_id = -1;
        duration = -1;
        seats_num = -1;
    }
    Ship(int _ship_id, int _destination_id, int _duration, Seat _seats[], int _seats_num)
    {
        ship_id = _ship_id;
        destination_id = _destination_id;
        duration = _duration;
        seats_num = _seats_num;
        for (int i = 0; i < _seats_num; i++)
        {
            seats[i] = _seats[i];
        }
    }

    int get_ship_id()
    {
        return ship_id;
    }
    int get_destination_id()
    {
        return destination_id;
    }
    int get_duration()
    {
        return duration;
    }
    int get_seats_num()
    {
        return seats_num;
    }
    Seat& get_seat(int index)
    {
        return seats[index];
    }
    int get_occupancy();
};
int Ship :: get_occupancy()
{
    int nr = 0;
    for (int i = 0; i < seats_num; i++)
    {
        if (get_seat(i).get_taken() == true)
            nr++;
    }
    float result = (float)nr / seats_num;
    return (int)(result * 100);
}


//Helper functions
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
//Gets the min/max price of a seat from a given ship/seat class
int Get_Min_Max(Ship &ship, int seat_num, int type, bool max)
{
    if (max != true)
    {
        int _max = 0;
        for (int i = 0; i < seat_num; i++)
        {
            if(ship.get_seat(i).get_type() == type && ship.get_seat(i).get_taken() != true)
                if (ship.get_seat(i).get_price() > _max)
                    _max = ship.get_seat(i).get_price();
        }
        return _max;
    }
    else
    {
        int min = 99999;
        for (int i = 0; i < seat_num; i++)
        {
            if(ship.get_seat(i).get_type() == type && ship.get_seat(i).get_taken() != true)
                if (ship.get_seat(i).get_price() < min)
                    min = ship.get_seat(i).get_price();
        }
        return min;
    }
}

// ################### MAIN CODE ################### //

//This function generates the class distribution i.e. how many seats for each class
Vector2 slice_seats(int amount)
{
    int x = RandomRange(amount / 50 + 1, amount / 15 + 1);
    int y = RandomRange(amount / 10 + 1, amount / 4 + 1);
    return Vector2{ x, y };
}

//This function generates a random ship using the world settings
Ship generate_ship(int ship_num)
{
    int id = RandomRange(0, 9);
    int dest = RandomRange(0, 9);
    int duration = RandomRange(10, 99);
    int seats = RandomRange(max(1, ship_size - ship_size_randomness * 10), min(ship_size + ship_size_randomness * 10, 1000));

    if (duration > 99)
        duration = 99;
    if (duration < 10)
        duration = 10;
    if (seats >= 1000)
        seats = 1000;

    int current_occupancy = RandomRange(max(0, occupancy - occup_random * 10), min(99, occupancy + occup_random * 10));

    Seat* actual_seats = new Seat[seats];
    Vector2 distribution = slice_seats(seats);
    for (int j = 0; j < seats; j++)
    {
        int chance = RandomRange(0, 100);
        bool taken;

        if (chance <= current_occupancy)
            taken = true;
        else
            taken = false;

        if (j < distribution.x)
        {
            actual_seats[j] = Seat(1, j, RandomRange(1000, 10000), taken, ship_num);
        }
        else if (j < distribution.y)
        {
            actual_seats[j] = Seat(2, j, RandomRange(400, 2500), taken, ship_num);
        }
        else
        {
            actual_seats[j] = Seat(3, j, RandomRange(25, 500), taken, ship_num);
        }
    }

    return Ship(id, dest, duration, actual_seats, seats);
}

// ################### GRAPHICS #################### //

//This functions are used to display the graphics
void DisplaySeats(int type, Ship &ship)
{
    int seat_num = ship.get_seats_num();
    for (int i = 0; i < seat_num; i++)
    {
        if (ship.get_seat(i).get_type() == type)
        {
            if (ship.get_seat(i).get_taken() == false)
            {
                if (type == 1)
                {
                    SetColor(14);
                    cout << "омн  ";
                    SetColor(15);
                }
                if (type == 2)
                {
                    SetColor(11);
                    cout << "Рўй  ";
                    SetColor(15);
                }
                if (type == 3)
                {
                    SetColor(10);
                    cout << "ўўў  ";
                    SetColor(15);
                }

                cout << ship.get_seat(i).get_number() + 1 << "  " << ship.get_seat(i).get_price() << "$ \n";
            }
        }
    }
}
void DisplaySeats_Debug(Ship &ship, int ship_id, Seat ticket)
{
    for (int i = 0; i < ship.get_seats_num(); i++)
    {
        cout << endl;
        //ship.get_seat(i).set_Taken(true);
        if (ship.get_seat(i).get_type() == 1)
        {
            SetColor(14);
            cout << "омн " << ship.get_seat(i).get_number() + 1 << "   ";
            if (ship.get_seat(i).get_taken() == false)
            {
                SetColor(2);
                cout << "Free";
            }
            else
            {
                SetColor(12);
                cout << "Taken";
            }
        }

        if (ship.get_seat(i).get_type() == 2)
        {
            SetColor(11);
            cout << "Рўй " << ship.get_seat(i).get_number() + 1<< "   ";
            if (ship.get_seat(i).get_taken() == false)
            {
                SetColor(2);
                cout << "Free";
            }
            else
            {
                SetColor(12);
                cout << "Taken";
            }
        }

        if (ship.get_seat(i).get_type() == 3)
        {
            SetColor(10);
            cout << "ўўў " << ship.get_seat(i).get_number() + 1 << "   ";
            if (ship.get_seat(i).get_taken() == false)
            {
                SetColor(2);
                cout << "Free";
            }
            else
            {
                SetColor(12);
                cout << "Taken";
            }
        }

        if (ship.get_seat(i).get_number() == ticket.get_number() && ticket.get_ship_id() == ship_id)
        {
            SetColor(13);
            cout << " Your Seat";
        }
    }

    SetColor(15);
}
void DisplayShips(Ship ships[])
{
    for (int i = 0; i < ships_no; i++)
    {
        if (i + 1 < 10)
            cout << " ";
        cout << i + 1 << ".   " << ship_names[ships[i].get_ship_id()].name << "     ";
        cout << destination_names[ships[i].get_destination_id()].name;
        cout << "   " << ships[i].get_duration() << " - Hours   ";
        cout << "[";
        for (int j = 0; j < 10; j++)
        {
            if (j * 10 <= ships[i].get_occupancy())
                cout << "л";
            else
                cout << "Б";
        }
        cout << "] ";
        cout << ships[i].get_occupancy() << "%   ";
        cout << min(Get_Min_Max(ships[i], ships[i].get_seats_num(), 1, true),
            min(Get_Min_Max(ships[i], ships[i].get_seats_num(), 3, true), Get_Min_Max(ships[i], ships[i].get_seats_num(), 2, true))) << "$" << endl;
    }
}
void DisplayTicket(Seat ticket, Ship ships[])
{
    cout << endl << "/////////////////////////" << endl;
    if (ticket.get_number() >= 0) 
    {
        if (ticket.get_type() == 1)
        {
            SetColor(14);
            cout << "омн First Class Ticket" << endl;
            SetColor(15);
        }
        if (ticket.get_type() == 2)
        {
            SetColor(11);
            cout << "Рўй Business Class Ticket" << endl;
            SetColor(15);
        }
        if (ticket.get_type() == 3)
        {
            SetColor(10);
            cout << "ўўў Economy Class Ticket" << endl;
            SetColor(15);
        }
        cout << "Seat Number: " << ticket.get_number() + 1 << endl;
        cout << "Price: " << ticket.get_price() << "$" << endl;
        cout << "Ship: " << ship_names[ships[ticket.get_ship_id()].get_ship_id()].name;
        cout << " -> " << destination_names[ships[ticket.get_ship_id()].get_destination_id()].name << endl;
    }
    else
    {
        SetColor(12);
        cout << "You don't have any tickets yet" << endl;
        SetColor(15);
    }
    cout << "/////////////////////////" << endl;
}

//Once you choose to change your ticket, this function frees it inside the ship
void Delete_Ticket(Seat& ticket)
{
    ticket.set_Taken(false);
}


int main()
{
    //Seed for the rand() function
    srand((unsigned int)time(0));

    Ship* main_ships = new Ship[ships_no]; // ships array;
    Seat* ticket = new Seat; // your ticket 
    for (int i = 0; i < ships_no; i++) 
        main_ships[i] = generate_ship(i);
    
    // Auxiliary variables used for the switch statement for the GUI interface
    int choice = 0; 
    int regA = 0;
    int regB = 0;
    int regC = 0;

    //GUI interface switch statement
    do
    {
        int index = 0;
        switch (choice)
        {
        case 0:
            system("CLS");
            cout << "Main-Menu" << endl << endl;
            cout << "Option select:" << endl;
            cout << "1. Buy ticket" << endl;
            cout << "2. View bought ticket" << endl;
            cout << "3. Settings[Debug]" << endl;
            cout << endl << endl << "Input: "; cin >> choice;
            break;
        case 1:
            system("CLS");
            cout << "Select a destination from the list below: " << endl << endl;
            cout << "No.  Ship_ID  Destination       Duration     Capacity           Cheapest" << endl;
            DisplayShips(main_ships);
            cout << endl << endl << "Input: "; cin >> regB;
            if (regB > ships_no || regB <= 0)
            {
                choice = 13;
                regB = 1;
                break;
            }
            if (main_ships[regB - 1].get_occupancy() >= 100)
                choice = 13;
            else
                choice = 12;
            break;
        case 12:
            system("CLS");
            cout << "Select a seating class: " << endl << endl;
            if (Get_Min_Max(main_ships[regB - 1], main_ships[regB - 1].get_seats_num(), 1, true) < 99999)
            {
                cout << "1. омн First class      ";
                cout << Get_Min_Max(main_ships[regB - 1], main_ships[regB - 1].get_seats_num(), 1, true) << "$ - ";
                cout << Get_Min_Max(main_ships[regB - 1], main_ships[regB - 1].get_seats_num(), 1, false) << "$\n";
            }
            if (Get_Min_Max(main_ships[regB - 1], main_ships[regB - 1].get_seats_num(), 2, true) < 99999)
            {
                cout << "2. Рўй Business class   ";
                cout << Get_Min_Max(main_ships[regB - 1], main_ships[regB - 1].get_seats_num(), 2, true) << "$ - ";
                cout << Get_Min_Max(main_ships[regB - 1], main_ships[regB - 1].get_seats_num(), 2, false) << "$\n";
            }
            if (Get_Min_Max(main_ships[regB - 1], main_ships[regB - 1].get_seats_num(), 3, true) < 99999)
            {
                cout << "3.  ў  Economy class    ";
                cout << Get_Min_Max(main_ships[regB - 1], main_ships[regB - 1].get_seats_num(), 3, true) << "$ - ";
                cout << Get_Min_Max(main_ships[regB - 1], main_ships[regB - 1].get_seats_num(), 3, false) << "$\n";
            }
            cout << endl << endl << "Input: "; cin >> regA;
            choice = 123;
            break;
        case 13:
            system("CLS");
            SetColor(4);
            cout << "The ship you have selected has no more room" << endl;
            SetColor(15);
            cout << "You will be redirected to the selection menu shortly" << endl;
            Sleep(1500);
            choice = 1;
            break;
        case 123:
            system("CLS");
            cout << "Select a seat from the list below: " << endl << endl;
            DisplaySeats(regA, main_ships[regB - 1]);
            cout << endl << endl << "Input: "; cin >> regC;
            choice = 1230;
            break;
        case 1230:
            system("CLS");
            if (main_ships[regB - 1].get_seat(regC - 1).get_taken() == true || main_ships[regB - 1].get_seat(regC - 1).get_number() < 0)
            {
                SetColor(4);
                cout << "The seat you are trying to buy is already taken" << endl;
                SetColor(15);
                cout << "You will be redirected to the seat selection menu shortly" << endl;
                Sleep(1000);
                choice = 123;
            }
            else
            {
                Delete_Ticket(*ticket);
                main_ships[regB - 1].get_seat(regC - 1).set_Taken(true);
                ticket = &main_ships[regB - 1].get_seat(regC - 1);
                SetColor(2);
                cout << "Your ticket has been purchased succesfully!" << endl;
                SetColor(15);
                cout << "You will be redirected to the menu shortly" << endl;
                Sleep(1000);
                choice = 2;
            }
            break;
        case 2:
            system("CLS");
            cout << "This is your current ticket" << endl;
            DisplayTicket(*ticket, main_ships);
            cout << endl << endl << "1. Change/Buy ticket" << endl;
            cout << "9. Go back" << endl;
            cout << endl << "Input: "; cin >> choice;
            if (choice == 9)
                choice = 0;
            else
                choice = 1;
            break;
        case 3:
            system("CLS");
            cout << "Settings Menu" << endl << endl;
            cout << "1. Generation Settings" << endl;
            cout << "2. Ships information" << endl;
            cout << "9. Go Back" << endl << endl;
            cout << "Input: "; cin >> choice;
            if (choice == 9)
                choice = 0;
            else
                choice += 30;
            break;
        case 31:
            system("CLS");
            cout << "Generation Settings" << endl << endl;
            cout << "1. Change Settings" << endl;
            cout << "2. Generate new World - (It will use the current settings)" << endl;
            cout << "9. Go Back" << endl << endl;
            cout << "Current Settings: " << endl;
            cout << "Ship Number:          ";
            SetColor(3); cout << ships_no;
            SetColor(8); cout << " Between [1] and [50] :: The number of ships to be generated" << endl;
            SetColor(15); cout << "Ship Size:            ";
            SetColor(3); cout << ship_size;
            SetColor(8); cout << " Between [3] and [1000] :: The number of seats on a given ship" << endl;
            SetColor(15); cout << "Ship Size Randomness: ";
            SetColor(3); cout << ship_size_randomness;
            SetColor(8); cout << " Between [1] and [50] :: Randomization between different ships size" << endl;
            SetColor(15); cout << "Occupancy:            ";
            SetColor(3); cout << occupancy;
            SetColor(8); cout << " Between [0] and [100] :: How full should the ship be" << endl;
            SetColor(15); cout << "Occupancy Randomness: ";
            SetColor(3); cout << occup_random;
            SetColor(8); cout << " Between [0] and [50] :: Randomization between different ships occupancy" << endl << endl;
            SetColor(15); cout << "Input: "; cin >> choice;
            if (choice == 9)
                choice = 3;
            else
                choice += 310;
            break;
        case 311:
            system("CLS");
            cout << "Ship Number [1]-[50]: ";
            SetColor(3);  cout << ships_no << " -> "; cin >> ships_no;
            SetColor(15); cout << "\nShip Size [3] - [1000]: ";
            SetColor(3); cout << ship_size << " -> "; cin >> ship_size;
            SetColor(15); cout << "\nShip Size Randomness [1] - [50]: ";
            SetColor(3); cout << ship_size_randomness << " -> "; cin >> ship_size_randomness;
            SetColor(15); cout << "\nOccupancy [0] - [100]: ";
            SetColor(3); cout << occupancy << " -> "; cin >> occupancy;
            SetColor(15); cout << "\nOccupancy Randomness [0] - [50]: ";
            SetColor(3); cout << occup_random << " -> "; cin >> occup_random;
            SetColor(15);
            choice = 31;
            break;
        case 312:
            system("CLS");
            SetColor(2);
            cout << "New World is being generated..." << endl;
            SetColor(15);
            Sleep(2000);
            Delete_Ticket(*ticket);
            main();
            break;
        case 32:
            system("CLS");
            cout << "Ship information menu" << endl << endl;
            cout << "Select a ship from the list" << endl;
            DisplayShips(main_ships);
            cout << "Input: "; cin >> regA;
            choice = 321;
            break;
        case 321:
            system("CLS");
            cout << "Seat List of ship " << ship_names[main_ships[regA - 1].get_ship_id()].name << endl << endl;
            DisplaySeats_Debug(main_ships[regA - 1], regA - 1, *ticket);
            cout << endl << endl << "Press 0 to go back" << endl;
            cout << "To change a seat availability enter the seat number: "; cin >> choice;
            if (choice == 0)
                choice = 3;
            else
            {
                if (choice - 1 == ticket->get_number())
                {
                    SetColor(4);
                    cout << endl << "The seat you have selected is your ticket, you cannot delete it from here!";
                    SetColor(15);
                    Sleep(1000);
                    choice = 321;
                }
                else
                {
                    main_ships[regA - 1].get_seat(choice - 1).set_Taken(!main_ships[regA - 1].get_seat(choice - 1).get_taken());
                    choice = 321;
                }
            }
            break;
        }
    } while (true);
}