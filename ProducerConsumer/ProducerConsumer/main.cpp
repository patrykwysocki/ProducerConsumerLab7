#include <thread>
#include <iostream>
#include <queue>
#include <mutex>
#include <chrono>

//mutual exclusion
std::mutex m_mutex;
//conditional variable used to block a thread
std::condition_variable m_condVar;
std::queue<int> m_queue;

bool finished = false;

void producer(int n) 
{
	//produce for N amount of times.
	for (int i = 0; i < n; ++i) 
	{
		//this will lock the thread
		std::lock_guard<std::mutex> m_lock(m_mutex);
		m_queue.push(i);
		std::cout << "pushing " << i << std::endl;

		//Unblocks all threads currently waiting
		m_condVar.notify_all();
	}

	std::lock_guard<std::mutex> m_lock(m_mutex);
	finished = true;
	m_condVar.notify_all();
}

void consumer() 
{
	while (true) 
	{
		std::unique_lock<std::mutex> m_lock(m_mutex);
		//reacquires the mutex lock and check if our condition is met or nor.
		//if not met, mutex is released and blocks current thread
		m_condVar.wait(m_lock, [] { return finished || !m_queue.empty(); });

		//while queue not empty consume whatever is in the queue.
		while (!m_queue.empty()) 
		{
			std::cout << "consuming " << m_queue.front() << std::endl;
			m_queue.pop();
		}
		//when finished break out of consuming
		if (finished)
		{
			break;
		}
	}
}

int main() 
{
	std::thread t1(producer, 100);
	std::thread t2(consumer);
	t1.join(); //wait for thread 1 to finish
	t2.join(); //wait for thread 2 to finish
	std::cout << "Producing and Consuming Finished !" << std::endl;
	system("pause");
}