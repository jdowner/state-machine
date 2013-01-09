
#include <stdexcept>
#include <iostream>
#include <boost/bind.hpp>
#include "State.h"
#include "Context.h"

#define CONNECT(function,instance) \
  boost::bind(&function,(instance),_1)

/**
 * A simple class to demonstrate state updates and transitions
 */
class Foo
{
  public:
    Foo() : m_isItTimeToExit(false)
    {
    }

    void update(const Context&)
    {
      std::cout << "FooState::update" << std::endl;
    }

    void entry(const Context&)
    {
      std::cout << "FooState::entry" << std::endl;
    }

    bool isItTimeToExit(const Context&)
    {
      bool current = m_isItTimeToExit;
      m_isItTimeToExit = true;
      return current;
    }

    bool isThereAProblem(const Context&)
    {
      return true;
    }

    void exit(const Context&)
    {
      std::cout << "FooState::exit" << std::endl;
    }

    bool isContact(const Context&)
    {
      return true;
    }

    bool m_isItTimeToExit;
};

void success_fn(const Context&)
{
  std::cout << "success" << std::endl;
}

void failure_fn(const Context&)
{
  std::cout << "failure" << std::endl;
}

int main(int argc, char** argv)
{
  try
  {
    Foo foo;
    Context context;

    typedef State<Context> State;
    typedef Transition<Context> Transition;

    // Create a set of states
    State stateA("a");
    State stateB("b");
    State stateC("c");
    State stateD("d");
    State success("success");
    State failure("failure");
    State terminal("terminal");

    // Connect the states to corresponding functions
    stateA.impl->onUpdate = CONNECT(Foo::entry, foo);
    stateB.impl->onUpdate = CONNECT(Foo::update, foo);
    stateB.impl->onExit = CONNECT(Foo::exit, foo);

    success.impl->onUpdate = &success_fn;
    failure.impl->onUpdate = &failure_fn;

    // Create transitions between states
    stateA.addTransition(isTrue<Context>, stateB);
    stateB.addTransition(CONNECT(Foo::isItTimeToExit, foo), stateC);
    stateB.addTransition(CONNECT(Foo::isThereAProblem, foo), success);
    stateC.addTransition(isTrue<Context>, success);
    stateD.addTransition(CONNECT(Foo::isThereAProblem, foo), failure);

    success.addTransition(isTrue<Context>, terminal);
    failure.addTransition(isTrue<Context>, terminal);

    stateA.setParent(stateD);
    stateB.setParent(stateD);
    stateC.setParent(stateD);

    // Run to state machine to completion (where stateC represents a terminal
    // state).
    for (State current = stateA; current != terminal; current = current.nextState(context))
    {
      current.update(context);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
  catch (...)
  {
    std::cerr << "Encountered unrecognized exception" << std::endl;
  }

  return 0;
}
