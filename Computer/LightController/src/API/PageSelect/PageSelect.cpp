#include "PageSelect.hpp"

size_t PageSelect::activePageID = 0;

void PageSelect::setActivePageID(size_t newActivePageID) { activePageID = newActivePageID; }

size_t PageSelect::getActivePageID() { return activePageID; }
