#ifndef _OBSERVER_H_
#define _OBSERVER_H_

#include "Commons_SMB3.h"

#include <string>

class Observer
{
public:
	virtual void OnNotify(SUBJECT_NOTIFICATION_TYPES notification, std::string data) { ; }
	virtual ~Observer() { ; }

protected:
	Observer() { ; }

};

#endif