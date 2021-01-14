#ifndef _SUBJECT_H_
#define _SUBJECT_H_

#include "Observer.h"
#include "Commons_SMB3.h"

#include <vector>

// A subject class is a class that other objects listen to (observers) and whenever something imporant happens in this subject class implementation it sends out a notification
class Subject abstract
{
public:
	// To add a new observer
	void AddObserver(Observer& newObserver) { mObservers.push_back(newObserver); }

	// Function to notify all observers
	void Notify(SUBJECT_NOTIFICATION_TYPES notification, std::string data)
	{
		for (unsigned int i = 0; i < mObservers.size(); i++)
		{
			mObservers[i].OnNotify(notification, data);
		}
	}

	virtual ~Subject() { mObservers.clear(); }

protected:
	Subject() { ; }

	std::vector<Observer> mObservers;
};

#endif