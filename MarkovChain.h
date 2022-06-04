#pragma once

#include <vector>
#include <unordered_map>
#include <optional>
#include <algorithm>
#include <fstream>

template<typename T>
class MarkovChain 
{
    struct Transition
    {
        T to;
        int count;
        float probabilityNormalized;
    };

    std::optional<T> previousState = std::nullopt;

    std::unordered_map<T, std::vector<Transition>> states;

    // with a vector : 
    // - addElement(T element) will be O(n) since we need to check if the element is already in the vector
    // - generate(int length) will be O(1) at each step we acces the next node using it's id in contiguous memory
    // with an unordered_map :
    // - addElement(T element) will be O(1) since we can find the element in the set in O(1)
    // - generate(int length) will be O(1) at each step BUT the memory is not contiguous anymore. But if there is a lot of elements, it will be worth it.

    void _normalizeProbabilities();

public:
    MarkovChain();
    
    void addElement(const T& element);

    std::vector<T> generate(unsigned int maxLength);

    void printToDot(std::ostream& out);
};

template<typename T>
MarkovChain<T>::MarkovChain()
{

}

template<typename T>
void MarkovChain<T>::_normalizeProbabilities()
{
    for (auto& [state, transitions] : states)
    {
        float sum = 0.0f;
        for (auto& transition : transitions)
            sum += transition.count;

        for (auto& transition : transitions)
            transition.probabilityNormalized = transition.count / sum;
    }
}

template<typename T>
void MarkovChain<T>::addElement(const T& element)
{
    // Finding if the state already exists
    std::unordered_map<T, std::vector<Transition>>::iterator currentIt = states.find(element);
    if (currentIt == states.end())
    {
        // If it doesn't exist, we create a new node with the element as data
        states.insert(std::make_pair(element, std::vector<Transition>()));
    }

    // We update the probabilities of the previous node if it exists
    if (!previousState)
    {
        previousState = element;
        return;
    }
    
    // Finding the previous node
    std::unordered_map<T, std::vector<Transition>>::iterator previousIt = states.find(previousState.value());
    if (previousIt == states.end())
        return;
    
    std::vector<Transition>::iterator transitionIt = std::find_if(previousIt->second.begin(), previousIt->second.end(), [element](Transition transition) { return transition.to == element; });
    if (transitionIt == previousIt->second.end())
    {
        // If the transition doesn't exist, we create it
        Transition newTransition;
        newTransition.to = element;
        newTransition.count = 1;
        newTransition.probabilityNormalized = 1.0f;
        previousIt->second.push_back(newTransition);
        //std::cerr << "Created transition from " << previousState.value() << " to " << element << std::endl;
    }
    else
    {
        // If the transition already exists, we update it 
        // (to updated probabillities of the node, we just add 1 to the transition probability. Normalizing will be done later)
        transitionIt->count++;
        //std::cerr << "    Updated transition from " << previousState.value() << " to " << element << std::endl;
    }

    previousState = element;
}

template<typename T>
std::vector<T> MarkovChain<T>::generate(unsigned int maxLength)
{
    _normalizeProbabilities();
    std::vector<T> generated;
    generated.reserve(maxLength);

    // start with a random state
    int index = rand() % states.size();
    auto it = states.begin();
    std::advance(it, index);
    T currentState = it->first;

    // generate the chain
    std::cerr << "Generating chain with " << maxLength << " steps" << std::endl;
    int itemCount = maxLength;
    while (itemCount >= 0)
    {
        // find the next state
        auto& transitions = states.at(currentState);

        if (transitions.size() == 0)
            return generated;

        float random = (float)rand() / (float)RAND_MAX; // between 0 and 1
        float sum = 0.0f;
        for (auto& transition : transitions)
        {
            sum += transition.probabilityNormalized;
            if (random < sum)
            {
                currentState = transition.to;
                generated.push_back(currentState);
                //std::cerr << currentState << " " << (itemCount) << " ";
                break;
            }
        }
        itemCount--;
    }

    return generated;
}

template<typename T>
void MarkovChain<T>::printToDot(std::ostream& out)
{
    _normalizeProbabilities();

    out << "digraph G {" << std::endl;
    for (auto& state : states)
    {
        out << "\"" << state.first << "\"" << std::endl;
        for (auto& transition : state.second)
        {
            out << "\"" << state.first << "\" -> \"" << transition.to << "\" [label=\"" << transition.probabilityNormalized << " (" << transition.count << ")\"]" << std::endl;
        }
    }
    out << "}" << std::endl;
}