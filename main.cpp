#include <iostream>
#include <vector>
#include <future>
#include <algorithm>
#include <cassert>

bool make_thread = false;

void merge(std::vector<int>& array, size_t begin, size_t middle, size_t end)
{
    size_t nl = middle - begin + 1;
    size_t nr = end - middle;

    std::vector<int> left(nl);
    std::vector<int> right(nr);

    for (int i = 0; i < nl; i++)
        left[i] = array[begin + i];
    for (int j = 0; j < nr; j++)
        right[j] = array[middle + 1 + j];

    size_t i = 0, j = 0;
    size_t k = begin;

    while (i < nl && j < nr) {
        if (left[i] <= right[j]) {
            array[k] = left[i];
            i++;
        }
        else {
            array[k] = right[j];
            j++;
        }
        k++;
    }
    while (i < nl) {
        array[k] = left[i];
        i++;
        k++;
    }
    while (j < nr) {
        array[k] = right[j];
        j++;
        k++;
    }
}

void mergeSort(std::vector<int>& array, size_t begin, size_t end)
{
    if (begin >= end)
        return;

    size_t middle = begin + (end - begin) / 2;

    if (make_thread && (middle - begin) > 10000) {
        auto futureLeft = std::async(std::launch::async, [&]() {
                mergeSort(array, begin, middle);
            });
         mergeSort(array, middle + 1, end);
    }
    else {
        mergeSort(array, begin, middle);
        mergeSort(array, middle + 1, end);
    }
    merge(array, begin, middle, end);
}

void print(std::vector<int>& vec)
{
    for(auto&v : vec)
        std::cout << v << " ";
}

int main(int argc, const char* argv[]) 
{
    std::vector<int> vec(10000000);
    for (auto& v : vec)
        v = std::rand() % 500;

    std::vector<int> vecCopy = vec;

    std::cout << "\nSingle thread\n";
    auto start = std::chrono::high_resolution_clock::now();
    mergeSort(vecCopy, 0, vecCopy.size() - 1);
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << "The time of merge sort: " << std::chrono::duration_cast<std::chrono::milliseconds> (stop - start).count() << " milliseconds\n";
    //print(vecCopy);

    std::cout << "\nMultithread\n";
    make_thread = true;
    start = std::chrono::high_resolution_clock::now();
    mergeSort(vec, 0, vec.size() - 1);
    stop = std::chrono::high_resolution_clock::now();
    std::cout << "The time of merge sort: " << std::chrono::duration_cast<std::chrono::milliseconds> (stop - start).count() << " milliseconds\n";
    //print(vec);

    assert(vecCopy == vec);
    std::cout << "\nSorted!" << std::endl;

    return 0;
}