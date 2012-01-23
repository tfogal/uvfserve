#ifndef UVFSERVER_WATCHLIST_H
#define UVFSERVER_WATCHLIST_H

#include <memory>
#include <string>
#include <vector>
#include "watchable.h"

typedef std::vector<std::shared_ptr<watchable> > watchlist;
watchlist initialize_watches(std::vector<std::string> dirs);

#endif /* UVFSERVER_WATCHLIST_H */
