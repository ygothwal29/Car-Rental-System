                            /*Problem Statement*/
/*You are required to implement a car rental system in C++ language using Object-Oriented
Programming concepts. Implement the system on command line interface.
List of basic functionalities:
1. Keep records of cars rented per customer/employee.
2. A customer is given a car if his/her customer_record is good enough. A customer_record
is updated based on several factors:
a. Car returned later than return_date.
b. Damage caused to the car. The condition in which the car has been returned.
c. The same will be for employees i.e. employee_record.
3. Maintain different databases for customers, employees, and cars.
4. Maintain three different classes for customers, employees, and manager.
a. A customer can see all the available cars. Can browse all the cars he is currently
renting. Can rent a limited number of cars as per his/her customer_record.
(Optional: If a customer is new, register him with an average customer_record
value). A fine will be imposed per day from the day of return.
b. An employee can see all the available cars. Can browse all the cars he/she is
currently renting. Can rent a limited number of cars as per his/her
employee_record. They are given a 15% discount on all cars. A fine will be
imposed per day from the day of return.
c. A manager can add/update/delete any customer or employee. Can also
update/delete/add cars in the store. Can see all the cars and who they are rented to.
5. Every method must be present in its appropriate class. Try to make use of both the private
and public functionalities for attributes. Prudently use inheritance, polymorphism, data
abstraction, and other concepts of OOPs. Start with atleast 5 cars, 5 customers, 5
employees and 1 manager. Feel free to choose default value(s) for other parameters.
6. For reference, you can check out the block diagram given on the next page.*/

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <ctime>
using namespace std;

// When using normal functionality, use 86400 as one day but when testing use 30seconds as oneday (or 3.5 minutes for a week) 
#define oneday 86400
// #define oneday 30

int stringtointeger(string s){
    stringstream ss(s);
    int x = 0;
    ss >> x;
    return x;
}

void display_due_date(time_t duedate){
    if(duedate == 0){
        cout << setw(15);
        cout << "Not rented\n";
        return;
    }
    auto now = time(0);
    if(now > duedate){
        cout << setw(15);
        cout << "Overdue\n";
    }
    // Display duedate in human readable format
    else{
        auto duedate_tm = localtime(&duedate);
        cout << duedate_tm->tm_mday << "/" << 1 + duedate_tm->tm_mon << "/" << 1900 + duedate_tm->tm_year << endl;
    }
}



//--------------------------------------------------------------------Car-----------------------------------------------------------------------//

class Car{
    public:
        string id;
        string name;
        string model;
        int rent;
        int price;
        int seats;
        string color;
        bool rented;
        time_t duedate;
        friend class Manager;
        static int count;
        int condition;

        Car(string name, string model, int rent, int price, int seats, string color, int condition = 100, time_t duedate = 0){
            this->id = to_string(++count);
            this->name = name;
            this->model = model;
            this->price = price;
            this->rent = rent;
            this->seats = seats;
            this->color = color;
            this->rented = false;
            this->condition = condition;
            this->duedate = duedate;
        }
};
int Car::count = 0;
vector<Car> cars;

//----------------------------------------------------------------------User-------------------------------------------------------------------//

class User{
    protected:
        string password;
    public:
        int id;
        string name;
        static int count_customer;
        static int count_employee;
        void view_all_cars();
        bool check_password(string password);
};
int User::count_customer = 0;
int User::count_employee = 0;

void User::view_all_cars(){
    
    cout << setw(3) << "Id" << setw(15) << "Name" << setw(15) << "Model" << setw(7) << "Rent" << setw(12) << "Price" << setw(6) << "Seats" << setw(10) << "Color" << setw(10) << "Available" << setw(10)<< "Condition" << setw(15)<< "Due Date" << endl;
    for(auto car: cars){
        cout << setw(3) << car.id << setw(15) << car.name << setw(15) << car.model << setw(7) << car.rent << setw(12) << car.price << setw(6) << car.seats << setw(10) << car.color << setw(10) << !car.rented << setw(10) << car.condition << setw(15);
        display_due_date(car.duedate);
    }
}

bool User::check_password(string password){
    return (this->password == password);
}

//--------------------------------------------------------------------Customer-----------------------------------------------------------------//

