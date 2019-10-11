#include <iostream>
#include <thread>
#include <vector>
#include <omp.h>
#include <stdio.h>
#include <mutex>
#define STEPS 100000
double f(double x);
double integral(double a, double b)
{
	double dx = (b - a) / STEPS;
	double acc = 0;
	for (unsigned i = 0; i<STEPS; ++i)
		acc += f(a + dx*(i + 0.5));
	return acc*dx;
}

double integral_cpp(double a, double b)
{
	auto T = std::thread::hardware_concurrency();
	double dx = (b - a) / STEPS;
	std::vector<std::thread> threads;
	double res = 0;
	std::mutex mtx;
	for (unsigned t = 0; t<T; ++t)
		threads.emplace_back(std::thread([T, t, dx, &res,&mtx, a]()
	{
		double r = 0;
		for (auto i = t; i<STEPS; i += T)
			r += f((i + 0.5)*dx + a);
		mtx.lock();
		res += r;
		mtx.unlock();
		return res;
	}));

	for (auto&thr : threads)
		thr.join();
	return res*dx;
}


/*double integral_OMP(double a, double b)
{
int t=omp_get_num_procs(); // всего потоков
double dx=(b-a)/STEPS;
double res=0;
#pragma omp parallel
{
int t=omp_get_thread_num();//индекс потока
double r=0;//частичная сумма
for (int i=t;i<STEPS; i+=T)
r+=f(dx*(i+0.5)+a);
#pragma omp critical
{
res+=r;
}
}
return res*dx;
}*/

int main()
{
	std::cout << "Result is " << integral(0, 1) << "\n";
	return 0;
	std::cin.get();
}
double f(double x)
{
	return 4 / (1 + x*x);
}

