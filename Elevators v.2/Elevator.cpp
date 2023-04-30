#include "Building.h"
#include "Elevator.h"
#include "Panel.h"
#include "Rider.h"
#include <iostream>
#include <vector>
using namespace std;

#include <cstdlib>

Elevator::Elevator(unsigned int capacity, int speed, int start)
:speed(speed), CAPACITY(capacity), location(Building::floors[start])
{
  start = Building::floors[start];
  direction = IDLE;
  timer = 0;
  atFloorIndex = -1;

  for(int i = 0; i < Building::FLOORS; i++)
  {
    panel.addButton(Building::floors[i].label);
  }
}


ostream& operator<<(ostream& out, const Elevator& e)
{
  cout << "Elevator at ";
  cout.width(5);
  cout << e.location;

  cout.width(11);
  switch(e.direction)
  {
    case 1:
    cout << "going UP";
    break;
    case -1:
    cout << "going DOWN";
    break;
    case 0:
    cout << "IDLE";
    break;
  }

  cout.width(3);
  cout << e.riders.size() << " riders";

  cout.width(16);
  if(e.atFloorIndex != -1)
    cout << " door is OPEN|" << e.timer;
  else
  {
    cout.width(15);
    cout << " ";
  }

  cout << " " << e.panel;

  return out;
}

void Elevator::openDoorTo(int floorindex)
{
  this->panel.clear(Building::floors[floorindex].label);
  this->atFloorIndex = floorindex;
  this->resetTimer();
}

void Elevator::board(const Rider& r)
{
  this->riders.push_back(r); // add rider

  //string to = Building::floors[r.to].label;
  this->panel.press(Building::floors[r.to].label); // press button

  if(r.goingUp) // set direction of elevator
    this->direction = this->UP;
  else if(r.goingDown)
    this->direction = this->DOWN;
  else throw "BOARD ERROR!";
    this->resetTimer(); // reset close door timer
}

bool Elevator::hasRiderForFloor() const
{
  if(atFloorIndex == -1)
    return false;

  for(int i = 0; i < riders.size(); i++)
    if(riders[i].to == atFloorIndex)
      return true; // find rider for floor

  return false; // end of function
}

void Elevator::removeRider()
{
  vector<Rider>:: iterator it;
  for(it = riders.begin(); it != riders.end(); it++)
  {
    if(it->to == atFloorIndex)
    {
      this->panel.clear(Building::floors[it->to].label);
      it = this->riders.erase(it);
      this->resetTimer();
      break;
    }
  }
}

bool Elevator::closeDoor()
{
  if(atFloorIndex == -1)
  return false;

  else
  {
    if(this->goingUp() && !Building::floors[atFloorIndex].hasUpRider())
    Building::floors[atFloorIndex].panel.clear(Building::floors[atFloorIndex].UP);
    else if(this->goingDown() && !Building::floors[atFloorIndex].hasDownRider())
    Building::floors[atFloorIndex].panel.clear(Building::floors[atFloorIndex].DOWN);
  }
  this->atFloorIndex = -1;
  return true;
}

bool Elevator::move()
{
  if(this->direction == this->UP)
  {
    for(int i = 0; i < Building::FLOORS; i++)
    {
      if(Building::floors[i].elevation < this->location || Building::floors[i].elevation > this->location + speed) continue;
      else if((Building::floors[i].panel.isLit(Floor::UP) || Building::floors[i].panel.isLit(Building::floors[i].label)) && this->location != Building::floors[i].elevation)
      {
        this->location = Building::floors[i].elevation;
        this->openDoorTo(i);
        return true;
      }
    }

    if(Building::floors[Building::FLOORS - 1].elevation <= this->location + speed)
    {
      this->location = Building::floors[Building::FLOORS - 1].elevation;
      this->atFloorIndex = Building::FLOORS - 1;
      return false;
    }
    else
    {
      this->location += speed;
      return true;
    }
  }
  else if(this->direction == this->DOWN)
  {
    for(int i = 0; i < Building::FLOORS; i++)
    {
      if(Building::floors[i].elevation > this->location || Building::floors[i].elevation < this->location - speed) continue;
      else if((Building::floors[i].panel.isLit(Floor::DOWN) || Building::floors[i].panel.isLit((Building::floors[i].label))) && this->location != Building::floors[i].elevation)
      {
        this->location = Building::floors[i].elevation;
        this->openDoorTo(i);
        return true;
      }
    }

    if(Building::floors[0].elevation >= this->location - speed)
    {
      this->location = Building::floors[0].elevation;
      this->atFloorIndex = 0;
      return false;
    }
    else
    {
      this->location -= speed;
      return true;
    }
  }
  return false;
}