#include <opencv2/opencv.hpp>
#include <iostream> 
#include <thread>
#include <chrono> 

#include <numeric> 
#include <algorithm> 
#include <functional> 

#include "LockingQueue.h"
#include "AtomicQueue.h"
#include "ChaseLev.h"

LockingQueue<int> locking_queue(1 << 8);
AtomicQueue<int> atomic_queue(1 << 8); 
ChaseLev<int> cl_queue(1 << 8); 

std::vector<int> enqueue1; 
std::vector<int> dequeue1; 

std::vector<int> enqueue2; 
std::vector<int> dequeue2; 

std::vector<int> enqueue3; 
std::vector<int> dequeue3; 

double average_of_greatest_n(std::vector<int> vec, size_t top_n) {
    if (vec.size() < top_n) {
        std::cerr << "Error: Vector must contain at least " << top_n << " elements." << std::endl;
        return 0.0; // Handle error appropriately
    }
    std::partial_sort(vec.begin(), vec.begin() + top_n, vec.end(), std::greater<int>());
    double sum = std::accumulate(vec.begin(), vec.begin() + top_n, 0.0);
    return sum / top_n;
}

void displayHistogram(const std::vector<int>& hist_data, const std::string& window_name) {
    int margin_l = 60; 
    int margin_b = 40; 
    int hist_w = 512; // histogram image width
    int hist_h = 400; // histogram image height
    int total_w = hist_w + margin_l + 20; 
    int total_h = hist_h + margin_b; 

    int bin_w = cvRound((double)hist_w / hist_data.size());

    int y_limit = 0; 
    for (auto value: hist_data) {
        y_limit = std::max(value, y_limit); 
    }

    // Create a blank image for the histogram
    cv::Mat histImage(total_h, total_w, CV_8UC3, cv::Scalar(255, 255, 255));

    // Find the maximum frequency for normalization
    int max_freq = 0;
    for (int val : hist_data) {
        if (val > max_freq) {
            max_freq = val;
        }
    }

    // Normalize the histogram values to the height of the image
    std::vector<int> normalized_hist(hist_data.size());
    for (int i = 0; i < (int)hist_data.size(); ++i) {
        double ratio = (double)hist_data[i] / y_limit;
        normalized_hist[i] = cvRound(std::min(1.0, ratio) * hist_h);    
    }

    int y_ticks = 5; 
    for (int i = 0; i <= y_ticks; i++) {
        int y = hist_h - (i * hist_h / y_ticks);
        int label_val = i * y_limit / y_ticks;
        
        line(histImage, cv::Point(margin_l - 5, y), cv::Point(margin_l, y), cv::Scalar(0, 0, 0), 1);
        cv::putText(histImage, std::to_string(label_val), cv::Point(5, y + 5), 
                    cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(50, 50, 50), 1);
    }

    int x_ticks = 8; // Number of markers on X axis
    for (int i = 0; i <= x_ticks; i++) {
        // Calculate which bin this tick represents
        int bin_idx = i * (hist_data.size() - 1) / x_ticks;
        int x = margin_l + (bin_idx * bin_w);

        // Draw tick mark
        line(histImage, cv::Point(x, hist_h), cv::Point(x, hist_h + 5), cv::Scalar(0, 0, 0), 1);

        // Draw bin index label
        std::string label = std::to_string(bin_idx);
        cv::putText(histImage, label, cv::Point(x - 10, hist_h + 20), 
                    cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(50, 50, 50), 1);
    }

    // draw main axis lines
    line(histImage, cv::Point(margin_l, 0), cv::Point(margin_l, hist_h), cv::Scalar(0, 0, 0), 2); // Y
    line(histImage, cv::Point(margin_l, hist_h), cv::Point(margin_l + hist_w, hist_h), cv::Scalar(0, 0, 0), 2); // X

    // Draw the histogram bins as lines
    for (int i = 1; i < (int)hist_data.size(); i++) {
        cv::Point p1(margin_l + bin_w * (i - 1), hist_h - normalized_hist[i - 1]);
        cv::Point p2(margin_l + bin_w * i, hist_h - normalized_hist[i]);
        line(histImage, p1, p2, cv::Scalar(255, 0, 0), 2, cv::LINE_AA);
    }

    // Display the histogram image
    imshow(window_name, histImage);
}