class Customer: public User{
    protected:
        vector<Car> rented_cars;
        int dues;
        int customer_record;

    public:

        friend class Manager;

        Customer(string name, string password, int customer_record = 100, int dues = 0, vector<Car> rented_cars = {}){
            this->name = name;
            this->password = password;
            this->id = ++count_customer;
            this->dues = dues;
            this->customer_record = customer_record;
            this->rented_cars = rented_cars;
        }
        void login();
        void rent_car();
        void return_car();
        void view_rented_cars();
        void view_dues();
        void clear_dues();
};

vector<Customer> customers;

void Customer::rent_car(){
    cout << "Enter the ID of the car you want to rent: ";
    string car_id;
    cin >> car_id;
    for(auto &car: cars){
        if(car.id == car_id ){
            if(car.rented){
                cout << "Car is already rented\n";
                return;
            }
            car.rented = true;
            // add rent in the dues of customer
            this->dues += car.rent;
            rented_cars.push_back(car);
            car.duedate = time(0) + oneday*7;
            cout << "Car rented successfully\n";
            return;
        }
    }
    cout << "Car with given ID not found\n\n\n";
}

void Customer::return_car(){
    cout << "Enter the ID of the car you want to return: ";
    string car_id;
    cin >> car_id;
    
    for(auto it = rented_cars.begin(); it != rented_cars.end(); ++it){
        if(it->id == car_id){
            // Update availability of the car in the original cars list
            for(auto &original_car: cars){
                if(original_car.id == car_id){
                    original_car.rented = false;
                    original_car.duedate = 0;
                    break;
                }
            }
            auto now = time(0);
            if(now > it->duedate){
                int fine = (it->rent*((now - it->duedate)/oneday))/5;
                this->dues += fine;
                this->customer_record -= 2*(now - it->duedate)/oneday;
                cout << "You have been fined "<< fine <<" for returning the car late\n";
            }
            // Remove car from rented cars list
            rented_cars.erase(it);
            cout << "Car returned successfully\n\n\n";
            return;
        }
    }
    cout << "Car not found\n";
}
void Customer::view_rented_cars(){
    if(rented_cars.empty()){
        cout << "No cars rented\n\n\n";
        return;
    }
    cout << setw(3) << "Id" << setw(15) << "Name" << setw(15) << "Model" << setw(7) << "Rent" << setw(12) << "Price" << setw(6) << "Seats" << setw(10) << "Color" << setw(10) << "Due Date" << setw(10) << "Condition" << setw(15) << "Due Date" << endl;
    for(auto car: rented_cars){
        cout << setw(3) << car.id << setw(15) << car.name << setw(15) << car.model << setw(7) << car.rent << setw(12) << car.price << setw(6) << car.seats << setw(10) << car.color << setw(10) << car.duedate << setw(10) << car.condition;
        display_due_date(car.duedate);
    }
    cout << "\n\n\n";
}

void Customer::view_dues(){
    cout << "Your outstanding dues are: " << this->dues << "\n\n\n";
}

void Customer::clear_dues(){
    if(this->dues == 0) {
        cout << "You don't have any dues\n\n\n";
        return;
    }
    cout << "Your outstanding dues are: " << this->dues << endl;
    cout << "Enter the amount you want to pay: ";
    string samount;
    cin >> samount;
    int amount = stringtointeger(samount);
    // if(amount > this->dues){
    //     cout << "Amount entered is greater than the dues\n";
    //     return;
    // }
    // else if(amount <= 0){
    //     cout << "You are not allowed to enter negative or null amount\n";
    //     return;
    // }
    if(amount > 0 && amount <= this->dues){
        this->dues -= amount;
        cout << "Dues cleared successfully\n";
        cout << "Your New outstanding dues are: " << this->dues << "\n\n\n";
    }
    else{
        cout << "Invalid amount\n";
        return;
    }
    
}

void Customer::login(){
    // Display menu for customer
    bool logged_in = true;
    cout << "Welcome " << this->name << endl;
    while (logged_in)
    {    
        cout << "Choose your option\n";
        cout << "1. View all cars\n2. View your rented cars\n3. Rent a car\n4. Return a car \n5. Check Your Dues\n6. Clear Dues \n0. Logout\nEnter your choice: ";
        char choice;
        cin >> choice;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (choice)
        {
        case '1':
            view_all_cars();
            break;
        case '2':
            view_rented_cars();
            break;
        case '3':
            rent_car();
            break;
        case '4':
            return_car();
            break;
        case '5':
            view_dues();
            break;
        case '6':
            clear_dues();
            break;
        case '0':
            logged_in = false;
            break;
        default:
            cout << "Invalid choice\n";
            break;
        }
    }

}


