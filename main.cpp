#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <cmath>
#include <random>
#include <iterator>
#include <algorithm>
#include <set>

using namespace std;

// Task 1
mutex task1;

class pcout: public std::ostream {
public:
    pcout(std::streambuf* sbuf)
        : std::ios(sbuf), std::ostream(sbuf) {
        std::lock_guard<std::mutex> task1_lg(task1);
    }
} pcout(std::cout.rdbuf());

void doSomething1() {
//    this_thread::sleep_for(chrono::seconds(2));
    //std::lock_guard<std::mutex> task1_lg(task1);
    pcout<<"Flow 1 ";
    for (int i=0;i<1000;i++)
     pcout<<"=";
//    pcout<<endl;
}

void doSomething2() {
//    this_thread::sleep_for(chrono::seconds(3));
    //std::lock_guard<std::mutex> task1_lg(task1);
    pcout<<"Flow 2 ";
    for (int i=0;i<1000;i++)
     pcout<<"+";
//    pcout<<endl;
}

void doSomething3() {
//    this_thread::sleep_for(chrono::seconds(4));
    //std::lock_guard<std::mutex> task1_lg(task1);
    pcout<<"Flow 3 ";
    for (int i=0;i<1000;i++)
     pcout<<"-";
//    pcout<<endl;
}

// Task 2


long doPrime(unsigned long& sim, unsigned long& nn) {
    unsigned long n=4;
    unsigned long lpr=0;
    bool simple;
    while (sim!=nn) {
     simple=true;
     for(int i=2;i<=sqrt(n);i++) {
        if ((n%i) == 0) {simple=false;break;}
     }
     if (simple) {++sim;lpr=n;}
     ++n;
    }

    return lpr;
}

// Task 3
mutex task3_m, task3_t;
mt19937 gen;
uniform_int_distribution<int> dis(0, 100);

void turn(set<int>& m,set<int>& t, int& time) {
    cout<<"Turn "<<time<<endl;
    cout<<"Master"<<endl;
    copy(m.begin(), m.end(), ostream_iterator<int>(cout, " "));
    cout<<endl;
    cout<<"Thief"<<endl;
    copy(t.rbegin(), t.rend(), ostream_iterator<int>(cout, " "));
    cout<<endl;
    cout<<"--------"<<endl;
}

void master_func(set<int>& m,set<int>& t, int& time) {
    while ((!m.empty()) && (time<100)) {
     scoped_lock l{ task3_m, task3_t };
     m.insert(dis(gen));
     turn(m,t,time);
     ++time;
     l.~scoped_lock();
     this_thread::sleep_for(chrono::milliseconds(2000));
    }
};

void thief_func(set<int>& m,set<int>& t, int& time) {
    while ((!m.empty()) && (time<100)) {
     scoped_lock l{ task3_t, task3_m };
     t.insert(*--(m.end()));
     m.erase(--m.end());
     turn(m,t,time);
     ++time;
     l.~scoped_lock();
     this_thread::sleep_for(chrono::milliseconds(1000));
    }
};

int main(int, char **){

    // ----Task 1
    // Создайте потокобезопасную оболочку для объекта cout. Назовите ее pcout. Необходимо,
    // чтобы несколько потоков могли обращаться к pcout и информация выводилась в консоль.
    // Продемонстрируйте работу pcout.

    cout<<"Task 1"<<endl;
    thread t11(doSomething1);
    thread t12(doSomething2);
    thread t13(doSomething3);
    t11.join();
    t12.join();
    t13.join();
    pcout<<endl;

    pcout<<"Task 2"<<endl;
    // Реализовать функцию, возвращающую i-ое простое число (например, миллионное простое число
    // равно 15485863). Вычисления реализовать во вторичном потоке. В консоли отображать прогресс
    // вычисления.
    long simple;
    unsigned long sim=3;
    unsigned long nn=1000000;
    thread t2([&]() { simple = doPrime(sim,nn); });
    t2.detach();
    const char ch[] ={"\\|/- "};
    int ii=0;
    pcout<<"Waiting ... ";
    while (sim<nn) {
     cout<<'\b';
     cout<<ch[ii++]<<" ";
     cout.width(10);
     cout<<(nn-sim);
     this_thread::sleep_for(chrono::milliseconds(5));
     for (int i=0;i<11;i++) cout<<'\b';
     if (ii==4) {ii=0;};
    }
    pcout<<endl<<nn<<"'th prime is "<<simple;
    pcout<<endl<<endl;

    pcout<<"Task 3"<<endl;
    // Промоделировать следующую ситуацию. Есть два человека (2 потока): хозяин и вор. Хозяин приносит
    // домой вещи. При этом у каждой вещи есть своя ценность. Вор забирает вещи, каждый раз забирает
    //вещь с наибольшей ценностью.
    set<int> m,t;
    m = {67, 43, 99};
    int time=0;
    thread master( [&]() {master_func(m,t,time);});
    thread thief([&]() {thief_func(m,t,time);});
    master.join();
    thief.join();
    cout<<"The end"<<endl;
    return 0;
}
