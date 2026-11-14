#include <iostream>
#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

// Структура для збереження результатів
struct Roots
{
    int count = 0;
    double x1 = 0.0;
    double x2 = 0.0;
};

// 1. Функція без специфікації виключень
Roots solvePlain(double a, double b, double c)
{
    if (!isfinite(a) || !isfinite(b) || !isfinite(c))
        throw invalid_argument("Коефіцієнти повинні бути скінченними числами.");

    if (a == 0.0)
        throw invalid_argument("Коефіцієнт a не може дорівнювати нулю.");

    double D = b * b - 4 * a * c;

    if (D < 0)
        throw domain_error("Дійсних коренів немає.");

    Roots r;

    if (D == 0)
    {
        r.count = 1;
        r.x1 = -b / (2 * a);
    }
    else
    {
        r.count = 2;
        r.x1 = (-b + sqrt(D)) / (2 * a);
        r.x2 = (-b - sqrt(D)) / (2 * a);
    }

    return r;
}

// 2. Функція без викидання виключень
// У сучасному C++ throw() відповідає noexcept(true).
bool solveNoexcept(double a, double b, double c, Roots& r) noexcept
{
    if (!isfinite(a) || !isfinite(b) || !isfinite(c) || a == 0.0)
        return false;

    double D = b * b - 4 * a * c;

    if (D < 0)
        return false;

    if (D == 0)
    {
        r.count = 1;
        r.x1 = -b / (2 * a);
        r.x2 = 0;
    }
    else
    {
        r.count = 2;
        r.x1 = (-b + sqrt(D)) / (2 * a);
        r.x2 = (-b - sqrt(D)) / (2 * a);
    }

    return true;
}

// 3. Функція зі стандартними виключеннями
Roots solveStandard(double a, double b, double c)
{
    return solvePlain(a, b, c);
}

// 4. Власне виключення: порожній клас
class EmptyError
{
};

Roots solveEmpty(double a, double b, double c)
{
    if (!isfinite(a) || !isfinite(b) || !isfinite(c) || a == 0)
        throw EmptyError{};

    double D = b * b - 4 * a * c;

    if (D < 0)
        throw EmptyError{};

    if (D == 0)
        return { 1, -b / (2 * a), 0 };

    return {
        2,
        (-b + sqrt(D)) / (2 * a),
        (-b - sqrt(D)) / (2 * a)
    };
}

// 5. Власне незалежне виключення з полями
class EquationError
{
public:
    double a, b, c;
    string message;

    EquationError(double aa, double bb, double cc, string m)
        : a(aa), b(bb), c(cc), message(move(m))
    {
    }
};

Roots solveFields(double a, double b, double c)
{
    if (!isfinite(a) || !isfinite(b) || !isfinite(c) || a == 0)
        throw EquationError(a, b, c,
            "Некоректні коефіцієнти рівняння.");

    double D = b * b - 4 * a * c;

    if (D < 0)
        throw EquationError(a, b, c,
            "Дискримінант від'ємний.");

    if (D == 0)
        return { 1, -b / (2 * a), 0 };

    return {
        2,
        (-b + sqrt(D)) / (2 * a),
        (-b - sqrt(D)) / (2 * a)
    };
}

// 6. Власне виключення — нащадок стандартного класу
class EquationRuntimeError : public runtime_error
{
public:
    double a, b, c;

    EquationRuntimeError(double aa, double bb, double cc,
        const string& message)
        : runtime_error(message), a(aa), b(bb), c(cc)
    {
    }
};

Roots solveDerived(double a, double b, double c)
{
    if (!isfinite(a) || !isfinite(b) || !isfinite(c) || a == 0)
        throw EquationRuntimeError(a, b, c,
            "Некоректні коефіцієнти рівняння.");

    double D = b * b - 4 * a * c;

    if (D < 0)
        throw EquationRuntimeError(a, b, c,
            "Дійсних коренів немає.");

    if (D == 0)
        return { 1, -b / (2 * a), 0 };

    return {
        2,
        (-b + sqrt(D)) / (2 * a),
        (-b - sqrt(D)) / (2 * a)
    };
}

// Функція виведення результату
void printRoots(const Roots& r)
{
    if (r.count == 1)
    {
        cout << "Один корінь: " << r.x1 << endl;
    }
    else if (r.count == 2)
    {
        cout << "Два корені: x1 = " << r.x1
            << ", x2 = " << r.x2 << endl;
    }
}

// Головна функція
int main()
{
    cout << "1. Функція без специфікації:" << endl;

    try
    {
        printRoots(solvePlain(1, -5, 6));
    }
    catch (const exception& e)
    {
        cout << "Виключення: " << e.what() << endl;
    }

    cout << endl;
    cout << "2. Функція noexcept:" << endl;

    Roots r;

    if (solveNoexcept(1, -4, 4, r))
        printRoots(r);
    else
        cout << "Некоректні дані або немає дійсних коренів." << endl;

    cout << endl;
    cout << "3. Стандартні виключення:" << endl;

    try
    {
        printRoots(solveStandard(0, 2, 1));
    }
    catch (const invalid_argument& e)
    {
        cout << "invalid_argument: " << e.what() << endl;
    }
    catch (const domain_error& e)
    {
        cout << "domain_error: " << e.what() << endl;
    }

    cout << endl;
    cout << "4. Власне порожнє виключення:" << endl;

    try
    {
        printRoots(solveEmpty(1, 2, 5));
    }
    catch (const EmptyError&)
    {
        cout << "Перехоплено EmptyError." << endl;
    }

    cout << endl;
    cout << "5. Власне виключення з полями:" << endl;

    try
    {
        printRoots(solveFields(0, 2, 1));
    }
    catch (const EquationError& e)
    {
        cout << e.message << endl;
        cout << "a = " << e.a
            << ", b = " << e.b
            << ", c = " << e.c << endl;
    }

    cout << endl;
    cout << "6. Виключення-нащадок стандартного класу:" << endl;

    try
    {
        printRoots(solveDerived(1, 2, 5));
    }
    catch (const EquationRuntimeError& e)
    {
        cout << e.what() << endl;
        cout << "a = " << e.a
            << ", b = " << e.b
            << ", c = " << e.c << endl;
    }

    return 0;
}