void login_customer(){
    bool logging_in = true;
    
    while (logging_in){    
        cout << "Choose your option\n";
        cout << "1. Login\n2. Register\n0. Exit \nEnter your choice: ";
        char choice;
        cin >> choice;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (choice)
        {
        case '1':
        {
            cout << "Enter Your Username\n";
            string name;
            getline(cin, name);
            cout << "Enter Your Password\n";
            string password;
            cin >> password;
            bool check = false;
            for(auto customer: customers){
                if(customer.name == name && customer.check_password(password)){
                    customer.login();
                    check = true;
                    break;
                }
            }
            if(!check) cout << "Login Failed: Invalid name or password\n";
            break;
        }
        case '2':
        {
            cout << "Enter a Username\n";
            string name;
            getline(cin, name);
            cout << "Enter Your Password\n";
            string password;
            cin >> password;
            cout << "Confirm Your Password\n";
            string confirm_password;
            cin >> confirm_password;
            if (password != confirm_password){
                cout << "Register Failed: Passwords do not match\n";
                break;
            }
            bool check = true;
            for(auto customer: customers){
                if(customer.name == name){
                    cout << "Register Failed: Username already exists\n";
                    check = false;
                    break;
                    
                }
            }
            if(check){
                Customer new_customer = Customer(name, password);
                customers.push_back(new_customer);
                cout << "Registered Successfully\nYou need to Login to enter your account\n\n\n";
            }
            break;
        }
        case '0':
            logging_in = false;
            break;
        default:
            cout << "Invalid choice\n";
            break;
        }
    }
}

//--------------------------------------------------------------------Employee--------------------------------------------------------------------//

class Employee: public User{
    protected:
        vector<Car> rented_cars;
        int dues;
        double employee_record;

    public:

        friend class Manager;

        Employee(string name, string password, double employee_record = 1.00, int dues = 0, vector<Car> rented_cars = {}){
            this->name = name;
            this->password = password;
            this->id = ++count_employee;
            this->dues = dues;
            this->employee_record = employee_record;
            this->rented_cars = rented_cars;
        }
        void login();
        void rent_car();
        void return_car();
        void view_rented_cars();
        void view_dues();
        void clear_dues();
};

vector<Employee> employees;

void Employee::rent_car(){
    cout << "Enter the ID of the car you want to rent: ";
    string car_id;
    cin >> car_id;
    for(auto &car: cars){
        if(car.id == car_id ){
            if(car.rented){
                cout << "Car is already rented\n";
                return;
            }
            car.rented = true;
            // add rent in the dues of customer
            this->dues += int(car.rent*0.85);
            rented_cars.push_back(car);
            cout << "Car rented successfully\n";
            return;
        }
    }
    cout << "Car with given ID not found\n\n\n";
}

void Employee::return_car(){
    cout << "Enter the ID of the car you want to return: ";
    string car_id;
    cin >> car_id;
    
    for(auto it = rented_cars.begin(); it != rented_cars.end(); ++it){
        if(it->id == car_id){
            // Update availability of the car in the original cars list
            for(auto &original_car: cars){
                if(original_car.id == car_id){
                    original_car.rented = false;
                    break;
                }
            }
            auto now = time(0);
            if(now > it->duedate){
                int fine = 0.17*(it->rent*((now - it->duedate)/oneday));
                this->dues += fine;
                this->employee_record -= 2*(now - it->duedate)/oneday;
                cout << "You have been fined "<< fine <<" for returning the car late\n";
            }
            // Remove car from rented cars list
            rented_cars.erase(it);
            cout << "Car returned successfully\n\n\n";
            return;
        }
    }
    cout << "Car not found\n";
}

