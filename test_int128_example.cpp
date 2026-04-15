#include <iostream>
#include <string_view>
#include "int128.h"
 
int main() {
    // 1) Конструирование
    Int128 a;                         // по умолчанию (обычно 0)
    Int128 b(int64_t{-42});           // из int64_t
    Int128 c(std::string_view("123456789012345678901234567890")); // из строки
 
    // 2) Преобразование в строку
    std::cout << "a=" << a.str() << "\n";
    std::cout << "b=" << b.str() << "\n";
    std::cout << "c=" << c.str() << "\n";
 
    // 3) Вывод в поток (operator<<)
    std::cout << "c (via <<) = " << c << "\n";
 
    // 4) Арифметика (+, -, *, /) и составные (+=, -=, *=, /=)
    Int128 x("100000000000000000000");   // 1e20
    Int128 y("3000000000000000000");     // 3e18
 
    Int128 sum = x + y;
    Int128 diff = x - y;
    Int128 prod = x * y;
    Int128 quot = x / y;                 // целочисленное деление
 
    std::cout << "sum=" << sum << "\n";
    std::cout << "diff=" << diff << "\n";
    std::cout << "prod=" << prod << "\n";
    std::cout << "quot=" << quot << "\n";
 
    // Составные операторы
    Int128 t("10");
    t += Int128("5");    // 15
    t *= Int128("7");    // 105
    t -= Int128("20");   // 85
    t /= Int128("4");    // 21 (если деление как в C++: trunc toward zero)
    std::cout << "t=" << t << "\n";
 
    // 5) Унарный минус
    Int128 neg = -c;
    std::cout << "-c=" << neg << "\n";
 
    // 6) Сравнения (==, !=)
    Int128 p("123");
    Int128 q(123);
    std::cout << std::boolalpha;
    std::cout << "p==q: " << (p == q) << "\n";
    std::cout << "p!=q: " << (p != q) << "\n";
 
    // 7) Явные приведения
    Int128 small("9223372036854775807"); // INT64_MAX
    int64_t i64 = static_cast<int64_t>(small);
    double d = static_cast<double>(c);
 
    std::cout << "i64=" << i64 << "\n";
    std::cout << "d=" << d << "\n";
 
    // 8) Примеры со знаками/границами
    Int128 min128("-170141183460469231731687303715884105728"); // -2^127
    Int128 max128("170141183460469231731687303715884105727");  //  2^127 - 1
    std::cout << "min128=" << min128 << "\n";
    std::cout << "max128=" << max128 << "\n";
 
    return 0;
}
 