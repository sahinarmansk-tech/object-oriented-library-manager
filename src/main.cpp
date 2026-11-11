#include "LibraryManager.h"

#include <iostream>

int main() {
    LibraryManager library("data/library_data.txt");
    if (!library.loadData()) {
        std::cerr << "The library could not start because its data file failed validation.\n"
                  << "Restore a valid backup before trying again; the file was not overwritten.\n";
        return 1;
    }
    library.run();
    return 0;
}