void Employee::view_rented_cars(){
    if(rented_cars.empty()){
        cout << "No cars rented\n\n\n";
        return;
    }
    cout << setw(3) << "Id" << setw(15) << "Name" << setw(15) << "Model" << setw(7) << "Rent" << setw(12) << "Price" << setw(6) << "Seats" << setw(10) << "Color" << setw(10) << "Due Date" << setw(10) << "Condition" << setw(15) << "Due Date" << endl;
    for(auto car: rented_cars){
        cout << setw(3) << car.id << setw(15) << car.name << setw(15) << car.model << setw(7) << car.rent << setw(12) << car.price << setw(6) << car.seats << setw(10) << car.color << setw(10) << car.duedate << setw(10) << car.condition;
        display_due_date(car.duedate);
    }
    cout << "\n\n\n";
}

void Employee::view_dues(){
    cout << "Your outstanding dues are: " << this->dues << "\n\n\n";
}

void Employee::clear_dues(){
    if(this->dues == 0) {
        cout << "You don't have any dues\n\n\n";
        return;
    }
    cout << "Your outstanding dues are: " << this->dues << endl;
    cout << "Enter the amount you want to pay: ";
    string samount;
    cin >> samount;
    int amount = stringtointeger(samount);
    // if(amount > this->dues){
    //     cout << "Amount entered is greater than the dues\n";
    //     return;
    // }
    // else if(amount <= 0){
    //     cout << "You are not allowed to enter negative or null amount\n";
    //     return;
    // }
    if(amount > 0 && amount <= this->dues){
        this->dues -= amount;
        cout << "Dues cleared successfully\n";
        cout << "Your New outstanding dues are: " << this->dues << "\n\n\n";
    }
    else{
        cout << "Invalid amount\n";
        return;
    }
    
}

void Employee::login(){
    // Display menu for employee
    bool logged_in = true;
    cout << "Welcome " << this->name << endl;
    while (logged_in)
    {    
        cout << "Choose your option\n";
        cout << "1. View all cars\n2. View your rented cars\n3. Rent a car\n4. Return a car \n5. Check Your Dues\n6. Clear Dues \n0. Logout\nEnter your choice: ";
        char choice;
        cin >> choice;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (choice)
        {
        case '1':
            view_all_cars();
            break;
        case '2':
            view_rented_cars();
            break;
        case '3':
            rent_car();
            break;
        case '4':
            return_car();
            break;
        case '5':
            view_dues();
            break;
        case '6':
            clear_dues();
            break;
        case '0':
            logged_in = false;
            break;
        default:
            cout << "Invalid choice\n";
            break;
        }
    }

}


void login_employee(){
    bool logging_in = true;
    
    while (logging_in){    
        cout << "Choose your option\n";
        cout << "1. Login\n0. Exit \nEnter your choice: ";
        char choice;
        cin >> choice;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (choice)
        {
        case '1':
        {
            cout << "Enter Your Username\n";
            string name;
            getline(cin, name);
            cout << "Enter Your Password\n";
            string password;
            cin >> password;
            bool check = false;
            for(auto employee: employees){
                if(employee.name == name && employee.check_password(password)){
                    employee.login();
                    check = true;
                    break;
                }
            }
            if(!check) cout << "Login Failed: Invalid name or password\n";
            break;
        }
        case '0':
            logging_in = false;
            break;
        default:
            cout << "Invalid choice\n";
            break;
        }
    }
}

//-------------------------------------------------------------------------Manager--------------------------------------------------------------//
// Manager is a sub class of User
class Manager: public User{
    public:
        void login();
        void add_car();
        void update_car();
        void delete_car();
        void view_all_customers();
        void add_customer();
        void update_customer();
        void delete_customer();
        void view_all_employees();
        void add_employee();
        void update_employee();
        void delete_employee();
};
Manager manager;

void Manager::add_car(){
    cout << "Enter the name of the car: ";
    string name;
    cin >> name;
    cout << "Enter the model of the car: ";
    string model;
    cin >> model;
    cout << "Enter the rent of the car: ";
    int rent;
    cin >> rent;
    cout << "Enter the price of the car: ";
    int price;
    cin >> price;
    cout << "Enter the number of seats in the car: ";
    int seats;
    cin >> seats;
    cout << "Enter the color of the car: ";
    string color;
    cin >> color;
    cars.push_back(Car(name, model, rent, price, seats, color));
    cout << "Car added successfully\n";

}

