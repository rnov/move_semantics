/*
 * Move semantics example that helps to understand better move semantics and its characteristics.
 * Keep in mind:
 *  - POD data does NOT implement move semantics (int, float, char, double ...)
 *  - std::move : cast to rvalue, does NOT move anything, neither guarantee the object will be moved !
 *  - std containers implement move semantics operations (vector, double queue, string) except array since it does not
 *    allocates in heap (does not use resources), but in stack.
 *  - std::move does NOT do anything at runtime
 *  - const object cannot be moved.
 * */
#include <string>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstring>

// custom class that represents a custom string object which implements move semantics functions (constructor,assignation)
struct My_string {

    // default constructor
    My_string()
            :_init(nullptr), _length(0), _size(0)
    {
        std::cout << "Default constructor has been called" << '\n';
    };

    // custom constructor
    My_string(const char* rhs)
            :_size(std::strlen(rhs)), _length(std::strlen(rhs))
    {
        _init = new char[_length]();  // +1
        //_init = (char*)std::malloc( sizeof(char) * _length);
        std::memcpy(_init, rhs, _length);
    }

    // Copy Constructor
    My_string(const My_string& rhs)
            :_length(rhs._length), _size(rhs._size)
    {
        _init = new char[rhs._length];
        std::memcpy(_init, rhs._init, rhs._length);
        std::cout << "COPY constructor has been called" << '\n';
    };

    // Move constructor
    My_string(My_string&& rsh) noexcept
    {
        _init = rsh._init;

        rsh._init = nullptr;
        _size = rsh._size;
        _length = rsh._length;

        rsh._size = 0;
        rsh._length = 0;
        std::cout << "MOVE constructor has been called" << '\n';
    };

    // Copy assignation
    My_string& operator=(const My_string& rhs)
    {
        if (_init!=nullptr)
            delete[] _init;
        _init = new char[rhs._length];

        std::memcpy(_init, rhs._init, rhs._length);
        _size = rhs._size;
        _length = rhs._length;

        std::cout << "COPY assignment has been called" << '\n';
        return *this;
    };

    // Move assignation
    My_string& operator=(My_string&& rhs)
    {

        if (this!=&rhs) {
            // delete what's already in current object
            delete[] _init;
            // assign direction
            _init = rhs._init;

            _length = rhs._length;
            _size = rhs._size;

            // Release the rhs pointer.
            rhs._init = nullptr;
            rhs._length = 0;  // not necessary
            rhs._size = 0;

            std::cout << "MOVE assignment has been called" << '\n';
        }
        return *this;
    };

    My_string& operator+=(const My_string& rhs)
    {
        if (_size<=rhs._length)
            resize(rhs._length);

        memcpy(&_init[_length], rhs._init, rhs._length);
        return *this;
    }

    ~My_string()
    {
        delete[] _init;
        std::cout << "DESTRUCTOR has been called" << '\n';
    };

    void print_str(std::string obj_name)
    {
        if (_init!=nullptr) {
            std::cout << obj_name << " : ";
            for (auto i = 0; i<strlen(_init); ++i)
                std::cout << _init[i];
            std::cout << '\n';
        }
        else {
            std::cout << obj_name << " : No data available" << '\n';
        }
    }

private:
    char* _init;
    size_t _length;
    size_t _size;

    void resize(size_t new_length)
    {
        // old pointer that points to our old data
        char* old = _init;
        // aux pointer that points to the new position
        char* aux = new char[_size+new_length];
        // copy old data to new space
        memcpy(aux, _init, _length);
        // object pointer points to the new position
        _init = aux;
        // delete old data pointed by old
        delete[] old;
    }
};

// custom class that represents a string and is implemented using the std string.
// since we are using std::string that manages resources (memory) it is a must use std::move in the move ctr.
struct Std_string {
    Std_string()
            :_std_string("default")
    {
        std::cout << "Std_string - Default constructor called\n";
    };

    Std_string(const Std_string& rhs) = default;

    Std_string& operator=(const Std_string& rhs)
    {
        _std_string = rhs._std_string;
        std::cout << "Std_string - Copy assignation called\n";
        return *this;
    }

    Std_string& operator=(Std_string&& rhs)
    {
        _std_string = std::move(rhs._std_string);
        std::cout << "Std_string - move assignation called\n";
        return *this;
    }

    ~Std_string()
    {
        std::cout << "Std_string - Destructor called\n";
    }

private:
    std::string _std_string;
};

// custom class that uses Std_string class to implement move ctr, it aims to prove that custom classes must use
// std::move functions in their implementations with non-POD object, otherwise the object would not be moved but copied
struct My_custom {

    My_custom()
    {
        std::cout << "My_custom-Default constructor called\n";
    }

    My_custom(const My_custom& rhs)
            :_str(rhs._str)
    {
        std::cout << "My_custom-move failed!\n";
    }

    // comment 1 of the following 2 functions that implement move ctr (right and wrong)
    My_custom(My_custom&& rhs) noexcept
    {
        _str = std::move(rhs._str);
        std::cout << "My_custom - move constructor called\n";
    }

//    My_custom(My_custom&& rhs) noexcept
//    {
//    	myA = rhs.myA;
//    	std::cout << "My_custom - (copy) move constructor called\n";
//    }

    ~My_custom()
    {
        std::cout << "My_custom - Destructor called\n";
    }

private:
    Std_string _str;
};

void custom_append()
{
    std::cout << "custom append (My_string)" << '\n';
    My_string lhs = "first Element ";
//    My_string lhs = "";  // empty ctr

    My_string rhs = "second addition ";

    lhs += rhs;
    lhs.print_str("lhs");
}

void case_1()
{
    std::cout << "Case 1 : " << '\n';
    My_string rhs("someRandomMessage");
    rhs.print_str("rhs");

    // move constructor
    My_string lhs_move(std::move(rhs));
    rhs.print_str("rhs");
    lhs_move.print_str("lhs_move");

    // copy constructor
    My_string lhs_copy = lhs_move;
    lhs_copy.print_str("lhs_copy");

}

void case_2()
{
    std::cout << "Case 2 : " << '\n';
    // copy assignment
    My_string lhs{};
    My_string rhs{"Not empty"};
    lhs = rhs;
}

void case_3()
{
    std::cout << "Case 3 : " << '\n';
    My_string rhs("thisIsAssignmentIsTooLong");
    My_string lhs("");  // custom ctr: "" -> 0
//    My_string lhs{};  // empty ctr
    // copy assignment
    lhs = rhs;
    lhs.print_str("lhs");
    // move assignment
    My_string lhs_move("");
    lhs_move = std::move(lhs);
    lhs_move.print_str("lhs_move");
    lhs.print_str("lhs");
};

void case_4()
{
    std::cout << "Trying to move (Case 4)\n";
    //My_custom a1 = f(My_custom()); // move-construct from rvalue temporary
    My_custom rhs{};
    My_custom lhs{std::move(rhs)};
    //My_custom lhs = std::move(rhs); // move-construct from xvalue
}

//*
int main()
{

    //case_2();

//    case_3();

//    case_4();

    custom_append();

//    case_1();

    return 0;
}
// */