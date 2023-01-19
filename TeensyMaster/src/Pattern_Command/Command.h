#ifndef COMMAND_H
#define COMMAND_H

#pragma once

class Command
{
public:
  virtual  ~Command() {}
  virtual void execute() = 0;
};


#endif