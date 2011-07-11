#ifndef PMUTEX_BOOST_H_
#define PMUTEX_BOOST_H_

#include <boost/thread/mutex.hpp>


class PMutex
{
public:
	PMutex()
	{}

	void lock() { m_mutex.lock(); }
	void unlock() { m_mutex.unlock(); }

private:
	friend class PCondition;
	friend class PMutexLocker;
    boost::mutex m_mutex;
};


class PMutexLocker
{
public:
	PMutexLocker(PMutex &mutex) :
		m_locker(mutex.m_mutex)
	{}

	void lock() { m_locker.lock(); }
	void unlock() { m_locker.unlock(); }

private:
	boost::unique_lock<boost::mutex> m_locker;
};

#endif /* PMUTEX_BOOST_H_ */