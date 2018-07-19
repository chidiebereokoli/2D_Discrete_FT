// Distributed two-dimensional Discrete FFT transform
// Chidiebere Nwabufo Okoli
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <numeric>
#include <future>
//#include <functional>
#include <algorithm>
#include <thread>
#include <math.h>
#include "Complex.h"
#include "InputImage.h"
constexpr unsigned int NUMTHREADS = 8;
using namespace std;
//undergrad students can assume NUMTHREADS will evenly divide the number of rows in tested images
//graduate students should assume NUMTHREADS will not always evenly divide the number of rows in tested images.
// I will test with a different image than the one given
int n;
int k;
Complex powerupN (Complex W, int n)      //THIS FUNCTION WAS NO LONGER USED(function for powering a complex number)
{
    Complex temp = W;
    for(int i = 1;i < n; ++i)
    {
        temp = temp*W;
    }
    return temp;
}
void rowTransform1D(Complex* h, int w, Complex* H) //You had to include this(&W) to enable you to pass the complex object needed to use fxn powerupN
{
    // Implement a simple 1-d DFT using the double summation equation
    // given in the assignment handout.  h is the time-domain input
    // data, w is the width (N), and H is the output array.
    // n = outer for loop indexer   k = inner for loop indexer
    //w = width of time domain input data
    // h = pointer to sequence through the input data
    // H = pointer to sequence through the output data that was just created
    //: while performing the row implementation, w = n = k

    //FOR EACH n, WE SUM UP 256ks'

    for (int n = 0; n < w ; ++n){
        Complex temp(0,0);
        for (int k = 0; k < w ; ++k){ // I now know why it was stopping at 128!!!!
            Complex DFT = h[k]*Complex(cos(2*M_PI*n*k/w), sin(-2*M_PI*n*k/w));
            // Complex DFT = h[k]*(powerupN(*W, k*n)); //previously, I was passing only n into the power function.
            temp = temp + DFT;
           // ++h;
        }
        H[n] = temp;     // Along the row......
    }
}
void columnTransform1D(Complex* h, int w, Complex* H) //You had to include this(&W) to enable you to pass the complex object needed to use fxn powerupN
{
    // Implement a simple 1-d DFT using the double summation equation
    // given in the assignment handout.  h is the time-domain input
    // data, w is the width (N), and H is the output array.
    // n = outer for loop indexer   k = inner for loop indexer
    //w = width of time domain input data
    // h = pointer to sequence through the input data
    // H = pointer to sequence through the output data that was just created
    //: while performing the row implementation, w = n = k

    //FOR EACH n, WE SUM UP 256ks'

    for (int n = 0; n < w ; ++n){
        Complex temp(0,0);
        for (int k = 0; k < w ; ++k){ // I now know why it was stopping at 128!!!!
           // Complex DFT = h[k+w]*(powerupN(*W, k*n)); //previously, I was passing only n into the power function.
            Complex DFT = h[k*w]*Complex(cos(2*M_PI*n*k/w), sin(-2*M_PI*n*k/w));
            temp = temp + DFT;
//  N.B n+w is what is needed to move along a column. Since the H index and the h index must be same (as we can see from the rowtransform,
//  H[n+w] will then correspond to h[k+w]
        }
        H[n*w] = temp;     // Along the column......
    }
}

void columnreverseTransform1D(Complex* h, int w, Complex* H) //You had to include this(&W) to enable you to pass the complex object needed to use fxn powerupN
{
    // Implement a simple 1-d DFT using the double summation equation
    // given in the assignment handout.  h is the time-domain input
    // data, w is the width (N), and H is the output array.
    // n = outer for loop indexer   k = inner for loop indexer
    //w = width of time domain input data
    // h = pointer to sequence through the input data
    // H = pointer to sequence through the output data that was just created
    //: while performing the row implementation, w = n = k

    //FOR EACH n, WE SUM UP 256ks'

    for (int n = 0; n < w ; ++n){
        Complex temp(0,0);
        for (int k = 0; k < w ; ++k){ // I now know why it was stopping at 128!!!!

            Complex DFT = h[k*w]*Complex(1/double(w)*cos(2*M_PI*n*k/w), 1/double(w)*sin(2*M_PI*n*k/w)); //Dividing by w, since we are taking the inverse DFT
            temp = temp + DFT;
//  N.B n+w is what is needed to move along a column. Since the H index and the h index must be same (as we can see from the rowtransform,
//  H[n+w] will then correspond to h[k+w]
        }
        H[n*w] = temp;     // Along the column......
    }
}


void rowreverseTransform1D(Complex* h, int w, Complex* H) //You had to include this(&W) to enable you to pass the complex object needed to use fxn powerupN
{
    // Implement a simple 1-d DFT using the double summation equation
    // given in the assignment handout.  h is the time-domain input
    // data, w is the width (N), and H is the output array.
    // n = outer for loop indexer   k = inner for loop indexer
    //w = width of time domain input data
    // h = pointer to sequence through the input data
    // H = pointer to sequence through the output data that was just created
    //: while performing the row implementation, w = n = k

    //FOR EACH n, WE SUM UP 256ks'
    for (int n = 0; n < w ; ++n){ //outer loop is the column indexer
        Complex temp(0,0);
        for (int k = 0; k < w ; ++k){ // the inner loop is the row indexer
            Complex DFT = h[k]*Complex(1/double(w)*cos(2*M_PI*n*k/w), 1/double(w)*sin(2*M_PI*n*k/w)); //Dividing by w, since we are taking the inverse DFT
            temp = temp + DFT;
            // ++h;

        }
        H[n] = temp;
    }
}