void Manager::add_customer(){
    cout << "Enter the name of the customer: ";
    string name;
    cin >> name;
    cout << "Enter the password of the customer: ";
    string password;
    cin >> password;
    customers.push_back(Customer(name, password));
    cout << "Customer added successfully\n";
}

void Manager::add_employee(){
    cout << "Enter the name of the employee: ";
    string name;
    cin >> name;
    cout << "Enter the password of the employee: ";
    string password;
    cin >> password;
    employees.push_back(Employee(name, password));
    cout << "Employee added successfully\n";
}

void Manager::view_all_customers(){
    cout << "ID\tName\tDues\tCustomer Record" << endl;
    for(auto customer: customers){
        cout << customer.id << "\t" << customer.name << "\t" << customer.dues << "\t"  << customer.customer_record << endl;
        cout << "\tRented Cars: \n";
        for(auto car: customer.rented_cars){
            cout << "\t" << car.name << " " << car.model << endl;
        }
    }
}

void Manager::view_all_employees(){
    cout << "ID\tName" << endl;
    for(auto employee: employees){
        cout << employee.id << "\t" << employee.name << endl;
        cout << "\tRented Cars: \n";
        for(auto car: employee.rented_cars){
            cout << "\t" << car.name << " " << car.model << endl;
        }
    }
}

void Manager::delete_car(){
    cout << "Enter the ID of the car you want to delete: ";
    string car_id;
    cin >> car_id;
    for(auto it = cars.begin(); it != cars.end(); ++it){
        if(it->id == car_id){
            if(it->rented){
                cout << "Car is rented, cannot delete\n";
                return;
            }
            cars.erase(it);
            cout << "Car deleted successfully\n";
            return;
        }
    }
    cout << "Car not found\n";
}

void Manager::delete_customer(){
    cout << "Enter the ID of the customer you want to delete: ";
    int customer_id;
    cin >> customer_id;
    for(auto it = customers.begin(); it != customers.end(); ++it){
        if(it->id == customer_id){
            if(it->dues > 0){
                cout << "Customer has dues, cannot delete\n";
                return;
            }
            else if(!it->rented_cars.empty()){
                cout << "Customer has rented cars, cannot delete\n";
                return;
            }
            customers.erase(it);
            cout << "Customer deleted successfully\n";
            return;
        }
    }
    cout << "Customer not found\n";
}

void Manager::delete_employee(){
    cout << "Enter the ID of the employee you want to delete: ";
    string employee_str;
    cin >> employee_str;
    int employee_id = stringtointeger(employee_str);
    for(auto it = employees.begin(); it != employees.end(); ++it){
        if(it->id == employee_id){
            if(it->dues > 0){
                cout << "Employee has dues, cannot delete\n";
                return;
            }
            else if(!it->rented_cars.empty()){
                cout << "Employee has rented cars, cannot delete\n";
                return;
            }
            employees.erase(it);
            cout << "Employee deleted successfully\n";
            return;
        }
    }
    cout << "Employee not found\n";

}

void Manager:: update_car(){
    cout << "Enter the ID of the car you want to update: ";
    string car_id;
    cin >> car_id;
    for(auto &car: cars){
        if(car.id == car_id){
            bool updating = true;
            while(updating){
                cout << "Choose your option\n";
                cout << "1. Name\n2. Model\n3. Rent\n4. Price\n5. Seats\n6. Color\n7. Is Returned \n8. Condition \n0. Finish Updating\nEnter your choice: ";
                char choice;
                cin >> choice;
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                switch (choice)
                {
                case '1':
                    cout << "Current car name: " << car.name << "\n";
                    cout << "Enter the new name: ";
                    cin >> car.name;
                    break;
                case '2':
                    cout << "Current car model: " << car.model << "\n";
                    cout << "Enter the new model: ";
                    cin >> car.model;
                    break;
                case '3':
                    cout << "Current car rent: " << car.rent << "\n";
                    cout << "Enter the new rent: ";
                    cin >> car.rent;
                    break;
                case '4':
                    cout << "Current car price: " << car.price << "\n";
                    cout << "Enter the new price: ";
                    cin >> car.price;
                    break;
                case '5':
                    cout << "Current car seats: " << car.seats << "\n";
                    cout << "Enter the new seats: ";
                    cin >> car.seats;
                    break;
                case '6':
                    cout << "Current car color: " << car.color << "\n";
                    cout << "Enter the new color: ";
                    cin >> car.color;
                    break;
                case '7':
                    cout << "Currently the car is " << (car.rented ? "rented" : "not rented") << "\n";
                    cout << "Enter the new condition: ";
                    cin >> car.rented;
                    break;
                case '8':
                    cout << "Currently the car condition is " << car.condition << "\n";
                    cout << "Enter the new condition: ";
                    cin >> car.condition;
                    break;
                case '0':
                    updating = false;
                    break;
                default:
                    cout << "Invalid choice\n";
                    break;
                }
            }
        }
    }
    cout << "Car with given ID not found\n";
}

