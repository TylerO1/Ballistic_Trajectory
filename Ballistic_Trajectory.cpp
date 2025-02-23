#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>

#define _USE_MATH_DEFINES
#include <cmath>


using namespace std;

struct  Position{
        double x;
        double y;
    };

struct CombinedPositions{
        Position basicPosition;
        Position baseballDragPosition;
        Position wiffleballDragPosition;
    };

struct Velocity{
    double Vx;
    double Vy;
};

struct Acceleration{
    double Ax;
    double Ay;
};

class BasePhysics
{
    //Structures
    //Members
    private:
        double A_gravity;
        Position position;
        Velocity Basic_Velocity;

    //Constructors and Destructors
    public:
        BasePhysics(double gravity, Position starting_position)
        {
            A_gravity = gravity;
            setPosition(starting_position);
        }
    //Gets and Sets
    public:
        void setPosition(Position newPoint)
        {
            position = newPoint;
        }
        void setVeloicity(double velocity, double angle)
        {
            updateVelocity_trig(velocity, angle);
        }
        Position getPosition(void)
        {
            return position;
        }
    //Methods
    public:
        Position newTimeStep(double dt)
        {
            StepWithTime(dt);
            return position;
        }
    private:
        void updateVelocity_trig (double velocity, double angle)
        {
            // Angle is assumed to be degrees, convert to radians
            double angle_rad = angle * M_PI/180;
            Basic_Velocity.Vx = velocity * cos(angle_rad);
            Basic_Velocity.Vy = velocity * sin(angle_rad);
        }
        void StepWithTime(double dt)
        {
            position.x = position.x + Basic_Velocity.Vx * dt;
            
            Basic_Velocity.Vy = Basic_Velocity.Vy - A_gravity * dt;
            position.y = position.y + Basic_Velocity.Vy * dt;
        }

};

class DragPhysics
{
    //Structures
    //Members
    private:
        double A_gravity;
        Position position;
        Velocity Drag_Velocity;
        Acceleration Drag_Acceleration;
        double mass;
        double Cd;
        double density;
        double area;

    //Constructors and Destructors
    public:
        DragPhysics(double gravity, Position starting_position, double diameter, double initialMass, double CoefficientOfDrag, double airDensity)
        {
            A_gravity = gravity;
            setPosition(starting_position);
            Drag_Acceleration.Ax = 0.0;
            Drag_Acceleration.Ay = 0.0;
            mass = initialMass;
            Cd = CoefficientOfDrag;
            density = airDensity;

            area = M_PI * pow((diameter/2), 2);
        }
    //Gets and Sets
    public:
        void setPosition(Position newPoint)
        {
            position = newPoint;
        }
        void setVeloicity(double velocity, double angle)
        {
            updateVelocity_trig(velocity, angle);
        }
        Position getPosition(void)
        {
            return position;
        }
    //Methods
    public:
        Position newTimeStep(double dt)
        {
            StepWithTime(dt);
            return position;
        }
    private:
        void updateVelocity_trig (double velocity, double angle)
        {
            // Angle is assumed to be degrees, convert to radians
            double angle_rad = angle * M_PI/180;
            Drag_Velocity.Vx = velocity * cos(angle_rad);
            Drag_Velocity.Vy = velocity * sin(angle_rad);
        }
        void StepWithTime(double dt)
        {
            Drag_Acceleration.Ax = (             (-Cd * density * area * pow(Drag_Velocity.Vx, 2))/(2 * mass));// * dt;
            Drag_Acceleration.Ay = (-A_gravity + (-Cd * density * area * pow(Drag_Velocity.Vy, 2))/(2 * mass));// * dt;

            Drag_Velocity.Vx = Drag_Velocity.Vx + Drag_Acceleration.Ax * dt;
            Drag_Velocity.Vy = Drag_Velocity.Vy + Drag_Acceleration.Ay * dt;

            position.x = position.x + Drag_Velocity.Vx * dt;
            position.y = position.y + Drag_Velocity.Vy * dt;

            cout << "Accel x: "<<Drag_Acceleration.Ax<<" Accel Y: "<<Drag_Acceleration.Ay<<" Velocity x: "<<Drag_Velocity.Vx<<" Velocity Y: "<<Drag_Velocity.Vy<<" Position x: "<<position.x<<" Position y: "<<position.y<<"\n";
        }

};


int main()
{
    vector<string> msg {"Hello", "C++", "World", "from", "VS Code", "and the C++ extension!"};

    for (const string& word : msg)
    {
        cout << word << " ";
    }
    cout << endl;

    /* 
    Ambient Conditions:
    Assuming Cedar Falls, Iowa "standard" pressure ~1027 mbar, measured 9 Feb 2025
    Temperature: 75 F (it was actually 24 that day, but the pressure measurement is fine)
    Air Density: 1.18 kg/m^3
    Velocity: 17.88 m/s (40 mph, about what a non-athelete's fastball would be)
    Diameter: 0.073 meters (one baseball, but no threading to mess up the flow)
    Dynamic Viscosity: 0.00001831 Newton Seconds/m^2 (derived from air temperature at sea level)
    Reynolds Number: 8.4 *10^4 (calculated from above)
    Coefficient of Drag: 0.4. (thanks to this table:)
        https://www.me.psu.edu/cimbala/me325web_Spring_2012/Labs/Drag/intro.pdf    
    
    Units to use: meters, kelvin, newtons, kilograms
    Note: the code doesn't care, but the math gets wonky if you aren't consistent. You can also use feet, rankine, pounds-force, slugs 
    */
    Position startingPosition;
    startingPosition.x = 0.0;
    startingPosition.y = 1.83; // 6 feet
    BasePhysics basic( 9.8, startingPosition);
    //Baseball
    DragPhysics Baseball_drag(9.8, startingPosition, 0.073, 0.142, 0.4, 1.18);
    //Wiffleball
    DragPhysics Wiffleball_drag(9.8, startingPosition, 0.073, 0.022, 0.4, 1.18);

    basic.setVeloicity(17.88, 0);
    Baseball_drag.setVeloicity(17.88, 0);
    Wiffleball_drag.setVeloicity(17.88, 0);


    list<CombinedPositions>CombinedPosition;
    CombinedPositions temp;
    temp.basicPosition = startingPosition;
    temp.baseballDragPosition = startingPosition;
    temp.wiffleballDragPosition = startingPosition;
    CombinedPosition.push_front(temp);

    double deltaTime = 0.1;

    while (CombinedPosition.back().basicPosition.y > -10.0 )
    {
        temp.basicPosition = basic.newTimeStep(deltaTime);
        temp.baseballDragPosition = Baseball_drag.newTimeStep(deltaTime);
        temp.wiffleballDragPosition = Wiffleball_drag.newTimeStep(deltaTime);
        CombinedPosition.push_back(temp);
    }
    ofstream myfile;
    myfile.open("Positions.csv");
    myfile << "Basic Physics x, Basic Physics y, Baseball Drag Physics x, Baseball Drag Physics y, Wiffleball Drag Physics x, Wiffleball Drag Physics y \n";
    
    list<CombinedPositions>::iterator i;
    for (i = CombinedPosition.begin(); i!=CombinedPosition.end(); i++)
    {
        myfile << i->basicPosition.x <<","<< i->basicPosition.y <<","<< i->baseballDragPosition.x <<","<< i->baseballDragPosition.y <<","<< i->wiffleballDragPosition.x <<","<< i->wiffleballDragPosition.y << "\n";
    }

    myfile.close();

}