void Transform2D(const char* inputFN)
{ // Do the 2D transform here.
    // 1) Use the InputImage object to read in the Tower.txt file and
    //    find the width/height of the input image.
    // Step (1) in the comments is the line above.
    InputImage image(inputFN);  // Create the helper object for reading the image
    // Your code here, steps 2-7
    // 2) Create a vector of complex objects of size width * height to hold
    //    values calculated
    int w = image.GetWidth();
    int h = image.GetHeight();
    int N = w*h; // N is the total size of the array
    std::vector<Complex> Comp_num(N);    //vector that holds the output of the row DFT
    std::vector<Complex> Comp_num_out(N); //vector that holds the output of the column DFT


    Complex W(cos(2*M_PI/w), sin(-1*2*M_PI/w));  // NO LONGER USED, WE NO LONGER USE THE powerupN function
   // Complex WW(cos(2*M_PI*k*n/N), sin(-1*2*M_PI*k*n/N));  // Complex number W.
    // 3) Do the individual 1D transforms on the rows assigned to each thread
    std::vector<thread*> threads;
//    Complex lW = powerupN(W,w);       //////////////////////////////////////clean up
    Complex *data = image.GetImageData();             //pointer with which to access the


    int index = 0;
    for(int i = 0;i < w ; i += NUMTHREADS)
    {
        for (int j = 0; j < NUMTHREADS; ++j)
        {  // 7 is the edge case..NOT TRUE, ACTUALLY
            ////////////////////////  std::thread t(&Transform1D, &data[i * 256], w, &Comp_num[i * 256], &lW);
            ///////////////////////////threads.emplace_back(std::move(t));
            // for(int j = 0; j < w; ++j) {
            //           Transform1D(&data[i*256], w, &Comp_num[i*256], &W);

            //rowTransform1D(&data[index], w, &Comp_num[index],&W);
            ///////////////////// threads.push_back(new thread{Transform1D, &data[i*256], w, &Comp_num[i*256], &W}); //i*256 puts pointer at the beginning of a row
            threads.push_back(new thread{rowTransform1D, &data[index], w, &Comp_num[index]});//i*256 sets the pointer at the right start point

          index += w; // this is consistent with the number of elements in a row ; number of summations done by row transform per index value
            //see the rowTransform1D function; for each n, we have 256(w) ks.
        }
        // 4) Force each thread to wait until all threads have completed their row calculations
        //    prior to starting column calculations
        for(auto &t : threads)
            { t->join(); }

        threads.clear();
    }



    // clears the thread vector, so that other threads can easily be inserted into the container.
    // 5) Perform column calculations
//return;



    index = 0;

    for(int i = 0; i < w ; i += NUMTHREADS)
    {
        for (int j = 0; j < NUMTHREADS; ++j)
        {
            //for(int j = 0; j < h; ++j) {
//           Transform1D(&Comp_num[i*256], w, &Comp_num_out[i*256], &W);
            threads.push_back(new thread{columnTransform1D, &Comp_num[index], h, &Comp_num_out[index]});
            ++index;
        }
        // 6) Wait for all column calculations to complete
        for (auto &t : threads)
        { t->join(); }

        threads.clear();
    }



    // clears the thread vector, so that other threads can easily be inserted into the container.

    // 7) Use SaveImageData() to output the final results

    cout << "Discrete Fourier Transform generated as MyAfterInverse.txt" << endl;

    string fn("MyAfter2D.txt"); // default file name

    image.SaveImageData(fn.c_str(), &Comp_num_out[0], w, h);

    std::vector<Complex> reversecolumn_DFT_output(N); //vector that holds the output of the reversecolumn_DFT
    std::vector<Complex> reverserow_DFT_output(N); //vector that holds the output of the reverserow DFT


    index = 0;

    for(int i = 0; i < w ; i += NUMTHREADS)
    {
        for (int j = 0; j < NUMTHREADS; ++j)
        {

            threads.push_back(new thread{rowreverseTransform1D, &Comp_num_out[index], h, &reverserow_DFT_output[index]});

            index += w;
        }

        for (auto &t : threads)
        { t->join(); }

        threads.clear();
    }


    index = 0;

    for(int i = 0;i < w ; i += NUMTHREADS)
    {
        for (int j = 0; j < NUMTHREADS; ++j)
        {
            threads.push_back(new thread{columnreverseTransform1D, &reverserow_DFT_output[index], w, &reversecolumn_DFT_output[index]});//i*256 sets the pointer at the right start point

            ++index; // this is consistent with the number of elements in a row ; number of summations done by row transform per index value
            //see the rowTransform1D function; for each n, we have 256(w) ks.
        }
        // 4) Force each thread to wait until all threads have completed their row calculations
        //    prior to starting column calculations
        for(auto &t : threads)
        { t->join(); }

        threads.clear();
    }


    cout << "Inverse Transform generated as MyAfterInverse.txt" << endl;

    image.SaveImageDataReal("MyAfterInverse.txt", &reversecolumn_DFT_output[0], w, h);

}
int main(int argc, char** argv)
{
    string fn("Tower.txt"); // default file name
    if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
    Transform2D(fn.c_str()); // Perform the transform.
}

  