void test1() {
    for (int i = 0; i < (1 << 8); i++) {
        auto start = std::chrono::high_resolution_clock::now(); 
        locking_queue.enqueue(10); 
        auto stop = std::chrono::high_resolution_clock::now(); 
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
        enqueue1[i] = duration; 
    }
}

void test2() {
    int r; 
    for (int i = 0; i < (1 << 8); i++) {
        auto start = std::chrono::high_resolution_clock::now(); 
        locking_queue.dequeue(r); 
        auto stop = std::chrono::high_resolution_clock::now(); 
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
        dequeue1[i] = duration; 
    }
}

void test3() {
    for (int i = 0; i < (1 << 8); i++) {
        auto start = std::chrono::high_resolution_clock::now(); 
        atomic_queue.try_enqueue(10);
        auto stop = std::chrono::high_resolution_clock::now(); 
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
        enqueue2[i] = duration; 
    }
}

void test4() {
    int r;
    for (int i = 0; i < (1 << 8); i++) {
        auto start = std::chrono::high_resolution_clock::now(); 
        atomic_queue.try_dequeue(r); 
        auto stop = std::chrono::high_resolution_clock::now(); 
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
        dequeue2[i] = duration; 
    }
}

void test5() {
    for (int i = 0; i < (1 << 8); i++) {
        auto start = std::chrono::high_resolution_clock::now(); 
        cl_queue.try_pushTail(10);
        auto stop = std::chrono::high_resolution_clock::now(); 
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
        enqueue3[i] = duration; 
    }

    int r;
    for (int i = (1 << 7); i < (1 << 8); i++) {
        auto start = std::chrono::high_resolution_clock::now(); 
        cl_queue.try_popTail(r); 
        auto stop = std::chrono::high_resolution_clock::now(); 
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
        dequeue3[i] = duration; 
    }
}

void test6() {
    int r;
    for (int i = 0; i < (1 << 7); i++) {
        auto start = std::chrono::high_resolution_clock::now(); 
        cl_queue.try_popHead(r); 
        auto stop = std::chrono::high_resolution_clock::now(); 
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
        dequeue3[i] = duration; 
    }
}

int main() {

    enqueue1.resize(1 << 8); 
    enqueue2.resize(1 << 8); 
    enqueue3.resize(1 << 8); 

    dequeue1.resize(1 << 8); 
    dequeue2.resize(1 << 8); 
    dequeue3.resize(1 << 8); 

    std::cout << "hello world" << std::endl;  

    std::thread t1(test1); 
    std::thread t2(test2);

    t1.join(); 
    t2.join();

    std::thread t3(test3); 
    std::thread t4(test4);

    t3.join(); 
    t4.join();

    std::thread t5(test5); 
    std::thread t6(test6);

    t5.join(); 
    t6.join();

    const size_t top_n = .01 * (1 << 8); 
    std::cout << average_of_greatest_n(enqueue1, top_n) << std::endl; 
    std::cout << average_of_greatest_n(dequeue1, top_n) << std::endl; 
    std::cout << average_of_greatest_n(enqueue2, top_n) << std::endl; 
    std::cout << average_of_greatest_n(dequeue2, top_n) << std::endl; 
    std::cout << average_of_greatest_n(enqueue3, top_n) << std::endl; 
    std::cout << average_of_greatest_n(dequeue3, top_n) << std::endl; 

    std::cout << "goodbye world" << std::endl; 

    std::sort(enqueue1.begin(), enqueue1.end(), std::greater<int>()); 
    std::sort(enqueue2.begin(), enqueue2.end(), std::greater<int>()); 
    std::sort(enqueue3.begin(), enqueue3.end(), std::greater<int>()); 
    std::sort(dequeue1.begin(), dequeue1.end(), std::greater<int>()); 
    std::sort(dequeue2.begin(), dequeue2.end(), std::greater<int>()); 
    std::sort(dequeue3.begin(), dequeue3.end(), std::greater<int>()); 

    displayHistogram(enqueue1, "blocking enqueue"); 
    displayHistogram(enqueue2, "atomic enqueue"); 
    displayHistogram(enqueue3, "chase lev enqueue"); 
    displayHistogram(dequeue1, "blocking dequeue"); 
    displayHistogram(dequeue2, "atomic dequeue"); 
    displayHistogram(dequeue3, "chase lev dequeue"); 

    cv::waitKey(0); 

    return 0; 
}
