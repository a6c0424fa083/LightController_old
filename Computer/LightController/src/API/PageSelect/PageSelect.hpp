#ifndef PAGESELECT_HPP
#define PAGESELECT_HPP

#include <cstdlib>

enum page : size_t
{
    HOME     = 0,
    PATCH    = 1,
    LOCATION = 2,
    EFFECTS  = 3,
    LIBRARY  = 4,
    SETTINGS = 5,
    NONE     = 255,
};

class PageSelect
{
public:
    static void   setActivePageID(size_t newActivePageID);
    static size_t getActivePageID();

private:
    static size_t activePageID;
};

#endif  // PAGESELECT_HPP