void Manager::update_customer(){
    cout << "Enter the ID of the customer you want to update: ";
    int customer_id;
    cin >> customer_id;
    for(auto &customer: customers){
        if(customer.id == customer_id){
            bool updating = true;
            while(updating){
                cout << "Choose your option\n";
                cout << "1. Name\n2. Password\n3. Dues\n4. Customer Record\n0. Finish Updating\nEnter your choice: ";
                char choice;
                cin >> choice;
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                switch (choice)
                {
                case '1':
                    cout << "Current Username: " << customer.name << "\n";
                    cout << "Enter the new username: ";
                    cin >> customer.name;
                    break;
                case '2':
                    cout << "Enter the new password: ";
                    cin >> customer.password;
                    break;
                case '3':
                    cout << "Current dues are: " << customer.dues << "\n";
                    cout << "Enter the new dues: ";
                    cin >> customer.dues;
                    break;
                case '4':
                    cout << "Current customer record: " << customer.customer_record << "\n";
                    cout << "Enter the new customer record: ";
                    cin >> customer.customer_record;
                    break;
                case '0':
                    updating = false;
                    break;
                default:
                    cout << "Invalid choice\n";
                    break;
                }
            }
        }
    }
    cout << "Customer with given ID not found\n";
}

void Manager::update_employee(){
    cout << "Enter the ID of the employee you want to update: ";
    string employee_str;
    cin >> employee_str;
    int employee_id = stringtointeger(employee_str);
    for(auto &employee: employees){
        if(employee.id == employee_id){
            bool updating = true;
            while(updating){
                cout << "Choose your option\n";
                cout << "1. Name\n2. Password\n3. Dues\n4. Employee Record\n0. Finish Updating\nEnter your choice: ";
                char choice;
                cin >> choice;
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                switch (choice)
                {
                case '1':
                    cout << "Current Username: " << employee.name << "\n";
                    cout << "Enter the new name: ";
                    cin >> employee.name;
                    cout << "Successfully updated name to" << employee.name << endl;
                    break;
                case '2':
                    
                    cout << "Enter the new password: ";
                    cin >> employee.password;
                    cout << "Successfully updated password\n";
                    break;
                case '3':
                    cout << "Current dues are: " << employee.dues << "\n";
                    cout << "Enter the new dues: ";
                    cin >> employee.dues;
                    cout << "Successfully updated dues to " << employee.dues << endl;
                    break;
                case '4':
                    cout << "Current employee record: " << employee.employee_record << "\n";
                    cout << "Enter the new employee record: ";
                    cin >> employee.employee_record;
                    cout << "Successfully updated employee record to " << employee.employee_record << endl;
                    break;
                case '0':
                    updating = false;
                    break;
                default:
                    cout << "Invalid choice\n";
                    break;
                }
            }
        }
    }
    cout << "Employee with given ID not found\n";
}

