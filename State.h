#ifndef STATE_H
#define STATE_H

#include <functional>
#include <memory>
#include <string>
#include <map>
#include <vector>

/**
 * Convenience function that can be used a no-op Action
 */
template <typename ContextType>
void null(const ContextType&) 
{
}

/**
 * Convenience function that can be used as an affirmative Requirement
 */
template <typename ContextType>
bool isFalse(const ContextType&)
{
  return false;
}

/**
 * Convenience function that can be used as an negative Requirement
 */
template <typename ContextType>
bool isTrue(const ContextType&)
{
  return true;
}

template <typename ContextType>
class Transition;

/**
 * A flexible state representation
 */
template <typename ContextType>
class State
{
  public:
    typedef std::function<void(const ContextType&)> Action;
    typedef Transition<ContextType> TransitionType;

    /**
     * This is the data contained by the state. It is memory that can be shared
     * by multiple instances of State.
     */
    struct Impl
    {
      State<ContextType> parent;
      Action onUpdate;
      Action onExit;
      std::string id;
      std::vector<TransitionType> transitions;
    };

  public:
    State()
    {
    }

    explicit State(const std::string& id) : impl(new Impl)
    {
      impl->onUpdate = null<ContextType>;
      impl->onExit = null<ContextType>;
      impl->id = id;
    }

    State(const State& state) : impl(state.impl)
    {
    }

    State& operator= (const State& state)
    {
      impl = state.impl;
      return *this;
    }

    bool operator== (const State& state) const
    {
      return (impl == state.impl);
    }

    bool operator!= (const State& state) const
    {
      return (impl != state.impl);
    }

    bool isNull() const
    {
      return impl.get() == 0;
    }

    void setUpdate(const Action& action)
    {
      impl->onUpdate = action;
    }

    void setExit(const Action& action)
    {
      impl->onExit = action;
    }

    void setParent(const State<ContextType>& parent)
    {
      impl->parent = parent;
    }

    void addTransition(const std::function<bool(const ContextType&)>& condition, const State& state)
    {
      TransitionType transition;
      transition.isSatisfiedBy = condition;
      transition.destination = state;
      impl->transitions.push_back(transition);
    }

    void addTransition(const TransitionType& transition)
    {
      impl->transitions.push_back(transition);
    }

    void update(const ContextType& context)
    {
      impl->onUpdate(context);
    }

    State nextState(const ContextType& context)
    {
      for (auto it = impl->transitions.begin(); it != impl->transitions.end(); ++it)
      {
        if (it->isSatisfiedBy(context))
        {
          impl->onExit(context);
          return it->destination;
        }
      }

      // None of the transition conditions on this state were satisified so try 
      // the parent node
      if (!impl->parent.isNull())
      {
        State next = impl->parent.nextState(context);
        if (next != impl->parent)
        {
          return next;
        }
      }

      return *this;
    }

    std::shared_ptr<Impl> impl;
};

/**
 * The Transition class represents a transition to a specified state given a
 * specified Requirement.
 */
template <typename ContextType>
class Transition
{
  public:
    typedef std::function<bool(const ContextType&)> Requirement;

  public:
    Transition()
    : isSatisfiedBy(isTrue<ContextType>)
    , destination(State<ContextType>())
    {
    }

    Requirement isSatisfiedBy;
    State<ContextType> destination;
};

#endif // STATE_H

