#ifndef PAGESELECT_HPP
#define PAGESELECT_HPP

#include <cstdlib>

class PageSelect
{
public:
    static void    setActivePageID(size_t newActivePageID);
    static size_t getActivePageID();

private:
    static size_t activePageID;
};

#endif  // PAGESELECT_HPP