// login function for manager
void Manager:: login(){
    bool logged_in = true;
    cout << "Welcome " << this->name << endl;
    while (logged_in)
    {    
        cout << "Choose your option\n";
        cout << "1. View all cars\n2. Add Car\n3. Update Car\n4. Delete Car\n5. View all customers\n6. Add Customer\n7. Update Customer\n8. Delete Customer\n9. View all employees\na. Add Employee\nb. Update Employee\nc. Delete Employee\n0. Logout\nEnter your choice: ";
        char choice;
        cin >> choice;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (choice)
        {
        case '1':
            view_all_cars();
            break;
        case '2':
            add_car();
            break;
        case '3':
            update_car();
            break;
        case '4':
            delete_car();
            break;
        case '5':
            view_all_customers();
            break;
        case '6':
            add_customer();
            break;
        case '7':
            update_customer();
            break;
        case '8':
            delete_customer();
            break;
        case '9':
            view_all_employees();
            break;
        case 'a':
            add_employee();
            break;
        case 'b':
            update_employee();
            break;
        case 'c':
            delete_employee();
            break;
        case '0':
            logged_in = false;
            break;
        default:
            cout << "Invalid choice\n";
            break;
        }
    }

}

void login_manager(){
    bool logging_in = true;
    
    while (logging_in){    
        cout << "Choose your option\n";
        cout << "1. Login\n0. Exit \nEnter your choice: ";
        char choice;
        cin >> choice;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (choice)
        {
        case '1':
        {
            cout << "Enter username\n";
            string name;
            getline(cin, name);
            cout << "Enter Your Password\n";
            string password;
            cin >> password;
            // if(manager.name == name && manager.check_password(password)){
            //     cout << "Logged in Successfully\n";
            //     manager.login();
            //     break;
            // }
            if(name == "admin" && password == "admin"){
                cout << "Logged in Successfully\n";
                manager.login();
                break;
            }
            cout << "Login Failed: Invalid name or password\n";
            break;
        }
        case '0':
            logging_in = false;
            break;
        default:
            cout << "Invalid choice\n";
            break;
        }
    }
}



//----------------------------------------------------------------Main Function-------------------------------------------------------------//
int main() {
    bool run_program = true;
    // initialize 10 cars
    cars.push_back(Car("Toyota", "Corolla", 1000, 2000000, 5, "White", 100, time(0) + oneday));
    cars.push_back(Car("Honda", "Civic", 1500, 2500000, 5, "Black",100, time(0) + oneday));
    cars.push_back(Car("Suzuki", "Cultus", 800, 1000000, 5, "Grey"));
    cars.push_back(Car("Suzuki", "Mehran", 500, 500000, 5, "White"));
    cars.push_back(Car("Toyota", "Vitz", 1200, 1500000, 5, "Red"));
    cars.push_back(Car("Rolls", "Royce", 50000, 20000000, 4, "White", 90));
    cars.push_back(Car("Ferrari", "Laferrari", 15000, 30000000, 2, "Red",100, time(0) + 600));
    cars.push_back(Car("Lamborghini", "Aventador", 20000, 40000000, 2, "Black",100, time(0) + 1200));
    cars.push_back(Car("Bugatti", "Veyron", 25000, 50000000, 2, "Blue"));
    cars.push_back(Car("Audi", "A8", 5000, 10000000, 5, "Black"));

    // initialize 5 customers
    customers.push_back(Customer("ali", "asd", 1.00, 5000, {cars[0], cars[1]}));
    customers.push_back(Customer("ahmed", "123"));
    customers.push_back(Customer("asad", "123"));
    customers.push_back(Customer("ahsan", "123"));
    customers.push_back(Customer("adeel", "123"));
    cars[0].rented = true;
    cars[1].rented = true;
    // initialize 5 employees
    employees.push_back(Employee("dev", "123",1.00,0));
    employees.push_back(Employee("daksh", "123"));
    employees.push_back(Employee("sway", "123"));
    employees.push_back(Employee("himan", "123",0.99,5000,{cars[6], cars[7]}));
    employees.push_back(Employee("kum", "123"));
    cars[6].rented = true;
    cars[7].rented = true;


    
    while (run_program) {
        
        cout << "#################################     Welcome to E-Car Rental System      #############################################\n";
        cout << "1. Customer\n2. Employee\n3. Manager\n0. Exit\nEnter your choice: ";
        char choice;
        cin >> choice;
        
        switch (choice) {
            case '0':
                run_program = false;
                break;
            case '1':
                login_customer();
                break;
            case '2':
                login_employee();
                break;
            case '3':
                login_manager();
                break;
            default:
                cout << "Invalid choice\n";
        }
        // _sleep(1000);
        system("cls"); // Change it to command which clears the terminal in your OS cls(Windows/Mac) or clear(Linux)
    }
    cin.get();
    return 0;
}