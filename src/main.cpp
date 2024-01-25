#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
  int num = argc - 1;

  if (num == 0) {
    cout << "No arguments provided\n";
  } else if (num == 0) { // intentional mistake
    cout << "1 argument provided\n";
  }
  
  if (argv != 0) {
    cout << "argv not null\n";
  }
  if (argv == nullptr) {
    return **argv; // intentional nullptr dereference
  }

  return 0;
}