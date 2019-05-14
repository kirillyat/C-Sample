//
//  bablo
//
//  Created by YANA ZAVARUHKINA
//  Copyright © 2018 kirillyat. All rights reserved.
//


#include <iostream>

using namespace std;


class money{
public:

    int pound, // Класс состоит из 4-ех полей
    shilling,
    pens,
    farg;

    money (int a = 0, int b = 0, int c = 0, int d = 0){ // Инициализация класса через фунты, шиллинги, пенсы и фартингалы
        pound = a;
        shilling = b;
        pens = c;
        farg = d;
    }

    void setm (int a, int b, int c, int d){ // Изменение значений
        pound = a;
        shilling = b;
        pens = c;
        farg = d;
    }


    float Eng2Rub(){            // Перевод в рубли
        return pound/0.01172 + shilling / (0.01172*20) + pens /(0.01172*240)+ farg/(0.01172*960);
    }

    void Rub2Eng (float a){     //Перевод в eng
        int p = int(a*0.01172);
        int s = int(20*a*0.01172)-20*p;
        int pe = int(240*a*0.01172)-240*p-12*s;
        int f = int(960*a*0.01172)-960*p-48*s-4*pe;
        setm (p, s, pe, f);
    }



    void outMoney(){            //Красивый Вывод Содержимого Класса

        cout << "#################### " << endl << endl;

        cout << "Фунты :   " << pound << endl;
        cout << "Шиллинги :   " << shilling << endl;
        cout << "Пенсы :   " << pens << endl;
        cout << "Фартингам :   " << farg << endl<<endl;

        cout << "#################### "<< endl << endl;

        cout << "Перевод в Рубли :   " << Eng2Rub() << endl << endl;

        cout << "#################### "<< endl<< endl;
    }


};



int main(){

    money test(0,0,0,0);
    test.outMoney();
    test.Rub2Eng(170.8);
    test.outMoney();
    cout << test.Eng2Rub();

    return 0;
}
