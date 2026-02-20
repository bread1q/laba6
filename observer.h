#ifndef OBSERVER_H
#define OBSERVER_H

#include <string>
#include <vector>
#include <algorithm>

class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const std::string& eventType, void* data = nullptr) = 0;
};

class Observable {
private:
    std::vector<Observer*> observers_;

public:
    virtual ~Observable() = default;

    void addObserver(Observer* observer) {
        if (std::find(observers_.begin(), observers_.end(), observer) == observers_.end()) {
            observers_.push_back(observer);
        }
    }

    void removeObserver(Observer* observer) {
        auto it = std::find(observers_.begin(), observers_.end(), observer);
        if (it != observers_.end()) {
            observers_.erase(it);
        }
    }

    void notifyObservers(const std::string& eventType, void* data = nullptr) {
        for (auto observer : observers_) {
            observer->update(eventType, data);
        }
    }
};

#endif // OBSERVER_H
