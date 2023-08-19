#include "optimized_thread.h"

using namespace std;

mutex coutLocker;
bool make_thread = true;

void quicksort(vector<int>& array, promise<void> p, int left, int right)
{
    if (left >= right) return;
    int left_bound = left;
    int right_bound = right;

    long middle = array[(left_bound + right_bound) / 2];

    do {
        while (array[left_bound] < middle) {
            left_bound++;
        }
        while (array[right_bound] > middle) {
            right_bound--;
        }
        if (left_bound <= right_bound) {
            std::swap(array[left_bound], array[right_bound]);
            left_bound++;
            right_bound--;
        }
    } while (left_bound <= right_bound);

    if (make_thread && (right_bound - left > 10000))
    {

        auto f = async(launch::async, [&]() {
            quicksort(array, p,left, right_bound);
            });
        quicksort(array, p, left_bound, right);
        p.set_value();
    }
    else {
        quicksort(array, p, left, right_bound);
        quicksort(array, p, left_bound, right);
        p.set_value();
    }
}

int main()
{
    vector<int> arr;
    RequestHandler_2 rh;
    promise<void> p;
    future<void> f = p.get_future();

    for (int i = 0; i < 100000; ++i)
        arr.push_back(rand() % 500000);


    rh.push_task(quicksort, arr, p, 0, arr.size() - 1);


    try {
        f.get();
    }
    catch (...) {
        cout << "Some error in thread" << endl;
    }

    this_thread::sleep_for(chrono::seconds(3));

    for (long i = 0; i < arr.size() - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            std::cout << "Unsorted" << std::endl;
            break;
        }
    }

    return 0;
}
