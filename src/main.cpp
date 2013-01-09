
#include <stdexcept>
#include <iostream>
#include "State.h"
#include "Context.h"

#define ACTION(instance,function) \
  [&](const Context& context) { instance.function(context); }

#define REQUIREMENT(instance,function) \
  [&](const Context& context) -> bool { return instance.function(context); }

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
    stateA.setUpdate(ACTION(foo, entry));
    stateB.setUpdate(ACTION(foo, update));
    stateB.setExit(ACTION(foo, exit));

    success.setUpdate(&success_fn);
    failure.setUpdate(&failure_fn);

    // Create transitions between states
    stateA.addTransition(isTrue<Context>, stateB);
    stateB.addTransition(REQUIREMENT(foo, isItTimeToExit), stateC);
    stateB.addTransition(REQUIREMENT(foo, isThereAProblem), success);
    stateC.addTransition(isTrue<Context>, success);
    stateD.addTransition(REQUIREMENT(foo, isThereAProblem), failure);

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
