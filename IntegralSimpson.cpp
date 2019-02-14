//
//  SimpsonFormula
//
//  Created by Kirill Yatsenko on 14/02/2019.
//  Copyright © 2018 kirillyat. All rights reserved.
//


#include <iostream>
#include <cmath>

using namespace std;
/* functions */
double f1(double x) {
    return 3/((x-1)*(x-1) + 1);
}

double f2(double x) {
    return sqrt(x+0.5);
}

double f3(double x) {
    return exp(-x);
}


/*==============================*/

//finding the roots of the equation by dividing the segment in half

double root(double f(double), double g(double), double a, double b, double e) {
    double c = (a+b)/2;
    double delta = f(c)-g(c);
    while (abs(delta) >= e) {
                if ((f(a)-g(a))*delta < 0)
                    b = c;
                else
                    a = c;
                c = (a+b)/2;
                delta = f(c)-g(c);
        }
    return c;
}

double SimpsonFormula(double f(double), double a, double b, int n) {
    double h = (b - a) / n;
    double I = h * (f(a) + f(b)) / 3;

    for (int k = 1; k<n; k++) {
        int t = 2;
        if (k % 2 == 1)
            t = t + 2;
        I = I + h * (t * f(a + k * h)) / 3;
    }
    return I;
}


double integral(double f(double), double a, double b, double e) {

    int n = 10;
    double I1 = SimpsonFormula(f, a, b, n);
    double I2 = SimpsonFormula(f, a, b, 2*n);
    double delta = abs(I1-I2);

    while (delta/15 >= e) {
        n *= 2;
        I1 = I2;
        I2 = SimpsonFormula(f, a, b, 2*n);
        delta = abs(I1-I2);
    }

    return I2;

}





int main() {

    double eps = 0.00001;

    double x1 = root(f1,f2, -0.3, 5.0, eps);
    double x2 = root(f2,f3, -0.3, 5.0, eps);
    double x3 = root(f1,f3, -0.3, 5.0, eps);

    double I1 = abs(integral(f1, min(x1, x3), max(x1, x3), eps));
    double I2 = abs(integral(f2, min(x1, x2), max(x1, x2), eps));
    double I3 = abs(integral(f3, min(x2, x3), max(x2, x3), eps));

    cout << "f1 and f2 intersection point x1  :  "<< x1 << endl;
    cout << "f2 and f3 intersection point x2  :  "<< x2 << endl;
    cout << "f1 and f3 intersection point x3  :  "<< x3 << endl;

    cout << endl;

    cout << "The square under the f1 graph (definite integral) :  "<< I1 << endl;
    cout << "The square under the f2 graph (definite integral) :  "<< I2 << endl;
    cout << "The square under the f3 graph (definite integral) :  "<< I3 << endl;

    cout << endl;

    double answer = max(max(I1, I2), I3)*2 - I1 - I2 - I3;

    cout << "The square bounded by the three functions  :  " << answer << endl;

    return 0;
}
