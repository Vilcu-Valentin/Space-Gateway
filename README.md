# Space-Gateway

Main File Name: POO_Project.cpp
Theme: Space Gateway

Classes: 

-Seat: It is used to store individual seats on a ship. The class stores a lot of data about the seat like,
       Seat_Type: what type of class is it, first class, bussiness, economy.
       Seat_Number: what id number the seat has
       Seat_Price: how much does the seat cost
       Seat_Availability: Is the seat taken or free
       
-Ship: The ship has a destination and a number of seats on it that you can choose to buy a ticket

Currently in the app you can buy tickets to destinations. You will be presented with a list of ships to choose from and then you will be presented
with a list of empty seats on the ship. After you buy the ticket you will redirected to a page where you can see a summary of your ticket, from there
you could either go back or change your ticket. (In the future, you will be able to buy multiple tickets at once)

Besides that you can use option 3 to generate a new world (aka. generate new ships), there you will see the current settings of the generator.
Additionally you can see current ships information. If you select a ship you will be presented with a list of all the seats, here you can type 
the seat number and it will change it's current occupancy (from taken to free; from free to taken)


Next Update:
Adding more types of ships using inheritence. Things like cargo ships or battleships etc.
Adding another class that will store user information, this could be used to apply discounts to tickets or restrictions to destinations (passport visa) etc.
Improving the generation: - General improvements
                          - Adding departure times to the ships and keeping track of which ship has been generated so that they will not overlap
Adding "Transportation Businesses", entities that can buy ships and have an inventory, they will be providing the ships to customers.
And more...
